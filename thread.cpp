#include "thread.h"

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
//             qDebug() << "read: " << read_buf;
//            uint8_t recv_buf[1024];
//            int recv_len = 0;
//            bzero(recv_buf, sizeof(recv_buf));
//            memcpy(recv_buf, read_buf, read_len);
//            recv_len = read_len;
//            emit sendDateToRecvText(recv_buf, recv_len);

            canRecv = false;
            emit sendDateToRecvText(read_buf,read_len);
        }
    }
    m_stopFlag = false;
}
