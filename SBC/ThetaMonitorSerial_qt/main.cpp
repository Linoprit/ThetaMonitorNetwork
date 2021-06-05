#include <QCoreApplication>
#include "worker_class.h"



worker_class            *work_loop;


int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    work_loop       = new worker_class();



    return a.exec();
}
