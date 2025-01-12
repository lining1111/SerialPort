#ifndef THREAD
#define THREAD

#include <QThread>
#include <QMutex>
#include <QMutexLocker>
#include <QDebug>
#include "serialport.h"

class Thread : public QThread {
Q_OBJECT
public:
    enum CardReaderType {
        PN532,
    };
public:
    Thread();

    ~Thread();

    void stop();

    void setPort(void *p) { port = (SerialPort *) p; }
//    void setCallBack(void (*p)(void *, void *), void *tmpUi, void *port){ QMutexLocker locker(&mutex); cb = p; ui = tmpUi; this->port = port; }

private:
    bool m_stopFlag;
//    void (*cb)(void *tmpUi, void* port);
//    void *ui;
    SerialPort *port;
    QMutex mutex;
public:
    bool canRecv = true;

    ///< 具体解析使用
    CardReaderType cardReaderType = PN532;

protected:
    void run();

signals:

    void sendDateToRecvText(uint8_t *recv_buf, int recv_len);
};

#endif // THREAD

