
#include "main.h"
#include "mbed_stats.h"
#include "mbed_mem_trace.h"

extern "C" {
#include "CO_OD.h"
}

#define DMX_START       4
#define DMX_FOOTPRINT   4

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

int main(void) {
    mbed_stats_heap_t heap_stats;
    mbed_mem_trace_set_callback(mbed_mem_trace_default_callback);

    LAN_eth = &_eth;
    LAN_packet = &_packet;

    USBport.baud(115200);

    ticker_1ms.attach_us(&CO_timer1ms_task, 1000);
    ticker_leds.attach(&CO_leds_task, 0.01);

    int rtn;
    CO_NMT_reset_cmd_t reset;

    wdog.kick(10); // First watchdog kick to trigger it

    // USBport.printf("|=====================|\r\n");
    // USBport.printf("|    Winch Dynamic    |\r\n");
    // USBport.printf("|    by ExMachina     |\r\n");
    // USBport.printf("|=====================|\r\n");
    // USBport.printf("\r\n");

    // Status led setup
    spd_led.period(0.02);
    //pos_led.period(0.02);


    // Artnet init
    can_led = 1;
    USBport.printf("\r\n");
    USBport.printf("\r\n");

    USBport.printf("LAN_thread\r\n");
    LAN_app_thread.start(LAN_app_task);
    USBport.printf(".\r\n");

    USBport.printf("\r\n");

    // CANopen init
    mbed_stats_heap_get(&heap_stats);
    printf("Current heap: %lu\r\n", heap_stats.current_size);
    printf("Max heap size: %lu\r\n", heap_stats.max_size);

    reset = CO_RESET_NOT;

    while(reset != CO_RESET_APP){
        // CANopen communication reset - initialize CANopen objects
        CO_ReturnError_t err;
        uint16_t timer1msPrevious;
        uint16_t timer1msCopy, timer1msDiff;

        wdog.kick(); // Kick the watchdog

        // initialize CANopen
        USBport.printf("malloc\r\n");
        err = CO_init(
                (int32_t)0,             // CAN module address
                (uint8_t)CO_LPC_NODEID,    // NodeID
                (uint16_t)CO_BUS_BITRATE); // bit rate

        if(err != CO_ERROR_NO){
            //err_led = 1;
            mbed_stats_heap_get(&heap_stats);
            printf("Current heap: %lu\r\n", heap_stats.current_size);
            printf("Max heap size: %lu\r\n", heap_stats.max_size);

            USBport.printf(" error: %d\r\n", err);
            USBport.printf(" size of OD_RAM: %d\r\n", sizeof(sCO_OD_RAM));
            USBport.printf(" size of OD_ROM: %d\r\n", sizeof(sCO_OD_ROM));
            USBport.printf(" size of OD_EEPROM: %d\r\n", sizeof(sCO_OD_EEPROM));
            while(1);
                CO_errorReport(CO->em, CO_EM_MEMORY_ALLOCATION_ERROR, CO_EMC_SOFTWARE_INTERNAL, err);
        }
        USBport.printf(". \r\n");

        mbed_stats_heap_get(&heap_stats);
        printf("Current heap: %lu\r\n", heap_stats.current_size);
        printf("Max heap size: %lu\r\n", heap_stats.max_size);

        CO_CANsetNormalMode(CO->CANmodule[0]);

        // Configure CAN transmit and receive interrupt

        USBport.printf("attach");
        CANport->attach(&CO_CANInterruptHandler, CAN::RxIrq);
        CANport->attach(&CO_CANInterruptHandler, CAN::TxIrq);
        USBport.printf(".\r\n");

        mbed_stats_heap_get(&heap_stats);
        printf("Current heap: %lu\r\n", heap_stats.current_size);
        printf("Max heap size: %lu\r\n", heap_stats.max_size);

        // Start CAN
        ticker_sync.attach_us(&CO_sync_task, 1000);     // Only start CO_sync after CO_init !

        USBport.printf("CO_app_thread");
        CO_app_thread.start(CO_app_task);
        USBport.printf(". \r\n");
        mbed_stats_heap_get(&heap_stats);
        printf("Current heap: %lu\r\n", heap_stats.current_size);
        printf("Max heap size: %lu\r\n", heap_stats.max_size);


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

    while (true) {

        if(CO != NULL && CO->CANmodule[0]->CANnormal) {
            uint32_t    abortCode=0;
            uint32_t    readSize;
            MFEnode_t   node;

#if (CO_LPC_NODEID == 1) // If the LPC is the master node
            while (err != 0) {
                CO->NMT->operatingState = CO_NMT_PRE_OPERATIONAL;

                CO_sendNMTcommand(CO, CO_NMT_RESET_NODE, CO_DRV_NODEID);

                Thread::wait(5000);

                err = MFE_scan(CO_DRV_NODEID, &node, 100);
                if (err) continue;

                err = MFE_connect(&node, 100);

                CO->NMT->operatingState = CO_NMT_OPERATIONAL;
            }

            abortCode = 0;
            readSize = 0;
            err = CO_SDO_read(CO_DRV_NODEID, 0x3f00, 0x01, dataBuf, sizeof(dataBuf), &abortCode, &readSize, 100);
            if (readSize == 4) {
                bfloat _temp = { .bytes = { dataBuf[0], dataBuf[1], dataBuf[2], dataBuf[3] << 0 }};

                USBport.printf("READ 3f00 1: %f\r\n", _temp.to_float);
                _temp.to_float += 10.0;
                err = CO_SDO_write(CO_DRV_NODEID, 0x3f00, 0x02, _temp.bytes, 4, &abortCode, 100);
                USBport.printf("WRITE 3f80 2: %d %x\r\n", err, swapBytes(abortCode));
            }
#endif
        }
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
                ip_addr.s_addr = 0x280080C0;
                nm_addr.s_addr = 0x00FFFFFF;
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
                // artnet_error("Failed to set network: %d", rtn);
                continue;
            }

            printf("Connecting\r\n");
            rtn = LAN_eth->connect();

            if (rtn != 0) {
                printf("Failed to connect: %d\r\n", rtn);
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

            // LAN_sock->set_blocking(false);

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
            LAN_set_dmx(&LAN_node, 0, 4);
            LAN_set_dmx_callback(&LAN_node, &_dmx_cb);
            LAN_set_name(&LAN_node, WDY_SHORT_NAME, WDY_LONG_NAME);

            LAN_announce(&LAN_node);
        }

        while (true) {
            int read_rtn = 0;

            read_rtn = LAN_read(&LAN_node, LAN_packet);
            if (read_rtn > 0) {
                printf("%d\r\n", read_rtn);
            } else if (read_rtn < 0) {
                printf("!%d\r\n", read_rtn);
            }

            spd_led = !spd_led;
            Thread::wait(1);
        }

        Thread::wait(50);
    }
}

void _dmx_cb(uint16_t port, uint8_t *dmx_data) {
    spd_led = (dmx_data[0] << 8) + dmx_data[1];
}
