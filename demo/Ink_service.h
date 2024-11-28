#ifndef INK_SERVICE_H
#define INK_SERVICE_H

#include <QCoreApplication>
#include <QObject>
#include "qtservice.h"
#include <QDebug>
#include <unistd.h>
#include <QSocketNotifier>
#include <QMutex>
#include <systemd/sd-daemon.h>
#include <signal.h>
#include <sys/socket.h>
#include <QThread>
#include <QSocketNotifier>


class Ink_service : public QObject, public QtService<QCoreApplication>
{
    Q_OBJECT
public:
    static Ink_service* getInstance(int argc, char **argv, QString name, QObject *parent = nullptr)
    {
        if (m_pInstance == nullptr)
        {
            QMutexLocker mlocker(&m_Mutex);  //双检索，支持多线程
            if (m_pInstance == nullptr)
            {
                m_pInstance = new Ink_service(argc,argv,name,parent);
            }
        }
        return m_pInstance;
    }


protected:
    void start() override;
    void stop() override;

private:
    explicit Ink_service(int argc, char **argv, QString name, QObject *parent = nullptr);
    Ink_service(); //禁止构造函数。
    Ink_service& operator ==(const Ink_service&); //禁止赋值拷贝函数。
    virtual  ~Ink_service();

    static void termSignalHandler(int);
    static Ink_service* m_pInstance;   //类的指针
    static QMutex m_Mutex;
    static int sigtermFd[2];
    QSocketNotifier *snTerm = nullptr;
    QThread *workerThread = nullptr;


public:
    class Relieve  //专用来析构 类指针的类
    {
    public:
        ~Relieve()
        {
            if (m_pInstance != nullptr)
            {
                m_pInstance->deleteLater();
                m_pInstance = nullptr;
            }
        }
    };
    static Relieve m_delete;


private slots:
    void handleSigTerm();


};

#endif // INK_SERVICE_H
