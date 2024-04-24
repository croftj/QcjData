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

# include "QcjData/Qcjtruefalse.h"

QcjTrueFalse::QcjTrueFalse(QWidget *p) : QWidget(p)
{
    ui.setupUi(this);
}

void QcjTrueFalse::setText( QString s )
{
   QString s1;
   if ( s.toInt() != 0 || 
        s.toLower() == "yes"  || s.toLower() == "y"  ||
        s.toLower() == "true"  || s.toLower() == "t" )
      s1 = "Yes";
   else 
      s1 = "No";

   ui.QcjYesNoSelect->setCurrentIndex(ui.QcjYesNoSelect->findText(s1));
}

QString QcjTrueFalse::text()
{
   QString s = ui.QcjYesNoSelect->currentText();
   if ( s == "Yes" ) 
      return("1");
   else 
      return("0");
}

void QcjTrueFalse::setLabel( QString )
{
}

