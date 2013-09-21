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

//#include <qfont.h>
//#include <qfontmetrics.h>
#ifndef QT4_DESIGNER_PLUGIN
#include <qpainter.h>
#include <qsqldatabase.h>
#include <qsqldriver.h>
#include <qsqlrecord.h>
#include <QKeyEvent>
#include <QSqlQuery>
#include <stdio.h>
#include "QcjData/QcjDataLogin.h"
#include "QcjData/QcjDataStatics.h"
#include "QcjData/QcjDataXML.h"
#endif

#include "QcjData/QcjDataSheet.h"

/*!
       \class QcjDataSheet
       
       \brief  Creates a Text Browser and pretty prints a table of data in
       html frormat.
       
       The  QcjDataSheet  class  creates a Text Browser and and populates it
       with  a  table  of  data.  It is similar to the QcjDataTbale class
       except  that  individual  rows  of data cannot be selected. Its
       purpose  is to give the user a nice presentation of the data in
       a table.
       
       The  table  and  the  fields  this object presents is under the
       control of the applications XML document.
*/ 
QcjDataSheet::QcjDataSheet(QWidget *pParent) :
          QTextBrowser(pParent)
{
   printf("QcjDataSheet::QcjDataSheet(): Enter\n");
   havePressed = false;
#ifdef QT4_DESIGNER_PLUGIN
   setFrameShape(QFrame::Box);
#endif
   setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
   setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
   printf("QcjDataSheet::QcjDataSheet(): Exit\n");
   fflush(stdout);
}

/*!
      \fn void QcjDataSheet::setDatabase()

       Acts like setDatabase in QcjDataForm and QcjDataTable.
       
       \sa QcjDataForm::setDatabase() \sa QcjDataTable::setDatabase()
*/ 
void QcjDataSheet::setDatabase()
{
#ifndef QT4_DESIGNER_PLUGIN
    dbTable = pFormDef->getTable(xmldef);
    printf("QcjDataSheet::setDatabase(): Enter, table = |%s|\n", qPrintable(dbTable));
    fflush(stdout);
    if ( pDb == 0 ) 
    {
        printf("QcjDataSheet::setDatabase(): No database yet, go login\n");
        QcjDataLogin *login = new QcjDataLogin();
        login->initialize();

        if (!login->exec()) 
        {
            return;
        }
    }
    printf("QcjDataSheet::setDatabase(): Setting up database elements\n");
    printf("QcjDataSheet::setDatabase(): Calling setFields\n");
    setFields(pFormDef->getFields(xmldef, 0));
    sortBy(pFormDef->getSortFields(xmldef));
    printf("QcjDataSheet::setDatabase(): Refreshing table\n");
//    refresh();

    printf("QcjDataSheet::setDatabase(): Exit\n");
    fflush(stdout);
#endif
}
/*
QcjDataSheet::QcjDataSheet(const QString &table, int rows, QWidget *pParent, const char *pName) :
          Q3DataSheet(pParent, pName)
{
   dbTable = table;
   sqlCursor = new Q3SqlCursor(dbTable);
   sqlCursor->setMode(Q3SqlCursor::ReadOnly);
   setSqlCursor(sqlCursor);
   insertMode  = false;
   connect(horizontalHeader(), SIGNAL(clicked(int)), this, SLOT(sortBy(int)));
   connect(this, SIGNAL(beforeInsert(QSqlRecord *)), this, SLOT(nextIdent(QSqlRecord *)));
}

QcjDataSheet::QcjDataSheet(const QcjDataFields *fields, const QString &table, int rows, QWidget *pParent, const char *pName) :
          Q3DataSheet(pParent, pName)
{
   dbTable = table;
   sqlCursor = new Q3SqlCursor(dbTable);
   sqlCursor->setMode(Q3SqlCursor::ReadOnly);
   setSqlCursor(sqlCursor);
   setFields(fields);
   insertMode  = false;
   connect(horizontalHeader(), SIGNAL(clicked(int)), this, SLOT(sortBy(int)));
   connect(this, SIGNAL(beforeInsert(QSqlRecord *)), this, SLOT(nextIdent(QSqlRecord *)));
}
*/
void QcjDataSheet::writeXmlDef(QString str)
{
    printf("QcjDataSheet::writeXmlDef(): Enter: str = %s\n", (const char*)str.toLocal8Bit());
    fflush(stdout);
    xmldef = str;
    printf("QcjDataSheet::writeXmlDef(): Exit\n");
    fflush(stdout);
}

/*!
      \fn void QcjDataSheet::setFields(std::vector<struct QcjDataFields> fds)

       Initializes  the  table  information  for  the class. It is not
       intended to be called directly.
       
       \sa setDatabase()
*/ 
void QcjDataSheet::setFields(std::vector<struct QcjDataFields> fds)
{
#ifndef QT4_DESIGNER_PLUGIN
   printf("QcjDataSheet::setFields():2: Enter\n");
   printf("QcjDataSheet::setFields(): xmldef = |%s|\n", (const char*)xmldef.toLocal8Bit());
   fflush(stdout);
   fields = fds;
   printf("QcjDataSheet::setFields(): exit\n");
   fflush(stdout);
#endif
}

/*!
      \fn void QcjDataSheet::setFilter(QString f)

       Sets the filter for the data in the table.
       
       \sa QcjDataTable::setFilter(QString)
*/ 
void QcjDataSheet::setFilter(QString f)
{
#ifndef QT4_DESIGNER_PLUGIN
   printf("QcjDataSheet::setFilter(): Enter, Setting filter: %s\n", qPrintable(filter));
   filter = f;
   printf("QcjDataSheet::setFilter(): Exit\n");
//   refresh();
#endif
}

/*!
      \fn void QcjDataSheet::sortBy(QStringList sl)

       Sets the sort order of the table
       
       \sa QcjDataTable::sortBy(QStringList)
*/ 
void QcjDataSheet::sortBy(QStringList sl)
{
#ifndef QT4_DESIGNER_PLUGIN
   printf("QcjDataSheet::sortBy():Enter, Sorting by stringlist\n");
   QStringList::Iterator it;
   sort = sl;
   for (it = sort.begin(); it != sort.end(); it++) 
   {
      printf("QcjDataSheet::sortBy():Sorting by field: |%s|\n", qPrintable(*it));
   }
   printf("QcjDataSheet::sortBy():Exit\n");
//   refresh();
#endif
}

/*!
      \fn void QcjDataSheet::sortBy(int section)

       Adds a field to the ordering.
       
       \sa QcjDataTable::sertBy(int)
*/ 
void QcjDataSheet::sortBy(int section)
{
#ifndef QT4_DESIGNER_PLUGIN
   printf("QcjDataSheet::sortBy():Sorting by section: %d\n", section);
   sortBy(fieldNames[section]);
#endif
}

/*!
      \fn void QcjDataSheet::sortBy(const QString field)

       Adds a named field to the ordering.
       
       \sa QcjDataTable::sortBy(QString)
*/ 
void QcjDataSheet::sortBy(const QString field)
{
#ifndef QT4_DESIGNER_PLUGIN
   while ( sort.count() >= 3 ) 
   {
      printf("QcjDataSheet::sortBy():Removing item %d from sort\n", sort.count());
      sort.pop_back();
      fflush(stdout);
   }
   printf("QcjDataSheet::sortBy():Prepending field |%s|\n", qPrintable(field));
   fflush(stdout);
   sort.prepend(field);
   printf("QcjDataSheet::sortBy():refreshing table\n");
   fflush(stdout);
//   refresh();
   printf("QcjDataSheet::sortBy():return\n");
   fflush(stdout);
#endif
}

/*!
      \fn void QcjDataSheet::refresh()

       Refreshes  the  table  contents  with  current  data  from  the
       database then formats and displays it in the Text Browser.
*/ 
void QcjDataSheet::refresh()
{
#ifndef QT4_DESIGNER_PLUGIN
   QString text;
   printf("QcjDataSheet::refresh(): Enter\n");
   QStringList field_names = pFormDef->getFieldNames(xmldef);
   QString sql = "select ";
   QStringList::const_iterator i;
   QString field_str = "";

   for (i = field_names.constBegin(); i != field_names.constEnd(); ++i)
   {
      if ( field_str.size() > 0 ) 
         field_str += ", ";
      field_str += QString("\"") + (*i) + QString("\"");
   }
   sql += field_str + QString(" from ") + pFormDef->getTable(xmldef);

   printf("QcjDataSheet::refresh(): filter = |%s|\n", qPrintable(filter));
   if ( filter.size() > 0 ) 
      sql += QString(" where ") + filter;

   printf("QcjDataSheet::refresh(): sort = |%s|\n", qPrintable(sort.join(", ")));
   if ( sort.size() > 0 ) 
      sql += QString(" order by ") + sort.join(", ");


   printf("QcjDataSheet::refresh(): sql = |%s|\n", qPrintable(sql));
   QSqlQuery q(sql);
   if ( q.isActive() ) 
   {
      while (q.next()) 
      {
         if ( text.size() == 0) 
         {
            QString lblBgColor = pFormDef->getFieldLabelBgColor(xmldef);
            QString lblColor = pFormDef->getFieldLabelColor(xmldef);
            text = "<html>\r\n<table width=\"100%\">\r\n";
            text += "   <tr bgcolor=\"" + lblBgColor + "\">\r\n";
            for (i = field_names.constBegin(); i != field_names.constEnd(); ++i)
               text += "      <th color=\"" + lblColor + "\">" + pFormDef->getFieldLabel(xmldef, (*i)) + "\r\n";
            text += "   </tr>\r\n";
         }
         text += "<tr>\n";
         for (i = field_names.constBegin(); i != field_names.constEnd(); ++i)
         {
            std::vector<struct QcjDataFields>::const_iterator ifields;
            QString fwidth = "";
            QString fheight = "";
            for (ifields = fields.begin(); ifields != fields.end(); ifields++) 
            {
               if ( (*ifields).width.size() > 0 ) 
                  fwidth = "width=\"" + (*ifields).width + "\"";
               if ( (*ifields).height.size() > 0 ) 
                  fheight = "height=\"" + (*ifields).height + "\"";
            }
            QString fieldType = pFormDef->getFieldType(xmldef, (*i));
            printf("QcjDataSheet::refresh(): fieldType = |%s|\n", qPrintable(fieldType));
            if ( fieldType == "money" ) 
            {
               text += "      <th align=\"right\" " + fwidth + fheight + ">"; 
               text += QcjMoneyEdit::formatCurrency(q.record().value((*i)).toString()).rightJustified(12).replace(" ", "&nbsp;");
            }
            else 
            {
               text += "      <th align=\"left\" " + fwidth + fheight + ">"; 
               text += q.record().value((*i)).toString();
            }
            text += "</th>\r\n";
         }
         text += "</tr>\n";
      }
   }
   if ( text != "" ) 
      text += "</table>\n</html>\n";
   setHtml(text);
   printf("QcjDataSheet::refresh(): Exit\n");
#endif
}

void QcjDataSheet::mouseDoubleClickEvent( QMouseEvent * evt )
{
   printf("QcjDataSheet:mouseDoubleClickEvent(): Enter\n");
   fflush(stdout);
   if ( evt->button() == Qt::LeftButton ) 
   {
      printf("QcjDataSheet:mouseDoubleClickEvent(): Emitting doubleClicked signal\n");
      fflush(stdout);
      emit doubleClicked();
   }
   printf("QcjDataSheet:mouseDoubleClickEvent(): Exit\n");
   fflush(stdout);
}

void QcjDataSheet::mousePressEvent ( QMouseEvent * evt )
{
   printf("QcjDataSheet:mousePressEvent(): Enter, x = %d, y = %d\n", evt->x(), evt->y());
   fflush(stdout);
   if ( evt->button() == Qt::LeftButton ) 
   {
      printf("QcjDataSheet:mousePressEvent(): Setting havePressed to true\n");
      fflush(stdout);
      havePressed = true;
   }
   QWidget::mousePressEvent(evt);
   printf("QcjDataSheet:mousePressEvent(): Exit\n");
   fflush(stdout);
}

void QcjDataSheet::mouseReleaseEvent ( QMouseEvent * evt )
{
   printf("QcjDataSheet:mouseReleaseEvent(): Enter, x = %d, y = %d\n", evt->x(), evt->y());
   fflush(stdout);
   if ( evt->button() == Qt::LeftButton && havePressed == true) 
   {
      printf("QcjDataSheet:mouseReleaseEvent(): Emitting clicked signal\n");
      fflush(stdout);
      havePressed = false;
      emit clicked();
   }
   QWidget::mouseReleaseEvent(evt);
   printf("QcjDataSheet:mouseReleaseEvent(): Exit\n");
   fflush(stdout);
}

