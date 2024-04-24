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

# include <QFileDialog>
# include "QcjData/Qcjfileentry.h"

QcjFileEntry::QcjFileEntry(QWidget *p) : QWidget(p)
{
   printf("QcjFileEntry::QcjFileEntry(): Enter\n");
   type = "file";
   ui.setupUi(this);
   connect(ui.browseBtn, SIGNAL(clicked()), this, SLOT(browseFiles()), Qt::UniqueConnection);
   printf("QcjFileEntry::QcjFileEntry(): Exit\n");
}

void QcjFileEntry::setText( QString s )
{
   ui.lineEdit->setText(s);
}

QString QcjFileEntry::text()
{
   return(ui.lineEdit->text());
}

void QcjFileEntry::setLabel( QString )
{
}

void QcjFileEntry::setFilter( QString f )
{
   filter = f;
}

void QcjFileEntry::setType( QString t )
{
   type = t;
}

void QcjFileEntry::browseFiles()
{
   printf("QcjFileEntry::browseFiles(): Enter\n");
   QString s;
   if ( type == "file" ) 
      s = QFileDialog::getOpenFileName( this, QString("Select File"), ui.lineEdit->text(), filter, (QString*)NULL, QFileDialog::DontResolveSymlinks);
   else
      s = QFileDialog::getExistingDirectory( this, QString("Select File"), ui.lineEdit->text());
   if ( s != "" ) {
      ui.lineEdit->setText(s);
   }
   printf("QcjFileEntry::browseFiles(): Exit\n");
}

