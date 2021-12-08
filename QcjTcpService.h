/*********************************************************************************
**
**   $Id: //depot/WorkInProgress/photogrotto/photogrotto.h#3 $
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

# include <QHostAddress>
# include <QString>
# include <QStringList>
# include <QTcpServer>

# include "QcjHttpServiceFactory.h"

#ifndef TCPSERVICE_H
#define TCPSERVICE_H

class QcjTcpServer : public QTcpServer
{
   Q_OBJECT

public:
   QcjTcpServer(QHostAddress address, int port, QObject *parent = 0);
   void setServiceFactory(QcjHttpServiceFactory *factory, int max_req= 10, 
                          QStringList *extraMethods = 0, long ttl = 200);

protected slots:
   void sendData(QTcpSocket*, QByteArray);
   void haveCloseSocket(QTcpSocket*);

protected:
   virtual void incomingConnection(qintptr sock);
   void getTcpParms();

private:
   QString                 m_err;
   QString                 m_errorString;
   QcjHttpServiceFactory*  m_factory;
   QHostAddress            m_address;
   int                     m_port;
   int                     m_maxRequests;
   QStringList*            m_extraMethods;
   long                    m_ttl;
};

#endif
