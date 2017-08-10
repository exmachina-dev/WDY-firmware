/*
 * pins.h
 * Copyright (C) 2017 Benoit Rapidel <benoit.rapidel+devs@exmachina.fr>
 *
 * Distributed under terms of the MIT license.
 */

#ifndef PINS_H
#define PINS_H

#include "config.h"

#include "PinNames.h"
#include "mbed.h"

#define LED1            P0_9
#define LED2            P1_18
#define LED3            NC
#define LED4            NC

#define LED_HMI1        P2_2
#define LED_HMI2        P2_3
#define LED_HMI3        P2_4
#define LED_HMI4        P2_5

#define BUTTON1         P0_16
#define BUTTON2         P0_15
#define BUTTON3         P0_17
#define BUTTON4         P0_18
#define BUTTON5         P0_19
#define BUTTON6         P0_20

#define LCD_BACKLIGHT   P0_22

#define FAN_P1          P3_26
#define FAN_P2          P3_25

#define DRV_ENABLE      P1_26
#define DRV_ERROR       P1_29
#define BRK_SENSOR1     P1_22
#define BRK_SENSOR2     P1_25

#define ISP_RXD         P0_3
#define ISP_TXD         P0_2
#define USBRX           ISP_RXD
#define USBTX           ISP_TXD

#define ADC_LOADPIN     P0_23
#define ADC_TEMP0       P0_24
#define ADC_TEMP1       P0_25

#define I2C0_SDA        P0_27
#define I2C0_SCL        P0_28
#define I2C1_SDA        P0_0
#define I2C1_SCL        P0_1
#define I2C2_SDA        P0_10
#define I2C2_SCL        P0_11

#define USB_DP          P0_29
#define USB_DN          P0_30

#define ENC_A           P1_20
#define ENC_B           P1_23
#define ENC_I           P1_24

#define CAN1_RXD        P0_0
#define CAN1_TXD        P0_1

#endif /* !PINS_H */
