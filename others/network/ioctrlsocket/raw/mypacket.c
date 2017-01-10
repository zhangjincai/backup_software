
#include "mypacket.h"
#include <string.h>
#include <arpa/inet.h>

void make_eth(ETH_T *eth,u16 type,struct who one)
{
    memcpy(eth->DestMac,one.Dmac,6);
    memcpy(eth->SrcMac,one.Smac,6);
    eth->Type=htons(type);
}
void make_arp(ARP_T *arp,u16 op,struct who one)
{
    arp->HardType  =htons(0x0001);
    arp->ProtType  =htons(0x0800);
    arp->HardLength=6;
    arp->ProtLength=4;

    arp->op        =htons(op);

    memcpy(arp->SendrMac,one.Smac,6);
    arp->SendrIp   =one.Sip;
    memcpy(arp->TargetMac,one.Dmac,6);
    arp->TargetIp  =one.Dip;
}
