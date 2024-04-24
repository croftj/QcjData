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

#ifndef DATATABLE_H
#define DATATABLE_H

//#ifndef QT4_DESIGNER_PLUGIN
//#include <Q3DataTable>
//#endif
#include <qlineedit.h>
#include <qmap.h>
#ifndef QT4_DESIGNER_PLUGIN
#include <qsqldatabase.h>
#include <qsqlrecord.h>
#endif
#include <qstring.h>
#include <QSqlField>
//Added by qt3to4:
#include <QFrame>
#include <vector>
#include "QcjData/QcjDataSqlTable.h"
#include "QcjData/QcjDataHelpers.h"
#include <stdlib.h>
#include <stdio.h>


class QcjDataTable : public QcjDataSqlTable
{
   Q_OBJECT
   Q_PROPERTY (QString xml_definition READ readXmlDef WRITE writeXmlDef)
   Q_PROPERTY (bool delegates READ readDelegates WRITE writeDelegates)
   
public:
   QcjDataTable(QWidget *pParent = 0);
//   QcjDataTable(const QString &table, int rows, QWidget *pParent, const char *pName);
//   QcjDataTable(const QcjDataFields *fields, const QString &table, int rows,
//            QWidget *pParent = 0, const char *pName = 0);
   ~QcjDataTable() {};

   QString readXmlDef() const { return(m_xmldef); };
   bool readDelegates() const { return(m_delegates); };
   void setDatabase(bool _m_autoRefresh = true);
   void setDatabase(QSqlDatabase *pdb, bool _m_autoRefresh = true);
   void setFilter(QList<QLineEdit*> fields, const QString &filter = QString());
   void setFilter(QString clause);
   void seek(int index);
   void writeXmlDef(QString s);
   void writeDelegates(bool flag);

public slots:   
   /*!
         \fn void refresh() 

          Refreshes   the  table  view  with  current  data  from  the
          database.
   */ 

   bool refresh(bool quiet = false, long limit = -1)
#ifndef QT4_DESIGNER_PLUGIN
   {
      return(QcjDataSqlTable::refresh(quiet, limit));
   };
#else
   {
      return(true);
   };
#endif

#ifdef QT4_DESIGNER_PLUGIN
   void findRow(QMap<QString, QString> clause);
   void nextRow();
   void previousRow();
#endif
   void setFields(const QcjDataFields *fields);
   void setFields(std::vector<struct QcjDataFields> fds);
   void sortBy(QStringList sl);
   void sortBy(const QString);
   void sortBy(int);
//   void setFieldItem(QSqlField field, QTableWidgetItem *pItem);

protected:
   void _setFields(const QcjDataFields *fields);

#ifdef QT4_DESIGNER_PLUGIN
/*
   Thises signals are defined on part for the benifit of the
   QT designer so they can be connected using the designer.
*/
signals:
   void rowActivated(QSqlRecord *);
   void rowSelected(QSqlRecord *);
   void emptyTable();
#endif   

private:
#ifndef QT4_DESIGNER_PLUGIN
   std::vector<struct QcjDataFields> fields;
//   Q3SqlCursor                *sqlCursor;
#endif
   QString                    m_dbTable;
   QStringList                m_sort;
   QMap<int, QString>         m_fieldNames;
   QMap<QString, QString>     m_fieldTypes;
   bool                       m_insertMode;
   bool                       m_autoRefresh;
};
#endif
