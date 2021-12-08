/*********************************************************************************
**
**   $Id$
**   Copyright (c) 2005 Joe Croft <joe@croftj.net>
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
*********************************************************************************/
#ifndef QCJACTION_H
#define QCJACTION_H

# include <QAction>
# include <QMutex>
# include <QMutexLocker>
# include <stdio.h>

static int QcjActionCounter = 0;
static QMutex QcjActionMutex;

class QcjAction : public QObject 
{
   Q_OBJECT

public:
   QcjAction(QAction * action, QObject *parent = NULL) :
      QObject(parent)
   {
      QMutexLocker lock(&QcjActionMutex);
      myId = ++QcjActionCounter;
      connect(action, SIGNAL(triggered()), this, SLOT(haveTrigger()));
   };

   int id()
   {
      return(myId);
   };

signals:
   void doit(int);

protected slots:
   void haveTrigger()
   {
      printf("QcjAction::haveTrigger(): emitting signal, myId = %d\n", myId);
      fflush(stdout);
      emit doit(myId);
   };

private:
   int            myId;
};

#endif
