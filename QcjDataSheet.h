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

#ifndef DATASHEET_H
#define DATASHEET_H

#ifndef QT4_DESIGNER_PLUGIN
#include <qsqldatabase.h>
#include <qsqlrecord.h>
#endif
#include <qmap.h>
#include <qstring.h>
#include <QTextBrowser>
#include <vector>
#include "QcjDataHelpers.h"

class QcjDataSheet : public QTextBrowser
{
   Q_OBJECT
   Q_PROPERTY (QString xml_definition READ readXmlDef WRITE writeXmlDef)
   
public:
   QcjDataSheet(QWidget *pParent = 0);
//   QcjDataSheet(const QString &table, int rows, QWidget *pParent, const char *pName);
//   QcjDataSheet(const QcjDataFields *fields, const QString &table, int rows,
//            QWidget *pParent = 0, const char *pName = 0);
   ~QcjDataSheet() {};

   QString readXmlDef() const { return(xmldef); };
   void setDatabase();
   void writeXmlDef(QString s);

public slots:   
   void refresh();
   void setFields(std::vector<struct QcjDataFields> fds);
   void setFilter(QString f);
   void sortBy(int);
   void sortBy(const QString);
   void sortBy(QStringList sl);

signals:
   void doubleClicked();
   void clicked();

protected:
   virtual void mouseDoubleClickEvent ( QMouseEvent * evt );
   virtual void mousePressEvent ( QMouseEvent * evt );
   virtual void mouseReleaseEvent ( QMouseEvent * evt );

private:
   std::vector<struct QcjDataFields> fields;
   QString                    dbTable;
   QString                    filter;
   QStringList                sort;
   QMap<int, QString>         fieldNames;
   QMap<QString, QString>     fieldTypes;
   QString                    xmldef;
   bool                       havePressed;
};
#endif

