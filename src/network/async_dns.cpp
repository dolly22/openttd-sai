#include "../stdafx.h"
#include "../debug.h"
#include "../core/lru_cache.hpp"
#include <ares.h>
#include "async_dns.h"
#include "network_base.h"
#include "network_internal.h"
#include "network_server.h"

#define DNS_RESOLVE_CACHE_SIZE 256

class dns_cache_entry {
	public:
		bool resolved;
		char host_name[NETWORK_HOSTNAME_LENGTH];
};

typedef LRUCache<uint32,dns_cache_entry> dns_resolve_cache_type;

ares_channel _channel;
dns_resolve_cache_type *_dns_resolve_cache;

void ADNS_Init()
{
	ares_addr_node *servers;

	ares_library_init(ARES_LIB_INIT_ALL);
	ares_init(&_channel);
	
	//HACK: Filter ipv6 servers (windows initializes some strange addresses)
	ares_get_servers(_channel, &servers);
	if (servers != NULL)
	{
		ares_addr_node *first = NULL;
		ares_addr_node *last = NULL;
		ares_addr_node *curr = servers;
		ares_addr_node *next = NULL;

		while(curr != NULL)
		{
			next = curr->next;
			if (curr->family == AF_INET)
			{
				if (first == NULL)
					first = curr;
				else
					last->next = curr;

				last = curr;
				curr->next = NULL;
			}
			curr = next;
		}

		if (first != NULL)
			ares_set_servers(_channel, first);
	}

	_dns_resolve_cache = new dns_resolve_cache_type(DNS_RESOLVE_CACHE_SIZE);
}

void ADNS_Close()
{
	if (_dns_resolve_cache)
		delete _dns_resolve_cache;

	if (_channel)
		ares_destroy(_channel);

	ares_library_cleanup();

	_channel = NULL;
	_dns_resolve_cache = NULL;
}

void ADNS_Process()
{
	fd_set read_fds, write_fds;
	int nfds;
	struct timeval *tvp, tv, maxtv;

	// max timeout
	maxtv.tv_usec = 500;

	if (!_channel)
		return;

	FD_ZERO(&read_fds);
	FD_ZERO(&write_fds);
	nfds = ares_fds(_channel, &read_fds, &write_fds);
	if (nfds == 0)
		return;

	tvp = ares_timeout(_channel, &maxtv, &tv);
	select(nfds, &read_fds, &write_fds, NULL, tvp);
	ares_process(_channel, &read_fds, &write_fds);
}

void resolve_callback(void *arg, int status, int timeouts, struct hostent *hostent)
{
	dns_cache_entry *cache_entry;
	
	if (status != ARES_SUCCESS)
    {
		DEBUG(net, 8, "[dns] %d result error: %s", _frame_counter, ares_strerror(status));
		cache_entry->resolved = false;
    }
	else
	{	
		uint32 client_ip = (uint32)arg;

		cache_entry = _dns_resolve_cache->fetch_ptr(client_ip, false);
		if (cache_entry != NULL)
		{
			cache_entry->resolved = true;
			strncpy_s(cache_entry->host_name, hostent->h_name, NETWORK_HOSTNAME_LENGTH - 1);
			cache_entry->host_name[NETWORK_HOSTNAME_LENGTH - 1] = '\0';

			DEBUG(net, 2, "[dns] %d resolved %s to %s", _frame_counter, inet_ntoa(*(struct in_addr *)&client_ip), hostent->h_name);
		}
	}	
}



void ADNS_Submit(NetworkAddress *client_address)
{
	uint32 client_ip;
	dns_cache_entry *cache_entry;
	const sockaddr_storage *addr = client_address->GetAddress();

	if (!_dns_resolve_cache)
		return;
	
	if (addr->ss_family == AF_INET) {
		client_ip = (uint32)((sockaddr_in*)addr)->sin_addr.s_addr;
	} else {
		// this family type not supported yet
		return;
	}

	// does it already exists in cache?
	if ((cache_entry = _dns_resolve_cache->fetch_ptr(client_ip, true)) == NULL)
	{
		if (!_channel)
			return;
	
		DEBUG(net, 2, "[dns] %d submit reverse resolve for %s", _frame_counter, inet_ntoa(*(struct in_addr *)&client_ip));

		dns_cache_entry cache_entry;
		cache_entry.resolved = false;
		cache_entry.host_name[0] = '\0';
		_dns_resolve_cache->insert(client_ip, cache_entry);
	
		ares_gethostbyaddr(_channel, &((sockaddr_in*)addr)->sin_addr.s_addr, sizeof(uint32), AF_INET, resolve_callback, (void *)client_ip);
	}
}

void ADNS_UpdateResolved(NetworkClientInfo *ci)
{
	if (!_channel || ci == NULL || ci->hostname_resolved)
		return;

	// run process first
	ADNS_Process();

	dns_cache_entry *cache_entry = NULL;

	const sockaddr_storage *addr = NetworkClientSocket::GetByClientID(ci->client_id)->client_address.GetAddress();
	if (addr->ss_family == AF_INET) {
		cache_entry = _dns_resolve_cache->fetch_ptr(((sockaddr_in*)addr)->sin_addr.s_addr, false);
	}

	// check cache, if resolved, copy to networkinfo
	if (cache_entry != NULL)
	{
		ci->hostname_resolved = cache_entry->resolved;

		// copy string and make it lowercase
		int i;
		for (i=0; i < NETWORK_HOSTNAME_LENGTH && cache_entry->host_name[i] != '\0'; i++)
		{
			if (isalpha(cache_entry->host_name[i]) && !islower(cache_entry->host_name[i]))
				ci->hostname[i] = tolower(cache_entry->host_name[i]);
			else
				ci->hostname[i] = cache_entry->host_name[i];
		}
		ci->hostname[i] = '\0';
	}
}


