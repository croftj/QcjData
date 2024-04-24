/*********************************************************************************
**
**   $Id$
**   Copyright (c) 2007-2024 Joe Croft <joe@croftj.net>
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

# ifndef QCJINTEGERENTRY_H
#define  QCJINTEGERENTRY_H

# include <QString>
# include "ui_Qcjintegerentry.h"

class QcjIntegerEntry : public QWidget
{
   Q_OBJECT

public:
   QcjIntegerEntry(QWidget *p = 0);
   void setText( QString s );
   void setLabel( QString s );
   QString text();
   void setMin(int i);
   void setMax(int i);

private:
   Ui::QcjIntegerEntry ui;
};
# endif

