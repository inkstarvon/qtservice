#include "Ink_service.h"

Ink_service* Ink_service::m_pInstance = nullptr;
QMutex Ink_service::m_Mutex;
int Ink_service::sigtermFd[2] = {0, 0};

Ink_service::Ink_service(int argc, char **argv , QString name, QObject *parent)
    : QObject(parent)
    , QtService<QCoreApplication>(argc, argv, name) {
    qDebug() << Q_FUNC_INFO << name;
    setServiceDescription(name);

    // 创建 socketpair
    if (socketpair(AF_UNIX, SOCK_STREAM, 0, sigtermFd) != 0) {
        qFatal("Couldn't create TERM socketpair");
    }

    workerThread = new QThread(this);
    connect(workerThread, &QThread::started, this, [this] {
        // 创建 QSocketNotifiers
        if(!snTerm) snTerm = new QSocketNotifier(sigtermFd[1], QSocketNotifier::Read);
        connect(snTerm, &QSocketNotifier::activated, this, &Ink_service::handleSigTerm);
    });

    // 注册信号处理函数
    struct sigaction term;
    term.sa_handler = Ink_service::termSignalHandler;
    sigemptyset(&term.sa_mask);
    term.sa_flags = 0;
    sigaction(SIGTERM, &term, nullptr);

    workerThread->start();
}

Ink_service::~Ink_service(){
    qDebug() << Q_FUNC_INFO;
    if( workerThread ){
        if (workerThread->isRunning()) {
            workerThread->quit();
            workerThread->wait();
        }
        QObject::disconnect(workerThread, nullptr, nullptr, nullptr);
        workerThread->deleteLater();
        workerThread = nullptr;
    }

    stop();
}


void Ink_service::handleSigTerm() {
    qDebug() << Q_FUNC_INFO;
    char tmp;
    ::read(sigtermFd[1], &tmp, sizeof(tmp));
    m_pInstance->m_delete.~Relieve();
}

void Ink_service::termSignalHandler(int) {
    qDebug() << Q_FUNC_INFO;
    char tmp = 1;
    ::write(sigtermFd[0], &tmp, sizeof(tmp));

    m_pInstance->m_delete.~Relieve();

}
