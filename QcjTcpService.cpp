/*********************************************************************************
**
**   $Id: //depot/WorkInProgress/photogrotto/photogrotto.cpp#6 $
**   Copyright (c) 2007 Joe Croft joe@croftj.net
**   
**   This file is part of Photogrotto
**
**   Photogrotto is free software; you can redistribute it and/or modify
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

# include <errno.h>
# include <stdio.h>
# include <string.h>
# include <QDebug>
# include <QFile>

# include "QcjTcpService.h"

FILE *logfp;

/*!
       \class QcjTcpServer
       
       \brief  Controlling  class  for  creating  a  http  server  for
       processing incomming connections and requests.
       
       This  class  functions as an  http server which accepts connections
       from   external  clients.  The  definition  of  the  server  is
       specified  in an XML file.
*/ 

/*!
       \fn QcjTcpServer::QcjTcpServer(QString hostname, , QObject *parent)
       
       This functioncreates a QcjTcpServer object. The  definition  of 
       the  server  is specified in an XML file. the file named by the
       parameter  \a  confName.  The Parameter \a parent points to the
       parent object of this one is any.
*/ 
QcjTcpServer::QcjTcpServer(QHostAddress address, int port, QObject *parent) :
      QTcpServer(parent),
      m_address(address),
      m_port(port)
{
   logfp = stderr;

   /*
          Open the XML file for the application.
   */ 
   fprintf(logfp, "stderr- listening on port %d\n", m_port);
   qDebug() << "listening on port" << (int)m_port;
   if ( ! listen(m_address, m_port) ) 
   {
      fprintf(logfp, "Error listening\n");
      return;
   }
}

/*!
    \fn void QcjTcpServer::setServiceFactory(QcjHttpService *factory)

    This function sets the pointer to an object that will create objects
    for handling the incomming requests from an http connection
*/
void QcjTcpServer::setServiceFactory(QcjHttpServiceFactory *factory, int max_req, 
                                     QStringList *extraMethods, long ttl)
{
    m_factory = factory;
    m_maxRequests = max_req;
    m_extraMethods = extraMethods;
    m_ttl = 200;
}

/*!
      \fn void QcjTcpServer::incomingConnection(qintptr sock)

       This   function  overloads  the  like  named  function  in  the
       QTcpServer class and is called whenever a new tcp connection is
       made.  It  creates  a new DbPhoto Object to handle the requests
       for the object and starts the thread for that object to run on.
       It   also  connects  the  appropriate  signals  and  slots  for
       controlling the thread.
*/ 
void QcjTcpServer::incomingConnection(qintptr sock)
{
   fprintf(logfp, "QcjTcpServer::incomingConnection(): Enter, sock = %d\n", sock);
   fflush(logfp);
   qDebug() << __FUNCTION__ << "Enter, sock = " << sock;

   /*
          Create   a   new   service  object  to  handle  this  socket
          connection.  Connect  the  appropriate  signals and slots to
          control it.
   */ 
   QcjHttpService *p = m_factory->newInstance(sock, m_maxRequests, m_extraMethods, m_ttl);
   connect(p, SIGNAL(send(QTcpSocket*, QByteArray)), this, SLOT(sendData(QTcpSocket*, QByteArray)));
   connect(p, SIGNAL(closeSocket(QTcpSocket*)), this, SLOT(haveCloseSocket(QTcpSocket*)));

   /*
          Start the thread to handle the requests.
   */ 
   p->start();
   fprintf(logfp, "QcjTcpServer::incomingConnection(): exit\n");
   fflush(logfp);
}

/*!
       \fn  void  QcjTcpServer::sendData(QTcpSocket *sock, QByteArray  buf)
       
       This  function  writes the buffer \a buf of data to the socket \a sock.
       This  is  called  by  the  thread handling the requests for the
       socket.
*/ 
void QcjTcpServer::sendData(QTcpSocket *sock, QByteArray buf)
{
   fprintf(logfp, "QcjTcpServer::sendData(): Enter, sending %d bytes to sock %d\n", buf.count(), sock->socketDescriptor());
   fflush(logfp);
   sock->write(buf);
   fprintf(logfp, "QcjTcpServer::sendData(): Exit\n");
   fflush(logfp);
}

/*!
       \fn void QcjTcpServer::haveCloseSocket(QTcpSocket *sock)
       
       This function is called then the socket \a sock is closed. It cleans up the 
       socket object.
*/ 
void QcjTcpServer::haveCloseSocket(QTcpSocket *sock)
{
   fprintf(logfp, "QcjTcpServer::haveCloseSocket(): Enter, sock %d\n", sock->socketDescriptor());
   fflush(logfp);
   sock->disconnectFromHost();
   fprintf(logfp, "QcjTcpServer::haveClocseSocket(): Exit\n");
   fflush(logfp);
}

