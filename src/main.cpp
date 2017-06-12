
#include "main.h"

#ifdef MBED_CONF_APP_MEMTRACE
#include "mbed_stats.h"
#include "mbed_mem_trace.h"
#endif

extern "C" {
#include "CO_OD.h"
}

#define DMX_START       0

#define ARTNET_DEBUG

// Global variables and objects

Watchdog wdog;

// CANopen timer and state
Timer               CO_timer;
CO_NMT_reset_cmd_t  reset;

extern "C" void mbed_reset();

Ticker ticker_sync;

Thread CO_app_thread;


// ArtNet
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

Thread LAN_app_thread(osPriorityAboveNormal, 1024 * 2);

//
// DMX device
dmx_device_union_t DMXdevice;
dmx_device_union_t _lastDMXdevice;
bool new_dmx_sig = false;



Thread UI_app_thread(osPriorityBelowNormal, 1024);
Ticker ticker_leds;

volatile bool WDY_UI_MENU_FLAG;
volatile bool WDY_UI_DOWN_FLAG;
volatile bool WDY_UI_UP_FLAG;
volatile bool WDY_UI_ENTER_FLAG;


int main(void) {

    USBport.baud(115200);

#ifdef MBED_CONF_APP_MEMTRACE
    mbed_stats_heap_t heap_stats;
    mbed_mem_trace_set_callback(mbed_mem_trace_default_callback);
#endif

    for (int i=0; i<10; i++) {
        led1 = !led1;
        led2 = !led2;
        led3 = !led3;
        led4 = !led4;
        can_led = !can_led;
        err_led = !err_led;
        Thread::wait(100);
    }
    /*
    for (int i=0; i<100; i++) {
        if (!button3.read()) {
            led1 = !led1;
            led2 = !led2;
            led3 = !led3;
            led4 = !led4;
        }
        can_led = !can_led;
        err_led = !err_led;
        Thread::wait(100);
    }
    */

    UI_app_thread.start(UI_app_task);

    LAN_eth = &_eth;
    LAN_packet = &_packet;

    WDY_STATE.init_state = 5;

    CO_timer.start();
    ticker_leds.attach_us(&CO_leds_task, 10 * 1000); // 10 ms

    wdog.kick(10); // First watchdog kick to trigger it

    // Fans default speed
    fan_top = 1.0;
    fan_bot = 1.0;

    WDY_STATE.init_state = 10;

    Thread::wait(200);


    WDY_STATE.init_state = 22;

    Thread::wait(200);



    // Encoder init
    encoder.setVelocityFrequency(WDY_ENCODER_VFQ);
    encoder.setPPR(WDY_ENCODER_PPR);
    encoder.setLinearFactor(WDY_ENCODER_FACTOR);

    WDY_STATE.init_state = 31;

    Thread::wait(200);

    // Artnet init
    can_led = 1;

    LAN_app_thread.start(LAN_app_task);

    WDY_STATE.init_state = 42;

#ifdef MBED_CONF_APP_MEMTRACE
    mbed_stats_heap_get(&heap_stats);
    DEBUG_PRINTF("Current heap: %lu\r\n", heap_stats.current_size);
    DEBUG_PRINTF("Max heap size: %lu\r\n", heap_stats.max_size);
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
        DEBUG_PRINTF("malloc");
        err = CO_init(
                (int32_t)0,             // CAN module address
                (uint8_t)CO_NODEID,    // NodeID
                (uint16_t)CO_BUS_BITRATE); // bit rate

        if(err != CO_ERROR_NO){
            err_led = 1;
#ifdef MBED_CONF_APP_MEMTRACE
            mbed_stats_heap_get(&heap_stats);
            DEBUG_PRINTF("Current heap: %lu\r\n", heap_stats.current_size);
            DEBUG_PRINTF("Max heap size: %lu\r\n", heap_stats.max_size);
#endif

            DEBUG_PRINTF(" error: %d\r\n", err);
            DEBUG_PRINTF(" size of OD_RAM: %d\r\n", sizeof(sCO_OD_RAM));
            DEBUG_PRINTF(" size of OD_ROM: %d\r\n", sizeof(sCO_OD_ROM));
            DEBUG_PRINTF(" size of OD_EEPROM: %d\r\n", sizeof(sCO_OD_EEPROM));
            while(1);
                CO_errorReport(CO->em, CO_EM_MEMORY_ALLOCATION_ERROR, CO_EMC_SOFTWARE_INTERNAL, err);
        }
        DEBUG_PRINTF(". \r\n");

        WDY_STATE.init_state = 11;

#ifdef MBED_CONF_APP_MEMTRACE
        mbed_stats_heap_get(&heap_stats);
        DEBUG_PRINTF("Current heap: %lu\r\n", heap_stats.current_size);
        DEBUG_PRINTF("Max heap size: %lu\r\n", heap_stats.max_size);
#endif

        CO_CANsetNormalMode(CO->CANmodule[0]);

        // Configure CAN transmit and receive interrupt

        DEBUG_PRINTF("CO_attach");
        CANport->attach(&CO_CANInterruptHandler, CAN::RxIrq);
        CANport->attach(&CO_CANInterruptHandler, CAN::TxIrq);
        DEBUG_PRINTF(".\r\n");

        WDY_STATE.init_state = 38;

#ifdef MBED_CONF_APP_MEMTRACE
        mbed_stats_heap_get(&heap_stats);
        DEBUG_PRINTF("Current heap: %lu\r\n", heap_stats.current_size);
        DEBUG_PRINTF("Max heap size: %lu\r\n", heap_stats.max_size);
#endif

        // Start CAN
        ticker_sync.attach_us(&CO_sync_task, 1000);     // Only start CO_sync after CO_init !

        DEBUG_PRINTF("CO_app_thread");
        CO_app_thread.start(CO_app_task);
        DEBUG_PRINTF(". \r\n");

        WDY_STATE.init_state = 67;

#ifdef MBED_CONF_APP_MEMTRACE
        mbed_stats_heap_get(&heap_stats);
        DEBUG_PRINTF("Current heap: %lu\r\n", heap_stats.current_size);
        DEBUG_PRINTF("Max heap size: %lu\r\n", heap_stats.max_size);
#endif


        can_led = 0;
        //err_led = 0;

        reset = CO_RESET_NOT;
        timer1msPrevious = CO_timer.read_ms();

        wdog.kick(5); // Change watchdog timeout

        DEBUG_PRINTF("done.\r\n\r\n");

        WDY_STATE.init_state = 75;

        while(reset == CO_RESET_NOT) {
            // loop for normal program execution

            wdog.kick(); // Kick the watchdog

            timer1msCopy = CO_timer.read_ms();
            timer1msDiff = timer1msCopy - timer1msPrevious;
            timer1msPrevious = timer1msCopy;


            // CANopen process
            reset = CO_process(CO, timer1msDiff, NULL);
            if (reset != CO_RESET_NOT) {
                WDY_STATE.init_state = -1;
            }

            // Nonblocking application code may go here.

            Thread::wait(1);
        }
    }

    // Program exit

    // Flash all leds
    can_led = 1;
    //err_led = 1;
    led1 = 1;
    //led2 = 1;

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

    uint16_t timer1msCopy;
    uint16_t timerTempPrevious = 0;
    uint16_t timerTempDiff = 0;

    MFEnode_t   node;

    float enc_position = 0;
    float enc_speed = 0;
    float real_speed;
    float real_position;

    float cmd_position = 0;
    float cmd_speed = 0;
    float cmd_accel = 0;
    float cmd_decel = 0;
    wdy_command_t cmd_command = WDY_CMD_NONE;

    uint16_t status = WDY_STS_NONE;

    uint16_t homing_time = 0;

    float last_position = 0;
    float last_speed = 0;
    float last_accel = 0;
    float last_decel = 0;
    wdy_command_t last_command = WDY_CMD_NONE;

    bdata_t nd_spd;
    bdata_t nd_pos;
    bdata_t nd_cmd;
    bdata_t nd_sts;
    bdata_t nd_accel;
    bdata_t nd_decel;
    bdata_t nd_temp;
    bdata_t nd_mot_spd;
    bdata_t nd_mot_pos;

    bool new_cmd_sig = false;
    bool new_spd_sig = false;
    bool new_pos_sig = false;
    bool new_acc_sig = false;
    bool new_dec_sig = false;

    while (true) {
        Thread::wait(WDY_LOOP_INTERVAL); // wait before to ensure this delay is always repected

        if(CO != NULL && CO->CANmodule[0]->CANnormal) {

            while (err != 0) {
                DEBUG_PRINTF("Connecting to drive\r\n");
                WDY_STATE.init_state = 80;
                CO->NMT->operatingState = CO_NMT_PRE_OPERATIONAL;

                CO_ReturnError_t state = CO_sendNMTcommand(CO, CO_NMT_RESET_NODE, CO_DRV_NODEID);
                if (state == CO_ERROR_TX_OVERFLOW) {    // First powerup, MFE takes time to talk
                    reset = CO_RESET_COMM;
                    Thread::wait(1000);
                    continue;
                }

                WDY_STATE.init_state = 85;

                Thread::wait(500);

                err = MFE_scan(CO_DRV_NODEID, &node, 100);
                if (err) {
                    Thread::wait(500);
                    continue;
                }

                WDY_STATE.init_state = 90;
                Thread::wait(500);

                err = MFE_connect(&node, 100);
                printf("drive: %d\r\n", err);

                CO->NMT->operatingState = CO_NMT_OPERATIONAL;

                if (err == 0)
                    WDY_STATE.init_state = 100;

                Thread::wait(50);
            }

            timer1msCopy = CO_timer.read_ms();

            if (new_dmx_sig) {                          // New DMX data
                uint16_t raw_speed = DMXdevice.parameter.speed;
                uint16_t raw_position = DMXdevice.parameter.position;
                uint8_t raw_command = DMXdevice.parameter.command;
                uint8_t raw_accel = DMXdevice.parameter.accel;
                uint8_t raw_decel = DMXdevice.parameter.decel;

#if WDY_INVERT_POS_COMMAND
                raw_position = DMX_MAX_VALUE16 - raw_position;
#endif

                map_DMX16_to_world(raw_speed, &cmd_speed, WDY_MAX_SPEED);
                map_DMX16_to_world(raw_position, &cmd_position, WDY_MAX_POSITION);
                map_DMXcommand_to_command(raw_command, &cmd_command);

                map_DMX8_to_world(raw_accel, &cmd_accel, WDY_MAX_ACCEL);
                map_DMX8_to_world(raw_decel, &cmd_decel, WDY_MAX_DECEL);

                DEBUG_PRINTF("DMX cmd %d\t pos %d spd %d acc %d dec %d\r\n",
                      raw_command, raw_position, raw_speed, raw_accel, raw_decel);
                DEBUG_PRINTF("REAL cmd %d\t pos %f spd %f acc %f dec %f\r\n",
                      cmd_command, cmd_position, cmd_speed, cmd_accel, cmd_decel);

                new_dmx_sig = false;                    // Reset signal

                new_cmd_sig = (last_command != cmd_command);
                new_pos_sig = (last_position != cmd_position);
                new_spd_sig = (last_speed != cmd_speed);
                new_acc_sig = (last_accel != cmd_accel);
                new_dec_sig = (last_decel != cmd_decel);
            }


            // Update drive status
            if (!(status & WDY_STS_COMM_FAULT)) {
                err = MFE_get_status(&node, &nd_sts);
                if (err != 0) {
                    DEBUG_PRINTF("get sts: %d\r\n", err);
                    status |= WDY_STS_COMM_FAULT;
                    continue;
                } else {
                    status &= !WDY_STS_COMM_FAULT;
                }
            }


            if (new_cmd_sig) {
                DEBUG_PRINTF("STATUS cmd %d sts %d err %d\r\n", cmd_command, status, err);

                if (cmd_command == WDY_CMD_ENABLE) {
                    nd_cmd.to_int = MFE_CMD_ENABLE;
                    err = MFE_set_command(&node, &nd_cmd);

                    status = status | WDY_STS_ENABLED;
                } else if (cmd_command == WDY_CMD_HOME) {
                    nd_cmd.to_int = MFE_CMD_HOME;
                    err = MFE_set_command(&node, &nd_cmd);

                    status = status | WDY_STS_HOME_IN_PROGRESS;
                    status = status & !WDY_STS_HOMED;
                } else if (cmd_command == WDY_CMD_HOME_ENCODER) {
                    encoder.setHome(WDY_ENCODER_HOME_OFFSET);   // Reset encoder position
                } else {  // WDY_CMD_NONE
                    nd_cmd.to_int = MFE_CMD_NONE;
                    err = MFE_set_command(&node, &nd_cmd);
                }

                if (err != 0) {
                    status = status | WDY_STS_COMM_FAULT;
                    continue;
                }
            }

            if (new_acc_sig) {
                nd_accel.to_float = linear_to_rot(cmd_accel, length_to_drum_diameter(WDY_MAX_POSITION));
                err = MFE_set_accel(&node, &nd_accel);
            }

            if (new_dec_sig) {
                nd_decel.to_float = linear_to_rot(cmd_decel, length_to_drum_diameter(WDY_MAX_POSITION));
                err += MFE_set_decel(&node, &nd_decel);
            }


            // Control loop of speed and position regarding external encoder
            if (cmd_command == WDY_CMD_ENABLE && (status & WDY_STS_HOMED)) {     // Normal loop

                // First, get real position from encoder
                enc_position = encoder.getPosition();

                // Compute actual speed according to position
                nd_spd.to_float = linear_to_rot(cmd_speed, length_to_drum_diameter(enc_position));
                err = MFE_set_speed(&node, &nd_spd);
                if (err != 0) {
                    status |= WDY_STS_COMM_FAULT;
                    continue;
                }

                if (new_pos_sig || new_spd_sig) {                       // New position required
#ifdef WDY_GEARBOX_RATIO
                    nd_pos.to_float = length_to_drum_turns(cmd_position) * WDY_GEARBOX_RATIO; // Position: real to turns
#else
                    nd_pos.to_float = length_to_drum_turns(cmd_position); // Position: real to turns
#endif
                    err = MFE_set_position(&node, &nd_pos);

                    if (err != 0) {
                        status |= WDY_STS_COMM_FAULT;
                        continue;
                    }

                }

                DEBUG_PRINTF("MOT sts %d spd %f pos %f mspd %f mpos %f\r\n",
                       status, nd_spd.to_float, nd_pos.to_float, encoder.getSpeed(), enc_position);
            } else if (status & WDY_STS_HOME_IN_PROGRESS) {      // Homing in progress
                if (nd_sts.to_int & MFE_STS_HOME_IN_PROGRESS) {
                    status |= WDY_STS_HOME_IN_PROGRESS;
                    homing_time++;

                    DEBUG_PRINTF("HOM htime %d inprogress\r\n", homing_time);
                } else if (nd_sts.to_int & MFE_STS_HOMED) {
                    Thread::wait(100);

                    encoder.setHome(WDY_ENCODER_HOME_OFFSET);   // Once the drive is homed, reset encoder position

                    status |= WDY_STS_HOMED;
                    status &= !WDY_STS_HOME_IN_PROGRESS;
                    status &= !WDY_STS_HOME_TIMEOUT;

                    // Set defaults
                    nd_accel.to_float = linear_to_rot(
                            WDY_DEFAULT_ACCEL, length_to_drum_diameter(WDY_MAX_POSITION));
                    nd_decel.to_float = linear_to_rot(
                            WDY_DEFAULT_DECEL, length_to_drum_diameter(WDY_MAX_POSITION));
                    err = MFE_set_accel(&node, &nd_accel);
                    err += MFE_set_decel(&node, &nd_decel);

                    DEBUG_PRINTF("HOM htime %d done\r\n", homing_time);

                    homing_time = 0;
                } else if (homing_time > (WDY_MAX_HOMING_TIME / WDY_LOOP_INTERVAL)) {
                    DEBUG_PRINTF("HOM htime %d timeout\r\n", homing_time);

                    homing_time = 0;

                    if (nd_sts.to_int & MFE_STS_UNPOWERED) {
                        status |= WDY_STS_UNPOWERED;
                    }

                    status |= WDY_STS_HOME_TIMEOUT;
                } else if (nd_sts.to_int & MFE_STS_HOME_TIMEOUT) {
                    DEBUG_PRINTF("HOM htime %d timeout\r\n", homing_time);

                    status |= WDY_STS_HOME_TIMEOUT;
                }
            } else {
                enc_position = encoder.getPosition();
                enc_speed = encoder.getSpeed();
                DEBUG_PRINTF("ENC mspd %f mpos %f\r\n", enc_speed, enc_position);
            }

            // store commands
            last_position = cmd_position;
            last_speed = cmd_speed;
            last_command = cmd_command;
            last_accel = cmd_accel;
            last_decel = cmd_decel;

            // reset signals
            new_cmd_sig = false;
            new_pos_sig = false;
            new_spd_sig = false;
            new_acc_sig = false;
            new_dec_sig = false;

            // copy status to WDY_STATE.status
            WDY_STATE.status = status;

            timerTempDiff = timer1msCopy - timerTempPrevious;
            if (timerTempDiff >= 500) {     // Adjust temperature according to drive temperature
                err = MFE_get_temp(&node, &nd_temp);
                if (err == 0) {
                    bdata_t _temp;
                    memcpy(&_temp.bytes, &nd_temp.bytes, BDATA_SIZE);

                    fan_top = 0.35 + (float)((_temp.to_float / 50.0) * 0.65);
                    fan_bot = 0.35 + (float)((_temp.to_float / 50.0) * 0.65);
                    DEBUG_PRINTF("TEMP tmp %f f1 %f f2 %f\r\n", _temp.to_float, (float) fan_top, (float) fan_bot);
                } else {
                    fan_top = 1.0;
                    fan_bot = 1.0;
                }

                timerTempPrevious = timer1msCopy;
                led1 = !led1;
            }

            led2 = !led2;
        } else {
            DEBUG_PRINTF("Waiting for CAN.\r\n");
            Thread::wait(1000);
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

// Led update task
static void CO_leds_task(void) {
    can_led = LED_GREEN_RUN(CO->NMT) ? true : false;
    err_led = LED_RED_ERROR(CO->NMT) ? true : false;
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
            DEBUG_PRINTF("LAN_set_network\r\n");
            char ip_str[14];
            char nm_str[14];
            char gw_str[14];

            LAN_eth->set_dhcp(dhcp);
            if (!dhcp) {
                DEBUG_PRINTF("MAC: %s\r\n", LAN_eth->get_mac_address());
                unsigned int values[6];
                uint8_t bytes[6];
                if( 6 == sscanf(LAN_eth->get_mac_address(), "%x:%x:%x:%x:%x:%x",
                            &values[0], &values[1], &values[2],
                            &values[3], &values[4], &values[5] ) ) {
                    /* convert to uint8_t */
                    for(uint8_t i = 0; i < 6; ++i )
                        bytes[i] = (uint8_t) values[i];
                }

                WDY_STATE.config.network.ip_addr.s_addr = (bytes[5] << 24) + (bytes[4] << 16) + ((bytes[3] + OEM_LO) << 8) + 0x02;
                WDY_STATE.config.network.nm_addr.s_addr = 0x000000FF;
                WDY_STATE.config.network.gw_addr.s_addr = 0x00000000;
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

            LAN_set_network(&LAN_node, ip_addr, bc_addr, gw_addr, nm_addr, mac_addr);
            LAN_set_port(&LAN_node, WDY_STATE.config.artnet.net, (WDY_STATE.config.artnet.subnet << 4) + WDY_STATE.config.artnet.universe);
            LAN_set_dmx(&LAN_node, WDY_STATE.config.artnet.dmx_addr, DMX_FOOTPRINT);
            LAN_set_dmx_callback(&LAN_node, &_dmx_cb);
            LAN_set_name(&LAN_node, WDY_SHORT_NAME, WDY_LONG_NAME);

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
    memcpy(&DMXdevice.data, dmx_data, DMX_FOOTPRINT);
    DMXdevice.parameter.speed = swapBytes16(DMXdevice.parameter.speed);
    DMXdevice.parameter.position = swapBytes16(DMXdevice.parameter.position);

    if (memcmp(&_lastDMXdevice.data, &DMXdevice.data, DMX_FOOTPRINT)) {
        memcpy(&_lastDMXdevice.data, &DMXdevice.data, DMX_FOOTPRINT);
        new_dmx_sig = true;
    }
}

void printbar(int length, int perc) {
    for (int i=0; i<length; i++) {
        if (perc > (i * 100 / length)) {
            if (!i)
                lcd.putc(1);
            else if (i == length-1)
                lcd.putc(5);
            else
                lcd.putc(3);
        } else {
            if (!i)
                lcd.putc(0);
            else if (i == length-1)
                lcd.putc(4);
            else
                lcd.putc(2);
        }
    }
}

void menu_interrupt(void) {
    WDY_UI_MENU_FLAG = true;
}

void down_interrupt(void) {
    WDY_UI_DOWN_FLAG = true;
}

void up_interrupt(void) {
    WDY_UI_UP_FLAG = true;
}

void enter_interrupt(void) {
    WDY_UI_ENTER_FLAG = true;
}

static void UI_app_task(void) {
    bool _init = true;
    uint8_t period_counter = 0;

    // Status led setup
    led1.period(0.02);
    led2.period(0.02);
    led3.period(0.02);
    led4.period(0.02);

    button2.mode(PullUp);
    button3.mode(PullUp);
    button4.mode(PullUp);
    button5.mode(PullUp);

    button2.fall(&enter_interrupt);
    button3.fall(&down_interrupt);
    button4.fall(&up_interrupt);
    button5.fall(&menu_interrupt);

    // I2C bus speed: 400 kHz
    i2c2.frequency(400 * 1000);

    lcd.setBacklight(true);
    lcd.setContrast(0x0a);

    lcd.setUDC(0, UDC_bar_left_empty);
    lcd.setUDC(1, UDC_bar_left_full);
    lcd.setUDC(2, UDC_bar_middle_empty);
    lcd.setUDC(3, UDC_bar_middle_full);
    lcd.setUDC(4, UDC_bar_right_empty);
    lcd.setUDC(5, UDC_bar_right_full);

    lcd.printf("   Winch  Dynamic\n");
    lcd.printf("    by ExMachina");

    using namespace LCD_UI;

    Menu _root_menu("Main menu");

    Menu _config_menu("Config");

    Menu _network_menu("Network");
    _network_menu.addItem(&set_action_dhcp, NULL, "DHCP");
    _network_menu.addItem(NULL, NULL, "IP address");
    _network_menu.addItem(NULL, NULL, "Netmask");
    _network_menu.addItem(NULL, NULL, "Gateway");
    _network_menu.addItem(NULL, &_config_menu, "--Back--");
    _config_menu.addItem(NULL, &_network_menu, "Network");

    Menu _artnet_menu("ArtNET");
    _artnet_menu.addItem(&set_action_net, NULL, "Net");
    _artnet_menu.addItem(&set_action_subnet, NULL, "Subnet");
    _artnet_menu.addItem(&set_action_universe, NULL, "Universe");
    _artnet_menu.addItem(&set_action_dmx_addr, NULL, "DMX address");
    _artnet_menu.addItem(NULL, &_config_menu, "--Back--");
    _config_menu.addItem(NULL, &_artnet_menu, "ArtNET");

    Menu _screen_menu("Screen");
    _screen_menu.addItem(&set_action_contrast, NULL, "Contrast");
    _screen_menu.addItem(&set_action_backlight, NULL, "Backlight");
    _screen_menu.addItem(NULL, &_config_menu, "--Back--");
    _config_menu.addItem(NULL, &_screen_menu, "Screen");

    _config_menu.addItem(NULL, &_root_menu, "--Back--");

    _root_menu.addItem(&info_action, NULL, "Infos");
    _root_menu.addItem(NULL, &_config_menu, "Config");
    _root_menu.addItem(&about_action, NULL, "About");

    UI ui(&lcd, &_root_menu);

    while (true) {
        while (_init) {
            led1 = 1;
            led2 = 1;
            led3 = 1;
            led4 = 1;

            lcd.locate(3, 1);

            if (WDY_STATE.init_state < 0) {
                lcd.printf("                    ");
                lcd.locate(3, 1);
                lcd.printf("ERROR %d", WDY_STATE.init_state);
            } else if (WDY_STATE.init_state >= 100) {
                _init = false;
                printbar(18, 100);
                Thread::wait(500);
                lcd.clear();

                lcd.setUDC(0, UDC_arrow_left);
                lcd.setUDC(1, UDC_arrow_right);
                lcd.setUDC(2, UDC_arrow_up);
                lcd.setUDC(3, UDC_arrow_down);
                lcd.setUDC(4, UDC_icon_menu);
                lcd.setUDC(5, UDC_icon_ok);
                lcd.setUDC(6, UDC_icon_cancel);

                led1 = 0;
                led2 = 0;
                led3 = 0;
                led4 = 0;
            } else {
                printbar(18, WDY_STATE.init_state);
            }

            Thread::wait(50);
        }

        // Update keys base on flags
        if (WDY_UI_MENU_FLAG) {
            ui.setKey(KEY_MENU);
            WDY_UI_MENU_FLAG = false;
        } else if (WDY_UI_UP_FLAG) {
            ui.setKey(KEY_UP);
            WDY_UI_UP_FLAG = false;
        } else if (WDY_UI_DOWN_FLAG) {
            ui.setKey(KEY_DOWN);
            WDY_UI_DOWN_FLAG = false;
        } else if (WDY_UI_ENTER_FLAG) {
            ui.setKey(KEY_ENTER);
            WDY_UI_ENTER_FLAG = false;
        } else {
            ui.setKey(KEY_NONE);
        }

        ui.update();

        // Update leds
        if (WDY_STATE.status != 0) {
            ui.blink_code(&led3, WDY_STATE.status);
        } else {
            led3 = 0;
        }
        
        led1 = ((float) (period_counter % 10)) / 10;

        period_counter++;


        /*
        if (WDY_UI_MENU_FLAG || _display_menu) {
            if (!_display_menu) {
                _display_menu = true;
                WDY_UI_MENU_FLAG = false;
                lcd.clear();
            } else if (WDY_UI_MENU_FLAG && _display_menu) {
                _display_menu = false;
                WDY_UI_MENU_FLAG = false;
                continue;
            }


            ui.update();
        } else {
            lcd.locate(1, 0);
            lcd.printf("%s       ", inet_ntoa(ip_addr));
            lcd.locate(1, 17);
            lcd.printf("%d", DMX_START);
            lcd.locate(2, 0);
            lcd.printf("%s       ", inet_ntoa(nm_addr));
        }
        */

        Thread::wait(50);
    }
}
