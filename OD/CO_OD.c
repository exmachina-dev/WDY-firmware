
/*******************************************************************************

   File - CO_OD.c
   CANopen Object Dictionary.

   Copyright (C) 2004-2008 Janez Paternoster

   License: GNU Lesser General Public License (LGPL).

   <http://canopennode.sourceforge.net>

   (For more information see <CO_SDO.h>.)
*/
/*
   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU Lesser General Public License as published by
   the Free Software Foundation, either version 2.1 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.


   Author: Janez Paternoster


   This file was automatically generated with CANopenNode ODE.
   DON'T EDIT THIS FILE MANUALLY !!!!

*******************************************************************************/


#include "CO_driver.h"
#include "CO_OD.h"
#include "CO_SDO.h"


/*******************************************************************************
    DEFINITION AND INITIALIZATION OF OBJECT DICTIONARY VARIABLES
*******************************************************************************/

/***** Definition for RAM variables *******************************************/
struct sCO_OD_RAM CO_OD_RAM = {
    CO_OD_FIRST_LAST_WORD,

/* 0x1001   */ 0x00,
/* 0x1002   */ 0x0L,
/* 0x1003   */ {0x0L, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L},
/* 0x1010   */ {0x3L},
/* 0x1011   */ {0x1L},
/* 0x1280   */ {{0x03, 0x602L, 0x582L, 0x01},
/* 0x1281   */ {0x03, 0x602L, 0x582L, 0x01}},

   CO_OD_FIRST_LAST_WORD,
};


/***** Definition for EEPROM variables ****************************************/
struct sCO_OD_EEPROM CO_OD_EEPROM = {
    CO_OD_FIRST_LAST_WORD,



    CO_OD_FIRST_LAST_WORD,
};


/***** Definition for ROM variables *******************************************/
struct sCO_OD_ROM CO_OD_ROM = {    //constant variables, stored in flash
    CO_OD_FIRST_LAST_WORD,

/* 0x1000   */ 0x20192L,
/* 0x1005   */ 0x80L,
/* 0x1006   */ 0x0L,
/* 0x1007   */ 0x0L,
/* 0x1008   */ {'C', 'A', 'N', 'o', 'p', 'e', 'n', 'N', 'o', 'd', 'e'},
/* 0x1009   */ {'3', '.', '0', '0'},
/* 0x100a   */ {'3', '.', '0', '0'},
/* 0x100b   */ 0x02,
/* 0x1014   */ 0x80L,
/* 0x1015   */ 0x0064,
/* 0x1016   */ {0x0L, 0x0L, 0x0L, 0x0L},
/* 0x1017   */ 0x01f4,
/* 0x1018   */ {0x04, 0x3dbL, 0x25L, 0x0L, 0x0L},
/* 0x1019   */ 0x00,
/* 0x1029   */ {0x00, 0x00, 0x01, 0x00, 0x00, 0x00},
/* 0x1200   */ {{0x02, 0x600L, 0x580L}},
/* 0x1400   */ {{0x02, 0x200L, 0xff},
/* 0x1401   */ {0x02, 0x300L, 0xfe},
/* 0x1402   */ {0x02, 0x400L, 0xfe}},
/* 0x1600   */ {{0x02, 0x62000108L, 0x62000208L, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L},
/* 0x1601   */ {0x00, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L},
/* 0x1602   */ {0x00, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L}},
/* 0x1800   */ {{0x06, 0x180L, 0xff, 0x0064, 0x00, 0x0000, 0x00},
/* 0x1801   */ {0x06, 0x280L, 0xfe, 0x0000, 0x00, 0x0000, 0x00},
/* 0x1802   */ {0x06, 0x380L, 0xfe, 0x0000, 0x00, 0x0000, 0x00}},
/* 0x1a00   */ {{0x02, 0x60000108L, 0x60000208L, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L},
/* 0x1a01   */ {0x00, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L},
/* 0x1a02   */ {0x00, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L, 0x0L}},
/* 0x1f80   */ 0x0L,
/* 0x2100   */ {0x00, 0x00},
/* 0x2101   */ {0x00, 0x00},
/* 0x2102   */ {0x00, 0x00},
/* 0x2103   */ {0x00, 0x00},
/* 0x2104   */ {0x00, 0x00},
/* 0x2105   */ {0x00, 0x00},
/* 0x2106   */ {0x00, 0x00},
/* 0x2107   */ {0x00, 0x00},
/* 0x2108   */ {0x00, 0x00},
/* 0x2109   */ {0x00, 0x00},
/* 0x210a   */ {0x00, 0x00},
/* 0x210b   */ {0x00, 0x00},
/* 0x210c   */ {0x00, 0x00},
/* 0x210d   */ {0x00, 0x00},
/* 0x2200   */ {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},

    CO_OD_FIRST_LAST_WORD
};


/*******************************************************************************
   STRUCTURES FOR RECORD TYPE OBJECTS
*******************************************************************************/
/* 0x1018   */ const CO_OD_entryRecord_t OD_record1018[5] ={
        {(void*)&CO_OD_ROM.identity.maxSubIndex, 0x05, 1},
        {(void*)&CO_OD_ROM.identity.vendorID, 0x85, 4},
        {(void*)&CO_OD_ROM.identity.productCode, 0x85, 4},
        {(void*)&CO_OD_ROM.identity.revisionNumber, 0x85, 4},
        {(void*)&CO_OD_ROM.identity.serialNumber, 0x85, 4}
};
/* 0x1200   */ const CO_OD_entryRecord_t OD_record1200[3] ={
        {(void*)&CO_OD_ROM.SDOServerParameter[0].maxSubIndex, 0x05, 1},
        {(void*)&CO_OD_ROM.SDOServerParameter[0].COB_IDClientToServer, 0x85, 4},
        {(void*)&CO_OD_ROM.SDOServerParameter[0].COB_IDServerToClient, 0x85, 4}
};
/* 0x1280   */ const CO_OD_entryRecord_t OD_record1280[4] ={
        {(void*)&CO_OD_RAM.SDOClientParameter[0].maxSubIndex, 0x06, 1},
        {(void*)&CO_OD_RAM.SDOClientParameter[0].COB_IDClientToServer, 0xbe, 4},
        {(void*)&CO_OD_RAM.SDOClientParameter[0].COB_IDServerToClient, 0xbe, 4},
        {(void*)&CO_OD_RAM.SDOClientParameter[0].nodeIDOfTheSDOServer, 0x0e, 1}
};
/* 0x1281   */ const CO_OD_entryRecord_t OD_record1281[4] ={
        {(void*)&CO_OD_RAM.SDOClientParameter[1].maxSubIndex, 0x06, 1},
        {(void*)&CO_OD_RAM.SDOClientParameter[1].COB_IDClientToServer, 0xbe, 4},
        {(void*)&CO_OD_RAM.SDOClientParameter[1].COB_IDServerToClient, 0xbe, 4},
        {(void*)&CO_OD_RAM.SDOClientParameter[1].nodeIDOfTheSDOServer, 0x0e, 1}
};
/* 0x1400   */ const CO_OD_entryRecord_t OD_record1400[3] ={
        {(void*)&CO_OD_ROM.RPDOCommunicationParameter[0].maxSubIndex, 0x05, 1},
        {(void*)&CO_OD_ROM.RPDOCommunicationParameter[0].COB_IDUsedByRPDO, 0x8d, 4},
        {(void*)&CO_OD_ROM.RPDOCommunicationParameter[0].transmissionType, 0x0d, 1}
};
/* 0x1401   */ const CO_OD_entryRecord_t OD_record1401[3] ={
        {(void*)&CO_OD_ROM.RPDOCommunicationParameter[1].maxSubIndex, 0x05, 1},
        {(void*)&CO_OD_ROM.RPDOCommunicationParameter[1].COB_IDUsedByRPDO, 0x8d, 4},
        {(void*)&CO_OD_ROM.RPDOCommunicationParameter[1].transmissionType, 0x0d, 1}
};
/* 0x1402   */ const CO_OD_entryRecord_t OD_record1402[3] ={
        {(void*)&CO_OD_ROM.RPDOCommunicationParameter[2].maxSubIndex, 0x05, 1},
        {(void*)&CO_OD_ROM.RPDOCommunicationParameter[2].COB_IDUsedByRPDO, 0x8d, 4},
        {(void*)&CO_OD_ROM.RPDOCommunicationParameter[2].transmissionType, 0x0d, 1}
};
/* 0x1600   */ const CO_OD_entryRecord_t OD_record1600[9] ={
        {(void*)&CO_OD_ROM.RPDOMappingParameter[0].numberOfMappedObjects, 0x0d, 1},
        {(void*)&CO_OD_ROM.RPDOMappingParameter[0].mappedObject1, 0x8d, 4},
        {(void*)&CO_OD_ROM.RPDOMappingParameter[0].mappedObject2, 0x8d, 4},
        {(void*)&CO_OD_ROM.RPDOMappingParameter[0].mappedObject3, 0x8d, 4},
        {(void*)&CO_OD_ROM.RPDOMappingParameter[0].mappedObject4, 0x8d, 4},
        {(void*)&CO_OD_ROM.RPDOMappingParameter[0].mappedObject5, 0x8d, 4},
        {(void*)&CO_OD_ROM.RPDOMappingParameter[0].mappedObject6, 0x8d, 4},
        {(void*)&CO_OD_ROM.RPDOMappingParameter[0].mappedObject7, 0x8d, 4},
        {(void*)&CO_OD_ROM.RPDOMappingParameter[0].mappedObject8, 0x8d, 4}
};
/* 0x1601   */ const CO_OD_entryRecord_t OD_record1601[9] ={
        {(void*)&CO_OD_ROM.RPDOMappingParameter[1].numberOfMappedObjects, 0x0d, 1},
        {(void*)&CO_OD_ROM.RPDOMappingParameter[1].mappedObject1, 0x8d, 4},
        {(void*)&CO_OD_ROM.RPDOMappingParameter[1].mappedObject2, 0x8d, 4},
        {(void*)&CO_OD_ROM.RPDOMappingParameter[1].mappedObject3, 0x8d, 4},
        {(void*)&CO_OD_ROM.RPDOMappingParameter[1].mappedObject4, 0x8d, 4},
        {(void*)&CO_OD_ROM.RPDOMappingParameter[1].mappedObject5, 0x8d, 4},
        {(void*)&CO_OD_ROM.RPDOMappingParameter[1].mappedObject6, 0x8d, 4},
        {(void*)&CO_OD_ROM.RPDOMappingParameter[1].mappedObject7, 0x8d, 4},
        {(void*)&CO_OD_ROM.RPDOMappingParameter[1].mappedObject8, 0x8d, 4}
};
/* 0x1602   */ const CO_OD_entryRecord_t OD_record1602[9] ={
        {(void*)&CO_OD_ROM.RPDOMappingParameter[2].numberOfMappedObjects, 0x0d, 1},
        {(void*)&CO_OD_ROM.RPDOMappingParameter[2].mappedObject1, 0x8d, 4},
        {(void*)&CO_OD_ROM.RPDOMappingParameter[2].mappedObject2, 0x8d, 4},
        {(void*)&CO_OD_ROM.RPDOMappingParameter[2].mappedObject3, 0x8d, 4},
        {(void*)&CO_OD_ROM.RPDOMappingParameter[2].mappedObject4, 0x8d, 4},
        {(void*)&CO_OD_ROM.RPDOMappingParameter[2].mappedObject5, 0x8d, 4},
        {(void*)&CO_OD_ROM.RPDOMappingParameter[2].mappedObject6, 0x8d, 4},
        {(void*)&CO_OD_ROM.RPDOMappingParameter[2].mappedObject7, 0x8d, 4},
        {(void*)&CO_OD_ROM.RPDOMappingParameter[2].mappedObject8, 0x8d, 4}
};
/* 0x1800   */ const CO_OD_entryRecord_t OD_record1800[7] ={
        {(void*)&CO_OD_ROM.TPDOCommunicationParameter[0].maxSubIndex, 0x05, 1},
        {(void*)&CO_OD_ROM.TPDOCommunicationParameter[0].COB_IDUsedByTPDO, 0x8d, 4},
        {(void*)&CO_OD_ROM.TPDOCommunicationParameter[0].transmissionType, 0x0d, 1},
        {(void*)&CO_OD_ROM.TPDOCommunicationParameter[0].inhibitTime, 0x8d, 2},
        {(void*)&CO_OD_ROM.TPDOCommunicationParameter[0].compatibilityEntry, 0x0d, 1},
        {(void*)&CO_OD_ROM.TPDOCommunicationParameter[0].eventTimer, 0x8d, 2},
        {(void*)&CO_OD_ROM.TPDOCommunicationParameter[0].SYNCStartValue, 0x0d, 1}
};
/* 0x1801   */ const CO_OD_entryRecord_t OD_record1801[7] ={
        {(void*)&CO_OD_ROM.TPDOCommunicationParameter[1].maxSubIndex, 0x05, 1},
        {(void*)&CO_OD_ROM.TPDOCommunicationParameter[1].COB_IDUsedByTPDO, 0x8d, 4},
        {(void*)&CO_OD_ROM.TPDOCommunicationParameter[1].transmissionType, 0x0d, 1},
        {(void*)&CO_OD_ROM.TPDOCommunicationParameter[1].inhibitTime, 0x8d, 2},
        {(void*)&CO_OD_ROM.TPDOCommunicationParameter[1].compatibilityEntry, 0x0d, 1},
        {(void*)&CO_OD_ROM.TPDOCommunicationParameter[1].eventTimer, 0x8d, 2},
        {(void*)&CO_OD_ROM.TPDOCommunicationParameter[1].SYNCStartValue, 0x0d, 1}
};
/* 0x1802   */ const CO_OD_entryRecord_t OD_record1802[7] ={
        {(void*)&CO_OD_ROM.TPDOCommunicationParameter[2].maxSubIndex, 0x05, 1},
        {(void*)&CO_OD_ROM.TPDOCommunicationParameter[2].COB_IDUsedByTPDO, 0x8d, 4},
        {(void*)&CO_OD_ROM.TPDOCommunicationParameter[2].transmissionType, 0x0d, 1},
        {(void*)&CO_OD_ROM.TPDOCommunicationParameter[2].inhibitTime, 0x8d, 2},
        {(void*)&CO_OD_ROM.TPDOCommunicationParameter[2].compatibilityEntry, 0x0d, 1},
        {(void*)&CO_OD_ROM.TPDOCommunicationParameter[2].eventTimer, 0x8d, 2},
        {(void*)&CO_OD_ROM.TPDOCommunicationParameter[2].SYNCStartValue, 0x0d, 1}
};
/* 0x1a00   */ const CO_OD_entryRecord_t OD_record1A00[9] ={
        {(void*)&CO_OD_ROM.TPDOMappingParameter[0].numberOfMappedObjects, 0x0d, 1},
        {(void*)&CO_OD_ROM.TPDOMappingParameter[0].mappedObject1, 0x8d, 4},
        {(void*)&CO_OD_ROM.TPDOMappingParameter[0].mappedObject2, 0x8d, 4},
        {(void*)&CO_OD_ROM.TPDOMappingParameter[0].mappedObject3, 0x8d, 4},
        {(void*)&CO_OD_ROM.TPDOMappingParameter[0].mappedObject4, 0x8d, 4},
        {(void*)&CO_OD_ROM.TPDOMappingParameter[0].mappedObject5, 0x8d, 4},
        {(void*)&CO_OD_ROM.TPDOMappingParameter[0].mappedObject6, 0x8d, 4},
        {(void*)&CO_OD_ROM.TPDOMappingParameter[0].mappedObject7, 0x8d, 4},
        {(void*)&CO_OD_ROM.TPDOMappingParameter[0].mappedObject8, 0x8d, 4}
};
/* 0x1a01   */ const CO_OD_entryRecord_t OD_record1A01[9] ={
        {(void*)&CO_OD_ROM.TPDOMappingParameter[1].numberOfMappedObjects, 0x0d, 1},
        {(void*)&CO_OD_ROM.TPDOMappingParameter[1].mappedObject1, 0x8d, 4},
        {(void*)&CO_OD_ROM.TPDOMappingParameter[1].mappedObject2, 0x8d, 4},
        {(void*)&CO_OD_ROM.TPDOMappingParameter[1].mappedObject3, 0x8d, 4},
        {(void*)&CO_OD_ROM.TPDOMappingParameter[1].mappedObject4, 0x8d, 4},
        {(void*)&CO_OD_ROM.TPDOMappingParameter[1].mappedObject5, 0x8d, 4},
        {(void*)&CO_OD_ROM.TPDOMappingParameter[1].mappedObject6, 0x8d, 4},
        {(void*)&CO_OD_ROM.TPDOMappingParameter[1].mappedObject7, 0x8d, 4},
        {(void*)&CO_OD_ROM.TPDOMappingParameter[1].mappedObject8, 0x8d, 4}
};
/* 0x1a02   */ const CO_OD_entryRecord_t OD_record1A02[9] ={
        {(void*)&CO_OD_ROM.TPDOMappingParameter[2].numberOfMappedObjects, 0x0d, 1},
        {(void*)&CO_OD_ROM.TPDOMappingParameter[2].mappedObject1, 0x8d, 4},
        {(void*)&CO_OD_ROM.TPDOMappingParameter[2].mappedObject2, 0x8d, 4},
        {(void*)&CO_OD_ROM.TPDOMappingParameter[2].mappedObject3, 0x8d, 4},
        {(void*)&CO_OD_ROM.TPDOMappingParameter[2].mappedObject4, 0x8d, 4},
        {(void*)&CO_OD_ROM.TPDOMappingParameter[2].mappedObject5, 0x8d, 4},
        {(void*)&CO_OD_ROM.TPDOMappingParameter[2].mappedObject6, 0x8d, 4},
        {(void*)&CO_OD_ROM.TPDOMappingParameter[2].mappedObject7, 0x8d, 4},
        {(void*)&CO_OD_ROM.TPDOMappingParameter[2].mappedObject8, 0x8d, 4}
};


/*******************************************************************************
   SDO SERVER ACCESS FUNCTIONS WITH USER CODE
*******************************************************************************/
#define WRITING (dir == 1)
#define READING (dir == 0)

CO_UNSIGNED32 CO_ODF(void*, CO_UNSIGNED16, CO_UNSIGNED8, CO_UNSIGNED16*, CO_UNSIGNED16, CO_UNSIGNED8, void*, const void*);


/*******************************************************************************
   OBJECT DICTIONARY
*******************************************************************************/
const CO_OD_entry_t CO_OD[CO_OD_NoOfElements] = {
    {0x1000, 0x00, 0x85, 4, (void*)&CO_OD_ROM.deviceType},

    {0x1001, 0x00, 0x36, 1, (void*)&CO_OD_RAM.errorRegister},

    {0x1002, 0x00, 0xb6, 4, (void*)&CO_OD_RAM.manufacturerStatusRegister},

    {0x1003, 0x08, 0x86, 4, (void*)&CO_OD_RAM.preDefinedErrorField[0]},

    {0x1005, 0x00, 0x8d, 4, (void*)&CO_OD_ROM.COB_ID_SYNCMessage},

    {0x1006, 0x00, 0x8d, 4, (void*)&CO_OD_ROM.communicationCyclePeriod},

    {0x1007, 0x00, 0x8d, 4, (void*)&CO_OD_ROM.synchronousWindowLength},

    {0x1008, 0x00, 0x05, 11, (void*)&CO_OD_ROM.manufacturerDeviceName[0]},

    {0x1009, 0x00, 0x05, 4, (void*)&CO_OD_ROM.manufacturerHardwareVersion[0]},

    {0x100a, 0x00, 0x05, 4, (void*)&CO_OD_ROM.manufacturerSoftwareVersion[0]},

    {0x100b, 0x00, 0x0d, 1, (void*)&CO_OD_ROM.CANNodeID},

    {0x1010, 0x01, 0x8e, 4, (void*)&CO_OD_RAM.storeParameters[0]},

    {0x1011, 0x01, 0x8e, 4, (void*)&CO_OD_RAM.restoreDefaultParameters[0]},

    {0x1014, 0x00, 0x85, 4, (void*)&CO_OD_ROM.COB_ID_EMCY},

    {0x1015, 0x00, 0x8d, 2, (void*)&CO_OD_ROM.inhibitTimeEMCY},

    {0x1016, 0x04, 0x8d, 4, (void*)&CO_OD_ROM.consumerHeartbeatTime[0]},

    {0x1017, 0x00, 0x8d, 2, (void*)&CO_OD_ROM.producerHeartbeatTime},

    {0x1018, 0x04, 0x05, 0, (void*)&OD_record1018},

    {0x1019, 0x00, 0x0d, 1, (void*)&CO_OD_ROM.synchronousCounterOverflowValue},

    {0x1029, 0x06, 0x0d, 1, (void*)&CO_OD_ROM.errorBehavior[0]},

    {0x1200, 0x02, 0x05, 0, (void*)&OD_record1200},

    {0x1280, 0x03, 0x06, 0, (void*)&OD_record1280},
    {0x1281, 0x03, 0x06, 0, (void*)&OD_record1281},

    {0x1400, 0x02, 0x05, 0, (void*)&OD_record1400},
    {0x1401, 0x02, 0x05, 0, (void*)&OD_record1401},
    {0x1402, 0x02, 0x05, 0, (void*)&OD_record1402},

    {0x1600, 0x08, 0x05, 0, (void*)&OD_record1600},
    {0x1601, 0x08, 0x05, 0, (void*)&OD_record1601},
    {0x1602, 0x08, 0x05, 0, (void*)&OD_record1602},

    {0x1800, 0x06, 0x05, 0, (void*)&OD_record1800},
    {0x1801, 0x06, 0x05, 0, (void*)&OD_record1801},
    {0x1802, 0x06, 0x05, 0, (void*)&OD_record1802},

    {0x1a00, 0x08, 0x05, 0, (void*)&OD_record1A00},
    {0x1a01, 0x08, 0x05, 0, (void*)&OD_record1A01},
    {0x1a02, 0x08, 0x05, 0, (void*)&OD_record1A02},

    {0x1f80, 0x00, 0x8d, 4, (void*)&CO_OD_ROM.NMTStartup},

    {0x2100, 0x02, 0x0d, 1, (void*)&CO_OD_ROM.remoteNodeArray[0]},

    {0x2101, 0x02, 0x0d, 1, (void*)&CO_OD_ROM.remoteNodeTX_PDO1Array[0]},

    {0x2102, 0x02, 0x0d, 1, (void*)&CO_OD_ROM.remoteNodeRX_PDO1Array[0]},

    {0x2103, 0x02, 0x0d, 1, (void*)&CO_OD_ROM.remoteNodeTX_PDO2Array[0]},

    {0x2104, 0x02, 0x0d, 1, (void*)&CO_OD_ROM.remoteNodeRX_PDO2Array[0]},

    {0x2105, 0x02, 0x0d, 1, (void*)&CO_OD_ROM.remoteNodeServerSDOArray[0]},

    {0x2106, 0x02, 0x0d, 1, (void*)&CO_OD_ROM.remoteNodeClientSDOArray[0]},

    {0x2107, 0x02, 0x0d, 1, (void*)&CO_OD_ROM.remoteNodeDigitalOutputBytesArray[0]},

    {0x2108, 0x02, 0x0d, 1, (void*)&CO_OD_ROM.remoteNodeInputBytesArray[0]},

    {0x2109, 0x02, 0x0d, 1, (void*)&CO_OD_ROM.remoteNodeTX_PDO3Array[0]},

    {0x210a, 0x02, 0x0d, 1, (void*)&CO_OD_ROM.remoteNodeRX_PDO3Array[0]},

    {0x210b, 0x02, 0x0d, 1, (void*)&CO_OD_ROM.remoteNodeRX_PDO5Array[0]},

    {0x210c, 0x02, 0x0d, 1, (void*)&CO_OD_ROM.remoteNodeTX_PDO4Array[0]},

    {0x210d, 0x02, 0x0d, 1, (void*)&CO_OD_ROM.remoteNodeRX_PDO4Array[0]},

    {0x2200, 0x00, 0x05, 10, (void*)&CO_OD_ROM.errorStatusBits[0]},

};
