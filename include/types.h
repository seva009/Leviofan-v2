#ifndef __TYPES_H__
#define __TYPES_H__

#include <stdint.h>

#define AP_SZ sizeof(AccessPoint)

struct AccessPoint {
  uint8_t selfsz;
  uint8_t data; // channel << 4 | secured 
  uint8_t mac[6];
  uint8_t ssid[32] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}; // kak mnoga budet' pamyati xavat' v eeprom...
} __attribute__((packed));

//some constants
#define ETH_HDRLEN 14  // Ethernet header length
#define ARP_HDRLEN 28  // ARP header length
#define ETH_HW_TYPE 0x0800
#define ARP_REPLY_OPCODE 2
#define MAC_ADDRESS_LENGTH 6
#define IPV4_LENGTH 4
#define ETH_ARP_HW_TYPE 1
#define ETHERNET_PROTOCOL_ARP 0x0806
#define PACKET_LENGTH ETH_HDRLEN + ARP_HDRLEN

struct arp_hdr {
    uint16_t hw_type;  // Hardware type
    uint16_t ptype;
    uint8_t hlen;
    uint8_t plen;
    uint16_t opcode;
    uint8_t sender_mac[MAC_ADDRESS_LENGTH];
    uint8_t sender_ip[IPV4_LENGTH];
    uint8_t target_mac[MAC_ADDRESS_LENGTH];
    uint8_t target_ip[IPV4_LENGTH];
} __attribute__((packed));

typedef struct arp_hdr arp_hdr_t;

typedef struct {
    uint8_t octet1;
    uint8_t octet2;
    uint8_t octet3;
    uint8_t octet4;
} ipv4_t;

typedef struct mac_address {
    uint8_t octet1;
    uint8_t octet2;
    uint8_t octet3;
    uint8_t octet4;
    uint8_t octet5;
    uint8_t octet6;
} mac_address_t;

#endif