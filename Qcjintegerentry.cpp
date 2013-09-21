/*********************************************************************************
**
**   $Id$
**   Copyright (c) 2007 Joe Croft <joe@croftj.net>
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

# include <QValidator>
# include "Qcjintegerentry.h"

QcjIntegerEntry::QcjIntegerEntry(QWidget *p) : QWidget(p)
{
   ui.setupUi(this);
   QValidator *validator = new QIntValidator(ui.lineEdit);
   ui.lineEdit->setValidator( validator );
}

void QcjIntegerEntry::setText( QString s )
{
   ui.lineEdit->setText(s);
}

void QcjIntegerEntry::setLabel( QString )
{
}

QString QcjIntegerEntry::text()
{
   return(ui.lineEdit->text());
}

void QcjIntegerEntry::setMin(int i)
{
   ((QIntValidator*)ui.lineEdit->validator())->setBottom(i);
}

void QcjIntegerEntry::setMax(int i)
{
   ((QIntValidator*)ui.lineEdit->validator())->setTop(i);
}

