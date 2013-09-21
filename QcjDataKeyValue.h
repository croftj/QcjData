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

#ifndef DATAKEYVALUE_H
#define DATAKEYVALUE_H

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

#ifndef QT4_DESIGNER_PLUGIN
#include <QSqlDatabase>
#endif

#include <vector>


#ifndef QT4_DESIGNER_PLUGIN
#include "QcjData/QcjDataHelpers.h"
#endif

class QcjDataXML;
 
class QcjDataKeyValue : public QFrame
{
   Q_OBJECT
   Q_PROPERTY (QString xml_definition READ readXmlDef WRITE writeXmlDef)

public:
   QcjDataKeyValue(QWidget *pParent = 0 );
   ~QcjDataKeyValue() 
   {
#ifndef QT4_DESIGNER_PLUGIN
      if ( deleteDb && db == 0 ) 
         delete db;
#endif
   };

   /*!
         \fn QString readXmlDef() const { return(xmldef); };
          Returns the name of the XML definition for this form.
   */ 
   QString readXmlDef() const { return(xmldef); };

   void setDatabase(QSqlDatabase *pdb);
   void setDatabase();

#ifndef QT4_DESIGNER_PLUGIN
   void setFields(std::vector<struct QcjDataFields> fds, QSettings *pConfig = NULL, QcjDataXML *pFormDef = NULL);
   void setFields(QcjDataFields *fields, QSettings *pConfig = NULL, QcjDataXML *pFormDef = NULL);
#endif

#ifndef QT4_ESIGNER_PLUGIN
   void updateFieldSettings(QSettings *pCfg = NULL, QcjDataXML *pFDef = NULL);
#endif

   void writeXmlDef(QString s);
   static QString findXML(QStringList app_path, QString app_name);

public slots:   
   void beginTransaction();
   void commitTransaction();

   QString getFieldText(QString name);
   QVariant getFieldValue(QString name);
   QWidget *getFieldWidget(QString name);

//   void initializeWidget(QString name, QString str);
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
   void updated();


protected:
#ifndef QT4_DESIGNER_PLUGIN
   void _setFields(QcjDataFields *fields, QSettings *pConfig = NULL, QcjDataXML *pFormDef = NULL);
#endif
   QString getDefaultFieldValue(QString defstring);

private:
   QMap<QString, QWidget*>             formFields;
   QMap<QString, QValidator*>          formValidators;
   QMap<QString, QString>              widgetProperties;
   QMap<QString, QString>              fieldDefaults;
   QMap<QString, QSqlTableModel*>     fieldModels;
   QMap<QString, QDataWidgetMapper*>   fieldMappers;
   QMap<QString, QSqlRecord*>          fieldRecords;
   QGridLayout                         *gl;
   QString                             dbTable;
   QString                             xmldef;
   bool                                inTransaction;
#ifndef QT4_DESIGNER_PLUGIN
   QSqlRecord                          sqlRecord;
   QDataWidgetMapper                   sqlMapper;
   QSqlTableModel                      *sqlModel;
   QSqlDatabase                        *db;
   bool                                deleteDb;
   std::vector<struct QcjDataFields>   fields;
#endif
};

#endif



