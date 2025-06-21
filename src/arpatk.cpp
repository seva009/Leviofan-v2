#include "types.h"
#include <WiFi.h>
#include <stdlib.h>
#include <string.h>

#include "esp_private/wifi.h"
#include "esp_wifi.h"
/*
    * ARP frame generator function
    * Generates an ARP reply frame with the given source and destination IP and MAC addresses.
    * Returns a pointer to the generated ARP frame.
*/
void* arpFrameGen(ipv4_t *src_ip, mac_address_t *src_mac, ipv4_t *dest_ip, mac_address_t *dest_mac) {
    uint8_t broadcast_mac_address[6];
    arp_hdr_t arp_pkt;
    uint8_t ether_frame[PACKET_LENGTH];

    memset(broadcast_mac_address, 0xff, sizeof(broadcast_mac_address));
    memcpy(ether_frame, broadcast_mac_address, MAC_ADDRESS_LENGTH * sizeof(uint8_t));
    memcpy(ether_frame + MAC_ADDRESS_LENGTH, src_mac, MAC_ADDRESS_LENGTH * sizeof(uint8_t));

    ether_frame[12] = ETHERNET_PROTOCOL_ARP >> 8; // shift right by 8 bits
    ether_frame[13] = ETHERNET_PROTOCOL_ARP & 0xFF; // mask to get the lower 8 bits

    arp_pkt.hw_type = htons(ETH_ARP_HW_TYPE);
    arp_pkt.ptype = htons(ETH_HW_TYPE);  // Ethernet protocol
    arp_pkt.hlen = MAC_ADDRESS_LENGTH;
    arp_pkt.plen = IPV4_LENGTH;
    arp_pkt.opcode = htons(ARP_REPLY_OPCODE);

    memcpy(&arp_pkt.sender_ip, src_ip, IPV4_LENGTH * sizeof(uint8_t));
    memcpy(&arp_pkt.target_ip, dest_ip, IPV4_LENGTH * sizeof(uint8_t));
    memcpy(&arp_pkt.sender_mac, src_mac, MAC_ADDRESS_LENGTH * sizeof(uint8_t));
    memcpy(&arp_pkt.target_mac, dest_mac, MAC_ADDRESS_LENGTH * sizeof(uint8_t));
    memcpy(ether_frame + ETH_HDRLEN, &arp_pkt, ARP_HDRLEN * sizeof(uint8_t));
    void *ether_frame_ptr = malloc(PACKET_LENGTH);
    memcpy(ether_frame_ptr, ether_frame, PACKET_LENGTH * sizeof(uint8_t));
    return ether_frame_ptr;
}

void sendArpPacket(ipv4_t *src_ip, mac_address_t *src_mac, ipv4_t *dest_ip, mac_address_t *dest_mac) {
    void *arp_frame = arpFrameGen(src_ip, src_mac, dest_ip, dest_mac);
    esp_wifi_internal_tx(WIFI_IF_STA, arp_frame, PACKET_LENGTH)?Serial.print("!"):Serial.print("");
    free(arp_frame);  // Free the allocated memory for the ARP frame
} 

uint8_t connect_wifi(const char* ssid, const char* password) {
    WiFi.mode(WIFI_AP_STA);
    WiFi.begin(ssid, password);
    Serial.print("Connecting to WiFi");
    uint8_t iterations = 0;
    while (WiFi.status() != WL_CONNECTED && iterations < 20) {
        delay(500);
        Serial.print(".");
        iterations++;
    }
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("Failed to connect to WiFi");
        return WiFi.status();  // Return error code if connection fails
    }
    else Serial.println("Connected!");
    return 0;
}