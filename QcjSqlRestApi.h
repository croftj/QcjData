/*********************************************************************************
**
**   $Id: $
**   Copyright (c) 2021 Joe Croft croftj@gmail.com
**   
**
**   QcjSqlRestApi is free software; you can redistribute it and/or modify
**   it under the terms of the GNU General Public License as published by
**   the Free Software Foundation; either version 2 of the License, or
**   (at your option) any later version.
**
**   Foobar is distributed in the hope that it will be useful,
**   but WITHOUT ANY WARRANTY; without even the implied warranty of
**   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**   GNU General Public License for more details.
**
**   You should have received a copy of the GNU General Public License
**   along with Foobar; if not, write to the Free Software
**   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
**
*********************************************************************************/
#ifndef QCJSQLRESTAPI_H
#define QCJSQLRESTAPI_H

# include <QString>
# include <QSqlDatabase>
# include <QVariant>
# include "QcjData/QcjHttpService.h"
# include "QcjData/QcjHttpServiceFactory.h"

class QcjSqlRestApi : public QcjHttpService
{
   Q_OBJECT

public:
   QcjSqlRestApi(int sockDescripter, long ttl = 0, QObject *parent = 0) :
      QcjHttpService(sockDescripter, parent, 20, 0, ttl),
      m_db(NULL)
   {
      printf("QcjSqlRestApi::QcjSqlRestApi(%s): sockDescripter = %d\n", qPrintable(QString::number((unsigned long)(currentThreadId()))), sockDescripter);
   };
   
   void setDatabase(QSqlDatabase* db)
   {
      m_db = db;
   }
   
protected:
   void processRequest(QMap<QString, QVariant> *req, QMap<QString, QVariant> *rsp);

private:
   QSqlDatabase*     m_db;
};

class QcjSqlRestApiFactory : public QcjHttpServiceFactory
{
   Q_OBJECT

public:
   QcjSqlRestApiFactory(QObject *parent = 0) :
      QcjHttpServiceFactory(parent)
   { }

   virtual QcjHttpService* newInstance(int socketDescripter, int max_req = 10, 
           QStringList *extraMethods = 0, long ttl = 200)
   {
      QcjSqlRestApi* service = new QcjSqlRestApi(socketDescripter, ttl);
      service->setDatabase(m_db);
      return(service);
   }

   void setDatabase(QSqlDatabase* db)
   {
      m_db = db;
   }

private:
   QSqlDatabase*  m_db;

};

#endif
