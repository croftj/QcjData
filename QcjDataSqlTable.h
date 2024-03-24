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

#ifndef DATASQLTABLE
#define DATASQLTABLE

#include <QSqlField>
#include <QSqlQuery>
#include <QSqlRecord>
#include <QSqlTableModel>
#include <QTableView>

class QcjDataSqlTable : public QTableView
{
   Q_OBJECT

public:
   QcjDataSqlTable(QWidget *parent);
   QcjDataSqlTable(QWidget *parent, QSqlDatabase *db);
   ~QcjDataSqlTable();

   void addColumn(QString field, QString label);
   void clearSelection();

   /*!
         \fn QSqlRecord *currentRecord()
          Returns the currently selected record.
   */ 
   QSqlRecord *currentRecord()
   {
      return(&rec);
   };
   QString filter();
   int findRow(QMap<QString, QString> clause, int from, bool caseSensitive = true);
   int rowCount() const;
   void selectRow(int row);
   void setFilter(QString clause);
   void setSort(QStringList sortList);
   void setTableName(QString name);
   QStringList sort();
   virtual QString readXmlDef() const = 0;
//   virtual void setFieldItem(QSqlField xf, QTableWidgetItem *i);

public slots:
   void findRow(QMap<QString, QString> clause);
   void recordActivated(const QModelIndex &index); 
   void recordSelected(int row);
   void recordSelected(const QModelIndex &index); 
   void recordSelected(const QModelIndex &current, const QModelIndex &prev);
   bool refresh(bool quiet = false, long limit = -1);

   void nextRow();
   void previousRow();

   void activateCurrentRecord();
/*
   void recordSelected(QTableWidgetItem *, QTableWidgetItem *);
*/

signals:
   /*!
         \fn void rowActivated(QSqlRecord *);
          Emitted every time a record is highligted
   */ 
   void rowActivated(QSqlRecord *);

   /*!
         \fn void rowSelected(QSqlRecord *);
          Emitted when a record is double clicked.
   */ 
   void rowSelected(QSqlRecord *);

   /*!
         \fn void emptyTable();
          Emitted when the table is refreshed and no records were present. This
          would typically be connected to the clearForm slot of the corresponding
          form for the table.
   */ 
   void emptyTable();
   
protected:
   QSqlDatabase   *myDb;
   bool           m_delegates;
   QString        m_xmldef;

private:
   void seekRow(int new_row);


   QSqlQuery   *pQuery;
   QSqlQueryModel *pModel;
   QStringList fields;
   QStringList labels;
   QString     tableName;
   QStringList sortFields;
   QString     whereClause;
   QSqlRecord  rec;
   QString     lastSql;
   int         current_row;
   int         haveCurrentChangedConnection;
};

#endif
