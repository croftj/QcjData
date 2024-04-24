/*********************************************************************************
**
**   $Id$
**   Copyright (c) 2005-2024 Joe Croft <joe@croftj.net>
**   
**   This file is part of QcjData Class Libraries.
**
**   QcjData Class Libraries is free software; you can redistribute it and/or modify
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
************************o*********************************************************/
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
