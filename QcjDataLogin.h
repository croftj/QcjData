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
************************o*********************************************************/
# ifndef DATALOGIN_H
#define  DATALOGIN_H

# include <QString>
#include <stdlib.h>
#include <stdio.h>

# include "ui_QcjDataLogin.h"

#define  QPSQL    "QPSQL"
#define  QOCI     "QOCI"
#define  QSQLITE  "QSQLITE"
#define  QMYSQL   "QMYSQL"
#define  QODBC    "QODBC"
#define  QDB2     "QDB2"
#define  QTDS     "QTDS"
#define  QIBASE   "QIBASE"

class QcjDataLogin : public QDialog
{
   Q_OBJECT

public:
   QcjDataLogin(QWidget *p = 0);
   void initialize();

public slots:
   void Login();
   void quitApp();

private:
   QString        dbType;
   Ui::QcjDataLogin  ui;
};
# endif


