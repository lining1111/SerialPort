//
// Created by lining on 6/8/21.
//

#ifndef _PN532_H
#define _PN532_H


#include <stdint.h>
#include <stdbool.h>
#include <vector>

using namespace std;

namespace pn532 {

    /**
     * 写
     * 0x00 0x00 0xff LEN LCS D4 CC Optional Input Data DCS 0x00
     * 读
     * 0x00 0x00 0xff LEN LCS D5 CC+1 Optional Input Data DCS 0x00
     *
     * LEN 包长度
     * LCS 包长度 checksum
     * D4 D5 (TFI) D4设备到pn532 D5 pn532到设备
     * CC 命令码
     * DCS 包数据checksum
     *
     */


    const uint8_t PN532_ACK[] = {0x00, 0x00, 0xFF, 0x00, 0xFF, 0x00};
    const uint8_t PN532_FRAME_START[] = {0x00, 0x00, 0xFF};


    struct PN532_HEAD {
        uint8_t preamble = 0x00;
        uint8_t start_code_0 = 0x00;
        uint8_t start_code_1 = 0xff;
        uint8_t len;
        uint8_t len_checksum;
    };

    struct PN532_TAIL {
        uint8_t data_checksum;
        uint8_t postamble = 0x00;
    };

#define CID_MAX_LEN 4

#define PN532_FRAME_MAX_LENGTH              255
#define PN532_DEFAULT_TIMEOUT               1000

#define PN532_PREAMBLE                      (0x00)
#define PN532_STARTCODE1                    (0x00)
#define PN532_STARTCODE2                    (0xFF)
#define PN532_POSTAMBLE                     (0x00)

#define PN532_HOSTTOPN532                   (0xD4)
#define PN532_PN532TOHOST                   (0xD5)

// PN532 Commands
#define PN532_COMMAND_DIAGNOSE              (0x00)
#define PN532_COMMAND_GETFIRMWAREVERSION    (0x02)
#define PN532_COMMAND_GETGENERALSTATUS      (0x04)
#define PN532_COMMAND_READREGISTER          (0x06)
#define PN532_COMMAND_WRITEREGISTER         (0x08)
#define PN532_COMMAND_READGPIO              (0x0C)
#define PN532_COMMAND_WRITEGPIO             (0x0E)
#define PN532_COMMAND_SETSERIALBAUDRATE     (0x10)
#define PN532_COMMAND_SETPARAMETERS         (0x12)
#define PN532_COMMAND_SAMCONFIGURATION      (0x14)
#define PN532_COMMAND_POWERDOWN             (0x16)
#define PN532_COMMAND_RFCONFIGURATION       (0x32)
#define PN532_COMMAND_RFREGULATIONTEST      (0x58)
#define PN532_COMMAND_INJUMPFORDEP          (0x56)
#define PN532_COMMAND_INJUMPFORPSL          (0x46)
#define PN532_COMMAND_INLISTPASSIVETARGET   (0x4A)
#define PN532_COMMAND_INATR                 (0x50)
#define PN532_COMMAND_INPSL                 (0x4E)
#define PN532_COMMAND_INDATAEXCHANGE        (0x40)
#define PN532_COMMAND_INCOMMUNICATETHRU     (0x42)
#define PN532_COMMAND_INDESELECT            (0x44)
#define PN532_COMMAND_INRELEASE             (0x52)
#define PN532_COMMAND_INSELECT              (0x54)
#define PN532_COMMAND_INAUTOPOLL            (0x60)
#define PN532_COMMAND_TGINITASTARGET        (0x8C)
#define PN532_COMMAND_TGSETGENERALBYTES     (0x92)
#define PN532_COMMAND_TGGETDATA             (0x86)
#define PN532_COMMAND_TGSETDATA             (0x8E)
#define PN532_COMMAND_TGSETMETADATA         (0x94)
#define PN532_COMMAND_TGGETINITIATORCOMMAND (0x88)
#define PN532_COMMAND_TGRESPONSETOINITIATOR (0x90)
#define PN532_COMMAND_TGGETTARGETSTATUS     (0x8A)

#define PN532_RESPONSE_INDATAEXCHANGE       (0x41)
#define PN532_RESPONSE_INLISTPASSIVETARGET  (0x4B)

#define PN532_WAKEUP                        (0x55)

#define PN532_SPI_STATREAD                  (0x02)
#define PN532_SPI_DATAWRITE                 (0x01)
#define PN532_SPI_DATAREAD                  (0x03)
#define PN532_SPI_READY                     (0x01)

#define PN532_I2C_ADDRESS                   (0x48 >> 1)
#define PN532_I2C_READBIT                   (0x01)
#define PN532_I2C_BUSY                      (0x00)
#define PN532_I2C_READY                     (0x01)
#define PN532_I2C_READYTIMEOUT              (20)

#define PN532_MIFARE_ISO14443A              (0x00)

// Mifare Commands
#define MIFARE_CMD_AUTH_A                   (0x60)
#define MIFARE_CMD_AUTH_B                   (0x61)
#define MIFARE_CMD_READ                     (0x30)
#define MIFARE_CMD_WRITE                    (0xA0)
#define MIFARE_CMD_TRANSFER                 (0xB0)
#define MIFARE_CMD_DECREMENT                (0xC0)
#define MIFARE_CMD_INCREMENT                (0xC1)
#define MIFARE_CMD_STORE                    (0xC2)
#define MIFARE_ULTRALIGHT_CMD_WRITE         (0xA2)

#define MIFARE_UID_MAX_LENGTH               MIFARE_UID_TRIPLE_LENGTH
#define MIFARE_UID_SINGLE_LENGTH            (4)
#define MIFARE_UID_DOUBLE_LENGTH            (7)
#define MIFARE_UID_TRIPLE_LENGTH            (10)
#define MIFARE_KEY_LENGTH                   (6)
#define MIFARE_BLOCK_LENGTH                 (16)

// NTAG2xx Commands
#define NTAG2XX_BLOCK_LENGTH                (4)

// Prefixes for NDEF Records (to identify record type)
#define NDEF_URIPREFIX_NONE                 (0x00)
#define NDEF_URIPREFIX_HTTP_WWWDOT          (0x01)
#define NDEF_URIPREFIX_HTTPS_WWWDOT         (0x02)
#define NDEF_URIPREFIX_HTTP                 (0x03)
#define NDEF_URIPREFIX_HTTPS                (0x04)
#define NDEF_URIPREFIX_TEL                  (0x05)
#define NDEF_URIPREFIX_MAILTO               (0x06)
#define NDEF_URIPREFIX_FTP_ANONAT           (0x07)
#define NDEF_URIPREFIX_FTP_FTPDOT           (0x08)
#define NDEF_URIPREFIX_FTPS                 (0x09)
#define NDEF_URIPREFIX_SFTP                 (0x0A)
#define NDEF_URIPREFIX_SMB                  (0x0B)
#define NDEF_URIPREFIX_NFS                  (0x0C)
#define NDEF_URIPREFIX_FTP                  (0x0D)
#define NDEF_URIPREFIX_DAV                  (0x0E)
#define NDEF_URIPREFIX_NEWS                 (0x0F)
#define NDEF_URIPREFIX_TELNET               (0x10)
#define NDEF_URIPREFIX_IMAP                 (0x11)
#define NDEF_URIPREFIX_RTSP                 (0x12)
#define NDEF_URIPREFIX_URN                  (0x13)
#define NDEF_URIPREFIX_POP                  (0x14)
#define NDEF_URIPREFIX_SIP                  (0x15)
#define NDEF_URIPREFIX_SIPS                 (0x16)
#define NDEF_URIPREFIX_TFTP                 (0x17)
#define NDEF_URIPREFIX_BTSPP                (0x18)
#define NDEF_URIPREFIX_BTL2CAP              (0x19)
#define NDEF_URIPREFIX_BTGOEP               (0x1A)
#define NDEF_URIPREFIX_TCPOBEX              (0x1B)
#define NDEF_URIPREFIX_IRDAOBEX             (0x1C)
#define NDEF_URIPREFIX_FILE                 (0x1D)
#define NDEF_URIPREFIX_URN_EPC_ID           (0x1E)
#define NDEF_URIPREFIX_URN_EPC_TAG          (0x1F)
#define NDEF_URIPREFIX_URN_EPC_PAT          (0x20)
#define NDEF_URIPREFIX_URN_EPC_RAW          (0x21)
#define NDEF_URIPREFIX_URN_EPC              (0x22)
#define NDEF_URIPREFIX_URN_NFC              (0x23)

#define PN532_GPIO_VALIDATIONBIT            (0x80)

/* Official PN532 Errors Definitions */
#define PN532_ERROR_NONE                                                (0x00)
///< Time Out, the target has not answered
#define PN532_ERROR_TIMEOUT                                             (0x01)
///< A CRC error has been detected by the CIU
#define PN532_ERROR_CRC                                                 (0x02)
///< A Parity error has been detected by the CIU
#define PN532_ERROR_PARITY                                              (0x03)

/**
 * During an anti-collision/select operation (ISO/IEC14443-3 Type A and
 * ISO/IEC18092 106 kbps passive mode), an erroneous Bit Count has been
 * detected
 */
#define PN532_ERROR_COLLISION_BITCOUNT                                  (0x04)
///< Framing error during MIFARE operation
#define PN532_ERROR_MIFARE_FRAMING                                      (0x05)

/**
 * An abnormal bit-collision has been detected during bit wise
 * anti-collision at 106 kbps
 */
#define PN532_ERROR_COLLISION_BITCOLLISION                              (0x06)
///< Communication buffer size insufficien
#define PN532_ERROR_NOBUFS                                              (0x07)
///< RF Buffer overflow has been detected by the CI
#define PN532_ERROR_RFNOBUFS                                            (0x09)

/**
 * In active communication mode, the RF field has not been switched on
 * in time by the counterpart (as defined in NFCIP-1 standard
 */
#define PN532_ERROR_ACTIVE_TOOSLOW                                      (0x0a)
///< RF Protocol error
#define PN532_ERROR_RFPROTO                                             (0x0b)

/**
 * Temperature error: the internal temperature sensor has detected
 * overheating, and therefore has automatically switched off the
 * antenna drivers
 */
#define PN532_ERROR_TOOHOT                                              (0x0d)
///< Internal buffer overflow
#define PN532_ERROR_INTERNAL_NOBUFS                                     (0x0e)
///< Invalid parameter (range, format...)
#define PN532_ERROR_INVAL                                               (0x10)

/**
 * DEP Protocol: The PN533 configured in target mode does not support
 * the command received from the initiator (the command received is not
 * one of the following: ATR_REQ, WUP_REQ, PSL_REQ, DEP_REQ, DSL_REQ,
 * RLS_REQ)
 */
#define PN532_ERROR_DEP_INVALID_COMMAND                                 (0x12)

/**
 * DEP Protocol, MIFARE or ISO/IEC14443-4: The data format does not
 * match to the specification.  Depending on the RF protocol used, it
 * can be: Bad length of RF received frame, Incorrect value of PCB or
 * PFB, Invalid or unexpected RF received frame, NAD or DID incoherence.
 */
#define PN532_ERROR_DEP_BADDATA                                         (0x13)
///< MIFARE: Authentication error
#define PN532_ERROR_MIFARE_AUTH                                         (0x14)
///< Target or Initiator does not support NFC Secur
#define PN532_ERROR_NOSECURE                                            (0x18)
///< I2C bus line is Busy. A TDA transaction is on going
#define PN532_ERROR_I2CBUSY                                             (0x19)
///< ISO/IEC14443-3: UID Check byte is wrong
#define PN532_ERROR_UIDCHECKSUM                                         (0x23)
/**
 * DEP Protocol: Invalid device state, the system is in a state which
 * does not allow the operation
 */
#define PN532_ERROR_DEPSTATE                                            (0x25)
///< Operation not allowed in this configuration (host controller interface)
#define PN532_ERROR_HCIINVAL                                            (0x26)

/**
 * This command is not acceptable due to the current context of the
 * PN533 (Initiator vs. Target, unknown target number, Target not in the
 * good state, ...)
 */
#define PN532_ERROR_CONTEXT                                             (0x27)
///< The PN533 configured as target has been released by its initiator
#define PN532_ERROR_RELEASED                                            (0x29)
/**
 * PN533 and ISO/IEC14443-3B only: the ID of the card does not match,
 * meaning that the expected card has been exchanged with another one.
 */
#define PN532_ERROR_CARDSWAPPED                                         (0x2a)
/**
 * PN533 and ISO/IEC14443-3B only: the card previously activated has
 * disappeared.
 */
#define PN532_ERROR_NOCARD                                              (0x2b)
/**
 * Mismatch between the NFCID3 initiator and the NFCID3 target in DEP
 * 212/424 kbps passive.
 */
#define PN532_ERROR_MISMATCH                                            (0x2c)
///< An over-current event has been detected
#define PN532_ERROR_OVERCURRENT                                         (0x2d)
///< NAD missing in DEP frame
#define PN532_ERROR_NONAD                                               (0x2e)
/* End: Official PN532 Errors Definitions */

///< Other Error Definitions
#define PN532_STATUS_ERROR                                              (-1)
#define PN532_STATUS_OK                                                 (0)

///<the baud rate and the modulation type to be used during the initialization
    enum Card_Baud {
        TypeA_106kbps = 0x00,
        FeliCa_212kbps = 0x01,
        FeliCa_424kbps = 0x02,
        TypeB_106kbps = 0x03,
        Jewel_106kbps = 0x04,
    };


//包结构体
#pragma pack(1)
    typedef struct {
        uint8_t ic;///< Version of the IC. For PN532, the contain of this byte is 0x32
        uint8_t ver;///<Version of the firmware
        uint8_t rev;///<Revision of the firmware
        /**
         * indicates which are the functionalities supported by the firmware.
         * When the bits are set to 1, the functionality is supported, otherwise (bit set to 0) it is not.
         * 7    6   5   4   3   2           1                       0
         * RFU  RFU RUF RFU RFU ISO18092    ISO/IEC14443 TypeB      ISO/IEC14443 TypeA
         */
        uint8_t support;
    } FirmwareVersion;

    typedef struct {
        uint8_t tg;///<target
        uint8_t sens_res[2];///<The first byte is the MSB, the second one the LSB.
        uint8_t sel_res;
        uint8_t nfcid_length;///<nfc id length
        uint8_t nfcid[CID_MAX_LEN];
    } PassiveTarget_ISOIEC1443TypeA;


#pragma pack()
    //base

    /**
     * pn532组包，发送数据
     * @param out out 输出数据
     * @param len_out out 输出数据长度
     * @param data in 输入数据 D4/D5 CC/CC+1 data
     * @param len 输入数据长度
     * @return PN532_STATUS_ERROR：失败 PN532_STATUS_OK：成功
     */
    int PN532_WriteFrame(uint8_t *out, uint16_t *len_out, uint8_t *data, uint16_t len);

    /**
     * PN532 解包，发送数据
     * @param out out 解包数据
     * @param len_out out 解包数据的长度
     * @param data in 原始数据
     * @param len in 原始数据长度
     * @return PN532_STATUS_ERROR：失败 PN532_STATUS_OK：成功
     */
    int PN532_ReadFrame(uint8_t *out, uint16_t *len_out, uint8_t *data, uint16_t len);

    /**
     * PN532 解包 回复信息
     * @param cmd out 回复命令，一般是发送命令+1
     * @param error out 错误 0x00：成功，其他：参考 PN532_ERROR_XXX
     * @param out out 回复信息指针
     * @param len_out out 回复信息长度
     * @param data in 原始数据
     * @param len in 原始数据长度
     * @return 0x00：成功，其他：参考 PN532_ERROR_XXX
     * @note 原始数据例子：
     *      成功带数据：00 00 FF 00 FF 00 00 00 FF 13 ED D5 41 00 54 45 53 54 00 00 00 00 00 00 00 00 00 00 00 00 AA 00
     *      成功不带数据：00 00 FF 00 FF 00 00 00 FF 03 FD D5 41 00 EA 00
     *      失败：00 00 FF 00 FF 00 00 00 FF 03 FD D5 41 27 C3 00
     */
    int PN532_GetResponse(uint8_t *cmd, uint8_t *error, uint8_t *out, uint16_t *len_out, uint8_t *data, uint16_t len);

    /**
     * PN532 组包 回复信息
     * @param out out 输出数据
     * @param len_out out 输出数据长度
     * @param cmd 回复的命令，一般
     * @param error 0x00：成功 其他：参考 PN532_ERROR_XXX
     * @param data 回复信息中数据
     * @param len 回复信息中数据长度
     * @return 回复信息长度
     */
    int Pn532_SetResponse(uint8_t *out, uint16_t *len_out, uint8_t cmd, uint8_t error, uint8_t *data, uint16_t len);

    //fun

    /**
     * 组包。获取固件版本号
     * @param out out输出数据
     * @param len out输出数据长度
     * @return PN532_STATUS_ERROR：失败 PN532_STATUS_OK：成功
     */
    int PN532_SetInfo_GetFirmwareVersion(uint8_t *out, uint16_t *len);

    /**
     * 解包，解析出固件版本信息
     * @param firmwareVersion out 固件版本信息
     * @param data in 数据包
     * @param len in 数据包长度
     * @return
     */
    int PN532_GetInfo_GetFirmwareVersion(FirmwareVersion &firmwareVersion, uint8_t *data, uint16_t len);

    /**
     * 组包，配置设备读取M1卡
     * @param out out 组包数据
     * @param len out 组包数据长度
     * @return PN532_STATUS_ERROR：失败 PN532_STATUS_OK：成功
     */
    int PN532_SetInfo_SamConfiguration(uint8_t *out, uint16_t *len);

    /**
     * 组包，获取M1卡 uid
     * @param out out 组包数据
     * @param len out 组包数据长度
     * @param card_baud in 波特率 详见 Card_Baud
     * @return PN532_STATUS_ERROR：失败 PN532_STATUS_OK：成功
     */
    int PN532_SetInfo_ReadPassiveTarget(uint8_t *out, uint16_t *len, uint8_t card_baud);

    /**
     * 组包，认证M1卡的指定块
     * @param out out 组包数据
     * @param len out 组包数据长度
     * @param uid in card uid
     * @param uid_length in card uid len
     * @param block_number in 要认证的块编号
     * @param key_type 密钥类型 (like MIFARE_CMD_AUTH_A or MIFARE_CMD_AUTH_B)
     * @param key in 密钥
     * @return PN532 error code.
     */
    int PN532_SetInfo_MifareClassicAuthenticateBlock(uint8_t *out, uint16_t *len, uint8_t *uid, uint16_t uid_length,
                                                     uint16_t block_number, uint16_t key_type, uint8_t *key);

    /**
     * 组包,读取指定块内容 M1卡
     * @param out out 组包数据
     * @param len out 组包数据长度
     * @param block_number in 块编号
     * @return PN532 error code.
     */
    int PN532_SetInfo_MifareClassicReadBlock(uint8_t *out, uint16_t *len, uint16_t block_number);

    /**
     * 组包，向指定块内写入数据 M1卡
     * @param out out 组包数据
     * @param len out 组包数据长度
     * @param block_number in 块编码
     * @param data in 写入数据
     * @param data_len in 写入数据的长度
     * @return
     */
    int PN532_SetInfo_MifareClassicWriteBlock(uint8_t *out, uint16_t *len, uint16_t block_number, uint8_t *data,
                                              uint16_t data_len);

    /**
     * 组包，读取指定块内容 tag2xx
     * @param out out 组包数据
     * @param len out 组包数据长度
     * @param block_number in 块编码
     * @return PN532 error code.
     */
    int PN532_SetInfo_Ntag2xxReadBlock(uint8_t *out, uint16_t *len, uint16_t block_number);

    /**
     * 组包，写入指定块内容 tag2xx
     * @param out out 组包数据
     * @param len out 组包数据长度
     * @param block_number in 块编号
     * @param data in 写入数据
     * @param data_len 写入数据长度
     * @return PN532 error code.
     */
    int PN532_SetInfo_Ntag2xxWriteBlock(uint8_t *out, uint16_t *len, uint16_t block_number, uint8_t *data,
                                        uint16_t data_len);

    /**
     * 组包，读取GPIO状态
     * @brief  pin state buffer (3 bytes) returned.
     * returns 3 bytes containing the pin state where:
     *     P3[0] = P30,   P7[0] = 0,   I[0] = I0,
     *     P3[1] = P31,   P7[1] = P71, I[1] = I1,
     *     P3[2] = P32,   P7[2] = P72, I[2] = 0,
     *     P3[3] = P33,   P7[3] = 0,   I[3] = 0,
     *     P3[4] = P34,   P7[4] = 0,   I[4] = 0,
     *     P3[5] = P35,   P7[5] = 0,   I[5] = 0,
     *     P3[6] = 0,     P7[6] = 0,   I[6] = 0,
     *     P3[7] = 0,     P7[7] = 0,   I[7] = 0,
     * @param out out 组包数据
     * @param len out 组包数据长度
     * @return PN532 error code.
     */
    int PN532_SetInfo_ReadGpio(uint8_t *out, uint16_t *len);

    /**
     * 组包，设置GPIO状态
     * @brief 要设置某一个gpio状态，首先应读出所有状态，然后按位去设置
     * @param out out 组包数据
     * @param len out 组包数据长度
     * @param data in 写入数据
     * @param data_len 写入数据长度，必须等于2
     * @return
     */
    int PN532_SetInfo_WriteGpio(uint8_t *out, uint16_t *len, uint8_t *data, uint16_t data_len);


    ////具体的操作

    /**
     * 组包，唤醒PN532 ，操作第一步
     * @param out out 组包数据
     * @param len out 组包数据长度
     * @return
     */
    int PN532_SetInfo_WakeUp(uint8_t *out, uint16_t *len);

    /**
     * 组包，获取当前卡id
     * @param out out 组包数据
     * @param len out 组包数据长度
     * @return
     */
    int PN532_SetInfo_GetCID(uint8_t *out, uint16_t *len);

    /**
     * 解包，获取当前卡id
     * @param vector_passiveTarget out 当前卡信息数组
     * @param data in 数据包
     * @param len in 数据包长度
     * @return
     */
    int PN532_GetInfo_GetCID(vector<PassiveTarget_ISOIEC1443TypeA> &vector_passiveTarget, uint8_t *data, uint16_t len);
}


#endif //_PN532_H
