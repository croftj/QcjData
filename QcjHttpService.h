/*********************************************************************************
**
**   $Id$
**   Copyright (c) 2006 Joe Croft joe@croftj.net
**   
**   This file is part of Qcj Classes.
**
**   QcjHttpService.h is free software; you can redistribute it and/or modify
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
# ifndef QCJHTTPSERVICE_H
# define QCJHTTPSERVICE_H

# include <QBuffer>
# include <QDataStream>
# include <QMap>
# include <QMutex>
# include <QString>
# include <QStringList>
# include <QTcpSocket>
# include <QThread>
# include <QTimer>
# include <QVariant>
# include <QWaitCondition>

class QcjHttpService  : public QThread
{
   Q_OBJECT

public:
   QcjHttpService(int socketDescripter, QObject *parent = 0, int max_req = 10, 
                  QStringList *extraMethods = 0, long ttl = 200);

   ~QcjHttpService()
   {
      foreach(QTcpSocket *sock, m_openSocketList)
      {
         qDebug() << __FUNCTION__ << "Closing socket " << (unsigned long int)sock;
         emit closeSocket(sock);
      }
   }

   typedef QList<QTcpSocket*> NetSocketList_t;

   /*!
          \enum ErrorCodes
          
          These names provide a symbolic method to reference the various
          status codes which are defined the the w3c.

          \li \c 200 OK
          \li \c 201 Created            
          \li \c 202 Accepted           
          \li \c 203 NoAuthorizedContent
          \li \c 204 NoContent          
          \li \c 205 RevertContent      
          \li \c 206 PartialContent     

          \li \c 300 MultipleChoices
          \li \c 301 MovedPermanently 
          \li \c 302 Found            
          \li \c 303 SeeOther         
          \li \c 304 NotModified      
          \li \c 305 UseProxy         
          \li \c 306 Unused           
          \li \c 307 TemporaryRedirect

          \li \c 400 BadRequest
          \li \c 401 Unauthorized
          \li \c 402 PaymentRequired
          \li \c 403 Forbidden
          \li \c 404 NotFound
          \li \c 405 MethodNotAllowed
          \li \c 406 NotAcceptable
          \li \c 407 ProxyAuthenticationRequired
          \li \c 408 RequestTimeout
          \li \c 409 Conflict
          \li \c 410 Gone
          \li \c 411 LengthRequired
          \li \c 412 PreconditionFailed
          \li \c 413 RequestEntityTooLong
          \li \c 414 RequestURITooLong
          \li \c 415 UnsupportedMediaType
          \li \c 416 RequestedRangeNotSatisfiable
          \li \c 417 ExpectationFailed

          \li \c 500 InternalServerError 
          \li \c 501 NotImplimented
          \li \c 502 BadGateway
          \li \c 503 ServiceUnavailable
          \li \c 504 GatewayTimeout
          \li \c 505 HTTPVersionNotSupported
   
   
   */ 
   enum ErrorCodes
   {
      OK = 200, Created, Accepted, NonAuthorizedContent, NoContent, 
               RevertContent, PartialContent,
      MultipleChoices = 300, MovedPermanently, Found, SeeOther, 
               NotModified, UseProxy, Unused, TemporyRedirect,
      BadRequest = 400, Unauthorized, PaymentRequired, Forbidden, NotFound,
               MethodNotAllowed, NotAcceptable, ProxyAuthenticationRequired, 
               RequestTimeout, Conflict, Gone, LengthRequired, 
               PreconditionFailed, RequestEntityTooLong, RequestURITooLong, 
               UnsupportedMediaType, RequestedRangeNotSatisfiable, 
               ExpectationFailed,
      InternalServerError = 500, NotImplimented, BadGateway, ServiceUnavailable, 
               GatewayTimeout, HTTPVersionNotSupported
   };

   /*!
          \fn void addMethods(QStringList *extraMethods)
          
          This  function  allows  the  caller  to  add additional HTTP
          methods which will be recognized by this object.
   */ 
   void addMethods(QStringList *extraMethods)
   {
      if ( extraMethods != 0 ) 
         methods << *extraMethods;
   };

   /*!
          void setMaxRequests(int max_req)
          
          This  function  sets  the  maximum  number of requests to be
          processed by this object.
   */ 
   void setMaxRequests(int max_req)
   {
      maxRequests = max_req;
   };

   /*!
          \fn void setTimeToLive(long ttl)
          
          This  function  sets  the  time,  in  milliseconds, for this
          object to process requests before dying.
   */ 
   void setTimeToLive(long ttl)
   {
      timeToLive = ttl;
   };

signals:
   void error(QAbstractSocket::SocketError);
   void send(QTcpSocket*, QByteArray);
   void closeSocket(QTcpSocket*);

protected slots:
   void haveData();
   void haveTimeout();
   void haveError(QAbstractSocket::SocketError);
   void haveDisconnect();
   void errorResponse(enum ErrorCodes ec, QString msg);
   void processResponse(QMap<QString, QVariant> rsp);
   void haveTimeOut();

private:

protected:
   virtual void processRequest(QMap<QString, QVariant> *, QMap<QString, QVariant> *) {};
   virtual void clearLocks() {};

   void sendResponse(QMap<QString, QVariant> *rsp);
   QMap<QString, QString> parseArguments(QString args);
   void run();

   NetSocketList_t                  m_openSocketList;
   QMap<QString, QVariant>          *request;
   QStringList                      methods;
   QByteArray                       inArray;
   QBuffer                          inBuffer;
   QDataStream                      inStream;
   QString                          lineBuffer;
   QTimer                           *ttlTimer;
   QTimer                           *errTimer;
   int                              maxRequests;
   long                             timeToLive;
   int                              socknum;
   bool                             exitFlag;
   bool                             inBody;
   Qt::HANDLE                       myThreadId;
   qlonglong                        expectingBytes;
   QMutex                           bufLock;
   QWaitCondition                   haveInput;
   QTcpSocket                       *sock;
};

# endif

