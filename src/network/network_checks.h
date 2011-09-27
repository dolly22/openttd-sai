#ifndef NETWORK_CHECKS_H
#define NETWORK_CHECKS_H

#ifdef ENABLE_NETWORK

bool PrecheckNetworkCommands(NetworkClientSocket *cs, CommandPacket cp);

#endif

#endif 