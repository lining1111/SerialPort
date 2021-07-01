//
// Created by lining on 6/8/21.
//

#include <cstring>
#include "pn532.h"
#include <iostream>

using namespace std;

namespace pn532 {

    int PN532_WriteFrame(uint8_t *out, uint16_t *len_out, uint8_t *data, uint16_t len) {
        if (len > PN532_FRAME_MAX_LENGTH || len < 1) {
            return PN532_STATUS_ERROR;
        }

        // Build frame to send as:
        // - Preamble (0x00)
        // - Start code  (0x00, 0xFF)
        // - Command length (1 byte)
        // - Command length checksum
        // - Command bytes
        // - Checksum
        // - Postamble (0x00)

        uint8_t frame[PN532_FRAME_MAX_LENGTH + 7];
        bzero(frame, sizeof(frame));
        uint8_t checksum = 0;

        //Preamble
        frame[0] = PN532_PREAMBLE;
        //Start code
        frame[1] = PN532_STARTCODE1;
        frame[2] = PN532_STARTCODE2;

        for (uint8_t i = 0; i < 3; i++) {
            checksum += frame[i];
        }

        //length
        frame[3] = len & 0xFF;
        //length checksum
        frame[4] = (~len + 1) & 0xFF;

        for (int i = 0; i < len; i++) {
            frame[5 + i] = data[i];
            checksum += data[i];
        }
        //Checksum
        frame[len + 5] = ~checksum & 0xff;
        //Postamble
        frame[len + 6] = PN532_POSTAMBLE;

        //copy
        memcpy(out, frame, (len + 7));
        *len_out = len + 7;

        return PN532_STATUS_OK;
    }

    int PN532_ReadFrame(uint8_t *out, uint16_t *len_out, uint8_t *data, uint16_t len) {
        uint8_t frame[PN532_FRAME_MAX_LENGTH + 7];
        bzero(frame, sizeof(frame));
        uint8_t checksum = 0;

        //copy
        memcpy(frame, data, len);
        uint8_t offset = 0;

        //find PN532_FRAME_START 0x00 0x00 0xff
        bool find_start = false;
        do {
            //必须所有的相同
            for (int i = 0; i < sizeof(PN532_FRAME_START); i++) {
                if (data[offset + i] == PN532_FRAME_START[i]) {
                    find_start = true;
                } else {
                    find_start = false;
                    break;
                }
            }
            //找到则退出，否则向后找
            if (find_start) {
                break;
            } else {
                offset++;
            }
        } while (offset < (len - sizeof(PN532_FRAME_START)));

        if (!find_start) {
            return PN532_STATUS_ERROR;
        }
        //check len len checksum
        uint8_t data_len = frame[offset + 3];
        uint8_t data_len_checksum = frame[offset + 4];

        if (((data_len + data_len_checksum) & 0xff) != 0) {
            cout << "len checksum fail" << endl;
            return PN532_STATUS_ERROR;
        }

        //check data checksum D4/D5---data data_checksum
        for (int i = 0; i < data_len + 1; i++) {
            checksum += frame[offset + 5 + i];
        }
        checksum &= 0xff;
        if (checksum != 0) {
            cout << "data checksum fail" << endl;
            return PN532_STATUS_ERROR;
        }

        //copy data
        memcpy(out, &frame[offset + 5], data_len);
        *len_out = data_len;

        return PN532_STATUS_OK;
    }

    int PN532_GetResponse(uint8_t *cmd, uint8_t *error, uint8_t *out, uint16_t *len_out, uint8_t *data, uint16_t len) {

        int index_start = 0;

        //find PN532_ACK
        bool find = false;
        do {
            //必须所有的相同
            for (int i = 0; i < sizeof(PN532_ACK); i++) {
                if (data[index_start + i] == PN532_ACK[i]) {
                    find = true;
                } else {
                    find = false;
                    break;
                }
            }
            //找到则退出，否则向后找
            if (find) {
                break;
            } else {
                index_start++;
            }

        } while (index_start < (len - sizeof(PN532_ACK)));

        if (!find) {
            return PN532_STATUS_ERROR;
        }

        //get response frame
        uint8_t response_frame[255] = {0};
        uint16_t response_frame_len = 0;
        bzero(response_frame, sizeof(response_frame));
        memcpy(response_frame, &data[index_start + sizeof(PN532_ACK)], (len - (index_start + sizeof(PN532_ACK))));
        response_frame_len = (len - (index_start + sizeof(PN532_ACK)));
        //get response data
        uint8_t response_data[255] = {0};
        uint16_t response_data_len = 0;
        bzero(response_data, sizeof(response_data));
        PN532_ReadFrame(response_data, &response_data_len, response_frame, response_frame_len);
        //get uint response_data[0] = D5
        *cmd = response_data[1];
        *error = response_data[2];
        memcpy(out, &response_data[3], (response_data_len - 3));
        *len_out = (response_data_len - 3);

        return *error;
    }

    int Pn532_SetResponse(uint8_t *out, uint16_t *len_out, uint8_t cmd, uint8_t error, uint8_t *data, uint16_t len) {
        uint8_t frame[255] = {0};
        uint16_t frame_index = 0;
        bzero(frame, sizeof(frame));

        //get response frame
        uint8_t data_in[255] = {0};
        uint16_t data_in_len = 0;
        bzero(data_in, sizeof(data_in));

        //D5
        data_in[0] = PN532_PN532TOHOST;
        data_in[1] = cmd;
        data_in[2] = error;
        data_in_len += 3;
        if (len > 0) {
            memcpy(&data_in[data_in_len], data, len);
            data_in_len += len;
        }
        //write frame
        PN532_WriteFrame(frame, &frame_index, data_in, data_in_len);

        //copy response_start
        int index = 0;
        memcpy(&out[index], PN532_ACK, sizeof(PN532_ACK));
        index += sizeof(PN532_ACK);
        //copy response data
        memcpy(&out[index], frame, frame_index);
        index += frame_index;
        *len_out = index;

        return index;
    }

    int PN532_SetInfo_GetFirmwareVersion(uint8_t *out, uint16_t *len) {
        int ret = 0;
        uint8_t cmd = PN532_COMMAND_GETFIRMWAREVERSION;
        uint8_t frame[255] = {0};
        uint16_t frame_index = 0;
        bzero(frame, sizeof(frame));

        frame[0] = PN532_HOSTTOPN532;
        frame[1] = cmd;
        frame_index += 2;

        ret = PN532_WriteFrame(out, len, frame, frame_index);
        return ret;
    }

    int PN532_GetInfo_GetFirmwareVersion(FirmwareVersion &firmwareVersion, uint8_t *data, uint16_t len) {
        int ret = 0;

        if (len >= sizeof(firmwareVersion)) {
            memcpy(&firmwareVersion, data, sizeof(firmwareVersion));
            ret = PN532_STATUS_OK;
        } else {
            ret = PN532_STATUS_ERROR;
        }

        return ret;
    }


    int PN532_SetInfo_SamConfiguration(uint8_t *out, uint16_t *len) {
        int ret = 0;
        uint8_t cmd = PN532_COMMAND_SAMCONFIGURATION;
        uint8_t frame[255] = {0};
        uint16_t frame_index = 0;
        bzero(frame, sizeof(frame));

        frame[0] = PN532_HOSTTOPN532;
        frame[1] = cmd;
        frame_index += 2;

        // Send SAM configuration command with configuration for:
        // - 0x01, normal mode
        // - 0x14, timeout 50ms * 20 = 1 second
        // - 0x01, use IRQ pin
        uint8_t params[3] = {0x01, 0x14, 0x01};

        //format
        memcpy(&frame[frame_index], params, sizeof(params));
        frame_index += sizeof(params);

        ret = PN532_WriteFrame(out, len, frame, frame_index);
        return ret;
    }

    int PN532_SetInfo_ReadPassiveTarget(uint8_t *out, uint16_t *len, uint8_t card_baud) {
        int ret = 0;
        uint8_t cmd = PN532_COMMAND_INLISTPASSIVETARGET;
        uint8_t frame[255] = {0};
        uint16_t frame_index = 0;
        bzero(frame, sizeof(frame));

        frame[0] = PN532_HOSTTOPN532;
        frame[1] = cmd;
        frame_index += 2;

        // Send passive read command for 1 card.  Expect at most a 7 byte UUID.
        uint8_t params[2] = {0x01, card_baud};

        //format
        memcpy(&frame[frame_index], params, sizeof(params));
        frame_index += sizeof(params);

        ret = PN532_WriteFrame(out, len, frame, frame_index);
        return ret;
    }

    int PN532_SetInfo_MifareClassicAuthenticateBlock(uint8_t *out, uint16_t *len, uint8_t *uid, uint16_t uid_length,
                                                     uint16_t block_number, uint16_t key_type, uint8_t *key) {
        int ret = 0;
        uint8_t cmd = PN532_COMMAND_INDATAEXCHANGE;
        uint8_t frame[255] = {0};
        uint16_t frame_index = 0;
        bzero(frame, sizeof(frame));

        frame[0] = PN532_HOSTTOPN532;
        frame[1] = cmd;
        frame_index += 2;

        // Build parameters for InDataExchange command to authenticate MiFare card.
        uint8_t params[3 + MIFARE_UID_MAX_LENGTH + MIFARE_KEY_LENGTH];
        bzero(params, sizeof(params));
        params[0] = 0x01;
        params[1] = key_type & 0xFF;
        params[2] = block_number & 0xFF;
        // params[3:3+keylen] = key
        for (uint8_t i = 0; i < MIFARE_KEY_LENGTH; i++) {
            params[3 + i] = key[i];
        }
        // params[3+keylen:] = uid
        for (uint8_t i = 0; i < uid_length; i++) {
            params[3 + MIFARE_KEY_LENGTH + i] = uid[i];
        }

        //format
        memcpy(&frame[frame_index], params, (3 + MIFARE_KEY_LENGTH + uid_length));
        frame_index += (3 + MIFARE_KEY_LENGTH + uid_length);

        ret = PN532_WriteFrame(out, len, frame, frame_index);
        return ret;
    }

    int PN532_SetInfo_MifareClassicReadBlock(uint8_t *out, uint16_t *len, uint16_t block_number) {
        int ret = 0;
        uint8_t cmd = PN532_COMMAND_INDATAEXCHANGE;
        uint8_t frame[255] = {0};
        uint16_t frame_index = 0;
        bzero(frame, sizeof(frame));

        frame[0] = PN532_HOSTTOPN532;
        frame[1] = cmd;
        frame_index += 2;

        uint8_t params[3] = {0x01, MIFARE_CMD_READ, (uint8_t) (block_number & 0xFF)};

        //format
        memcpy(&frame[frame_index], params, sizeof(params));
        frame_index += sizeof(params);

        ret = PN532_WriteFrame(out, len, frame, frame_index);
        return ret;
    }

    int PN532_SetInfo_MifareClassicWriteBlock(uint8_t *out, uint16_t *len, uint16_t block_number, uint8_t *data,
                                              uint16_t data_len) {
        int ret = 0;
        uint8_t cmd = PN532_COMMAND_INDATAEXCHANGE;
        uint8_t frame[255] = {0};
        uint16_t frame_index = 0;
        bzero(frame, sizeof(frame));

        frame[0] = PN532_HOSTTOPN532;
        frame[1] = cmd;
        frame_index += 2;

        uint8_t params[MIFARE_BLOCK_LENGTH + 3];
        bzero(params, sizeof(params));
        params[0] = 0x01;  // Max card numbers
        params[1] = MIFARE_CMD_WRITE;
        params[2] = block_number & 0xFF;
        for (uint8_t i = 0; i < data_len; i++) {
            params[3 + i] = data[i];
        }

        //format
        memcpy(&frame[frame_index], params, sizeof(params));
        frame_index += sizeof(params);

        ret = PN532_WriteFrame(out, len, frame, frame_index);
        return ret;
    }

    int PN532_SetInfo_Ntag2xxReadBlock(uint8_t *out, uint16_t *len, uint16_t block_number) {
        int ret = 0;
        uint8_t cmd = PN532_COMMAND_INDATAEXCHANGE;
        uint8_t frame[255] = {0};
        uint16_t frame_index = 0;
        bzero(frame, sizeof(frame));

        frame[0] = PN532_HOSTTOPN532;
        frame[1] = cmd;
        frame_index += 2;

        uint8_t params[3] = {0x01, MIFARE_CMD_READ, (uint8_t) (block_number & 0xFF)};

        //format
        memcpy(&frame[frame_index], params, sizeof(params));
        frame_index += sizeof(params);

        ret = PN532_WriteFrame(out, len, frame, frame_index);
        return ret;
    }

    int PN532_SetInfo_Ntag2xxWriteBlock(uint8_t *out, uint16_t *len, uint16_t block_number, uint8_t *data,
                                        uint16_t data_len) {
        int ret = 0;
        uint8_t cmd = PN532_COMMAND_INDATAEXCHANGE;
        uint8_t frame[255] = {0};
        uint16_t frame_index = 0;
        bzero(frame, sizeof(frame));

        frame[0] = PN532_HOSTTOPN532;
        frame[1] = cmd;
        frame_index += 2;

        uint8_t params[NTAG2XX_BLOCK_LENGTH + 3];
        bzero(params, sizeof(params));
        params[0] = 0x01;  // Max card numbers
        params[1] = MIFARE_ULTRALIGHT_CMD_WRITE;
        params[2] = block_number & 0xFF;
        for (uint8_t i = 0; i < data_len; i++) {
            params[3 + i] = data[i];
        }

        //format
        memcpy(&frame[frame_index], params, sizeof(params));
        frame_index += sizeof(params);

        ret = PN532_WriteFrame(out, len, frame, frame_index);
        return ret;
    }

    int PN532_SetInfo_ReadGpio(uint8_t *out, uint16_t *len) {
        int ret = 0;
        uint8_t cmd = PN532_COMMAND_READGPIO;
        uint8_t frame[255] = {0};
        uint16_t frame_index = 0;
        bzero(frame, sizeof(frame));

        frame[0] = PN532_HOSTTOPN532;
        frame[1] = cmd;
        frame_index += 2;

        ret = PN532_WriteFrame(out, len, frame, frame_index);
        return ret;
    }

    int PN532_SetInfo_WriteGpio(uint8_t *out, uint16_t *len, uint8_t *data, uint16_t data_len) {
        int ret = 0;
        uint8_t cmd = PN532_COMMAND_INDATAEXCHANGE;
        uint8_t frame[255] = {0};
        uint16_t frame_index = 0;
        bzero(frame, sizeof(frame));

        frame[0] = PN532_HOSTTOPN532;
        frame[1] = cmd;
        frame_index += 2;

        uint8_t params[2];
        // 0x80, the validation bit.
        if (data_len > 2) {
            data_len = 2;
        }
        for (int i = 0; i < data_len; i++) {
            params[i] = 0x80 | data[i];
        }

        //format
        memcpy(&frame[frame_index], params, sizeof(params));
        frame_index += sizeof(params);

        ret = PN532_WriteFrame(out, len, frame, frame_index);
        return ret;
    }

    int PN532_SetInfo_WakeUp(uint8_t *out, uint16_t *len) {
        int ret = 0;

        //唤醒是固定的编码 55 55 00 00 00 00 00 00 00 00 00 00 00 00 00 00 FF 03 FD D4 14 01 17 00
        uint8_t wakeup[24] = {0x55, 0x55, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                              0x00, 0x00, 0xFF, 0x03, 0xFD, 0xD4, 0x14, 0x01, 0x17, 0x00};

        memcpy(out, wakeup, sizeof(wakeup));
        *len = sizeof(wakeup);

        ret = PN532_STATUS_OK;
        return ret;
    }

    int PN532_SetInfo_GetCID(uint8_t *out, uint16_t *len) {
        int ret = 0;

        uint8_t card_baud = 0x00;
        ret = PN532_SetInfo_ReadPassiveTarget(out, len, card_baud);

        return ret;
    }

    int PN532_GetInfo_GetCID(vector<PassiveTarget_ISOIEC1443TypeA> &vector_passiveTarget, uint8_t *data, uint16_t len) {
        int ret = 0;

        uint8_t card_num = data[0];//最小0 最大2

        //依次读取卡信息
        int card_index = 0;
        int data_index = 1;
        do {
            PassiveTarget_ISOIEC1443TypeA item;
            //1.先获取id前面的数据，主要获取id长度信息
            memcpy(&item, &data[data_index], 5);
            data_index += 5;
            memcpy(&item.nfcid, &data[data_index], item.nfcid_length);
            data_index += item.nfcid_length;

            vector_passiveTarget.push_back(item);
            card_index++;
        } while (card_index < card_num);

        ret = PN532_STATUS_OK;
        return ret;
    }


}