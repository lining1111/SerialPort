//
// Created by lining on 7/1/21.
//

#include "pn532.h"
#include <vector>

using namespace std;
using namespace pn532;

int main(int argc, char *argv[]){
    uint8_t data[10]={0x01, 0x01, 0x00, 0x04, 0x08, 0x04, 0x5C, 0x61, 0x91, 0xFB};

    vector<PassiveTarget_ISOIEC1443TypeA> vector_passiveTarget;

    PN532_GetInfo_GetCID(vector_passiveTarget,data, sizeof(data));

}