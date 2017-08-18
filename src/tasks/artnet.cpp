/*
 * artnet.cpp
 * Copyright (C) 2017 Benoit Rapidel <benoit.rapidel+devs@exmachina.fr>
 *
 * Distributed under terms of the MIT license.
 */

#include "artnet.h"


#include "mbed.h"
#include "EthernetInterface.h"

#include "common.h"

#include "WDY/helpers.h"

volatile bool new_dmx_sig = false;
dmx_device_union_t DMXdevice;
dmx_device_union_t _lastDMXdevice;

uint8_t mac_addr[6];
in_addr ip_addr;
in_addr nm_addr;
in_addr gw_addr;
in_addr bc_addr;
bool dhcp = false;

uint8_t subnet[2];
uint8_t swin[4];
uint8_t swout[4];

artnet_node_t LAN_node;

extern uint16_t swapBytes16(uint16_t);

extern EthernetInterface* LAN_eth;
extern UDPSocket* LAN_sock;
extern artnet_packet_t* LAN_packet;

static void _dmx_cb(uint16_t port, uint8_t *data);

static uint32_t _get_serial_number();

extern "C" void mbed_mac_address(char *s) {
    char mac[6];
    uint32_t sn = _get_serial_number();
    mac[0] = 0x00;
    mac[1] = LAN_ESTA_CODE_HI;
    mac[2] = LAN_ESTA_CODE_LO;
    mac[3] = (char) ((sn & 0xff0000) >> 16);
    mac[4] = (char) ((sn & 0xff00) >> 8);
    mac[5] = (char) ((sn & 0xff) >> 0);
    memcpy(s, mac, 6);
}


void LAN_app_task(void) {
    int rtn;

    while (true) {
        rtn = 1;

        while (rtn != 0) {
            DEBUG_PRINTF("LAN_set_network\r\n");
            char ip_str[15];
            char nm_str[15];
            char gw_str[15];

            printf("NET DHC %d ", WDY_STATE.config.network.dhcp);
            printf("NET STA %d ", WDY_STATE.config.network.static_addr);
            printf("NET IPA %x %s ", WDY_STATE.config.network.ip_addr.s_addr, inet_ntoa(WDY_STATE.config.network.ip_addr));
            printf("NET NMA %x %s ", WDY_STATE.config.network.nm_addr.s_addr, inet_ntoa(WDY_STATE.config.network.nm_addr));
            printf("NET GWA %x %s\r\n", WDY_STATE.config.network.gw_addr.s_addr, inet_ntoa(WDY_STATE.config.network.gw_addr));
            printf("ART NET %d ", WDY_STATE.config.artnet.net);
            printf("ART SUB %d ", WDY_STATE.config.artnet.subnet);
            printf("ART UNI %d ", WDY_STATE.config.artnet.universe);
            printf("ART DMX %d\r\n", WDY_STATE.config.artnet.dmx_addr);
            printf("SCR CTR %d ", WDY_STATE.config.screen.contrast);
            printf("SCR BCK %d\r\n", WDY_STATE.config.screen.backlight);

            DEBUG_PRINTF("MAC: %s\r\n", LAN_eth->get_mac_address());
            unsigned int values[6];
            if( 6 == sscanf(LAN_eth->get_mac_address(), "%x:%x:%x:%x:%x:%x",
                        &values[0], &values[1], &values[2],
                        &values[3], &values[4], &values[5] ) ) {
                /* convert to uint8_t */
                for(uint8_t i = 0; i < 6; ++i )
                    mac_addr[i] = (uint8_t) values[i];
            }

            LAN_eth->set_dhcp(WDY_STATE.config.network.dhcp);
            if (!WDY_STATE.config.network.dhcp) {
                if (!WDY_STATE.config.network.static_addr) { // Auto generated network configuration
                    WDY_STATE.config.network.ip_addr.s_addr = (mac_addr[5] << 24) + (mac_addr[4] << 16) + ((mac_addr[3] + OEM_LO) << 8) + 0x02;
                    WDY_STATE.config.network.nm_addr.s_addr = 0x000000FF;
                    WDY_STATE.config.network.gw_addr.s_addr = 0x00000000;
                }
                strncpy(ip_str, inet_ntoa(WDY_STATE.config.network.ip_addr), sizeof(ip_str));
                strncpy(nm_str, inet_ntoa(WDY_STATE.config.network.nm_addr), sizeof(nm_str));
                strncpy(gw_str, inet_ntoa(WDY_STATE.config.network.gw_addr), sizeof(gw_str));

                DEBUG_PRINTF("IP: %s\r\n", ip_str);
                DEBUG_PRINTF("NM: %s\r\n", nm_str);
                rtn = LAN_eth->set_network(ip_str, nm_str, gw_str);
            } else {
                rtn = 0;
            }

            if (rtn != 0) {
                DEBUG_PRINTF("Failed to set network: %d\r\n", rtn);
                Thread::wait(10000);
                // artnet_error("Failed to set network: %d", rtn);
                continue;
            }

            DEBUG_PRINTF("Connecting\r\n");
            rtn = LAN_eth->connect();

            if (rtn != 0) {
                DEBUG_PRINTF("Failed to connect: %d\r\n", rtn);
                Thread::wait(10000);
                // artnet_error("Failed to set network: %d", rtn);
                continue;
            }

            LAN_sock = new UDPSocket();

            DEBUG_PRINTF("LAN_eth: %s %s\r\n", LAN_eth->get_ip_address(), LAN_eth->get_netmask());
            inet_aton(LAN_eth->get_ip_address(), &WDY_STATE.config.network.ip_addr);
            inet_aton(LAN_eth->get_netmask(), &WDY_STATE.config.network.nm_addr);

            bc_addr.s_addr = (uint32_t) WDY_STATE.config.network.ip_addr.s_addr & (uint32_t) WDY_STATE.config.network.nm_addr.s_addr;
            bc_addr.s_addr |= (uint32_t) 0xffffffff & (uint32_t) ~WDY_STATE.config.network.nm_addr.s_addr;

            DEBUG_PRINTF("LAN_eth: %s", inet_ntoa(WDY_STATE.config.network.ip_addr));
            DEBUG_PRINTF(" %s", inet_ntoa(WDY_STATE.config.network.nm_addr));
            DEBUG_PRINTF(" %s\r\n", inet_ntoa(bc_addr));

            rtn = LAN_sock->open(LAN_eth);
            if (rtn != 0) {
                // artnet_error(&LAN_node, "Could not create LAN_socket: %d", nsapi_rtn);
                DEBUG_PRINTF("Could not create LAN_socket: %d\r\n", rtn);
                Thread::wait(10000);
                continue;
            }


            // allow bcasting
            if(LAN_sock->set_broadcast(true) != 0) {
                // artnet_error(&LAN_node, "Failed to bind to LAN_socket %s", artnet_net_last_error());
                // artnet_net_close(&LAN_sock);
                continue;
            }

            LAN_sock->set_blocking(false);

            // allow reusing 6454 port _
            // Not possible with UDPSocket ?
            uint8_t value = 1;
            rtn = LAN_sock->setsockopt((int)NSAPI_SOCKET, (int)NSAPI_REUSEADDR, &value, sizeof(int));
            if (rtn != 0) {
                // artnet_error(&LAN_node, "Failed to bind to LAN_socket %s", artnet_net_last_error());
                // artnet_net_close(&LAN_sock);
                continue;
            }

            // bind LAN_sockets
            rtn = LAN_sock->bind(ARTNET_PORT);
            if (rtn != 0) {
                // artnet_error(&LAN_node, "Failed to bind to LAN_socket: %d %s", ret, artnet_net_last_error());
                // artnet_net_close(&LAN_sock);
                continue;
            }

            DEBUG_PRINTF("LAN_socket done.\r\n");

            DEBUG_PRINTF("LAN_init.\r\n");
            rtn = LAN_init(&LAN_node);

            LAN_set_network(&LAN_node,
                    WDY_STATE.config.network.ip_addr,
                    bc_addr,
                    WDY_STATE.config.network.gw_addr,
                    WDY_STATE.config.network.nm_addr,
                    mac_addr);

            LAN_set_port(&LAN_node, WDY_STATE.config.artnet.net, (WDY_STATE.config.artnet.subnet << 4) + WDY_STATE.config.artnet.universe);
            LAN_set_dmx(&LAN_node, WDY_STATE.config.artnet.dmx_addr, WDY_DMX_FOOTPRINT);
            LAN_set_dmx_callback(&LAN_node, &_dmx_cb);
            LAN_set_name(&LAN_node, WDY_SHORT_NAME, WDY_LONG_NAME);
            LAN_set_esta(&LAN_node, LAN_ESTA_CODE_LO, LAN_ESTA_CODE_HI);
            LAN_set_oem(&LAN_node, LAN_OEM_CODE_LO, LAN_OEM_CODE_HI);

            LAN_announce(&LAN_node);
        }

        while (true) {
            int read_rtn = 0;

            read_rtn = LAN_read(&LAN_node, LAN_packet);
            if (read_rtn > 0) {
                DEBUG_PRINTF("%d\r\n", read_rtn);
            }

            Thread::wait(10);
        }

        Thread::wait(50);
    }
}

void _dmx_cb(uint16_t port, uint8_t *dmx_data) {
    memcpy(&DMXdevice.data, dmx_data, WDY_DMX_FOOTPRINT);
    DMXdevice.parameter.speed = swapBytes16(DMXdevice.parameter.speed);
    DMXdevice.parameter.position = swapBytes16(DMXdevice.parameter.position);

    if (memcmp(&_lastDMXdevice.data, &DMXdevice.data, WDY_DMX_FOOTPRINT)) {
        memcpy(&_lastDMXdevice.data, &DMXdevice.data, WDY_DMX_FOOTPRINT);
        new_dmx_sig = true;
    }
}

#define IAP_LOCATION 0x1FFF1FF1

typedef void (*IAP)(unsigned long [], unsigned long[] );
IAP iap_entry = (IAP) IAP_LOCATION;

static uint32_t _get_serial_number() {
    unsigned long command[5] = {0,0,0,0,0};
    unsigned long result[5] = {0,0,0,0,0};

    // See User Manual section 32.8.7
    command[0] = 58;  // read device serial number
    iap_entry(command, result);
    if (result[0] == 0) {
        return result[1] ^ result[2] ^ result[3] ^ result[4];
    }

    return 0;
}
