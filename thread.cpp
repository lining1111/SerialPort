#include "thread.h"
#include "pn532.h"

using namespace pn532;

Thread::Thread() {
    m_stopFlag = false;
}

Thread::~Thread() {
    stop();
}

void Thread::stop() {
    QMutexLocker locker(&mutex);
    m_stopFlag = true;
}

void Thread::run() {
    m_stopFlag = false;
    uint8_t read_buf[1024];
    int read_len = 0;
    while (1) {
        {
            QMutexLocker locker(&mutex);
            if (m_stopFlag)
                break;
        }
        if (!canRecv)
            continue;

        bzero(read_buf, sizeof(read_buf));
        read_len = port->readSerialPort(5000, read_buf, sizeof(read_buf));
        if (read_len > 0) {

            canRecv = false;
            emit sendDateToRecvText(read_buf, read_len);

            //解析回复
            uint8_t cmd = 0;
            uint8_t recv_data[1024];
            uint16_t recv_len = 0;
            bzero(recv_data, sizeof(recv_data));
            if (cardReaderType == PN532) {
                PN532_GetResponse(&cmd, recv_data, &recv_len, read_buf, read_len);
                qDebug("cmd: %02x", cmd);
                qDebug("recv data len: %d", recv_len);
                if (recv_len > 0) {
                    string print_data = "recv data: ";
                    for (int i = 0; i < recv_len; i++) {
                        char value[4];
                        bzero(value, sizeof(value));
                        sprintf(value, "%02x ", recv_data[i]);
                        print_data.append(value);
                    }
                    qDebug(print_data.c_str());
                }

            }
        }
    }
    m_stopFlag = false;
}
