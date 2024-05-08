#ifndef ARP_H
#define ARP_H

void ARP_sendrequest(char*);
void ARP_sendreplyrouter(char * originip, char * originmac);
bool arp_isResolved(uint8_t ip);
#endif
