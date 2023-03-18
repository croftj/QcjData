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
//#include <qregexp.h>
#include <qsqldatabase.h>
#include <qsqldriver.h>
#include <qsqlrecord.h>
//Added by qt3to4:
#include <QKeyEvent>
#include <QSqlQuery>
#include <stdio.h>
#include "QcjData/QcjDataLogin.h"
#include "QcjData/QcjDataStatics.h"
#include "QcjData/QcjDataXML.h"
#endif

#include "QcjData/QcjDataTable.h"

/*!
       \class QcjDataTable
       
       \brief Creates a frame which holds a table view for viewing and
       accessing a table of data in a database.
       
       The  QcjDataTable class creates a table view which can be added to
       a  dialog or other widget. It allows the user to scroll through
       the data, set the ordering of the data as well as select either
       individual or multple records.
       
       The  actual fields and the table to be manipulated by the obect
       is  controlled  by  an  XML  file  and  therefore can be easily
       manipulated without recompiling the application.
*/ 

/*!
      \fn QcjDataTable::QcjDataTable(QWidget *pParent)

       Creates the QcjDataTable and its frame.
*/ 
QcjDataTable::QcjDataTable(QWidget *pParent) :
#ifndef QT4_DESIGNER_PLUGIN
          QcjDataSqlTable(pParent)
#else
          QFrame(pParent)
#endif
{
#ifndef QT4_DESIGNER_PLUGIN
   printf("QcjDataTable::QcjDataTable(): Enter\n");
//   sqlCursor = NULL;
   m_insertMode  = false;
//   connect(this, SIGNAL(beforeInsert(QSqlRecord *)), this, SLOT(nextIdent(QSqlRecord *)));
   connect((const QObject*)(horizontalHeader()), SIGNAL(sectionClicked(int)), this, SLOT(sortBy(int)));
   printf("QcjDataTable::QcjDataTable(): Exit\n");
   fflush(stdout);
#else
   setFrameShape(QFrame::Box);
#endif
}

void QcjDataTable::setDatabase(QSqlDatabase *pdb, bool autoRefresh)
{
#ifndef QT4_DESIGNER_PLUGIN
   myDb = new QSqlDatabase(*pdb);
   setDatabase(autoRefresh);
#endif
}

/*!
      \fn void QcjDataTable::setDatabase(bool autoRefresh = true)

       Performs  the required initialization of the table. If required
       the  database  connection will be initialized and the user will
       be prompted for login information.

       The parameter autoRefresh indicates whether or not the table should
       be automatically refreshed when this function is called,the table 
       order is set, or the fields for the table are set. If autoRefresh is false,
       then the refresh function will have to be called explicately after
       those operations are performed, otherwise those operations will
       refresh the table automatically.
*/ 
void QcjDataTable::setDatabase(bool autoRefresh)
{
#ifndef QT4_DESIGNER_PLUGIN
    printf("QcjDataTable::setDatabase(): m_xmldef = %s\n", qPrintable(m_xmldef));
    fflush(stdout);
    m_dbTable = pFormDef->getTable(m_xmldef);
    printf("QcjDataTable::setDatabase(): m_dbTable = |%s|\n", qPrintable(m_dbTable));
    fflush(stdout);

    m_autoRefresh = autoRefresh;

    if ( myDb == 0 ) 
      myDb = new QSqlDatabase(*pDb);

    if ( myDb == 0 ) 
    {
        printf("QcjDataTable::setDatabase(): No database yet, go login\n");
        QcjDataLogin *login = new QcjDataLogin();
        login->initialize();

        if (!login->exec()) 
        {
            return;
        }
    }
    printf("QcjDataTable::setDatabase(): Setting up database elements\n");
    fflush(stdout);
    setTableName(m_dbTable);
    printf("QcjDataTable::setDatabase(): Calling setFields\n");
    fflush(stdout);
    setFields(pFormDef->getFields(m_xmldef, 0));
    printf("QcjDataTable::setDatabase(): Calling sortBy\n");
    fflush(stdout);
    sortBy(pFormDef->getSortFields(m_xmldef));
    if ( m_autoRefresh ) 
    {
       printf("QcjDataTable::setDatabase(): Refreshing table\n");
       fflush(stdout);
       refresh();
    }

    printf("QcjDataTable::setDatabase(): Exit\n");
    fflush(stdout);
#endif
}

/*!
   \fn void QcjDataTable::setFilter(QList<QLineEdit*> fields)

   Sets the where clause to use for filtering the contens of the
   table and calls <em>refresh()</em>. The parameter
   <em>fields</em> is expected to be a list of QLineEdit widgets
   that have the dynamic property 'search_group' set to specify
   which group of table columns whould be searched for the value.

   Caveats: If a widget w/o the required property is passed in, it 
   will skipped. Each search term will be surounded with '%' chars
   to allow searched on partial strings.
*/
void QcjDataTable::setFilter(QList<QLineEdit*> fields)
{
   QString where;
   QString wdt_text;
   QMap<QString, QStringList> groupNames;
   foreach (QWidget* wdt, fields)
   {
      /*******************************************************/
      /* Start by getting the text from the widget, whatever */
      /* the type                                            */
      /*******************************************************/
      if ( wdt->inherits("QLineEdit")  ) 
      {
         wdt_text = static_cast<QLineEdit*>(wdt)->text();         
      }
      else if ( wdt->inherits("QTextEdit")  ) 
      {
         wdt_text = static_cast<QTextEdit*>(wdt)->toPlainText();         
      }
      else if ( wdt->inherits("QDoubleSpinBox")  ) 
      {
         wdt_text = static_cast<QDoubleSpinBox*>(wdt)->cleanText();         
      }
      else if ( wdt->inherits("QSpinBox")  ) 
      {
         wdt_text = static_cast<QSpinBox*>(wdt)->cleanText();         
      }
      else if ( wdt->inherits("QComboBox")  ) 
      {
         wdt_text = static_cast<QComboBox*>(wdt)->itemData(static_cast<QComboBox*>(wdt)->currentIndex()).toString();         
      }

      if (wdt->property("search_group").isValid())
      {
         QString group_name = wdt->property("search_group").toString();
         QStringList field_names = pFormDef->getFieldGrouping(m_xmldef, group_name);
         if ( ! groupNames.contains(group_name))
         {
            groupNames.insert(group_name, field_names);
         }
         foreach(QString fn, field_names)
         {
            if (where.length() > 0)
            {
               where += " or ";
            }
            QString xlate = pFormDef->getSearchXlate(m_xmldef, fn);
            if ( ! xlate.isEmpty() && ! wdt_text.isEmpty())
            {
               where += fn + " = regexp_replace('" + wdt_text + "', " + xlate + ") or ";
            }
            where += fn + " like '%" + wdt_text + "%'";
         }
      }
   }
   setFilter(where);
}

void QcjDataTable::setFilter(QString filter)
{
   QcjDataSqlTable::setFilter(filter);
}

/*!
      \fn void QcjDataTable::writeXmlDef(QString str)

       Saves the name of the XML definition to use for this table.
       
       This function should not be called directly by the user.
       
       \sa setDatabade()
*/ 
void QcjDataTable::writeXmlDef(QString str)
{
    printf("QcjDataTable::writeXmlDef(): Enter: str = %s\n", qPrintable(str));
    fflush(stdout);
    m_xmldef = str;
    printf("QcjDataTable::writeXmlDef(): Exit\n");
    fflush(stdout);
}

/*!
      \fn void QcjDataTable::setFields(const QcjDataFields *fds) 

       Initializes the table with the fields specified by the array of
       data fields pointed to by the parameter <em>fds</em>.
       
       This function should not be called directly by the user.
       
       \sa setDatabade()
*/ 
void QcjDataTable::setFields(const QcjDataFields *fds) 
{
#ifndef QT4_DESIGNER_PLUGIN
   printf("DateTable::setFields(): Enter\n");
   fflush(stdout);
   const QcjDataFields *pFds = fds;
   while (fds->dataName != QString::null && fds->dataName != QString("--ENDOFFIELDS--"))
   {
      printf("DateTable::setFields(): New field |%s|\n", qPrintable(fds->dataName));
      fflush(stdout);
      fields.push_back(*fds);
      fds++;
   }

   struct QcjDataFields *p = new struct QcjDataFields;
   p->dataName = QString("--ENDOFFIELDS--");
   fields.push_back(*p);
   delete p;
   _setFields(pFds);
   printf("DateTable::setFields(): Exit\n");
   fflush(stdout);
#endif
}

/*!
       Overloaded  version of the function, this one uses a STL vector
       for the table information.
       
       This function should not be called directly by the user.
       
       \sa setDatabade()
*/ 
void QcjDataTable::setFields(std::vector<struct QcjDataFields> fds)
{
#ifndef QT4_DESIGNER_PLUGIN
   printf("QcjDataTable::setFields():2: Enter\n");
   printf("QcjDataTable::setFields(): m_xmldef = |%s|\n", qPrintable(m_xmldef));
   fflush(stdout);
   fields = fds;
   struct QcjDataFields *field_ptr = new struct QcjDataFields[fields.size() + 1];
   struct QcjDataFields *fp = field_ptr;
   unsigned x;
   
   for (x = 0; x < fields.size(); x++) 
   {
      *(fp++) = fields[x];
   }
   _setFields(field_ptr);
   printf("QcjDataTable::setFields(): deleting field array fp\n");
   fflush(stdout);
   delete [] field_ptr;
#endif
   printf("QcjDataTable::setFields(): exit\n");
   fflush(stdout);
}

/*!
      \fn void QcjDataTable::_setFields(const QcjDataFields *fields) 

       This function does the actial initializatin of table object.
       
       This function should not be called directly by the user.
       
       \sa setDatabade()
*/ 
void QcjDataTable::_setFields(const QcjDataFields *fields) 
{
#ifndef QT4_DESIGNER_PLUGIN
   //
   // Iterate through all of the field definitions and 
   // place them into the grid.
   //

   printf("QcjDataTable::_setFields():Enter\n");
   for (int col = 0; fields->dataName != QString() && fields->dataName != "--ENDOFFIELDS--"; col++) 
   {
      QString dataName = fields->dataName;
      if ( fields->format.size() > 0 ) 
      {
         dataName = fields->format;
      }
      printf("QcjDataTable::_setFields(): New field |%s|\n", qPrintable(fields->dataName));
      fflush(stdout);
      m_fieldNames[col] = fields->dataName;
      m_fieldTypes[QString(fields->dataName)] = fields->propName;
      addColumn(dataName, fields->label);
      QFont f = font();
      QFontMetrics fm(f);
      int fwidth = fm.maxWidth();
      printf("\nDataTable::_setFields():Element %d |%s|\n", col, qPrintable(fields->dataName));
      fflush(stdout);
      if ( fields->minWidth > 0 ) 
      {
         printf("QcjDataTable::_setFields():Setting col %d min width %d, (%d pixels)\n", col, fields->minWidth, fwidth * fields->minWidth);
         fflush(stdout);
//         setColumnWidth(col, fwidth * fields->minWidth);
      }
      fields++;
   }
   if ( m_autoRefresh ) 
   {
      refresh();
      printf("QcjDataTable::_setFields(): Selecting row 0\n");
      fflush(stdout);
      selectRow(0);
   }
#endif
   printf("QcjDataTable::_setFields(): Exit\n");
   fflush(stdout);
}

void QcjDataTable::seek(int index)
{
#ifndef QT4_DESIGNER_PLUGIN
   selectRow(index);
#endif
}

/*!
      \fn void QcjDataTable::sortBy(QStringList sl)

       Sets the ordering of the table.
       
       \sa void QcjDataSqlTable::setSort(QStringList)
*/ 
void QcjDataTable::sortBy(QStringList sl)
{
#ifndef QT4_DESIGNER_PLUGIN
   printf("QcjDataTable::sortBy():Sorting by stringlist\n");
   QStringList::Iterator it;
   m_sort = sl;
   for (it = m_sort.begin(); it != m_sort.end(); it++) 
   {
      printf("QcjDataTable::sortBy():Sorting by field: |%s|\n", qPrintable(*it));
   }
   setSort(m_sort);
   if ( m_autoRefresh ) 
      refresh();
#endif
}

/*!
      \fn void QcjDataTable::sortBy(const QString field)

       This  is  an  overloaded version of the above function. It will
       add the field named by the parameter <em>field</em> to the sort
       order.  This field will have a higher precedence than any field
       added  previously.  Currently the number of fields which can be
       sorted on is arbitrarily set to 3.
*/ 
void QcjDataTable::sortBy(const QString parmfield)
{
#ifndef QT4_DESIGNER_PLUGIN
   QString field;
   QString order;

   QString newfield;
   QString neworder;

   printf("QcjDataTable::sortBy():Enter, newfield = %s\n", qPrintable(newfield));

//   QRegExp re("(.*) (asc|desc|ASC|DESC)");
   QRegExp re("(.*) (ASC|DESC)");
   re.setMinimal(true);
   re.setCaseSensitivity(Qt::CaseInsensitive);


   if ( parmfield.contains(re) ) 
   {
      newfield = re.cap(1);
      neworder = re.cap(2);
   }
   else 
   {
      newfield = parmfield;
      neworder = "asc";
   }
   printf("QcjDataTable::sortBy():newfield = %s\n", qPrintable(newfield));
   printf("QcjDataTable::sortBy():neworder = %s\n", qPrintable(neworder));

   if ( m_sort.count() > 0 ) 
   {
      printf("QcjDataTable::sortBy():m_sort[0] = %s\n", qPrintable(m_sort[0]));
      if ( m_sort[0].contains(re) ) 
      {
         field = re.cap(1);
         order = re.cap(2);
      }
      else 
      {
         field = m_sort[0];
         order = "asc";
      }
      printf("QcjDataTable::sortBy():field = %s\n", qPrintable(field));
      printf("QcjDataTable::sortBy():order = %s\n", qPrintable(order));

      if ( field == newfield ) 
      {
         if ( order == "asc" ) 
            order = "desc";
         else
            order = "asc";
         m_sort[0] = field + " " + order;
         printf("QcjDataTable::sortBy():m_sort[0] = %s\n", qPrintable(m_sort[0]));
         field = "";
      }
      else 
         field = newfield + " " + neworder;
   }
   else
      field = newfield + " " + neworder;

   printf("QcjDataTable::sortBy():field = %s\n", qPrintable(field));
   if ( field != "" ) 
   {
      while ( m_sort.count() >= 3 ) 
      {
         printf("QcjDataTable::sortBy():Removing item %d from m_sort\n", m_sort.count());
         m_sort.pop_back();
         fflush(stdout);
      }
      printf("QcjDataTable::sortBy():Prepending field |%s|\n", qPrintable(field));
      fflush(stdout);
      m_sort.prepend(field);
   }
   setSort(m_sort);
   if ( m_autoRefresh ) 
   {
      printf("QcjDataTable::sortBy():refreshing table\n");
      fflush(stdout);
      refresh();
   }
   printf("QcjDataTable::sortBy():return\n");
   fflush(stdout);
#endif
}

/*!
      \fn void QcjDataTable::sortBy(int section)

       This  is  an  overloaded version of the above function. Add the
       specified  section  to  the  ordering  of  the  table. This new
       section  will  have  a higher precedence then any section added
       before  it.  Currently,  it  is connected so that when the user
       clicks  on the header of a column, that column will be added to
       the sort order.
*/ 
void QcjDataTable::sortBy(int section)
{
#ifndef QT4_DESIGNER_PLUGIN
   printf("QcjDataTable::sortBy():Sorting by section: %d\n", section);
   sortBy(m_fieldNames[section]);
   if ( ! m_autoRefresh ) 
      refresh();
#endif
}

/*
void QcjDataTable::setFieldItem(QSqlField field, QTableWidgetItem *pItem)
{
#ifndef QT4_DESIGNER_PLUGIN
//   printf("QcjDataTable::setFieldItem():Enter\n");

   QString name = field.name();
//   printf("QcjDataTable::setFieldItem():Field name = %s\n", (const char*)name.toLocal8Bit());
//   fflush(stdout);
//   printf("QcjDataTable::setFieldItem():Field type = %s\n", (const char*)m_fieldTypes[name].toLocal8Bit());
//   fflush(stdout);
   QString s = field.value().toString();
   if (m_fieldTypes[name] == "money") 
   {
//      printf("QcjDataTable::setFieldItem():Field value = %s\n", (const char*)s);
      fflush(stdout);
      QString s1, s2;

      QRegExp re0("^([0-9]+)$");
      re0.search(s);
      s1 = re0.cap(1);
      s.replace(re0, s1 + ".00");
      
      QRegExp re1("(\\.[0-9])$");
      re1.search(s);
      s1 = re1.cap(1);
      s.replace(re1, s1 + "0");
      
      QRegExp re2("([0-9])([0-9]{3}\\.)");
      re2.setMinimal(true);
      re2.search(s);
      s1 = re2.cap(1);
      s2 = re2.cap(2);
//      printf("QcjDataTable::setFieldItem(): s1 = |%s|, s2 = |%s|\n", (const char*)s1, (const char*)s2);
      s.replace(re2, s1 + "," + s2);

      QRegExp re3("([0-9])([0-9]{3},)");
      re3.setMinimal(true);

      re3.search(s);
      s1 = re3.cap(1);
      s2 = re3.cap(2);
      s.replace(re3, s1 + "," + s2);

      re3.search(s);
      s1 = re3.cap(1);
      s2 = re3.cap(2);
      s.replace(re3, s1 + "," + s2);

      re3.search(s);
      s1 = re3.cap(1);
      s2 = re3.cap(2);
      s.replace(re3, s1 + "," + s2);
      s = "$" + s;

//      printf("QcjDataTable::setFieldItem():drawing text %s\n", (const char*)s);
      fflush(stdout);
   }
//   printf("QcjDataTable::setFieldItem():Setting field text = |%s|\n", (const char*)s.toLocal8Bit());
//   fflush(stdout);
   pItem->setText(s);
#endif
}
*/
