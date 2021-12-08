#ifndef QCJHTTPSERVICEFACTORY_H
#define QCJHTTPSERVICEFACTORY_H

#include <QObject>
#include <QStringList>

#include "QcjHttpService.h"

class QcjHttpServiceFactory : public QObject
{
   Q_OBJECT

public:
   QcjHttpServiceFactory(QObject *parent = 0) :
      QObject(parent)
   { }

   virtual QcjHttpService* newInstance(int socketDescripter, int max_req = 10, 
           QStringList *extraMethods = 0, long ttl = 200) = 0;
};

#endif
