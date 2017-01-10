
#ifndef  _MY_PACKET_H_
#define  _MY_PACKET_H_

typedef unsigned char   u8;
typedef unsigned short  u16;
typedef unsigned int    u32;

struct who{
   u8  Dmac[6];//目标mac
   u8  Smac[6];//源mac
   u32 Dip;
   u32 Sip;
};

#pragma   pack(1)
//以态网首部
typedef struct eth{
    u8     DestMac[6];
    u8     SrcMac[6];
    u16    Type;
}ETH_T;
//arp协议头
typedef struct arp{
    u16    HardType;
    u16    ProtType;
    u8     HardLength;
    u8     ProtLength;
    u16    op;
    u8     SendrMac[6];
    u32    SendrIp;
    u8     TargetMac[6];
    u32    TargetIp;
}ARP_T;

void make_eth(ETH_T *eth,u16 type,struct who one);
void make_arp(ARP_T *arp,u16 op,struct who one);
#endif  //_MY_PACKET_H_
