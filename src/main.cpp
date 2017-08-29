
/*
 * main.cpp
 * Copyright (C) 2017 Benoit Rapidel <benoit.rapidel+devs@exmachina.fr>
 *
 * Distributed under terms of the MIT license.
 */

#include "main.h"

#ifdef MBED_CONF_APP_MEMTRACE
#include "mbed_stats.h"
#include "mbed_mem_trace.h"
#endif

extern "C" {
#include "CO_OD.h"
}

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

Thread LAN_app_thread(osPriorityAboveNormal, 1024 * 2);

Thread UI_app_thread(osPriorityBelowNormal, 1024);
Ticker ticker_leds;

/* WDY */
wdy_state_t WDY_STATE;


int main(void) {

    USBport.baud(115200);

#ifdef MBED_CONF_APP_MEMTRACE
    mbed_stats_heap_t heap_stats;
    mbed_mem_trace_set_callback(mbed_mem_trace_default_callback);
#endif

    UI_app_thread.start(UI_app_task);

    LAN_eth = &_eth;
    LAN_packet = &_packet;

    wdy_init_msg("Timer and fans");
    WDY_STATE.init_state = 5;

    CO_timer.start();
    ticker_leds.attach_us(&CO_leds_task, 10 * 1000); // 10 ms

    wdog.kick(10); // First watchdog kick to trigger it

    // Fans default speed
    fan_top.period_us(40); // 1 / 40 = 0.025 * 1000000 = 25kHz
    fan_bot.period_us(40);
    fan_top = 1.0;
    fan_bot = 1.0;

    wdy_init_msg("Reading config");
    WDY_STATE.init_state = 10;

    bool ret = WDY_eeprom_read_config(&eeprom, &eeprom_data);
    printf("RET %d", ret);
    printf(" STS %d", eeprom_data.eeprom_state);
    printf(" VER %d\r\n", eeprom_data.eeprom_version);
    printf("SOF CFG %d\r\n", sizeof(wdy_config_t));
    printf("SOF EEP %d\r\n", sizeof(wdy_eeprom_data_t));

    if (eeprom_data.eeprom_state == WDY_EEPROM_STATE_BLANK || eeprom_data.eeprom_state == 255) {
        WDY_eeprom_erase_config(&eeprom);

        eeprom_data.config.screen.backlight = 10;
        eeprom_data.config.screen.contrast = 0x0a;

        eeprom_data.config.network.dhcp = false;
        eeprom_data.config.network.static_addr = true;
        eeprom_data.config.network.ip_addr.s_addr = 0x02000002;
        eeprom_data.config.network.nm_addr.s_addr = 0x000000FF;
        eeprom_data.config.network.gw_addr.s_addr = 0;

        eeprom_data.config.artnet.net = 0;
        eeprom_data.config.artnet.subnet = 0;
        eeprom_data.config.artnet.universe = 0;
        eeprom_data.config.artnet.dmx_addr = 16;

        WDY_eeprom_write_config(&eeprom, &eeprom_data.config);

        ret = WDY_eeprom_read_config(&eeprom, &eeprom_data);
        printf("sNET DHC %d ", eeprom_data.config.network.dhcp);
        printf("sNET STA %d ", eeprom_data.config.network.static_addr);
        printf("sNET IPA %s ", inet_ntoa(eeprom_data.config.network.ip_addr));
        printf("sNET NMA %s ", inet_ntoa(eeprom_data.config.network.nm_addr));
        printf("sNET GWA %s\r\n", inet_ntoa(eeprom_data.config.network.gw_addr));
        printf("sART NET %d ", eeprom_data.config.artnet.net);
        printf("sART SUB %d ", eeprom_data.config.artnet.subnet);
        printf("sART UNI %d ", eeprom_data.config.artnet.universe);
        printf("sART DMX %d\r\n", eeprom_data.config.artnet.dmx_addr);
        printf("sSCR CTR %d ", eeprom_data.config.screen.contrast);
        printf("sSCR BCK %d\r\n", eeprom_data.config.screen.backlight);
    } else {
        printf("NET DHC %d ", eeprom_data.config.network.dhcp);
        printf("NET STA %d ", eeprom_data.config.network.static_addr);
        printf("NET IPA %s ", inet_ntoa(eeprom_data.config.network.ip_addr));
        printf("NET NMA %s ", inet_ntoa(eeprom_data.config.network.nm_addr));
        printf("NET GWA %s\r\n", inet_ntoa(eeprom_data.config.network.gw_addr));
        printf("ART NET %d ", eeprom_data.config.artnet.net);
        printf("ART SUB %d ", eeprom_data.config.artnet.subnet);
        printf("ART UNI %d ", eeprom_data.config.artnet.universe);
        printf("ART DMX %d\r\n", eeprom_data.config.artnet.dmx_addr);
        printf("SCR CTR %d ", eeprom_data.config.screen.contrast);
        printf("SCR BCK %d\r\n", eeprom_data.config.screen.backlight);

    }
    memcpy(&WDY_STATE.config, &eeprom_data.config, WDY_EEPROM_CONF_SIZE);

    wdy_init_msg("Encoder");
    WDY_STATE.init_state = 22;

    Thread::wait(200);



    // Encoder init
    encoder.setVelocityFrequency(WDY_ENCODER_VFQ);
    encoder.setPPR(WDY_ENCODER_PPR);
    encoder.setLinearFactor(WDY_ENCODER_FACTOR);

    wdy_init_msg("ArtNET");
    WDY_STATE.init_state = 31;

    Thread::wait(200);

    // Artnet init
    can_led = 1;

    LAN_app_thread.start(LAN_app_task);

    wdy_init_msg("CANopen");
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

        wdy_init_msg("Starting comm");
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

        wdy_init_msg("Starting CANopen");
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

        wdy_init_msg("CANopen ready");
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

        wdy_init_msg("CANopen started");
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
    err_led = 1;

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
    float enc_velocity = 0;

    float load;

    float cmd_position = 0;
    float cmd_speed = 0;
    float cmd_accel = 0;
    float cmd_decel = 0;
    wdy_command_t cmd_command = WDY_CMD_NONE;

    float vel_command_lin = 0;
    float vel_command_spl = 0;
    float p_term = 0;
    float i_term = 0;
    float d_term = 0;
    float following_error = 0;
    float following_error_sum = 0;
    float following_error_last = 0;

    uint16_t status = WDY_STS_NONE;

    uint16_t homing_time = 0;

    float last_position = 0;
    float last_speed = 0;
    float last_accel = 0;
    float last_decel = 0;
    wdy_command_t last_command = WDY_CMD_NONE;

    bdata_t nd_vel;
    bdata_t nd_pos;
    bdata_t nd_cmd;
    bdata_t nd_sts;
    bdata_t nd_accel;
    bdata_t nd_decel;
    bdata_t nd_temp;

    bool new_cmd_sig = false;
    bool new_mov_sig = false;

    WDY_motion::Planner planner;

    while (true) {
        Thread::wait(WDY_LOOP_INTERVAL); // wait before to ensure this delay is always repected

        if(CO != NULL && CO->CANmodule[0]->CANnormal) {

            /* Waiting for the drive to be ready */
            while (err != 0) {
                DEBUG_PRINTF("Connecting to VFD\r\n");
                wdy_init_msg("Connecting to VFD");
                WDY_STATE.init_state = 80;
                CO->NMT->operatingState = CO_NMT_PRE_OPERATIONAL;

                CO_ReturnError_t state = CO_sendNMTcommand(CO, CO_NMT_RESET_NODE, CO_DRV_NODEID);
                if (state == CO_ERROR_TX_OVERFLOW) {    // First powerup, MFE takes time to talk
                    reset = CO_RESET_COMM;
                    Thread::wait(1000);
                    continue;
                }

                Thread::wait(500);

                err = MFE_scan(CO_DRV_NODEID, &node, 100);
                if (err) {
                    wdy_init_msg("VFD not ready");
                    WDY_STATE.init_state = 83;

                    Thread::wait(500);
                    continue;
                }

                wdy_init_msg("VFD ready");
                WDY_STATE.init_state = 90;
                Thread::wait(500);

                err = MFE_connect(&node, 100);
                DEBUG_PRINTF("VFD connect: %d\r\n", err);

                CO->NMT->operatingState = CO_NMT_OPERATIONAL;

                if (err == 0) {
                    wdy_init_msg("Done.");
                    WDY_STATE.init_state = 100;
                }

                Thread::wait(50);
            }

            timer1msCopy = CO_timer.read_ms();

            /* New DMX data */
            if (new_dmx_sig) {
                uint16_t raw_speed = DMXdevice.parameter.speed;
                uint16_t raw_position = DMXdevice.parameter.position;
                uint8_t raw_command = DMXdevice.parameter.command;
                uint8_t raw_accel = DMXdevice.parameter.accel;
                uint8_t raw_decel = DMXdevice.parameter.decel;

                // Reset signal
                new_dmx_sig = false;

#if WDY_INVERT_POS_COMMAND
                raw_position = DMX_MAX_VALUE16 - raw_position;
#endif

                // Conversion between n * 8 bit data to float and command
                map_DMX16_to_world(raw_speed, &cmd_speed, WDY_MIN_SPEED, WDY_MAX_SPEED);
                map_DMX16_to_world(raw_position, &cmd_position, WDY_MIN_POSITION, WDY_MAX_POSITION);
                map_DMXcommand_to_command(raw_command, &cmd_command);

                map_DMX8_to_world(raw_accel, &cmd_accel, WDY_MIN_ACCEL, WDY_MAX_ACCEL);
                map_DMX8_to_world(raw_decel, &cmd_decel, WDY_MIN_DECEL, WDY_MAX_DECEL);

                DEBUG_PRINTF("DMX cmd %d\t pos %d spd %d acc %d dec %d\r\n",
                        raw_command, raw_position, raw_speed, raw_accel, raw_decel);
                DEBUG_PRINTF("REAL cmd %d\t pos %f spd %f acc %f dec %f\r\n",
                        cmd_command, cmd_position, cmd_speed, cmd_accel, cmd_decel);

                // Set signals if any parameter has changed
                new_cmd_sig = (last_command != cmd_command);
                new_mov_sig =
                    (last_position != cmd_position) | (last_speed != cmd_speed) |
                    (last_accel != cmd_accel) | (last_decel != cmd_decel);
            }


            /* VFD status update */
            err = MFE_get_status(&node, &nd_sts);
            if (err != 0) {
                DEBUG_PRINTF("VFD comm fault: %d\r\n", err);
                status = ADD_FLAG(status, WDY_STS_COMM_FAULT);

                // TODO: Try reset sequence
                continue;
            } else {
                uint32_t vfd_status = nd_sts.to_int;

                status = REMOVE_FLAG(status, WDY_STS_COMM_FAULT);

                // Check ES
                status = SWITCH_FLAG(status, WDY_STS_EMERGENCY_STOP, WDY_EMERGENCY_STOP_ENGAGED);

                // Check VFD readiness
                if (!WDY_EMERGENCY_STOP_ENGAGED && CHECK_FLAG(vfd_status, MFE_STS_IS_READY)) {
                    status = ADD_FLAG(status, WDY_STS_IS_READY);
                } else
                    status = REMOVE_FLAG(status, WDY_STS_IS_READY);

                // Check brakes
                if (WDY_BRAKE1_ENGAGED || WDY_BRAKE2_ENGAGED || CHECK_FLAG(vfd_status, MFE_STS_BRAKE_ACTIVE)) {
                    status = ADD_FLAG(status, WDY_STS_BRAKE_ACTIVE);
                    DEBUG_PRINTF("BRAKE %d %d %d\r\n", WDY_BRAKE1_ENGAGED, WDY_BRAKE2_ENGAGED, CHECK_FLAG(vfd_status, MFE_STS_BRAKE_ACTIVE));
                }
                else {
                    status = REMOVE_FLAG(status, WDY_STS_BRAKE_ACTIVE);
                }

                // Check hard limits
                status = SWITCH_FLAG(status, WDY_STS_HARD_LIMIT_FW, CHECK_FLAG(vfd_status, MFE_STS_HARD_LIMIT_FW));
                status = SWITCH_FLAG(status, WDY_STS_HARD_LIMIT_RW, CHECK_FLAG(vfd_status, MFE_STS_HARD_LIMIT_RW));

                // Check soft limits
                status = SWITCH_FLAG(status, WDY_STS_SOFT_LIMIT_FW, CHECK_FLAG(vfd_status, MFE_STS_SOFT_LIMIT_FW));
                status = SWITCH_FLAG(status, WDY_STS_SOFT_LIMIT_RW, CHECK_FLAG(vfd_status, MFE_STS_SOFT_LIMIT_RW));

                // Check idle status
                status = SWITCH_FLAG(status, WDY_STS_IS_IDLE, CHECK_FLAG(vfd_status, MFE_STS_IS_IDLE));
            }


            /* Check load */
            load = read_loadpin(&adc_loadpin);
            if (load < WDY_MIN_LOAD) {   // Underloaded
                status = ADD_FLAG(status, WDY_STS_UNDERLOADED);
                if (!CHECK_FLAG(status, WDY_STS_IS_IDLE))
                    planner.plan(cmd_position, 0.0);    // Plan to stop
                DEBUG_PRINTF("LOAD: U %f\r\n", load);
            } else if (load > WDY_MAX_LOAD) {
                status = ADD_FLAG(status, WDY_STS_OVERLOADED);
                // Force stop
                nd_cmd.to_int = MFE_CMD_NONE;
                err = MFE_set_command(&node, &nd_cmd);
                DEBUG_PRINTF("LOAD: O %f\r\n", load);
                continue;
            } else {
                status = REMOVE_FLAG(status, WDY_STS_UNDERLOADED);
                status = REMOVE_FLAG(status, WDY_STS_OVERLOADED);
                DEBUG_PRINTF("LOAD: N %f\r\n", load);
            }

            DEBUG_PRINTF("VFD sts: %d %d\r\n", status);


            /* Send new command to VFD */
            if (new_cmd_sig) {
                if (cmd_command == WDY_CMD_ENABLE) {
                    nd_cmd.to_int = MFE_CMD_ENABLE;
                    err = MFE_set_command(&node, &nd_cmd);

                    status = status | WDY_STS_ENABLED;
                } else if (cmd_command == WDY_CMD_HOME) {
                    nd_cmd.to_int = MFE_CMD_ENABLE | MFE_CMD_HOME;
                    err = MFE_set_command(&node, &nd_cmd);

                    status = ADD_FLAG(status, WDY_STS_HOME_IN_PROGRESS);
                    status = REMOVE_FLAG(status, WDY_STS_HOMED);

                    Thread::wait(WDY_LOOP_INTERVAL * 5);    // Wait to let the VFD update its status
                } else if (cmd_command == WDY_CMD_HOME_ENCODER) {
                    encoder.setHome(WDY_ENCODER_HOME_OFFSET);   // Reset encoder position
                } else {  // WDY_CMD_NONE
                    nd_cmd.to_int = MFE_CMD_NONE;
                    err = MFE_set_command(&node, &nd_cmd);
                }

                if (err != 0) {
                    status = ADD_FLAG(status, WDY_STS_COMM_FAULT);
                    continue;
                }
            }


            /* Plan new move */
            // TODO: Should we plan even if not homed ?
            if (new_mov_sig) {
                planner.set_accel(cmd_accel);
                planner.set_decel(cmd_decel);
                planner.plan(cmd_position, cmd_speed);
                DEBUG_PRINTF("STATUS cmd %d sts %d err %d\r\n", cmd_command, status, err);
            }


            /* Check homing status */
            if (cmd_command == WDY_CMD_ENABLE && CHECK_FLAG(status, WDY_STS_HOMED)) {   // Homed
                // TODO: Should we plan even if not homed ?

                WDY_motion::move_cmd_t move = planner.get_next_interval();

                enc_position = encoder.getPosition();
                enc_velocity = encoder.getSpeed();
                following_error = move.position - enc_position;

                p_term = WDY_POS_KP * following_error;

                bool _ki = false;
                if (WDY_POS_KIMODE == KIMODE_ALWAYS)
                    _ki = true;
                else if (WDY_POS_KIMODE == KIMODE_STEADY_STATE &&
                        move.phase == WDY_motion::PHASE_IDLE)
                    _ki = true;
                else if (WDY_POS_KIMODE == KIMODE_SMART &&
                        (move.phase == WDY_motion::PHASE_IDLE || move.phase == WDY_motion::PHASE_PLATE))
                    _ki = true;

                if (_ki) {
                    following_error_sum += following_error;
                    i_term = WDY_POS_KI * following_error_sum;
                } else
                    i_term = 0.0;

                if (WDY_POS_KD != 0.0) {
                    d_term = WDY_POS_KD * (following_error - following_error_last);
                    following_error_last = following_error;
                } else
                    d_term = 0.0;

                vel_command_lin = p_term + i_term + d_term;
                vel_command_spl = linear_to_rot(vel_command_lin, length_to_drum_diameter(enc_position));

#ifdef WDY_GEARBOX_RATIO
                nd_vel.to_float = vel_command_spl * WDY_GEARBOX_RATIO; // Position: real to turns
#else
                nd_vel.to_float = vel_command_spl; // Position: real to turns
#endif
                err = MFE_set_velocity(&node, &nd_vel);

                if (err != 0) {
                    status = ADD_FLAG(status, WDY_STS_COMM_FAULT);
                    continue;
                }

                DEBUG_PRINTF("MOT sts %d spd %f pos %f mspd %f mpos %f\r\n",
                        status, nd_vel.to_float, move.position, enc_velocity, enc_position);



            } else if (CHECK_FLAG(status, WDY_STS_HOME_IN_PROGRESS)) {      // Homing in progress
                Thread::wait(250 - WDY_LOOP_INTERVAL);

                if (CHECK_FLAG(nd_sts.to_int, MFE_STS_HOME_IN_PROGRESS)) {
                    status = ADD_FLAG(status, WDY_STS_HOME_IN_PROGRESS);
                    homing_time++;

                    DEBUG_PRINTF("HOM htime %d inprogress\r\n", homing_time * WDY_LOOP_INTERVAL_S);
                } else if (homing_time >= 2 && CHECK_FLAG(nd_sts.to_int, MFE_STS_HOMED)) {
                    DEBUG_PRINTF("DRV STS %d\r\n", nd_sts.to_int);
                    Thread::wait(100);

                    encoder.setHome(WDY_ENCODER_HOME_OFFSET);   // Once the drive is homed, reset encoder position

                    status = ADD_FLAG(status, WDY_STS_HOMED);
                    status = REMOVE_FLAG(status, WDY_STS_HOME_IN_PROGRESS);
                    status = REMOVE_FLAG(status, WDY_STS_HOME_TIMEOUT);
                    DEBUG_PRINTF("STATUS cmd %d sts %d err %d\r\n", cmd_command, status, err);

                    // Set defaults
                    nd_accel.to_float = linear_to_rot(
                            WDY_DEFAULT_ACCEL, length_to_drum_diameter(WDY_MAX_POSITION));
                    nd_decel.to_float = linear_to_rot(
                            WDY_DEFAULT_DECEL, length_to_drum_diameter(WDY_MAX_POSITION));
                    err = MFE_set_accel(&node, &nd_accel);
                    err += MFE_set_decel(&node, &nd_decel);

                    DEBUG_PRINTF("HOM htime %d done\r\n", homing_time * WDY_LOOP_INTERVAL_S);

                    homing_time = 0;
                } else if (homing_time > (WDY_MAX_HOMING_TIME / WDY_LOOP_INTERVAL)) {
                    DEBUG_PRINTF("HOM htime %d timeout\r\n", homing_time * WDY_LOOP_INTERVAL_S);

                    homing_time = 0;
                    status = ADD_FLAG(status, WDY_STS_HOME_TIMEOUT);
                } else if (CHECK_FLAG(nd_sts.to_int, MFE_STS_HOME_TIMEOUT)) {
                    DEBUG_PRINTF("HOM htime %d timeout\r\n", homing_time * WDY_LOOP_INTERVAL_S);

                    status = ADD_FLAG(status, WDY_STS_HOME_TIMEOUT);
                } else {
                    DEBUG_PRINTF("HOME IN PROGRESS %d\r\n", homing_time * WDY_LOOP_INTERVAL_S);
                    homing_time++;
                }
            }

            // store commands
            last_position = cmd_position;
            last_speed = cmd_speed;
            last_command = cmd_command;
            last_accel = cmd_accel;
            last_decel = cmd_decel;

            // reset signals
            new_cmd_sig = false;
            new_mov_sig = false;

            // copy status to WDY_STATE.status
            //WDY_STATE.status = status;

            timerTempDiff = timer1msCopy - timerTempPrevious;
            if (timerTempDiff >= 500) {     // Adjust temperature according to drive temperature
                err = MFE_get_temp(&node, &nd_temp);
                if (err == 0) {
                    bdata_t _temp;
                    memcpy(&_temp.bytes, &nd_temp.bytes, BDATA_SIZE);

                    fan_top = 0.36 + (float)((_temp.to_float / 50.0) * 0.64);
                    fan_bot = 0.50 + (float)((_temp.to_float / 50.0) * 0.50);
                    DEBUG_PRINTF("TEMP tmp %f f1 %f f2 %f\r\n", _temp.to_float, (float) fan_top, (float) fan_bot);
                    DEBUG_PRINTF("TEMP tmp0 %f tmp1 %f\r\n", adc_temp0.read(), adc_temp1.read());
                    DEBUG_PRINTF("LOAD %f %f\r\n", adc_loadpin.read(), read_loadpin(&adc_loadpin));
                } else {
                    fan_top = 1.0;
                    fan_bot = 1.0;
                }

                timerTempPrevious = timer1msCopy;
            }
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
