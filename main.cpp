
#include "main.h"
#include "mbed_stats.h"
#include "mbed_mem_trace.h"

extern "C" {
#include "CO_OD.h"
}

#define DMX_START       0
#define DMX_FOOTPRINT   5

#define ARTNET_DEBUG

// Global variables and objects

Watchdog wdog;

volatile uint16_t   CO_timer1ms = 0U;   // variable increments each millisecond

extern "C" void mbed_reset();

EthernetInterface* LAN_eth = NULL;
EthernetInterface _eth;

UDPSocket* LAN_sock = NULL;

artnet_packet_t* LAN_packet = NULL;
artnet_packet_t _packet;

uint8_t mac_addr[6] = {0x00, 0x02, 0xf7, 0xf2, 0xa8, 0x30};
in_addr ip_addr;
in_addr nm_addr;
in_addr gw_addr;
in_addr bc_addr;
bool dhcp = false;

uint8_t subnet[2];
uint8_t swin[4];
uint8_t swout[4];

artnet_node_t LAN_node;

Ticker ticker_1ms;
Ticker ticker_sync;
Ticker ticker_leds;

Thread CO_app_thread;
Thread LAN_app_thread;

Serial USBport(USBTX, USBRX);

dmx_device_union_t DMXdevice;
dmx_device_union_t _lastDMXdevice;
bool new_command_sig = false;

int main(void) {
#if MBED_CONF_APP_MEMTRACE
    mbed_stats_heap_t heap_stats;
    mbed_mem_trace_set_callback(mbed_mem_trace_default_callback);
#endif

    LAN_eth = &_eth;
    LAN_packet = &_packet;

    USBport.baud(115200);

    ticker_1ms.attach_us(&CO_timer1ms_task, 1000);
    ticker_leds.attach(&CO_leds_task, 0.01);

    CO_NMT_reset_cmd_t reset;

    wdog.kick(10); // First watchdog kick to trigger it

    printf("|=====================|\r\n");
    printf("|    Winch Dynamic    |\r\n");
    printf("|    by ExMachina     |\r\n");
    printf("|=====================|\r\n");
    printf("\r\n");

    // Status led setup
    spd_led.period(0.02);
    //pos_led.period(0.02);


    // Artnet init
    can_led = 1;
    printf("\r\n");
    printf("\r\n");

    printf("LAN_thread");
    LAN_app_thread.start(LAN_app_task);
    printf(".\r\n");

    printf("\r\n");

#if MBED_CONF_APP_MEMTRACE
    mbed_stats_heap_get(&heap_stats);
    printf("Current heap: %lu\r\n", heap_stats.current_size);
    printf("Max heap size: %lu\r\n", heap_stats.max_size);
#endif

    // CANopen init
    reset = CO_RESET_NOT;

    while(reset != CO_RESET_APP){
        // CANopen communication reset - initialize CANopen objects
        CO_ReturnError_t err;
        uint16_t timer1msPrevious;
        uint16_t timer1msCopy, timer1msDiff;

        wdog.kick(); // Kick the watchdog

        // initialize CANopen
        printf("malloc");
        err = CO_init(
                (int32_t)0,             // CAN module address
                (uint8_t)CO_NODEID,    // NodeID
                (uint16_t)CO_BUS_BITRATE); // bit rate

        if(err != CO_ERROR_NO){
            err_led = 1;
#if MBED_CONF_APP_MEMTRACE
            mbed_stats_heap_get(&heap_stats);
            printf("Current heap: %lu\r\n", heap_stats.current_size);
            printf("Max heap size: %lu\r\n", heap_stats.max_size);
#endif

            printf(" error: %d\r\n", err);
            printf(" size of OD_RAM: %d\r\n", sizeof(sCO_OD_RAM));
            printf(" size of OD_ROM: %d\r\n", sizeof(sCO_OD_ROM));
            printf(" size of OD_EEPROM: %d\r\n", sizeof(sCO_OD_EEPROM));
            while(1);
                CO_errorReport(CO->em, CO_EM_MEMORY_ALLOCATION_ERROR, CO_EMC_SOFTWARE_INTERNAL, err);
        }
        USBport.printf(". \r\n");

#if MBED_CONF_APP_MEMTRACE
        mbed_stats_heap_get(&heap_stats);
        printf("Current heap: %lu\r\n", heap_stats.current_size);
        printf("Max heap size: %lu\r\n", heap_stats.max_size);
#endif

        CO_CANsetNormalMode(CO->CANmodule[0]);

        // Configure CAN transmit and receive interrupt

        USBport.printf("CO_attach");
        CANport->attach(&CO_CANInterruptHandler, CAN::RxIrq);
        CANport->attach(&CO_CANInterruptHandler, CAN::TxIrq);
        USBport.printf(".\r\n");

#if MBED_CONF_APP_MEMTRACE
        mbed_stats_heap_get(&heap_stats);
        printf("Current heap: %lu\r\n", heap_stats.current_size);
        printf("Max heap size: %lu\r\n", heap_stats.max_size);
#endif

        // Start CAN
        ticker_sync.attach_us(&CO_sync_task, 1000);     // Only start CO_sync after CO_init !

        USBport.printf("CO_app_thread");
        CO_app_thread.start(CO_app_task);
        USBport.printf(". \r\n");

#if MBED_CONF_APP_MEMTRACE
        mbed_stats_heap_get(&heap_stats);
        printf("Current heap: %lu\r\n", heap_stats.current_size);
        printf("Max heap size: %lu\r\n", heap_stats.max_size);
#endif


        can_led = 0;
        //err_led = 0;

        reset = CO_RESET_NOT;
        timer1msPrevious = CO_timer1ms;

        wdog.kick(5); // Change watchdog timeout

        USBport.printf("done.\r\n\r\n");

        while(reset == CO_RESET_NOT) {
            // loop for normal program execution

            wdog.kick(); // Kick the watchdog

            timer1msCopy = CO_timer1ms;
            timer1msDiff = timer1msCopy - timer1msPrevious;
            timer1msPrevious = timer1msCopy;


            // CANopen process
            reset = CO_process(CO, timer1msDiff, NULL);

            // Nonblocking application code may go here.

            Thread::wait(1);
        }
    }

    // Program exit

    // Flash all leds
    can_led = 1;
    //err_led = 1;
    spd_led = 1;
    //pos_led = 1;

    // Terminate threads
    // CO_tmr_thread.terminate();
    // CO_app_thread.terminate();


    // Delete objects from memory
    CO_delete(0); // CAN module address

    // Reset
    mbed_reset();

    return 1;
}

// Application thread
static void CO_app_task(void){
    uint8_t err = 1;
    uint8_t dataBuf[CO_SDO_BUFFER_SIZE];
    bdata_t netdataBuf;

    while (true) {

        if(CO != NULL && CO->CANmodule[0]->CANnormal) {
            uint32_t    abortCode=0;
            uint32_t    readSize;
            MFEnode_t   node;

#if (CO_NODEID == 1) // If the LPC is the master node
            while (err != 0) {
                printf("Connecting to drive\r\n");
                CO->NMT->operatingState = CO_NMT_PRE_OPERATIONAL;

                CO_sendNMTcommand(CO, CO_NMT_RESET_NODE, CO_DRV_NODEID);

                Thread::wait(5000);

                err = MFE_scan(CO_DRV_NODEID, &node, 100);
                if (err) continue;

                err = MFE_connect(&node, 100);
                printf("drive: %d\r\n", err);

                CO->NMT->operatingState = CO_NMT_OPERATIONAL;
            }

            abortCode = 0;
            readSize = 0;
            err = CO_SDO_read(CO_DRV_NODEID, 0x3f00, 0x15, dataBuf, sizeof(dataBuf), &abortCode, &readSize, 100);

            if (readSize == 4 && false) {
                bfloat _temp = { .bytes = { dataBuf[0], dataBuf[1], dataBuf[2], dataBuf[3] << 0 }};

                USBport.printf("READ 3f00 1: %f\r\n", _temp.to_float);
                _temp.to_float += 10.0;
                err = CO_SDO_write(CO_DRV_NODEID, 0x3f00, 0x16, _temp.bytes, 4, &abortCode, 100);
                USBport.printf("WRITE 3f80 2: %d %x\r\n", err, swapBytes32(abortCode));
            // } else {
            //     USBport.printf("READ 3f80 2: %d %x\r\n", err, swapBytes32(abortCode));
            }

            if (new_command_sig) {
                bint _spd = { .to_integer = DMXdevice.parameter.speed };
                bint _pos = { .to_integer = DMXdevice.parameter.position };
                err = CO_SDO_write(CO_DRV_NODEID, 0x3f00, 0x04, _spd.bytes, 4, &abortCode, 100);
                err = CO_SDO_write(CO_DRV_NODEID, 0x3f00, 0x05, _pos.bytes, 4, &abortCode, 100);
                USBport.printf("write DMX values: %d %d %x\r\n", _spd.to_integer, err, swapBytes32(abortCode));
                new_command_sig = false;
            };
#endif
        }

        Thread::wait(5);
    }

}

// Sync task
static void CO_sync_task(void){
    if(CO != NULL && CO->CANmodule[0]->CANnormal) {
        bool_t syncWas;

        // Process Sync and read inputs
        syncWas = CO_process_SYNC_RPDO(CO, TMR_TASK_INTERVAL);

        // Further I/O or nonblocking application code may go here.


        // Write outputs
        CO_process_TPDO(CO, syncWas, TMR_TASK_INTERVAL);

        // verify timer overflow
        if(0) {
            CO_errorReport(CO->em, CO_EM_ISR_TIMER_OVERFLOW, CO_EMC_SOFTWARE_INTERNAL, 0U);
        }
    }
}

// Timer update task
static void CO_timer1ms_task(void) {
    CO_timer1ms++;
}

// Led update task
static void CO_leds_task(void) {
    can_led = LED_GREEN_RUN(CO->NMT) ? true : false;
    //err_led = LED_RED_ERROR(CO->NMT) ? true : false;
}

// CAN interrupt function
void CO_CANInterruptHandler(void){
    CO_CANinterrupt(CO->CANmodule[0]);
}

static void LAN_app_task(void) {
    int rtn;

    while (true) {
        rtn = 1;

        while (rtn != 0) {
            printf("LAN_set_network\r\n");
            char ip_str[14];
            char nm_str[14];
            char gw_str[14];

            LAN_eth->set_dhcp(dhcp);
            if (!dhcp) {
                printf("MAC: %s\r\n", LAN_eth->get_mac_address());
                unsigned int values[6];
                uint8_t bytes[6];
                if( 6 == sscanf(LAN_eth->get_mac_address(), "%x:%x:%x:%x:%x:%x",
                            &values[0], &values[1], &values[2],
                            &values[3], &values[4], &values[5] ) )
                {
                    /* convert to uint8_t */
                    for(uint8_t i = 0; i < 6; ++i )
                        bytes[i] = (uint8_t) values[i];
                }

                ip_addr.s_addr = (bytes[5] << 24) + (bytes[4] << 16) + ((bytes[3] + OEM_LO) << 8) + 0x02;
                nm_addr.s_addr = 0x000000FF;
                gw_addr.s_addr = 0x00000000;
                strncpy(ip_str, inet_ntoa(ip_addr), sizeof(ip_str));
                strncpy(nm_str, inet_ntoa(nm_addr), sizeof(nm_str));
                strncpy(gw_str, inet_ntoa(gw_addr), sizeof(gw_str));

                printf("IP: %s\r\n", ip_str);
                printf("NM: %s\r\n", nm_str);
                rtn = LAN_eth->set_network(ip_str, nm_str, gw_str);
            } else {
                rtn = 0;
            }

            if (rtn != 0) {
                printf("Failed to set network: %d\r\n", rtn);
                Thread::wait(10000);
                // artnet_error("Failed to set network: %d", rtn);
                continue;
            }

            printf("Connecting\r\n");
            rtn = LAN_eth->connect();

            if (rtn != 0) {
                printf("Failed to connect: %d\r\n", rtn);
                Thread::wait(10000);
                // artnet_error("Failed to set network: %d", rtn);
                continue;
            }

            LAN_sock = new UDPSocket();

            printf("LAN_eth: %s %s\r\n", LAN_eth->get_ip_address(), LAN_eth->get_netmask());
            inet_aton(LAN_eth->get_ip_address(), &ip_addr);
            inet_aton(LAN_eth->get_netmask(), &nm_addr);

            bc_addr.s_addr = (uint32_t) ip_addr.s_addr & (uint32_t) nm_addr.s_addr;
            bc_addr.s_addr |= (uint32_t) 0xffffffff & (uint32_t) ~nm_addr.s_addr;

            printf("LAN_eth: %s", inet_ntoa(ip_addr));
            printf(" %s", inet_ntoa(nm_addr));
            printf(" %s\r\n", inet_ntoa(bc_addr));

            rtn = LAN_sock->open(LAN_eth);
            if (rtn != 0) {
                // artnet_error(&LAN_node, "Could not create LAN_socket: %d", nsapi_rtn);
                printf("Could not create LAN_socket: %d\r\n", rtn);
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

            printf("LAN_socket done.\r\n");

            printf("LAN_init.\r\n");
            rtn = LAN_init(&LAN_node);

            LAN_set_network(&LAN_node, ip_addr, bc_addr, gw_addr, nm_addr, mac_addr);
            LAN_set_port(&LAN_node, 0, 0);
            LAN_set_dmx(&LAN_node, DMX_START, DMX_FOOTPRINT);
            LAN_set_dmx_callback(&LAN_node, &_dmx_cb);
            LAN_set_name(&LAN_node, WDY_SHORT_NAME, WDY_LONG_NAME);

            LAN_announce(&LAN_node);
        }

        while (true) {
            int read_rtn = 0;

            read_rtn = LAN_read(&LAN_node, LAN_packet);
            if (read_rtn > 0) {
                printf("%d\r\n", read_rtn);
            }

            Thread::wait(1);
        }

        Thread::wait(50);
    }
}

void _dmx_cb(uint16_t port, uint8_t *dmx_data) {
    memcpy(&_lastDMXdevice.data, &DMXdevice.data, DMX_FOOTPRINT);

    memcpy(&DMXdevice.data, dmx_data, DMX_FOOTPRINT);
    DMXdevice.parameter.speed = swapBytes16(DMXdevice.parameter.speed);
    DMXdevice.parameter.position = swapBytes16(DMXdevice.parameter.position);
    spd_led = (float)DMXdevice.parameter.speed / 0xFFFF;
    pos_led = (float)DMXdevice.parameter.position / 0xFFFF;

    if (_lastDMXdevice.parameter.speed != DMXdevice.parameter.speed ||
            _lastDMXdevice.parameter.position != DMXdevice.parameter.position ||
            _lastDMXdevice.parameter.command != DMXdevice.parameter.command) {
        new_command_sig = true;
    }
}
