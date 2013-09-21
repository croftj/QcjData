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
#include <QtGui>
#include <QMessageBox>
#include <QSqlError>
#include "QcjData/QcjDataLogin.h"
#include "QcjData/QcjDataSqlTable.h"
#include "QcjData/QcjDataStatics.h"

#include <QtGui>
#include <QMessageBox>
#include <QSqlError>
#include "QcjData/QcjDataSqlTable.h"
#include "QcjData/QcjDataStatics.h"

/*!
       \class QcjDataSqlTable
       
       \brief  This class provides the the actual table view object to
       be  used by the <em>QcjDataTable</em> object. 
       
       The QcjDataSqlTable handles the lower
       level processing required for the Table view and little support
       for  determining  the  actual  definition of the columns of the
       table to be viewed.
*/ 

/*!
       \fn QcjDataSqlTable::QcjDataSqlTable(QWidget *parent)
       
       Creates and initializes the QcjDataSqlTable.
*/ 
QcjDataSqlTable::QcjDataSqlTable(QWidget *parent) : QTableView(parent)
{
   pQuery = NULL;
   myDb = 0;
   current_row = 0;
   horizontalHeader()->setHighlightSections(false);
   verticalHeader()->setHighlightSections(true);
   setSelectionBehavior(QAbstractItemView::SelectRows);
//   setFocusPolicy(Qt::NoFocus);

//   setSelectionMode(QAbstractItemView::NoSelection);

   connect(this, SIGNAL(doubleClicked(const QModelIndex &)),
           this, SLOT(recordActivated(const QModelIndex &)));

/*
   connect(this, SIGNAL(entered(const QModelIndex &)),
           this, SLOT(recordSelected(const QModelIndex &)));

   connect((const QObject*)(verticalHeader()), SIGNAL(sectionClicked(int)), 
           this, SLOT(recordSelected(int)));
   connect(this, SIGNAL(pressed(const QModelIndex &)),
           this, SLOT(recordSelected(const QModelIndex &)));
*/
/*
   connect(this, SIGNAL(clicked(const QModelIndex &)),
           this, SLOT(recordSelected(const QModelIndex &)));
*/

   connect(this, SIGNAL(activated(const QModelIndex &)),
           this, SLOT(recordSelected(const QModelIndex &)));
/*



   connect(this->selectionModel(), SIGNAL(currentChanged(const QModelIndex &, const QModelIndex &)),
           this, SLOT(recordSelected(const QModelIndex &, const QModelIndex &)));

   connect(this->selectionModel(), SIGNAL(currentRowChanged(const QModelIndex &, const QModelIndex &)),
           this, SLOT(recordSelected(const QModelIndex &, const QModelIndex &)));

   connect(this->selectionModel(), SIGNAL(selectionChanged(const QModelIndex &, const QModelIndex &)),
           this, SLOT(recordSelected(const QModelIndex &, const QModelIndex &)));
*/
/*
   connect(this, SIGNAL(currentItemChanged(QTableWidgetItem *, QTableWidgetItem *)),
           this, SLOT(recordSelected(QTableWidgetItem *, QTableWidgetItem *)));
   connect((const QObject*)(horizontalHeader()), SIGNAL(sectionClicked(int)), 
           this, SLOT(selectRow(int)));
*/
   haveCurrentChangedConnection = false;

}

/*!
      \fn QcjDataSqlTable::QcjDataSqlTable(QWidget *parent, QSqlDatabase myDb)

       Creates  a  QcjDataSqlTable  object  using  the passed in database
       object.
*/ 
QcjDataSqlTable::QcjDataSqlTable(QWidget *parent, QSqlDatabase *pdb) : QTableView(parent)
{
   myDb = new QSqlDatabase(*pdb);
   pQuery = new QSqlQuery(*myDb);
}


/*!
      \fn QcjDataSqlTable::~QcjDataSqlTable()

       Destroys the QcjDataSqlTable object.
*/ 
QcjDataSqlTable::~QcjDataSqlTable()
{
   if ( myDb != 0 ) 
      delete myDb;
   {
   }
   if ( pQuery == NULL ) 
      return;
   delete pQuery;
}

/*!
      \fn void QcjDataSqlTable::setTableName(QString name) 

       Sets the database table name which this view will be associated
       with.
*/ 
void QcjDataSqlTable::setTableName(QString name) 
{
   tableName = name;
}

/*!
       Adds  a  column to the Table View. The parameter <em>field</em>
       should   have  the  name  of  the  database  field  the  column
       represents  while  <em>label</em>  should have the lable to use
       for the column.
*/ 
void QcjDataSqlTable::addColumn(QString field, QString label)
{
   fields << field;
   labels << label;
//   insertColumn(0);
}

/*!
      \nint QcjDataSqlTable::findRow(QMap<QString, QString> clause, int from, bool caseSensitive)

       Iterates  through  all  of  the  records  begining  with record
       specified  by  <em>from</em> attempting to match the fields and
       their     values     specified     by    <em>clause</em>.    If
       <em>caseSensitive</em>   is  true,  the  search  will  be  case
       sensitive.
       
       The tests for each field will be for an exact match.
*/ 
int QcjDataSqlTable::findRow(QMap<QString, QString> clause, int from, bool caseSensitive)
{
   int row = from;
   int rv = -1;
   int old_row = current_row;

//   refresh();
   printf("QcjDataSqlTable::findRow(): Enter\n");
   if ( pQuery == NULL ) 
      return(0);

/*
   if ( row > pQuery->size() ) 
      row = 0;
   else if (row < 0)
      row = 0;
*/
   if ( pQuery->seek(row) ) 
   {
      while (pQuery->isValid()) 
      {
         printf("QcjDataSqlTable::findRow(): Testing row\n");
         QMap<QString, QString>::iterator it;
         bool nomatch = false;
         for (it = clause.begin(); it != clause.end(); ++it) 
         {
            QSqlField f = pQuery->record().field(it.key());
            printf("QcjDataSqlTable::findRow(): Testing |%s| vs |%s|\n", 
                        (const char*)f.value().toString().toLocal8Bit(),
                        (const char*)it.value().toLocal8Bit()
                  );
            if ( caseSensitive )
            {
               if ( f.value().toString() != it.value() )
               {
                  nomatch = true;
                  printf("QcjDataSqlTable::findRow(): No Match!!!\n"); 
                  break;  
               }
            }
            else 
            {
               if (f.value().toString().toLower() != it.value().toLower())
               {
                  nomatch = true;
                  printf("QcjDataSqlTable::findRow(): No Match!!!\n"); 
                  break;  
               }
            }
         }
         if ( nomatch == false ) 
            break;
         pQuery->next();
      }
   }
   if ( ! pQuery->isValid() ) 
   {
      rv = -1;
      current_row = old_row;
      pQuery->seek(current_row);
      rec = pQuery->record();
      printf("QcjDataSqlTable::findRow(): Query not valid, Ignoring request\n");
   }
   else 
   {
      rv = pQuery->at();
      printf("QcjDataSqlTable::findRow(): Selecting Row %d\n", pQuery->at());
      current_row = pQuery->at();
      rec = pQuery->record();
      scrollTo(pModel->index(current_row, 0));
      QTableView::selectRow(current_row);
   }
   printf("QcjDataSqlTable::findRow(): Exit\n");
   return(rv);
}

void QcjDataSqlTable::nextRow()
{
   printf("QcjDataSqlTable::nextRow(): Enter\n");
   fflush(stdout);
   selectRow(current_row + 1);
   printf("QcjDataSqlTable::nextRow(): Exit\n");
   fflush(stdout);
}

void QcjDataSqlTable::previousRow()
{
   printf("QcjDataSqlTable::previousRow(): Enter\n");
   fflush(stdout);
   selectRow(current_row - 1);
   printf("QcjDataSqlTable::previousRow(): Exit\n");
   fflush(stdout);
}

/*!
       \fn void QcjDataSqlTable::findRow(QMap<QString, QString> clause)
       
       Overloaded  version  of the above function. Finds the first row
       which satisfies <em>clause</em>.
*/ 
void QcjDataSqlTable::findRow(QMap<QString, QString> clause)
{
    findRow(clause, 0, true);  
}

/*!
      \fn void QcjDataSqlTable::selectRow(int new_row)

       Seeks,  highlights  and  makes  current  the  row  specified by
       <em>new_row</em>
*/ 
void QcjDataSqlTable::selectRow(int new_row)
{
   printf("QcjDataSqlTable::selectRow(): Enter new_row = %d\n", new_row);
   fflush(stdout);
   if ( pQuery == NULL ) 
      return;

/*
   for (row = 0; row < rowCount(); row++) 
   {
      for (col = 0; col < columnCount(); col++)
      {
         item(row, col)->setBackgroundColor(QColor(QApplication::palette().background().color()));
         item(row, col)->setTextColor(QColor(QApplication::palette().text().color()));
      }
   }
*/
//   refresh();
   printf("QcjDataSqlTable::selectRow(): table refreshed\n");
   fflush(stdout);
/*
   if ( new_row > pQuery->size() ) 
      new_row = pQuery->size() - 1;
   else if (new_row < 0)
      new_row = 0;
*/
   printf("QcjDataSqlTable::selectRow():Seeking new_row\n");
   fflush(stdout);
   if ( pQuery->seek(new_row) ) 
   {
      printf("QcjDataSqlTable::selectRow():Fetching record\n");
      fflush(stdout);
      rec = pQuery->record();
      printf("QcjDataSqlTable::selectRow():Selecting table widget new_row\n");
      fflush(stdout);
      current_row = new_row;
/*
      for (col = 0; col < columnCount(); col++) 
      {
         if ( col == 0 )
            setCurrentItem(item(new_row, col));
         item(new_row, col)->setBackgroundColor(QColor(QApplication::palette().highlight().color()));
         item(new_row, col)->setTextColor(QColor(QApplication::palette().highlightedText().color()));
      }
*/
   }
   printf("QcjDataSqlTable::selectRow(): Selecting row\n");
   fflush(stdout);
   QTableView::selectRow(current_row);
//   printf("QcjDataSqlTable::selectRow(): Emitting signal!\n");
//   fflush(stdout);
//   emit rowSelected(&rec);
   printf("QcjDataSqlTable::selectRow(): Exit new_row = %d\n", new_row);
   fflush(stdout);
   return;
}

/*!
   \fn void QcjDataSqlTable::clearSelection()

       Currently not imlimented
*/ 
void QcjDataSqlTable::clearSelection()
{
//   setRangeSelected(QTableWidgetSelectionRange(0, 0, rowCount() - 1, columnCount() - 1), false);
}

/*!
      \fn void QcjDataSqlTable::refresh()

       Refreshes  the  Table  view  with  the  current  data  from the
       database.  The  records selected and their order will take into
       account the filter set and the order criteria set.
       
       \sa setFilter() \sa setSort()
*/ 
bool QcjDataSqlTable::refresh(bool quiet, long limit)
{
   QStringList::iterator it;
   QString field_str = QString::null;
   QString sort_str = QString::null;
   QString sql;
   int col;
   int rows;
//   bool error = false;

   printf("QcjDataSqlTable::refresh(): Enter\n");
   fflush(stdout);
   printf("QcjDataSqlTable::refresh(): tableName: |%s|, limit = %ld\n", (const char*)tableName.toLocal8Bit(), limit);
   fflush(stdout);
   if ( ! myDb->isOpen() ) 
   {
      printf("QcjDataSqlTable::refresh(): QcjDatabase closed, logging in again\n");
      fflush(stdout);
      if ( ! myDb->open() )
      {
         printf("QcjDataSqlTable::refresh(): Error opening db connection!\n");
         fflush(stdout);
         return(false);
      }
   }
   if ( tableName.size() == 0)
      return(true);

   QString sqlcount  = "select count(*) from " + tableName;
   if ( whereClause.size() > 0 ) 
      sqlcount += " where " + whereClause;
   printf("QcjDataSqlTable::refresh(): sqlcount = |%s|\n", (const char*)sqlcount.toLocal8Bit());
   fflush(stdout);
   QSqlQuery qcnt(sqlcount);
   if ( qcnt.next() ) 
   {
      rows = qcnt.value(0).toInt();
      if ( rows == 0 ) 
      {
         printf("QcjDataSqlTable::refresh(): no records found\n");
         fflush(stdout);
         this->setEnabled(false);
         emit emptyTable();
      }
      else
      {
         printf("QcjDataSqlTable::refresh(): found %d rows\n", rows);
         fflush(stdout);
         this->setEnabled(true);
      }
   }
   else
   {
      printf("QcjDataSqlTable::refresh(): no count record available\n");
      return(true);
   }


   QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
   if ( pQuery == NULL ) 
   {
      printf("QcjDataSqlTable::refresh(): Query is null\n");
      pQuery = new QSqlQuery(*myDb);
      pModel = new QSqlQueryModel(this);
      QTableView::setModel(pModel);
      if ( rows > 0 ) 
      {
         printf("QcjDataSqlTable::refresh(): Have %d rows, connecting selectionModel's currentChanged to my recordSelected\n", rows);
         connect(this->selectionModel(), SIGNAL(currentChanged(const QModelIndex &, const QModelIndex &)),
                 this, SLOT(recordSelected(const QModelIndex &, const QModelIndex &)));
         haveCurrentChangedConnection = true;
      }
      else
         printf("QcjDataSqlTable::refresh(): Have 0 rows, skipped connecting selectionModel's currentChanged to my recordSelected\n");
   }
   else if ( ! haveCurrentChangedConnection && rows > 0 )
   {
      connect(this->selectionModel(), SIGNAL(currentChanged(const QModelIndex &, const QModelIndex &)),
               this, SLOT(recordSelected(const QModelIndex &, const QModelIndex &)));
         haveCurrentChangedConnection = true;
   }

   for (it = fields.begin(); it != fields.end(); ++it ) 
   {
      printf("QcjDataSqlTable::refresh(): Adding field |%s|\n", (const char*)(*it).toLocal8Bit());
      if ( field_str != QString::null ) 
         field_str += ", ";
      field_str += (*it);
   }

   for (it = sortFields.begin(); it != sortFields.end(); ++it ) 
   {
      QString fname;
      QString forder;

      QRegExp re("(.*) (ASC|DESC)");
      re.setMinimal(true);
      re.setCaseSensitivity(Qt::CaseInsensitive);

      printf("QcjDataSqlTable::refresh(): Adding sort field |%s|\n", (const char*)(*it).toLocal8Bit());
      if ( sort_str != QString::null ) 
         sort_str += ", ";
      sort_str += (*it);
   }

   sql = "select " + field_str + " from " + tableName;
   if ( whereClause.size() > 0 ) 
      sql += " where " + whereClause;
   if ( sort_str != QString::null ) 
      sql += " order by " + sort_str;
   if ( limit >= 0 ) 
      sql += " limit " + QString::number(limit);
   printf("QcjDataSqlTable::refresh(): sql = |%s|\n", (const char*)sql.toLocal8Bit());

//   pQuery = new QSqlQuery(sql, db);
   pModel->clear();
   for (col = 0, it = labels.begin(); it != labels.end(); ++it, col++ ) 
   {
      printf("QcjDataSqlTable::refresh(): Setting label for col %d to |%s|\n", col, (const char*)(*it).toLocal8Bit());
      fflush(stdout);
      pModel->setHeaderData(col, Qt::Horizontal, QVariant((*it)));
   }

   printf("QcjDataSqlTable::refresh(): executing sql statement |%s|\n", qPrintable(sql));
   fflush(stdout);
   pQuery->exec(sql);
   printf("QcjDataSqlTable::refresh(): query complete\n");
   fflush(stdout);
   if ( rows > 0 )
   {
      printf("QcjDataSqlTable::refresh(): have data in the table, set the model\n");
      fflush(stdout);
      pModel->setQuery(*pQuery);
      printf("QcjDataSqlTable::refresh(): Set the column widths\n");
      fflush(stdout);
      for (int col = 0; rows < 200 && col < pQuery->record().count(); col++) 
         resizeColumnToContents(col);

      printf("QcjDataSqlTable::refresh(): select the first row, current_row = %d\n", current_row);
      fflush(stdout);
      QTableView::selectRow(current_row);
   }
   else
      QTableView::selectRow(0);

   printf("QcjDataSqlTable::refresh(): testing for errors\n");
   fflush(stdout);
   if (pModel->lastError().type() != QSqlError::NoError)
//   if ( error ) 
   {
      QString errString = "Error with query of " + tableName + ", " +
                           pQuery->lastError().databaseText()  + " - " +
                           pQuery->lastError().driverText();
      if ( ! quiet ) 
      {
         QMessageBox::warning(NULL, "Error!", errString, "Okay");
      }
      printf("QcjDataSqlTable::refresh(): Exit w/ error: %s\n", qPrintable(errString));
      fflush(stdout);
      QApplication::restoreOverrideCursor();
      return(false);
   }

   if ( rows > 0 ) 
   {
      printf("QcjDataSqlTable::refresh(): Setting header sort order indicators for table |%s|\n", qPrintable(tableName));
      for (it = sortFields.begin(); it != sortFields.end(); ++it ) 
      {
         int findex;
         QString fname;
         QString forder;

   //      QRegExp re("(.*) (asc|desc|ASC|DESC)");
         QRegExp re("(.*) (ASC|DESC)");
         re.setMinimal(true);
         re.setCaseSensitivity(Qt::CaseInsensitive);

         horizontalHeader()->setSortIndicatorShown(true);
         if ( (*it).contains(re) ) 
         {
            fname = re.cap(1);
            forder = re.cap(2);
         }
         else 
         {
            fname = (*it);
            forder = "ASC";
         }
         findex = pModel->record().indexOf(fname);

         printf("QcjDataSqlTable::refresh(): Setting sort indicator, field name |%s|, idx %d, order |%s|\n", qPrintable(fname), findex, qPrintable(forder));

         if ( forder.toLower() == "desc" ) 
            horizontalHeader()->setSortIndicator(findex, Qt::DescendingOrder);
         else
            horizontalHeader()->setSortIndicator(findex, Qt::AscendingOrder);
         break;
      }
   }
   printf("QcjDataSqlTable::refresh(): Exit\n");
   fflush(stdout);
   QApplication::restoreOverrideCursor();
   return(true);
}

/*!
      \fn void QcjDataSqlTable::setSort(QStringList sortList)

       Sets  the  fields to sort by and their precidence and calls <em>refresh()</em>. 
       The items in
       the  string  list  can be just the field name or the field name
       with  a  qualifier  such  as 'ASC' or 'DESC'. At this level, as
       many  fields  can be added ad desired. They will be prioritized
       by  their position in the list. The first field named will have
       the highest priority.
*/ 
void QcjDataSqlTable::setSort(QStringList sortList)
{
   sortFields = sortList;
}

/*!
      \fn void QcjDataSqlTable::setFilter(QString clause)

       Sets  the  where clause to use for filtering the contens of the
       table    and    calls    <em>refresh()</em>.    The   parameter
       <em>clause</em>  should be a standard where cluase (without the
       word 'where'). If a field specified does not exist in the table
       an error will be generated when the table is refreshed.
*/ 
void QcjDataSqlTable::setFilter(QString clause)
{
   printf("QcjDataSqlTable::setFilter(): Enter\n");
   fflush(stdout);
   printf("QcjDataSqlTable::setFilter(): this = %ld\n", (long)this);
   fflush(stdout);
   whereClause = clause;
   printf("QcjDataSqlTable::setFilter(): calling refresh\n");
   fflush(stdout);
   refresh();
   printf("QcjDataSqlTable::setFilter(): Exit\n");
   fflush(stdout);
}

/*!
      \fn QString QcjDataSqlTable::filter()

       Returns the where clause set for the table.
*/ 
QString QcjDataSqlTable::filter()
{
   return(whereClause);
}

/*!
      \fn QStringList QcjDataSqlTable::sort()

       Returns  a  list  of  fields representing the sort order of the
       table.
*/ 
QStringList QcjDataSqlTable::sort()
{
   return(sortFields);
}

/*!
      \fn void QcjDataSqlTable::recordSelected(int row)
       Internal slot called when a record is selected.

       Currently not used. \sa void QcjDataSqlTable::recordSelected(const QModelIndex &index)
*/ 
void QcjDataSqlTable::recordSelected(int row)
{
   printf("QcjDataSqlTable::recordSelected():Enter: row = %d\n", row);
   fflush(stdout);
//   setItemSelected(pNew, false);
   selectRow(row);
   printf("QcjDataSqlTable::recordSelected():Exit\n");
   fflush(stdout);
}

/*!
      \fn void QcjDataSqlTable::recordSelected(const QModelIndex &index)

       Internal slot called when a record is selected.
*/ 
void QcjDataSqlTable::recordSelected(const QModelIndex &index)
{
   printf("QcjDataSqlTable::recordSelected():Enter: row = %d\n", index.row());
   fflush(stdout);
//   setItemSelected(pNew, false);
   selectRow(index.row());
   printf("QcjDataSqlTable::recordSelected():Exit\n");
   fflush(stdout);
}

/*!
      \fn void QcjDataSqlTable::recordActivated(const QModelIndex &index)
       Internal slot called when a record is double clicked.
*/ 
void QcjDataSqlTable::recordActivated(const QModelIndex &index)
{
   printf("QcjDataSqlTable::recordActivated():Enter: row = %d\n", index.row());
   fflush(stdout);
//   setItemSelected(pNew, false);
//   seekRow(index.row());
   emit rowActivated(&rec);
   printf("QcjDataSqlTable::recordActivated():Exit\n");
   fflush(stdout);
}

/*!
      \fn void QcjDataSqlTable::recordSelected(const QModelIndex &current, const QModelIndex &prev)

       Internal slot used when a record is selected.
*/ 
void QcjDataSqlTable::recordSelected(const QModelIndex &current, const QModelIndex &)
{
   printf("QcjDataSqlTable::recordSelected():Enter: row = %d\n", current.row());
   fflush(stdout);
//   setItemSelected(pNew, false);
   seekRow(current.row());
   emit rowSelected(&rec);
   printf("QcjDataSqlTable::recordSelected():Exit\n");
   fflush(stdout);
}

/*!
      \fn void QcjDataSqlTable::seekRow(int new_row)

       Seeks   and   positions   the   current   record   at  the  row
       <em>new_row</em> if it is valid. If the row is valid the row is
       made the current record and the record is highlighted.
*/ 
void QcjDataSqlTable::seekRow(int new_row)
{
   printf("QcjDataSqlTable::seekRow(): Enter new_row = %d\n", new_row);
   fflush(stdout);
   if ( pQuery == NULL ) 
      return;

   printf("QcjDataSqlTable::seekRow(): table refreshed\n");
   fflush(stdout);
   printf("QcjDataSqlTable::seekRow():Seeking new_row\n");
   fflush(stdout);
   if ( pQuery->seek(new_row) ) 
   {
      printf("QcjDataSqlTable::seekRow():Fetching record\n");
      fflush(stdout);
      rec = pQuery->record();
      printf("QcjDataSqlTable::seekRow():Selecting table widget new_row\n");
      fflush(stdout);
   }
   printf("QcjDataSqlTable::seekRow(): Selecting row\n");
   fflush(stdout);
   current_row = new_row;
   printf("QcjDataSqlTable::seekRow(): Exit new_row = %d\n", new_row);
   fflush(stdout);
   return;
}
