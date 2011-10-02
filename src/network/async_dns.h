#ifndef ASYNCDNS_H
#define ASYNCDNS_H

#include "../stdafx.h"
#include "../debug.h"
#include <ares.h>
#include "network_base.h"

void ADNS_Init();
void ADNS_Close();
void ADNS_Process();

void ADNS_Submit(NetworkAddress *client_address);
void ADNS_UpdateResolved(NetworkClientInfo *ci);

#endif
