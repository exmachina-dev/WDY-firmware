
/*******************************************************************************

   File: CO_OD.h
   CANopen Object Dictionary.

   Copyright (C) 2004-2008 Janez Paternoster

   License: GNU Lesser General Public License (LGPL).

   <http://canopennode.sourceforge.net>

   (For more information see <CO_SDO.h>.)

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

#ifndef CO_OD_H
#define CO_OD_H


/*******************************************************************************
   CANopen DATA DYPES
*******************************************************************************/
typedef uint8_t      CO_UNSIGNED8;
typedef uint16_t     CO_UNSIGNED16;
typedef uint32_t     CO_UNSIGNED32;
typedef uint64_t     CO_UNSIGNED64;
typedef int8_t       CO_INTEGER8;
typedef int16_t      CO_INTEGER16;
typedef int32_t      CO_INTEGER32;
typedef int64_t      CO_INTEGER64;
typedef float32_t    CO_REAL32;
typedef float64_t    CO_REAL64;
typedef char_t       CO_VISIBLE_STRING;
typedef oChar_t      CO_OCTET_STRING;
typedef domain_t     CO_DOMAIN;


/*******************************************************************************
    FILE INFO:
        FileName:     ???
        FileVersion:  ???
        CreationTime: ???
        CreationDate: ???
        CreatedBy:    ???
*******************************************************************************/


/*******************************************************************************
    DEVICE INFO:

*******************************************************************************/


/*******************************************************************************
    FEATURES
*******************************************************************************/
    #define CO_NO_SYNC                1       // Associated objects: 0x1005, 0x1006, 0x1007
    #define CO_NO_EMERGENCY           1       // Associated objects: 0x1014, 0x1015
    #define CO_NO_SDO_SERVER          1       // Associated objects: 0x1200
    #define CO_NO_SDO_CLIENT          2       // Associated objects: 0x1280, 0x1281
    #define CO_NO_RPDO                3       // Associated objects: 0x1400, 0x1401, 0x1402, 0x1600, 0x1601, 0x1602
    #define CO_NO_TPDO                3       // Associated objects: 0x1800, 0x1801, 0x1802, 0x1a00, 0x1a01, 0x1a02
    #define CO_NO_NMT_MASTER          1       


/*******************************************************************************
    OBJECT DICTIONARY
*******************************************************************************/
   #define CO_OD_NoOfElements             51


/*******************************************************************************
    TYPE DEFINITIONS FOR RECORDS
*******************************************************************************/
/* 0x1018 */  typedef struct{
        CO_UNSIGNED8        maxSubIndex;
        CO_UNSIGNED32       vendorID;
        CO_UNSIGNED32       productCode;
        CO_UNSIGNED32       revisionNumber;
        CO_UNSIGNED32       serialNumber;
} OD_identity_t;
/* 0x1200[1] */  typedef struct{
        CO_UNSIGNED8        maxSubIndex;
        CO_UNSIGNED32       COB_IDClientToServer;
        CO_UNSIGNED32       COB_IDServerToClient;
} OD_SDOServerParameter_t;
/* 0x1280[2] */  typedef struct{
        CO_UNSIGNED8        maxSubIndex;
        CO_UNSIGNED32       COB_IDClientToServer;
        CO_UNSIGNED32       COB_IDServerToClient;
        CO_UNSIGNED8        nodeIDOfTheSDOServer;
} OD_SDOClientParameter_t;
/* 0x1400[3] */  typedef struct{
        CO_UNSIGNED8        maxSubIndex;
        CO_UNSIGNED32       COB_IDUsedByRPDO;
        CO_UNSIGNED8        transmissionType;
} OD_RPDOCommunicationParameter_t;
/* 0x1600[3] */  typedef struct{
        CO_UNSIGNED8        numberOfMappedObjects;
        CO_UNSIGNED32       mappedObject1;
        CO_UNSIGNED32       mappedObject2;
        CO_UNSIGNED32       mappedObject3;
        CO_UNSIGNED32       mappedObject4;
        CO_UNSIGNED32       mappedObject5;
        CO_UNSIGNED32       mappedObject6;
        CO_UNSIGNED32       mappedObject7;
        CO_UNSIGNED32       mappedObject8;
} OD_RPDOMappingParameter_t;
/* 0x1800[3] */  typedef struct{
        CO_UNSIGNED8        maxSubIndex;
        CO_UNSIGNED32       COB_IDUsedByTPDO;
        CO_UNSIGNED8        transmissionType;
        CO_UNSIGNED16       inhibitTime;
        CO_UNSIGNED8        compatibilityEntry;
        CO_UNSIGNED16       eventTimer;
        CO_UNSIGNED8        SYNCStartValue;
} OD_TPDOCommunicationParameter_t;
/* 0x1a00[3] */  typedef struct{
        CO_UNSIGNED8        numberOfMappedObjects;
        CO_UNSIGNED32       mappedObject1;
        CO_UNSIGNED32       mappedObject2;
        CO_UNSIGNED32       mappedObject3;
        CO_UNSIGNED32       mappedObject4;
        CO_UNSIGNED32       mappedObject5;
        CO_UNSIGNED32       mappedObject6;
        CO_UNSIGNED32       mappedObject7;
        CO_UNSIGNED32       mappedObject8;
} OD_TPDOMappingParameter_t;


/*******************************************************************************
    STRUCTURES FOR VARIABLES IN DIFFERENT MEMORY LOCATIONS
*******************************************************************************/
#define  CO_OD_FIRST_LAST_WORD     0x55 //Any value from 0x01 to 0xFE. If changed, EEPROM will be reinitialized.

/***** Structure for RAM variables ********************************************/
struct sCO_OD_RAM{
               CO_UNSIGNED32     FirstWord;

/* 0x1001     */ CO_UNSIGNED8                    errorRegister;
/* 0x1002     */ CO_UNSIGNED32                   manufacturerStatusRegister;
/* 0x1003     */ CO_UNSIGNED32                   preDefinedErrorField[1];
/* 0x1010     */ CO_UNSIGNED32                   storeParameters[1];
/* 0x1011     */ CO_UNSIGNED32                   restoreDefaultParameters[1];
/* 0x1280[2]  */ OD_SDOClientParameter_t         SDOClientParameter[2];

               CO_UNSIGNED32     LastWord;
};

/***** Structure for EEPROM variables *****************************************/
struct sCO_OD_EEPROM{
               CO_UNSIGNED32     FirstWord;



               CO_UNSIGNED32     LastWord;
};


/***** Structure for ROM variables ********************************************/
struct sCO_OD_ROM{
               CO_UNSIGNED32     FirstWord;

/* 0x1000     */ CO_UNSIGNED32                   deviceType;
/* 0x1005     */ CO_UNSIGNED32                   COB_ID_SYNCMessage;
/* 0x1006     */ CO_UNSIGNED32                   communicationCyclePeriod;
/* 0x1007     */ CO_UNSIGNED32                   synchronousWindowLength;
/* 0x1008     */ CO_VISIBLE_STRING               manufacturerDeviceName[11];
/* 0x1009     */ CO_VISIBLE_STRING               manufacturerHardwareVersion[4];
/* 0x100a     */ CO_VISIBLE_STRING               manufacturerSoftwareVersion[4];
/* 0x100b     */ CO_UNSIGNED8                    CANNodeID;
/* 0x1014     */ CO_UNSIGNED32                   COB_ID_EMCY;
/* 0x1015     */ CO_UNSIGNED16                   inhibitTimeEMCY;
/* 0x1016     */ CO_UNSIGNED32                   consumerHeartbeatTime[4];
/* 0x1017     */ CO_UNSIGNED16                   producerHeartbeatTime;
/* 0x1018     */ OD_identity_t                   identity;
/* 0x1019     */ CO_UNSIGNED8                    synchronousCounterOverflowValue;
/* 0x1029     */ CO_UNSIGNED8                    errorBehavior[6];
/* 0x1200[1]  */ OD_SDOServerParameter_t         SDOServerParameter[1];
/* 0x1400[3]  */ OD_RPDOCommunicationParameter_t RPDOCommunicationParameter[3];
/* 0x1600[3]  */ OD_RPDOMappingParameter_t       RPDOMappingParameter[3];
/* 0x1800[3]  */ OD_TPDOCommunicationParameter_t TPDOCommunicationParameter[3];
/* 0x1a00[3]  */ OD_TPDOMappingParameter_t       TPDOMappingParameter[3];
/* 0x1f80     */ CO_UNSIGNED32                   NMTStartup;
/* 0x2100     */ CO_UNSIGNED8                    remoteNodeArray[2];
/* 0x2101     */ CO_UNSIGNED8                    remoteNodeTX_PDO1Array[2];
/* 0x2102     */ CO_UNSIGNED8                    remoteNodeRX_PDO1Array[2];
/* 0x2103     */ CO_UNSIGNED8                    remoteNodeTX_PDO2Array[2];
/* 0x2104     */ CO_UNSIGNED8                    remoteNodeRX_PDO2Array[2];
/* 0x2105     */ CO_UNSIGNED8                    remoteNodeServerSDOArray[2];
/* 0x2106     */ CO_UNSIGNED8                    remoteNodeClientSDOArray[2];
/* 0x2107     */ CO_UNSIGNED8                    remoteNodeDigitalOutputBytesArray[2];
/* 0x2108     */ CO_UNSIGNED8                    remoteNodeInputBytesArray[2];
/* 0x2109     */ CO_UNSIGNED8                    remoteNodeTX_PDO3Array[2];
/* 0x210a     */ CO_UNSIGNED8                    remoteNodeRX_PDO3Array[2];
/* 0x210b     */ CO_UNSIGNED8                    remoteNodeRX_PDO5Array[2];
/* 0x210c     */ CO_UNSIGNED8                    remoteNodeTX_PDO4Array[2];
/* 0x210d     */ CO_UNSIGNED8                    remoteNodeRX_PDO4Array[3];
/* 0x2200     */ CO_OCTET_STRING                 errorStatusBits[10];

               CO_UNSIGNED32     LastWord;
};


/***** Declaration of Object Dictionary variables *****************************/
extern struct sCO_OD_RAM CO_OD_RAM;

extern struct sCO_OD_EEPROM CO_OD_EEPROM;

extern struct sCO_OD_ROM CO_OD_ROM;


/*******************************************************************************
    ALIASES FOR OBJECT DICTIONARY VARIABLES
*******************************************************************************/
/* 0x1000   data type: CO_UNSIGNED32 */
    #define OD_deviceType                                     CO_OD_ROM.deviceType

/* 0x1001   data type: CO_UNSIGNED8 */
    #define OD_errorRegister                                  CO_OD_RAM.errorRegister

/* 0x1002   data type: CO_UNSIGNED32 */
    #define OD_manufacturerStatusRegister                     CO_OD_RAM.manufacturerStatusRegister

/* 0x1003   data type: CO_UNSIGNED32 */
    #define OD_preDefinedErrorField                           CO_OD_RAM.preDefinedErrorField
    #define ODL_preDefinedErrorField_arrayLength              8

/* 0x1005   data type: CO_UNSIGNED32 */
    #define OD_COB_ID_SYNCMessage                             CO_OD_ROM.COB_ID_SYNCMessage

/* 0x1006   data type: CO_UNSIGNED32 */
    #define OD_communicationCyclePeriod                       CO_OD_ROM.communicationCyclePeriod

/* 0x1007   data type: CO_UNSIGNED32 */
    #define OD_synchronousWindowLength                        CO_OD_ROM.synchronousWindowLength

/* 0x1008   data type: CO_VISIBLE_STRING */
    #define OD_manufacturerDeviceName                         CO_OD_ROM.manufacturerDeviceName
    #define ODL_manufacturerDeviceName_stringLength           11

/* 0x1009   data type: CO_VISIBLE_STRING */
    #define OD_manufacturerHardwareVersion                    CO_OD_ROM.manufacturerHardwareVersion
    #define ODL_manufacturerHardwareVersion_stringLength      4

/* 0x100a   data type: CO_VISIBLE_STRING */
    #define OD_manufacturerSoftwareVersion                    CO_OD_ROM.manufacturerSoftwareVersion
    #define ODL_manufacturerSoftwareVersion_stringLength      4

/* 0x100b   data type: CO_UNSIGNED8 */
    #define OD_CANNodeID                                      CO_OD_ROM.CANNodeID

/* 0x1010   data type: CO_UNSIGNED32 */
    #define OD_storeParameters                                CO_OD_RAM.storeParameters
    #define ODL_storeParameters_arrayLength                   1
    #define ODA_storeParameters_saveAllParameters             0

/* 0x1011   data type: CO_UNSIGNED32 */
    #define OD_restoreDefaultParameters                       CO_OD_RAM.restoreDefaultParameters
    #define ODL_restoreDefaultParameters_arrayLength          1
    #define ODA_restoreDefaultParameters_restoreAllDefaultParameters 0

/* 0x1014   data type: CO_UNSIGNED32 */
    #define OD_COB_ID_EMCY                                    CO_OD_ROM.COB_ID_EMCY

/* 0x1015   data type: CO_UNSIGNED16 */
    #define OD_inhibitTimeEMCY                                CO_OD_ROM.inhibitTimeEMCY

/* 0x1016   data type: CO_UNSIGNED32 */
    #define OD_consumerHeartbeatTime                          CO_OD_ROM.consumerHeartbeatTime
    #define ODL_consumerHeartbeatTime_arrayLength             4

/* 0x1017   data type: CO_UNSIGNED16 */
    #define OD_producerHeartbeatTime                          CO_OD_ROM.producerHeartbeatTime

/* 0x1018   data type: OD_identity_t */
    #define OD_identity                                       CO_OD_ROM.identity

/* 0x1019   data type: CO_UNSIGNED8 */
    #define OD_synchronousCounterOverflowValue                CO_OD_ROM.synchronousCounterOverflowValue

/* 0x1029   data type: CO_UNSIGNED8 */
    #define OD_errorBehavior                                  CO_OD_ROM.errorBehavior
    #define ODL_errorBehavior_arrayLength                     6
    #define ODA_errorBehavior_communication                   0
    #define ODA_errorBehavior_communicationOther              1
    #define ODA_errorBehavior_communicationPassive            2
    #define ODA_errorBehavior_generic                         3
    #define ODA_errorBehavior_deviceProfile                   4
    #define ODA_errorBehavior_manufacturerSpecific            5

/* 0x1200   data type: OD_SDOServerParameter_t */
    #define OD_SDOServerParameter                             CO_OD_ROM.SDOServerParameter

/* 0x1280   data type: OD_SDOClientParameter_t, array[2] */
    #define OD_SDOClientParameter                             CO_OD_RAM.SDOClientParameter

/* 0x1400   data type: OD_RPDOCommunicationParameter_t, array[3] */
    #define OD_RPDOCommunicationParameter                     CO_OD_ROM.RPDOCommunicationParameter

/* 0x1600   data type: OD_RPDOMappingParameter_t, array[3] */
    #define OD_RPDOMappingParameter                           CO_OD_ROM.RPDOMappingParameter

/* 0x1800   data type: OD_TPDOCommunicationParameter_t, array[3] */
    #define OD_TPDOCommunicationParameter                     CO_OD_ROM.TPDOCommunicationParameter

/* 0x1a00   data type: OD_TPDOMappingParameter_t, array[3] */
    #define OD_TPDOMappingParameter                           CO_OD_ROM.TPDOMappingParameter

/* 0x1f80   data type: CO_UNSIGNED32 */
    #define OD_NMTStartup                                     CO_OD_ROM.NMTStartup

/* 0x2100   data type: CO_UNSIGNED8 */
    #define OD_remoteNodeArray                                CO_OD_ROM.remoteNodeArray
    #define ODL_remoteNodeArray_arrayLength                   2
    #define ODA_remoteNodeArray_node01Type                    0
    #define ODA_remoteNodeArray_node02Type                    1

/* 0x2101   data type: CO_UNSIGNED8 */
    #define OD_remoteNodeTX_PDO1Array                         CO_OD_ROM.remoteNodeTX_PDO1Array
    #define ODL_remoteNodeTX_PDO1Array_arrayLength            2
    #define ODA_remoteNodeTX_PDO1Array_node01Type             0
    #define ODA_remoteNodeTX_PDO1Array_node02Type             1

/* 0x2102   data type: CO_UNSIGNED8 */
    #define OD_remoteNodeRX_PDO1Array                         CO_OD_ROM.remoteNodeRX_PDO1Array
    #define ODL_remoteNodeRX_PDO1Array_arrayLength            2
    #define ODA_remoteNodeRX_PDO1Array_node01Type             0
    #define ODA_remoteNodeRX_PDO1Array_node02Type             1

/* 0x2103   data type: CO_UNSIGNED8 */
    #define OD_remoteNodeTX_PDO2Array                         CO_OD_ROM.remoteNodeTX_PDO2Array
    #define ODL_remoteNodeTX_PDO2Array_arrayLength            2
    #define ODA_remoteNodeTX_PDO2Array_node01Type             0
    #define ODA_remoteNodeTX_PDO2Array_node02Type             1

/* 0x2104   data type: CO_UNSIGNED8 */
    #define OD_remoteNodeRX_PDO2Array                         CO_OD_ROM.remoteNodeRX_PDO2Array
    #define ODL_remoteNodeRX_PDO2Array_arrayLength            2
    #define ODA_remoteNodeRX_PDO2Array_node01Type             0
    #define ODA_remoteNodeRX_PDO2Array_node02Type             1

/* 0x2105   data type: CO_UNSIGNED8 */
    #define OD_remoteNodeServerSDOArray                       CO_OD_ROM.remoteNodeServerSDOArray
    #define ODL_remoteNodeServerSDOArray_arrayLength          2
    #define ODA_remoteNodeServerSDOArray_node01Type           0
    #define ODA_remoteNodeServerSDOArray_node02Type           1

/* 0x2106   data type: CO_UNSIGNED8 */
    #define OD_remoteNodeClientSDOArray                       CO_OD_ROM.remoteNodeClientSDOArray
    #define ODL_remoteNodeClientSDOArray_arrayLength          2
    #define ODA_remoteNodeClientSDOArray_node01Type           0
    #define ODA_remoteNodeClientSDOArray_node02Type           1

/* 0x2107   data type: CO_UNSIGNED8 */
    #define OD_remoteNodeDigitalOutputBytesArray              CO_OD_ROM.remoteNodeDigitalOutputBytesArray
    #define ODL_remoteNodeDigitalOutputBytesArray_arrayLength 2
    #define ODA_remoteNodeDigitalOutputBytesArray_node01Type  0
    #define ODA_remoteNodeDigitalOutputBytesArray_node02Type  1

/* 0x2108   data type: CO_UNSIGNED8 */
    #define OD_remoteNodeInputBytesArray                      CO_OD_ROM.remoteNodeInputBytesArray
    #define ODL_remoteNodeInputBytesArray_arrayLength         2
    #define ODA_remoteNodeInputBytesArray_node01Type          0
    #define ODA_remoteNodeInputBytesArray_node02Type          1

/* 0x2109   data type: CO_UNSIGNED8 */
    #define OD_remoteNodeTX_PDO3Array                         CO_OD_ROM.remoteNodeTX_PDO3Array
    #define ODL_remoteNodeTX_PDO3Array_arrayLength            2
    #define ODA_remoteNodeTX_PDO3Array_node01Type             0
    #define ODA_remoteNodeTX_PDO3Array_node02Type             1

/* 0x210a   data type: CO_UNSIGNED8 */
    #define OD_remoteNodeRX_PDO3Array                         CO_OD_ROM.remoteNodeRX_PDO3Array
    #define ODL_remoteNodeRX_PDO3Array_arrayLength            2
    #define ODA_remoteNodeRX_PDO3Array_node01Type             0
    #define ODA_remoteNodeRX_PDO3Array_node02Type             1

/* 0x210b   data type: CO_UNSIGNED8 */
    #define OD_remoteNodeRX_PDO5Array                         CO_OD_ROM.remoteNodeRX_PDO5Array
    #define ODL_remoteNodeRX_PDO5Array_arrayLength            2
    #define ODA_remoteNodeRX_PDO5Array_node01Type             0
    #define ODA_remoteNodeRX_PDO5Array_node02Type             1

/* 0x210c   data type: CO_UNSIGNED8 */
    #define OD_remoteNodeTX_PDO4Array                         CO_OD_ROM.remoteNodeTX_PDO4Array
    #define ODL_remoteNodeTX_PDO4Array_arrayLength            2
    #define ODA_remoteNodeTX_PDO4Array_node01Type             0
    #define ODA_remoteNodeTX_PDO4Array_node02Type             1

/* 0x210d   data type: CO_UNSIGNED8 */
    #define OD_remoteNodeRX_PDO4Array                         CO_OD_ROM.remoteNodeRX_PDO4Array
    #define ODL_remoteNodeRX_PDO4Array_arrayLength            2
    #define ODA_remoteNodeRX_PDO4Array_node01Type             0
    #define ODA_remoteNodeRX_PDO4Array_node02Type             1

/* 0x2200   data type: CO_OCTET_STRING */
    #define OD_errorStatusBits                                CO_OD_ROM.errorStatusBits
    #define ODL_errorStatusBits_stringLength                  10


#endif
