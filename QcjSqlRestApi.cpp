/*********************************************************************************
**
**   $Id$
**   Copyright (c) 2021 Joe Croft croftj@gmail.com
**   
**
**   QcjSqlRestApi is free software; you can redistribute it and/or modify
**   it under the terms of the GNU General Public License as published by
**   the Free Software Foundation; either version 2 of the License, or
**   (at your option) any later version.
**
**   QcjSqlRestApi is distributed in the hope that it will be useful,
**   but WITHOUT ANY WARRANTY; without even the implied warranty of
**   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**   GNU General Public License for more details.
**
**   You should have received a copy of the GNU General Public License
**   along with QcjSqlRestApi; if not, write to the Free Software
**   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
**
*********************************************************************************/
# include <QBuffer>
# include <QByteArray>
# include <QDir>
# include <QFile>
# include <QJsonDocument>
# include <QJsonObject>
# include <QJsonArray>
# include <QSqlError>
# include <QSqlRecord>
# include <QSqlQuery>
# include <QUrl>
# include "QcjSqlRestApi.h"

extern FILE *logfp;

/*!
      \class QcjSqlRestApi
      
      \brief Class for processing individual photo requests.
      
      The      QcjSqlRestApi    class    is a subclass for the
      QcjHttpService  class. It handles requests for individual
      photos.  It  will  resize  and properly set the number of
      colors as requested.
*/ 


/*!
      \fn void QcjSqlRestApi::processRequest(QMap<QString, QVariant> *req, QMap<QString, QVariant> *rsp)

       Processes  the incoming request parameter <em>req</em>. It then
       formulates  a  response  and puts it into the map pointed to by
       the  parameter  <em>req</em>.  Both  parameters  must  point to
       valid, though possibly emtpy QMaps.

       Only the "GET" method is recognized at this time.

       The following Resources are recognized. Anyother requests will cause an error.

       /index.html   // This resource returns a page which makes regular updates
                     // using the stsus.info resource
       /status.info  // This resource returns a json string like this:
         {
            "total_lines": <total number of lines>,
            "sent_lines" : <number of lines sent to the controller>,
            "last_line:  : "<last line sent to the controller>"
         }

      The Arguments string will be ignored.
*/ 
void QcjSqlRestApi::processRequest(QMap<QString, QVariant> *req, QMap<QString, QVariant> *rsp)
{
   int t_height, t_width;

   qDebug() << QString("QcjSqlRestApi::processRequest(%1): Enter\n").arg(QString::number((unsigned long)(currentThreadId())));
   fflush(logfp);

   QMapIterator<QString, QVariant> i(*req);
   while (i.hasNext()) 
   {
      i.next();
      qDebug() << QString("QcjSqlRestApi::processRequest(%1): req: |%2| = |%3|").arg(QString::number((unsigned long)(currentThreadId()))).arg(i.key()).arg(i.value().toString());
      fflush(logfp);
   }

   /***********************************************/
   /*   Get  the  values  from  the  request map  */
   /*   which   are   required  to  satisfy  the  */
   /*   request                                   */
   /***********************************************/
   QString method = req->value("Method").toString();
   QString resource = req->value("Resource").toString();
   QString args = req->value("Arguments").toString();
   QString host = req->value("Host").toString();

   /***********************************************/
   /*   Parse out the arguments from the request  */
   /*   line                                      */
   /***********************************************/
   QMap<QString, QString> arguments = parseArguments(args);

   QString fileType;
   QString fileBase;

   /***********************************************/
   /*   Here we handle the GET requests.          */
   /***********************************************/
   if ( method == "GET" ) 
   {
      if (m_db == NULL || ! m_db->isOpen())
      {
         qDebug() << QString("QcjSqlRestApi::processRequest(%1): No database\n").arg((unsigned long)(currentThreadId()));
         fflush(logfp);
         errorResponse(QcjHttpService::ServiceUnavailable, "Requested database sevice is not available");
         return;
      }
      if (resource != "/query" || ! arguments.contains("q"))
      {
         qDebug() << QString("QcjSqlRestApi::processRequest(%1): Error in resource, exiting\n").arg((unsigned long)(currentThreadId()));
         fflush(logfp);
         errorResponse(QcjHttpService::NotFound, "Requested resource not found, try /query?q=...");
         return;
      }

      /***********************************************/
      /*   Process the image requests here           */
      /***********************************************/
      if (resource == "/query") 
      {
         QJsonDocument jdoc;
         QJsonObject top;

         QString sql = arguments["q"];
         QSqlQuery q1(*m_db);
         q1.prepare(sql);
         qDebug() << "sql = " << sql;
         if (q1.exec())
         {
            top.insert(QString("success"), true);
            if (sql.startsWith("select ", Qt::CaseInsensitive))
            {
               int count = 0;
               QSqlRecord rec = q1.record();
               QJsonArray records;
               while (q1.next())
               {
                  QJsonObject record;
                  for (int x = 0; x < rec.count(); x++)
                  {
                     qDebug() << "fn: " << rec.fieldName(x) << " = " << rec.value(x).toString();
                     record.insert(rec.fieldName(x), q1.value(x).toString());
                  }
                  records.push_back(record);
                  count++;
               }
               top.insert(QString("data"), records);
               top.insert(QString("count"), count);
            }
            else
            {
               top.insert(QString("count"), q1.numRowsAffected());
            }
         }
         else
         {
            QSqlError err = q1.lastError();
            top.insert(QString("success"), false);
            top.insert(QString("database_error"), q1.lastError().databaseText());
            top.insert(QString("driver_error"), q1.lastError().driverText());
            top.insert(QString("native_error"), q1.lastError().nativeErrorCode());
         }
         jdoc.setObject(top);

         /***********************************************/
         /*   Build the response.                       */
         /***********************************************/
         rsp->insert(QString("Status"), QcjHttpService::OK);
         rsp->insert(QString("Content-Length"), QVariant(jdoc.toJson(QJsonDocument::Indented).length()));
         rsp->insert(QString("Content-Type"), QVariant("text/json"));
         rsp->insert(QString("Request-Body"), QVariant(jdoc.toJson(QJsonDocument::Indented)));
      }
   }
   qDebug() << QString("QcjSqlRestApi::processRequest(%1): Exit\n").arg(QString::number((unsigned long)(currentThreadId())));
   fflush(logfp);
}
