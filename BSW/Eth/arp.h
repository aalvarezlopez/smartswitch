#ifndef ARP_H
#define ARP_H

void ARP_sendrequest(char*);
void ARP_sendreplyrouter(char * originip, char * originmac);
#endif
