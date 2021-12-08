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

#ifndef DATAFORM_H
#define DATAFORM_H

#include <QComboBox>
#include <QDataWidgetMapper>
#include <QFrame>
#include <QGridLayout>
#include <QLineEdit>
#include <QMap>
#include <QSettings>
#include <QSqlRecord>
#include <QSqlTableModel>
#include <QString>
#include <stdlib.h>
#include <stdio.h>


#ifndef QT4_DESIGNER_PLUGIN
#include <QSqlDatabase>
#endif

#include <vector>


#ifndef QT4_DESIGNER_PLUGIN
#include "QcjData/QcjDataHelpers.h"
#endif

class QcjDataXML;
 
class QcjDataForm : public QFrame
{
   Q_OBJECT
   Q_PROPERTY (QString xml_definition READ readXmlDef WRITE writeXmlDef)

public:
   QcjDataForm(QWidget *pParent = 0 );
   ~QcjDataForm()
   {
#ifndef QT4_DESIGNER_PLUGIN
      if ( deleteDb && db == 0 ) 
         delete db;
#endif
   };

   QString getLastIdent();
   /*!
         \fn QSqlRecord *record()
          Returns a pointer QSqlRecord for this form
   */ 
   QSqlRecord *record()
   {
#ifndef QT4_DESIGNER_PLUGIN
      return(&sqlRecord);
#else
      return(NULL);
#endif
   };
   /*!
         \fn QString readXmlDef()
          Returns the name of the XML definition for this form.
   */ 
   QString readXmlDef() const { return(xmldef); };
   void setDatabase(QSqlDatabase *pdb);
   void setDatabase();

#ifndef QT4_DESIGNER_PLUGIN
   void setFields(std::vector<struct QcjDataFields> fds, QSettings *pConfig = NULL, QcjDataXML *pFormDef = NULL);
   void setFields(QcjDataFields *fields, QSettings *pConfig = NULL, QcjDataXML *pFormDef = NULL);
   void setValues(QSqlRecord *rec);
#endif

   /*!
         \fn bool transactionOpen()
         Returns  true  if  a  transaction  is currently open for the
         database of this form
   */ 
   bool transactionOpen()
   {
      return(inTransaction);
   };

   /*!
         \fn bool insertActive()
         Returns true if an insertRecord call was made and no
         subsequent updateRecord call made.
   */
   bool insertActive()
   {
      return(insertMode);
   };

#ifndef QT4_DESIGNER_PLUGIN
   void updateFieldSettings(QSettings *pCfg = NULL, QcjDataXML *pFDef = NULL);
#endif

   void writeXmlDef(QString s);
   static QString findXML(QStringList app_path, QString app_name);
   QMap<QString, QString> getIndexMap();
   QString getIndexFilter();
   QString makeFilter(QStringList exceptions);
   QString getFieldText(QString name);
   QVariant getFieldValue(QString name);
   QWidget *getFieldWidget(QString name);


public slots:   
   void beginTransaction();
   void commitTransaction(bool emitUpdated = true);
   void deleteRecord();
   void haveFileName(QString fn)
   {
      printf("QcjDataForm::haveFileName(): Enter fn = |%s|\n", qPrintable(fn));
      emit fileName(fn);
   };
   void initializeWidget(QString name, QString str);
   void insertRecord();
   void insertRecord(QMap<QString, QString> &fields);
   void clearForm();
   void refresh(QSqlRecord *);
   void refresh();
   void resetFieldEnables();
   void rollbackTransaction();
   void updateRecord();
   void setEnabled(bool b);
   void setReadOnly(bool b);
   void setFieldText(QString name, QString str);
   void setFieldValue(QString name, QVariant val);
   void setFieldEnabled(QString name, bool f);
   void setFocus();
   bool validateEdits();

signals:
   /*!
      \fn void activeRecord(QMap<QString, QString> indexmap)
          Emitted    when    a    record    is    updated.   Parameter
          <em>indexmap</em>  will  hold  the index information for the
          record.
   */ 
   void activeRecord(QMap<QString, QString> indexmap);

   /*!
      \fn void fileName(QString fn)
      Emitted when a Image file is selected. The parameter <em>fn</em> is name of the file selected.
   */
   void fileName(QString);
   /*!
          \fn void updated();

          Emitted every time a record is updated or deleted. Typically this signal will
          be connected to the refresh slot if the associated QcjDataTable object (if any).
   */ 
   void updated();


protected:
#ifndef QT4_DESIGNER_PLUGIN
   void _setFields(QcjDataFields *fields, QSettings *pConfig = NULL, QcjDataXML *pFormDef = NULL);
#endif
   QString getDefaultFieldValue(QString defstring);

private:
   QMap<QString, QWidget*>    formFields;
   QMap<QString, QWidget*>    formWidgets;
   QMap<QString, QValidator*> formValidators;
   QMap<QString, QString>     widgetProperties;
   QMap<QString, QString>     fieldDefaults;
   QMap<QWidget*, QString>    fieldSearch;
   QGridLayout                *gl;
   QString                    dbTable;
   QString                    ident;
   QString                    strIdent;
   QString                    strWhere;
   bool                       insertMode;
   bool                       inTransaction;
   QString                    xmldef;
#ifndef QT4_DESIGNER_PLUGIN
//   Q3SqlForm                   *sqlForm;
//   Q3SqlCursor                 *sqlCursor;
   QSqlRecord                 sqlRecord;
   QDataWidgetMapper          sqlMapper;
   QSqlTableModel             *sqlModel;
   QSqlDatabase               *db;
   bool                       deleteDb;
   bool                       invalidData;
   std::vector<struct QcjDataFields> fields;
#endif
};

#endif


