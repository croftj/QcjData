/*********************************************************************************
**
**   $Id$
**   Copyright (c) 2006-2024 Joe Croft joe@croftj.net
**   
**   This file is part of Qcj Classes.
**
**   QcjHttpService.cpp is free software; you can redistribute it and/or modify
**   it under the terms of the GNU General Public License as published by
**   the Free Software Foundation; either version 2 of the License, or
**   (at your option) any later version.
**
**   QcjData Class Libraries is distributed in the hope that it will be useful,
**   but WITHOUT ANY WARRANTY; without even the implied warranty of
**   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**   GNU General Public License for more details.
**
**   You should have received a copy of the GNU General Public License
**   along with QcjData Class Libraries; if not, write to the Free Software
**   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
**
*********************************************************************************/

# include "QcjHttpService.h"
# include <QUrl>

/*!
       \class QcjHttpService
       
       \brief Http protocol service
       
       This  class  is a base class for providing core functionalitity
       for  an  http service. It provides basic protocol packaging and
       unpackaging.  It  is  expected  to be subclassed to provide the
       actual  functionality.

       The actual processing of requests are performed on a seperate thread 
       from the main application. While the new thread which is created
       for this object handles parsing the requests, processing then and 
       formulating appropriate responses, the main application thread
       is still responsible for handling the basic IO for the object. The 
       passing of data back and forth from the IO and this object is done
       through signals and slots and a couple of buffers. This is mandated
       by the fact that the Qt library cannot handle socket operations to be
       called by a thread from which the socket wasn't created under.
*/


/*!
       \fn     QcjHttpService::QcjHttpService(int    socketDescripter,
       QObject  *parent,  int max_req, QStringList *extraMethods, long
       ttl)
       
       This  is  the constructor. It creates it's QThread object, then
       sets  up  to  handle  requests  from the socket whose number is
       passed  in  the parameter \a socketDescripter. The parameter \a
       max_req  specifies  how many requests the service should handle
       before  quiting.  The  parameter \a ttl specified how long thie
       service should stay a live in milliseconds.
       
       By  default,  the  HTTP  methods  GET,  POST, PUT, HEAD, TRACE,
       OPTIONS   and   DELETE  are  recognized.  For  flexibility  and
       custimization,  additional  methods  may be passed in using the
       parameter \a extraMethods.
*/ 

QcjHttpService::QcjHttpService(int socketDescripter, QObject *parent, int max_req, QStringList *extraMethods, long ttl) : 
   QThread(parent)
{
//   printf("QcjHttpService::QcjHttpService(%s): Enter, socknum = %d\n", qPrintable(QString::number(currentThreadId())), socknum);
//   fflush(stdout);
   if ( socketDescripter >= 0 ) 
   {
      socknum = socketDescripter;

      timeToLive = ttl;
      maxRequests = max_req;
      methods << "GET";
      methods << "POST";
      methods << "PUT";
      methods << "HEAD";
      methods << "TRACE";
      methods << "OPTIONS";
      methods << "DELETE";
      if ( extraMethods != 0 ) 
         methods << *extraMethods;

      if ((sock = new QTcpSocket()) == NULL)
      {
//         printf("QcjHttpService::QcjHttpService(%s): Error creating socket\n", qPrintable(QString::number(currentThreadId())));
//         fflush(stdout);
         return;
      }
//      printf("QcjHttpService::QcjHttpService(%s): setting descripter\n", qPrintable(QString::number(currentThreadId())));
//      fflush(stdout);
      if (! sock->setSocketDescriptor(socknum)) {
//         printf("QcjHttpService::QcjHttpService(%s): Exit in error\n", qPrintable(QString::number(currentThreadId())));
//         fflush(stdout);
         return;
      }
//      printf("QcjHttpService::QcjHttpService(%s): Connecting signals and handles\n", qPrintable(QString::number(currentThreadId())));
//      fflush(stdout);

      /********************************************************/
      /* Add the socket to the list of open sockets so they   */
      /* can be easily closed later if this object is deleted */
      /********************************************************/
      m_openSocketList.append(sock);
      connect(sock, SIGNAL(readyRead()),                          this, SLOT(haveData()), Qt::QueuedConnection);
      connect(sock, SIGNAL(disconnected()),                       this, SLOT(haveDisconnect()), Qt::QueuedConnection);
      connect(sock, SIGNAL(error(QAbstractSocket::SocketError)),  this, SLOT(haveError(QAbstractSocket::SocketError)));

      inBuffer.setBuffer(&inArray);
      inBuffer.open(QBuffer::ReadWrite);
      inStream.setDevice(&inBuffer);
      inBody = false;
      request = 0;
   }
//   printf("QcjHttpService::QcjHttpService(%s): exit\n", qPrintable(QString::number(currentThreadId())));
//   fflush(stdout);
}

/*!
       \fn  run()
       
       This function should not be called directly.
       
       This  function is called when the start() function is called for
       the  object.  The thread remains alive as long as this function
       does not exit. Once run exits, the thread is killed.
*/ 
void QcjHttpService::run()
{
   QString method;
   QString path;
   QString version;
   bool err = false;

//   printf("QcjHttpService::run(%s): Enter, socknum = %d\n", qPrintable(QString::number(currentThreadId())), socknum);
//   fflush(stdout);

   ttlTimer = new QTimer;
   connect(ttlTimer, SIGNAL(timeout()), this, SLOT(haveTimeOut()));
   ttlTimer->start(timeToLive);

   errTimer = new QTimer;
   connect(errTimer, SIGNAL(timeout()), this, SLOT(haveTimeOut()));

   exitFlag = false;
   myThreadId = currentThreadId();
   lineBuffer.clear();
   bufLock.lock();
   while (1) 
   {
      if ( exitFlag ) 
      {
//         printf("QcjHttpService::run(%s): exit flag set, exiting!\n", qPrintable(QString::number(currentThreadId())));
//         fflush(stdout);
         clearLocks();
//         printf("QcjHttpService::run(%s): Calling return\n", qPrintable(QString::number(currentThreadId())));
//         fflush(stdout);
         return;
      }

      if ( inArray.size() == 0 )
      {
//         printf("QcjHttpService::run(%s): waiting input, sock %d\n", qPrintable(QString::number(currentThreadId())), socknum);
//         fflush(stdout);
         haveInput.wait(&bufLock);
      }

      if ( exitFlag ) 
      {
//         printf("QcjHttpService::run(%s): exit flag set, exiting!\n", qPrintable(QString::number(currentThreadId())));
//         fflush(stdout);
         clearLocks();
//         printf("QcjHttpService::run(%s): Calling return\n", qPrintable(QString::number(currentThreadId())));
//         fflush(stdout);
         return;
      }

      if ( ! inBody ) 
      {
//         printf("QcjHttpService::run(%s): Not in body, have %d bytes available\n", qPrintable(QString::number(currentThreadId())), inArray.size());
//         fflush(stdout);
         while ( inArray.size() > 0 )
         {
            char ch;
            ch = *(inArray.data());
            inArray.remove(0, 1);
            bufLock.unlock();
            lineBuffer += ch;
            if ( ch == '\0' ) 
            {
//               printf("QcjHttpService::run(%s): have null byte\n", qPrintable(QString::number(currentThreadId())));
//               fflush(stdout);
               bufLock.lock();
               continue;
            }

            if ( ch == '\n' )
            {
//               printf("QcjHttpService::run(%s): Have line: |%s|\n", qPrintable(QString::number(currentThreadId())), qPrintable(lineBuffer));
//               fflush(stdout);
               errTimer->start(500);
               QString in;
               in.append(lineBuffer);
               lineBuffer.clear();
               in.remove('\r');
               in.remove('\n');
               if ( request == 0 ) 
               {
//                  printf("QcjHttpService::run(%s): Processing request\n", qPrintable(QString::number(currentThreadId())));
//                  fflush(stdout);
                  request = new QMap<QString, QVariant>;

                  method.clear();
                  path.clear();
                  version.clear();
                  QStringList sl = in.split(" ");

//                  printf("QcjHttpService::run(%s): Have %d request parts\n", qPrintable(QString::number(currentThreadId())), sl.size());
//                  fflush(stdout);

                  if ( sl.size() > 0 ) 
                     method = sl[0];
                  if ( sl.size() > 1 ) 
                     path = sl[1];
                  if ( sl.size() > 2 ) 
                     version = sl[2];
                  else 
                     err = true;

//                  printf("QcjHttpService::run(%s): method = |%s|\n", qPrintable(QString::number(currentThreadId())), qPrintable(method));
//                  fflush(stdout);
                  if ( err ) 
                  {
//                     printf("QcjHttpService::run(%s): Returning error and exiting\n", qPrintable(QString::number(currentThreadId())));
//                     fflush(stdout);
                     errorResponse(QcjHttpService::BadRequest, "Malformed Request");
                     delete request;
                     request = 0;
                  }
                  else if ( methods.contains(method) )
                  {
//                     printf("QcjHttpService::run(%s): Have valid method\n", qPrintable(QString::number(currentThreadId())));
//                     fflush(stdout);
                     QString args("");
                     QString resource("");
                     QStringList reslist = path.split("?");
                     if ( reslist.size() == 2 ) 
                     {
                        resource = reslist[0];
                        args = reslist[1];
                     }
                     else
                        resource = path;

                     request->insert("Resource", resource);
                     request->insert("Arguments", args);
                     request->insert("Method", method);
                     request->insert("Version", version);
                     request->insert("Content-Length", QVariant((qlonglong)0));
                  }
                  else
                  {
                     errorResponse(QcjHttpService::BadRequest, "Invalid method: " + method);
                     delete request;
                     request = 0;
                  }
               }
               else if ( in.size() > 0 )
               {
                  QRegExp re("(.+): (.*)");
                  re.setMinimal(true);

//                  printf("QcjHttpService::run(%s): Testing for tag\n", qPrintable(QString::number(currentThreadId())));
//                  fflush(stdout);
                  QStringList sl = in.split(": ");
                  if ( sl.size() == 1 ) 
                  {
                     sl = in.split(":");
                  }
                  if ( sl.size() == 2 )
                  {
                     QString tag = sl[0];
                     QString val = sl[1];
//                     printf("QcjHttpService::run(%s): tag = |%s|, val = |%s|\n", qPrintable(QString::number(currentThreadId())), qPrintable(tag), qPrintable(val));
//                     fflush(stdout);
                     bool isint;
                     long x = val.toLong(&isint);
                     if ( isint ) 
                        request->insert(tag, (qlonglong)x);
                     else
                        request->insert(tag, val);
                  }
               }
               else 
               {
                  expectingBytes = request->value("Content-Length").toLongLong();
//                  printf("QcjHttpService::run(%s): content length = %ld\n", qPrintable(QString::number(currentThreadId())), (long)expectingBytes);
//                  fflush(stdout);
                  if ( expectingBytes == 0 ) 
                  {
//                     printf("QcjHttpService::run(%s): Not getting any data\n", qPrintable(QString::number(currentThreadId())));
//                     fflush(stdout);
                     QMap<QString, QVariant> response;
                     errTimer->stop();
//                     printf("QcjHttpService::run(%s): Calling processRequest()\n", qPrintable(QString::number(currentThreadId())));
//                     fflush(stdout);
                     processRequest(request, &response);
                     processResponse(response);
//                     printf("QcjHttpService::run(%s): Clearing bufs etc.()\n", qPrintable(QString::number(currentThreadId())));
//                     fflush(stdout);
                     delete request;
                     request = 0;
                  }
                  else
                  {
                     inBody = true;
                     break;
                  }
               }
            }
            bufLock.lock();
         }
      }
//      printf("QcjHttpService::run(%s): waiting for more data to come in\n", qPrintable(QString::number(currentThreadId())));
//      fflush(stdout);
   }
//   printf("QcjHttpService::run(%s): Exit\n", qPrintable(QString::number(currentThreadId())));
//   fflush(stdout);
}

/*!
       \fn void QcjHttpService::haveData()
       
       This  function  recieves  incomming data from the socket. It is
       not  actually executed as part of the thread, but by the thread
       the socket was created in.
*/ 
void QcjHttpService::haveData()
{
//   printf("QcjHttpService::haveData(%s): Enter bytes avail = %ld\n", qPrintable(QString::number(currentThreadId())), (long)sock->bytesAvailable());
//   fflush(stdout);
   while (sock->bytesAvailable() > 0) 
   {
      char buf[1024];
      int bytes;
      if ( (bytes = sock->read(buf, sizeof(buf))) > 0 )
      {
         bufLock.lock();
//         printf("QcjHttpService::haveData(%s): appending %d bytes () to inArray\n", qPrintable(QString::number(currentThreadId())), bytes);
//         fflush(stdout);
         inArray.append(QByteArray(buf, bytes));
//         printf("QcjHttpService::haveData(%s): inArray has %d bytes available\n", qPrintable(QString::number(currentThreadId())), inArray.size());
//         fflush(stdout);
         bufLock.unlock();
      }
      else if ( bytes < 0 )
         return;
   }
//   printf("QcjHttpService::haveData(%s): waking thread\n", qPrintable(QString::number(currentThreadId())));
//   fflush(stdout);
   haveInput.wakeAll();
//   printf("QcjHttpService::haveData(%s): Exit\n", qPrintable(QString::number(currentThreadId())));
//   fflush(stdout);
}


/*!
       \fn      void     QcjHttpService::processResponse(QMap<QString,
       QVariant> rsp)
       
       This  function  processes  responses and sends them out. If the
       maximum  number  of  requests have been fulfilled, it will emit
       the signal closeSocket triggering this threads ultimate demise.
*/ 
void QcjHttpService::processResponse(QMap<QString, QVariant> rsp)
{
//   printf("QcjHttpService::processResponse(%s): Enter\n", qPrintable(QString::number(currentThreadId())));
//   fflush(stdout);
   sendResponse(&rsp);

//   printf("QcjHttpService::processResponse(%s): maxRequests = %d\n", qPrintable(QString::number(currentThreadId())), maxRequests);
//   fflush(stdout);
   if ( --maxRequests == 0) 
   {
//      printf("QcjHttpService::processResponse(%s): terminating socket connection\n", qPrintable(QString::number(currentThreadId())));
//      fflush(stdout);
      emit closeSocket(sock);
   }
//   printf("QcjHttpService::processResponse(%s): Exit\n", qPrintable(QString::number(currentThreadId())));
//   fflush(stdout);
}

/*!
       \fn  void  QcjHttpService::sendResponse(QMap<QString, QVariant>
       *rsp)
       
       This  function  formulates the actual response and emits a send
       signal to have it written to the socket.
*/ 
void QcjHttpService::sendResponse(QMap<QString, QVariant> *rsp)
{
   QByteArray body;

//   printf("QcjHttpService::sendResponse(%s): Enter\n", qPrintable(QString::number(currentThreadId())));
//   fflush(stdout);

   QMapIterator<QString, QVariant> i(*rsp);
   while (i.hasNext()) 
   {
      i.next();
//      if ( i.key() == "Request-Body" ) 
//         printf("DbPhotoService::sendResponse(%s): rsp: |%s| %d length\n", qPrintable(QString::number(currentThreadId())), qPrintable(i.key()), i.value().toByteArray().size());
//      else
//         printf("DbPhotoService::sendResponse(%s): rsp: |%s| = |%s|\n", qPrintable(QString::number(currentThreadId())), qPrintable(i.key()), qPrintable(i.value().toString()));
//      fflush(stdout);
   }

   if ( rsp->value("Status").toInt() != QcjHttpService::OK ) 
   {
//      printf("QcjHttpService::sendResponse(%s): Sending error response\n", qPrintable(QString::number(currentThreadId())));
//      fflush(stdout);
      emit send(sock, qPrintable("HTTP/1.1 " + rsp->value("Status").toString() + " " + rsp->value("StatusString").toString() + "\r\n"));
//      printf("QcjHttpService::sendResponse(%s): returning\n", qPrintable(QString::number(currentThreadId())));
//      fflush(stdout);
      return;
   }

//   printf("QcjHttpService::sendResponse(%s): Sending regular response\n", qPrintable(QString::number(currentThreadId())));
//   fflush(stdout);
   emit send(sock, qPrintable("HTTP/1.1 " + rsp->value("Status").toString() + " " + rsp->value("StatusString").toString() + "\r\n"));

//   printf("QcjHttpService::sendResponse(%s): Iterating headers\n", qPrintable(QString::number(currentThreadId())));
//   fflush(stdout);
   QMapIterator<QString, QVariant> it(*rsp);
   while (it.hasNext()) 
   {
      it.next();
//      printf("QcjHttpService::sendResponse(%s): fetching key\n", qPrintable(QString::number(currentThreadId())));
//      fflush(stdout);
      QString tag = it.key();
      if ( tag == "Request-Body" ) 
      {
//         printf("QcjHttpService::sendResponse(%s): Setting body\n", qPrintable(QString::number(currentThreadId())));
//         fflush(stdout);
         body = it.value().toByteArray();
      }
      else 
      {
//         printf("QcjHttpService::sendResponse(%s): Sending tag\n", qPrintable(QString::number(currentThreadId())));
//         fflush(stdout);
         QString val = it.value().toString();
         emit send(sock, qPrintable(tag + ": " + val + "\r\n"));
      }
   }
//   printf("QcjHttpService::sendResponse(%s): Sending body\n", qPrintable(QString::number(currentThreadId())));
//   fflush(stdout);

   if ( body.size() > 0 ) 
   {
      emit send(sock, "\r\n");
      emit send(sock, body);
   }
//   printf("QcjHttpService::sendResponse(%s): Exit\n", qPrintable(QString::number(currentThreadId())));
//   fflush(stdout);
}

/*!
      \fn void QcjHttpService::haveTimeout()

       This  slot is called when senders are too slow sending in their
       requests. It generates a error.
*/ 
void QcjHttpService::haveTimeout()
{
   errorResponse(QcjHttpService::RequestTimeout, "You're too slow!");
}

/*!
      \fn void QcjHttpService::haveError(QAbstractSocket::SocketError err)
       This slot is called when an error is detected on the socket. It
       sets up to terminate the thread.
*/ 
void QcjHttpService::haveError(QAbstractSocket::SocketError)
{
//   printf("QcjHttpService::haveError(%s): Enter, flagging thread %s to exit\n", qPrintable(QString::number(currentThreadId())), qPrintable(QString::number(myThreadId)));
//   fflush(stdout);
   exitFlag = true;
   haveInput.wakeAll();
//   printf("QcjHttpService::haveError(%s): Exit\n", qPrintable(QString::number(currentThreadId())));
//   fflush(stdout);
}


/*!
       \fn  void  QcjHttpService::haveDisconnect() This slot is called
       when the socket closes. It sets up to terminate the thread.
*/ 
void QcjHttpService::haveDisconnect()
{
//   printf("QcjHttpService::haveDisconnect(%s): Enter, flagging thread %s to exit\n", qPrintable(QString::number(currentThreadId())), qPrintable(QString::number(myThreadId)));
//   fflush(stdout);
   exitFlag = true;
   m_openSocketList.removeAll(sock);

   haveInput.wakeAll();
//   printf("QcjHttpService::haveDisconnect(%s): Exit\n", qPrintable(QString::number(currentThreadId())));
//   fflush(stdout);
}

/*!
      \fn void QcjHttpService::errorResponse(enum ErrorCodes ec, QString msg)

       This function creates an error response and sends it out
*/ 
void QcjHttpService::errorResponse(enum ErrorCodes ec, QString msg)
{
   QMap<QString, QVariant> responses;
   responses.insert("Status", QString::number(ec));
   responses.insert("StatusString", msg);
   sendResponse(&responses);
   emit closeSocket(sock);
}

/*!
       \fn                    QMap<QString,                   QString>
       QcjHttpService::parseArguments(QString args)
       
       This function parses the incomming request string and returns a
       map of arguments and their values as a QMap.
*/ 
QMap<QString, QString> QcjHttpService::parseArguments(QString args)
{
   QMap<QString, QString> rv;
   QStringList sl = args.split("&");
   for (int x = 0; x < sl.size(); x++) 
   {
      QStringList sl1 = sl[x].split("=");
      if ( sl1.size() == 2 ) 
         rv.insert(sl1[0], QUrl::fromPercentEncoding(sl1[1].toUtf8()));
   }
   return(rv);
}

/*!
       \fn void QcjHttpService::haveTimeOut()
       
       This  is  called  when  the  ttl  time  expires.  It  emits the
       closeSocket() signal triggering the threads demise.
*/ 
void QcjHttpService::haveTimeOut()
{
//   printf("QcjHttpService::haveTimeOut(%s): Enter\n", qPrintable(QString::number(currentThreadId())));
//   fflush(stdout);
   emit closeSocket(sock);
//   printf("QcjHttpService::haveTimeOut(%s): Exit\n", qPrintable(QString::number(currentThreadId())));
//   fflush(stdout);
}









