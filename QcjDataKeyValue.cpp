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

#ifndef QT4_DESIGNER_PLUGIN
#include <stdlib.h>
#include <qfile.h>
#include <qfont.h>
#include <qfontmetrics.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qmessagebox.h>
#include <qregexp.h>
#include <QBuffer>
#include <QSpinBox>
#include <QSqlDriver>
#include <QSqlError>
#include <qsqlrecord.h>
#include <QGridLayout>
#include <stdio.h>
#include <qstringlist.h>
#include "QcjData/QcjDataDelegate.h"
#include "QcjData/QcjDataHelpers.h"
#include "QcjData/QcjDataLogin.h"
#include "QcjData/QcjDataXML.h"
#endif

#include <QSqlField>
#include <QSqlQuery>
#include "QcjData/QcjDataStatics.h"

#include "QcjData/QcjDataKeyValue.h"

#define  QCJ_FIRSTRECORD   0  // Value to use to retrieve the first record of a query

static bool isA(QObject *obj, const char *type);

/*!                                                                 
       \class QcjDataKeyValue
       
       \brief  This  class  generates  a  QFrame containing a database
       form.
       
       The  QcjDataKeyValue  class creates a QFrame object holding a form for
       entering  data  into multiple records of a database table. It is assumed that
       this table is a set of records forming key/value pairs. 
       
       The definition of this form is
       defined in a &lt;form&gt; element of the applications XML file. The definition
       of the form must include the name of the field to use as the 'key', the name of the field
       which holds the value. Each of the individual field definitions of the form
       must hold the value of the field used as the key.

       Example:
       <p>
<pre>
<code>
   <form name="my_params" table_name="system_params" auto_commit="yes" key_field="name" value_field="value" >
       <field key="def_descr" label="Default Description" row="0" col="0" colspan="0" rowspan="0" min_width="0" max_width="0" read_only="0" type="lineedit" />
       <field key="def_date" label="Default Date" row="1" col="0" colspan="0" rowspan="0" min_width="0" max_width="0" read_only="0" type="dateedit" />
       <field key="def_measure" label="Default Unit of Measure" row="2" col="0" colspan="0" rowspan="0" min_width="0" max_width="0" read_only="0" type="strsel" init="string:"Oz.,Qt.,Lb.,Ea." default="Ea." />
       <field key="Convert Metric" label="Convert to Metric" row="3" col="0" colspan="0" rowspan="0" min_width="0" max_width="0" read_only="0" type="yesno" default="Y" />
   </form>
</code>
</pre>

*/

QcjDataKeyValue::QcjDataKeyValue(QWidget *pParent) :
          QFrame(pParent)
{
   printf("QcjDataKeyValue::QcjDataKeyValue(): Enter\n");
#ifndef QT4_DESIGNER_PLUGIN
   printf("QcjDataKeyValue::QcjDataKeyValue(): dbTable = |%s|\n", (const char*)dbTable.toLocal8Bit());
   fflush(stdout);
   db = pDb;
   db = 0;
   deleteDb = false;
   setFocusPolicy(Qt::StrongFocus);
   xmldef = "";
   inTransaction = false;
#else
   setFrameShape(QFrame::Box);
#endif
   printf("QcjDataKeyValue::QcjDataKeyValue(): Exit\n");
   fflush(stdout);
}

/*!                                                                 
      \fn void QcjDataKeyValue::writeXmlDef(QString str)
       Sets the name of the XML definition to use for this form
*/                                                                  
void QcjDataKeyValue::writeXmlDef(QString str)
{
   printf("QcjDataKeyValue::writeXmlDef(): Enter setting xmldef to |%s|\n", (const char*)str.toLocal8Bit());
   fflush(stdout);
   xmldef = str;
   printf("QcjDataKeyValue::writeXmlDef(): Exit\n");
   fflush(stdout);
}



void QcjDataKeyValue::setDatabase(QSqlDatabase *pdb)
{
#ifndef QT4_DESIGNER_PLUGIN
   db = new QSqlDatabase(*pdb);
   deleteDb = true;
   setDatabase();
#endif
}

/*!                                                                 
       \fn void QcjDataKeyValue::setDatabase()

       Sets  sets up the database for this form. If the database needs
       connected  to,  this  function  will  prompt the user for login
       information. It will also get the name of the XML definition to
       use for the form.
*/                                                                  
void QcjDataKeyValue::setDatabase()
{
#ifndef QT4_DESIGNER_PLUGIN
   printf("QcjDataKeyValue::setDatabase(): Enter\n");
   fflush(stdout);

   if ( pDb != 0 && db == 0 ) 
   {
      printf("QcjDataKeyValue::setDatabase(): Using existing database\n");
      fflush(stdout);
      db = pDb;
   }

   printf("QcjDataKeyValue::setDatabase(): Checking if we logged in yet\n");
   fflush(stdout);
   if ( db == 0 ) 
   {
      QcjDataLogin *login = new QcjDataLogin();
      login->initialize();
      login->show();
      if (! login->exec() )
      {
         printf("QcjDataKeyValue::setDatabase(): Error with login\n");
         fflush(stdout);
         return;
      }
      delete login;
      db = pDb;
      if ( pFormDef->database() == 0 ) 
         pFormDef->setDatabase(db);
   }

   printf("QcjDataKeyValue::setDatabase(): xmldef = |%s|\n", qPrintable(xmldef));
   fflush(stdout);

   printf("QcjDataKeyValue::setDatabase(): retrieving table\n");
   fflush(stdout);
   dbTable = pFormDef->getTable(xmldef);
   printf("QcjDataKeyValue::setDatabase(): got table\n");
   fflush(stdout);

   printf("QcjDataKeyValue::setDatabase(): dbTable = |%s|\n", qPrintable(dbTable));
   fflush(stdout);
   gl = new QGridLayout(this);
   gl->activate();
   printf("QcjDataKeyValue::setDatabase(): Calling setFields()\n");
   fflush(stdout);
   setFields(pFormDef->getFields(xmldef, this), pConfig, pFormDef);
   refresh();
   printf("QcjDataKeyValue::setDatabase(): Exit\n");
   fflush(stdout);
#endif
}


#ifndef QT4_DESIGNER_PLUGIN

void QcjDataKeyValue::setFields(QcjDataFields *fds, QSettings *pCfg, QcjDataXML *pFDef) 
{
   printf("QcjDataKeyValue::setFields():1: Enter\n");
   printf("QcjDataKeyValue::setFields(): xmldef = |%s|\n", (const char*)xmldef.toLocal8Bit());
   fflush(stdout);
   const QcjDataFields *pFds = fds;
   while (pFds->dataName != QString::null && pFds->dataName != QString("--ENDOFFIELDS--"))
   {
      printf("QcjDataKeyValue::setFields(): New field |%s|\n", (const char*)pFds->dataName.toLocal8Bit());
      fflush(stdout);
      fields.push_back(*pFds);
      pFds++;
   }

   struct QcjDataFields *p = new struct QcjDataFields;
   p->dataName = QString("--ENDOFFIELDS--");
   fields.push_back(*p);
   delete p;
   _setFields(fds, pCfg, pFDef);
   printf("QcjDataKeyValue::setFields():1: Exit\n");
   fflush(stdout);
}

void QcjDataKeyValue::setFields(std::vector<struct QcjDataFields> fds, QSettings *pCfg, QcjDataXML *pFDef)
{
   fields = fds;
   struct QcjDataFields f[60];
//   struct QcjDataFields f[fields.size() + 1];
   unsigned x;

   printf("QcjDataKeyValue::setFields():2: Enter\n");

   printf("QcjDataKeyValue::setFields(): xmldef = |%s|\n", (const char*)xmldef.toLocal8Bit());
   fflush(stdout);
   for (x = 0; x < fields.size(); x++) 
      f[x] = fields[x];
   _setFields(f, pCfg, pFDef);
   printf("QcjDataKeyValue::setFields():2: Exit\n");
   fflush(stdout);
}

void QcjDataKeyValue::_setFields(QcjDataFields *fields, QSettings *pCfg, QcjDataXML *pFDef) 
{
   QWidget *wdt, *lastwdt = NULL;
   int count = 0;
   printf("QcjDataKeyValue::_setFields(): Enter\n");
   fflush(stdout);

   QSizePolicy labelPolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);
   QSizePolicy wdtPolicySingl(QSizePolicy::Minimum, QSizePolicy::Maximum);
   QSizePolicy wdtPolicyMulti(QSizePolicy::MinimumExpanding, QSizePolicy::Expanding);
   if ( db == NULL ) 
   {
      printf("QcjDataKeyValue::_setFields(): No QcjDatabase, Exit\n");
      return;
   }
   QSqlDriver *dbdrv = db->driver();

   setFocusPolicy(Qt::StrongFocus);

   printf("QcjDataKeyValue::_setFields(): Iterating field info\n");
   fflush(stdout);

   //
   // Iterate through all of the field definitions and 
   // place them into the grid.
   //
   while (fields->dataName != QString::null && fields->dataName != QString("--ENDOFFIELDS--"))
   {
      count++;
      printf("QcjDataKeyValue::_setFields(): *************** New field, count = %d\n", count);
      fflush(stdout);
      printf("QcjDataKeyValue::_setFields(): New field |%s|\n", (const char*)fields->key.toLocal8Bit());
      fflush(stdout);
      if ( fields->widget == NULL ) 
      {
         printf("QcjDataKeyValue::_setFields(): New edit field\n");
         fflush(stdout);
         wdt = new QLineEdit(this);
      }
      else 
      {
         printf("QcjDataKeyValue::_setFields(): Existing Widget\n");
         fflush(stdout);
         wdt = fields->widget;
         printf("QcjDataKeyValue::_setFields():Have widget\n");
         fflush(stdout);
      }
      if ( lastwdt != NULL ) 
         setTabOrder(lastwdt, wdt);
      lastwdt = wdt;
      if ( fields->ro ) 
        wdt->setFocusPolicy(Qt::NoFocus);

      /***********************************************/
      /*   Create and configure the widget mappings  */
      /*   for this entry                            */
      /***********************************************/
      QSqlTableModel *sqlModel = new QSqlTableModel(this, *db);
      sqlModel->setTable(dbTable);
      sqlModel->setEditStrategy(QSqlTableModel::OnManualSubmit);
      fieldModels[fields->key] = sqlModel;

      QDataWidgetMapper *sqlMapper = new QDataWidgetMapper(this);   
      sqlMapper->setModel(sqlModel);
      sqlMapper->setItemDelegate(new QcjDataDelegate(this));
      fieldMappers[fields->key] = sqlMapper;

      printf("QcjDataKeyValue::_setFields(): performing initial select\n");
      fflush(stdout);
   //   sqlModel->select();

      printf("QcjDataKeyValue::_setFields(): Models db name = '%s'\n", qPrintable(sqlModel->database().databaseName()));
      fflush(stdout);

      printf("QcjDataKeyValue::_setFields(): Getting Record Info from (%ld:%s)\n", (long)dbdrv, (const char *)dbTable.toLocal8Bit());
      fflush(stdout);
      QSqlRecord recInfo = sqlModel->record();
      printf("QcjDataKeyValue::_setFields():  Record has %d fields\n", recInfo.count());
      fflush(stdout);

      printf("QcjDataKeyValue::_setFields():adding mapping for field %s, index %d\n", qPrintable(fields->key), recInfo.indexOf(fields->dataName));
      fflush(stdout);


      sqlMapper->addMapping(wdt, recInfo.indexOf(fields->valueName));

      QFont f = wdt->font();
      QFontMetrics fm(f);
      int fwidth = fm.maxWidth();
      printf("QcjDataKeyValue::_setFields():Element |%s|\n", (const char*)fields->key.toLocal8Bit());
      fflush(stdout);
      if ( fields->minWidth > 0 ) 
      {
         printf("QcjDataKeyValue::_setFields():Setting min width %d\n", fwidth * fields->minWidth);
         fflush(stdout);
         wdt->setMinimumWidth(fwidth * fields->minWidth);
      }
      if ( fields->maxWidth > 0 ) 
      {
         printf("QcjDataKeyValue::_setFields():Setting max width %d\n", fwidth * fields->maxWidth);
         fflush(stdout);
         if ( ::isA(wdt, "QcjPhotoEntry") ) 
         {
            printf("QcjDataKeyValue::_setFields():Setting max width %d\n", fields->maxWidth);
            wdt->setMaximumWidth(fields->maxWidth);
         }
         else 
         {
            printf("QcjDataKeyValue::_setFields():Setting max width %d\n", fwidth * fields->maxWidth);
            wdt->setMaximumWidth(fwidth * fields->maxWidth);
         }
      }
      QSqlField fi = recInfo.field(fields->valueName);
      printf("QcjDataKeyValue::_setFields():field has max length %d\n", fi.length());
      QVariant v = fi.type();
      printf("QcjDataKeyValue::_setFields():Field type: |%s|\n", (const char*)v.toString().toLocal8Bit());
      printf("QcjDataKeyValue::_setFields():Widget Class: |%s|\n", wdt->metaObject()->className());
      printf("QcjDataKeyValue::_setFields():Field type: |%d|\n", fi.typeID());
      fflush(stdout);


      if (QString(wdt->metaObject()->className()) == QString("QLineEdit") && (v != 0))
      {
         printf("QcjDataKeyValue::_setFields():Setting fields max length to %d\n", fi.length());
         fflush(stdout);
         ((QLineEdit*)wdt)->setMaxLength(fi.length());
      }
      else if (QString(wdt->metaObject()->className()) == QString("QcjStringSelect")) 
      {
         printf("QcjDataKeyValue::_setFields():Have strsel init = |%s|\n", qPrintable(fields->init));
         QStringList conf = fields->init.split(':');
         printf("QcjDataKeyValue::_setFields():Split complete have %d items\n", conf.count());
         if ( fields->ro ) 
            wdt->setEnabled(false);
         if ( conf.count() >= 2 ) 
         {
            QString type = conf[0];
            QString name = conf[1];
            printf("QcjDataKeyValue::_setFields():Name = |%s|, type = |%s|\n", (const char*)name.toLocal8Bit(), (const char*)type.toLocal8Bit());
            fflush(stdout);
            if ( pCfg != NULL && type == "config" ) 
            {
               QString def = "";
               if ( pFDef != NULL ) 
               {
                  printf("QcjDataKeyValue::_setFields():getting defaults for |%s|\n", (const char*)name.toLocal8Bit());
                  fflush(stdout);
                  def = pFDef->getConfigurationDefault(name);
               }
               printf("QcjDataKeyValue::_setFields(): default = |%s|\n", (const char*)def.toLocal8Bit());
               fflush(stdout);
               ((QcjStringSelect*)wdt)->initialize(pCfg->value(name, def).toString());
            }
            else if ( type == "string" )
            {
               printf("QcjDataKeyValue::_setFields(): Initializing string select\n");
               fflush(stdout);
               ((QcjStringSelect*)wdt)->initialize(name);
            }
            else if ( type == "field" ) 
            {
               if ( name.length() > 0 ) 
               {
                  QStringList sl;
                  QString init;
                  sl  = name.split('.');
                  QString sql = "select ident, " + sl[1] + " from " + sl[0] + " order by " + sl[1];
                  printf("QcjDataKeyValue::_setFields():QcjStringSelect init sql = |%s|\n", (const char*)sql.toLocal8Bit());
                  fflush(stdout);
                  QSqlQuery *query = new QSqlQuery(sql);
                  for ( int x = 0; query->next(); x++ ) 
                  {
                     if ( x > 0 ) 
                        init += ",";
                     init += query->value(0).toString();
                     init += " - ";
                     init += query->value(1).toString();
                     printf("QcjDataKeyValue::_setFields():init = |%s|\n", (const char*)init.toLocal8Bit());
                     fflush(stdout);
                  }
                  ((QcjStringSelect*)wdt)->initialize(init);
               }
            }
            else if ( type == "sqlquery" ) 
            {
               QString sql = name;
               QString init;
               printf("QcjDataKeyValue::_setFields():QcjStringSelect init sql = |%s|\n", (const char*)sql.toLocal8Bit());
               fflush(stdout);
               QSqlQuery *query = new QSqlQuery(sql);
               for ( int x = 0; query->next(); x++ ) 
               {
                  QVariant v;
                  if ( (v = query->value(1)) == QVariant::Invalid ) 
                  {
                     if ( x > 0 ) 
                        init += ",";
                     init += query->value(0).toString();
                     printf("QcjDataKeyValue::_setFields():init = |%s|\n", qPrintable(init));
                     fflush(stdout);
                  }
                  else 
                  {
                     printf("QcjDataKeyValue::_setFields():QcjStringSelect String = |%s|, data = |%s|\n", 
                            qPrintable(query->value(0).toString()), 
                            qPrintable(query->value(0).toString()));
                     ((QcjStringSelect*)wdt)->addItem(query->value(0).toString(), query->value(1));
                     fflush(stdout);
                  }
               }
               if ( init != QString::null ) 
               {
                  printf("QcjDataKeyValue::_setFields():QcjStringSelect init string = |%s|\n", (const char*)init.toLocal8Bit());
                  fflush(stdout);
                  ((QcjStringSelect*)wdt)->initialize(init);
               }
            }
         }
      }

      else if (QString(wdt->metaObject()->className()) == QString("QcjYesNoSelect")) 
      {
         printf("QcjDataKeyValue::_setFields():have yesno field\n");
         fflush(stdout);
         if ( fields->defvalue.size() == 0 ) 
         {
            printf("QcjDataKeyValue::_setFields():Setting yesno default to \"N\"\n");
            fflush(stdout);
            fields->defvalue = "N";
         }
      }

      printf("QcjDataKeyValue::_setFields():done with field type\n");
      fflush(stdout);
      formFields[fields->key] = wdt;

      if ( fields->validator != NULL ) 
      {
         formValidators[fields->key] = fields->validator;
      }

      if ( fields->propName != (const char*)NULL ) 
      {
         printf("QcjDataKeyValue::_setFields(): Setting Property Name: |%s|\n", qPrintable(fields->propName));
         fflush(stdout);
         widgetProperties[fields->key] = fields->propName;
      }

      printf("QcjDataKeyValue::_setFields():Default Text: |%s|\n", qPrintable(fields->defvalue));
      fflush(stdout);
      fieldDefaults[fields->key] = fields->defvalue;

      
      printf("QcjDataKeyValue::_setFields(): rowSpan = %d\n", fields->rowSpan);
      fflush(stdout);
      printf("QcjDataKeyValue::_setFields(): colSpan = %d\n", fields->colSpan);
      fflush(stdout);
      if ( fields->rowSpan <= 0) 
      {
         printf("QcjDataKeyValue::_setFields(): Adding label- row = %d, col = %d\n", fields->row, fields->col * 2);
         fflush(stdout);
         QLabel *lbl = new QLabel(fields->label, this);
         lbl->setSizePolicy(labelPolicy);
         lbl->setAlignment(Qt::AlignTop|Qt::AlignRight);
         gl->addWidget(lbl, fields->row, fields->col * 2);
      }
      else 
      {
         int rows = (fields->rowSpan == 1) ? 0 : fields->rowSpan;
         printf("QcjDataKeyValue::_setFields(): rows = %d\n", rows);
         fflush(stdout);
         printf("QcjDataKeyValue::_setFields(): Adding multi row label- beg row = %d, row span = %d beg col = %d, colspan = %d\n", 
                                       fields->row, rows, (fields->col * 2), 1);
         fflush(stdout);
         QLabel *lbl = new QLabel(fields->label, this);
         lbl->setAlignment(Qt::AlignTop|Qt::AlignRight);
         lbl->setSizePolicy(labelPolicy);
         gl->addWidget(lbl, fields->row, (fields->col * 2), rows, 1);
      }
      if ( fields->colSpan > 1 || fields->rowSpan > 1) 
      {
         int rows = (fields->rowSpan <= 1) ? 1 : fields->rowSpan;
         printf("QcjDataKeyValue::_setFields(): rows = %d\n", rows);
         fflush(stdout);
         int cols = (fields->colSpan <= 1) ? 1 : fields->colSpan;
         printf("QcjDataKeyValue::_setFields(): cols = %d\n", cols);
         fflush(stdout);
         if ( fields->rowSpan > 1 ) 
         {
            printf("QcjDataKeyValue::_setFields(): Multi Policy\n");
            wdt->setSizePolicy(wdtPolicyMulti);
         }
         else
         {
            printf("QcjDataKeyValue::_setFields(): Single Policy\n");
            wdt->setSizePolicy(wdtPolicySingl);
         }
         printf("QcjDataKeyValue::_setFields(): Adding multicell widget- beg row = %d, row span = %d beg col = %d, colspan = %d\n", 
                                        fields->row, rows, (fields->col * 2) + 1, cols);
         fflush(stdout);
         gl->addWidget(wdt, fields->row, (fields->col * 2) + 1, rows, cols > 1 ? ((cols - 1) * 2) + 1 : cols);
      }
      else
      {
         printf("QcjDataKeyValue::_setFields(): Adding widget- row = %d, col = %d\n", fields->row, (fields->col * 2) + 1);
         fflush(stdout);
         wdt->setSizePolicy(wdtPolicySingl);
         gl->addWidget(wdt, fields->row, (fields->col * 2) + 1);
      }

      printf("QcjDataKeyValue::_setFields(): done with field\n");
      fflush(stdout);
      fields++;
      if (fields->dataName != "--ENDOFFIELDS--")
         printf("QcjDataKeyValue::_setFields(): FALSE |%s|\n", (const char*)fields->dataName.toLocal8Bit());
      else
         printf("QcjDataKeyValue::_setFields(): TRUE |%s|\n", (const char*)fields->dataName.toLocal8Bit());
      fflush(stdout);
   }
   printf("QcjDataKeyValue::_setFields(): All fields Complete\n");
   fflush(stdout);
   setReadOnly(false);
   printf("QcjDataKeyValue::_setFields(): Exit\n");
   fflush(stdout);
   return;
}

/*!                                                                 
      \fn void QcjDataKeyValue::updateFieldSettings(QSettings *pCfg, QcjDataXML *pFDef) 

       Updates the fields which are string selects with  current data.
       This  is good for when new records are added to the database or
       settings are updated.
*/                                                                  
void QcjDataKeyValue::updateFieldSettings(QSettings *pCfg, QcjDataXML *pFDef) 
{
   int count = 0;
   printf("QcjDataKeyValueupdateFieldSettings(): Enter\n");
   fflush(stdout);

   QSqlRecord recInfo = db->record(dbTable);

   printf("QcjDataKeyValue::updateFieldSettings(): Iterating field info\n");
   fflush(stdout);

   //
   // Iterate through all of the field definitions and 
   // place them into the grid.
   //
   std::vector<struct QcjDataFields>::iterator it;
   for ( it = fields.begin(); it < fields.end(); ++it)
   {
      QString dataName = (*it).dataName;
      if ( dataName == "--ENDOFFIELDS--" ) 
         break;
      count++;
      printf("QcjDataKeyValue::updateFieldSettings(): *************** New field, count = %d\n", count);
      fflush(stdout);
      printf("QcjDataKeyValue::updateFieldSettings(): New field |%s|\n", (const char*)(*it).key.toLocal8Bit());
      fflush(stdout);

      QWidget *wdt = (*it).widget;
      if (QString(wdt->metaObject()->className()) == QString("QcjStringSelect")) 
      {
         ((QcjStringSelect*)wdt)->clear();
         printf("QcjDataKeyValue::updateFieldSettings():Have strsel init = |%s|\n", (const char*)(*it).init.toLocal8Bit());
         QStringList conf = (*it).init.split(':');
         printf("QcjDataKeyValue::updateFieldSettings():Split complete have %d items\n", conf.count());
         if ( conf.count() >= 2 ) 
         {
            QString type = conf[0];
            QString name = conf[1];
            printf("QcjDataKeyValue::updateFieldSettings():Name = |%s|, type = |%s|\n", (const char*)name.toLocal8Bit(), (const char*)type.toLocal8Bit());
            if ( pCfg != NULL && type == "config" ) 
            {
               QString def = "";
               if ( pFDef != NULL ) 
               {
                  printf("QcjDataKeyValue::updateFieldSettings():getting defaults for |%s|\n", (const char*)name.toLocal8Bit());
                  def = pFDef->getConfigurationDefault(name);
               }
               printf("QcjDataKeyValue::updateFieldSettings(): default = |%s|\n", (const char*)def.toLocal8Bit());
               ((QcjStringSelect*)wdt)->initialize(pCfg->value(name, def).toString());
            }
            else if ( type == "string" )
            {
               printf("QcjDataKeyValue::updateFieldSettings(): Initializing string select\n");
               ((QcjStringSelect*)wdt)->initialize(name);
            }
            else if ( type == "field" ) 
            {
               QStringList sl;
               QString init;
               sl  = name.split('.');
               QString sql = "select init, " + sl[1] + " from " + sl[0] + " order by " + sl[1];
               printf("QcjDataKeyValue::updateFieldSettings():QcjStringSelect init sql = |%s|\n", (const char*)sql.toLocal8Bit());
               QSqlQuery *query = new QSqlQuery(sql);
               for ( int x = 0; query->next(); x++ ) 
               {
                  if ( x > 0 ) 
                     init += ",";
                  init += query->value(0).toString();
                  init += " - ";
                  init += query->value(1).toString();
                  printf("QcjDataKeyValue::updateFieldSettings():init = |%s|\n", (const char*)init.toLocal8Bit());
               }
               ((QcjStringSelect*)wdt)->initialize(init);
            }
            else if ( type == "sqlquery" ) 
            {
               QString sql = name;
               QString init;
               printf("QcjDataKeyValue::updateFieldSettings():QcjStringSelect init sql = |%s|\n", (const char*)sql.toLocal8Bit());
               QSqlQuery *query = new QSqlQuery(sql);
               for ( int x = 0; query->next(); x++ ) 
               {
                  QVariant v;
                  if ( (v = query->value(1)) == QVariant::Invalid ) 
                  {
                     if ( x > 0 ) 
                        init += ",";
                     init += query->value(0).toString();
                     printf("QcjDataKeyValue::updateFieldSettings():init = |%s|\n", (const char*)init.toLocal8Bit());
                  }
                  else 
                  {
                     printf("QcjDataKeyValue::updateFieldSettings():QcjStringSelect String = |%s|, data = |%s|\n", (const char*)query->value(0).toString().toLocal8Bit(),
                                                                                                (const char*)query->value(1).toString().toLocal8Bit());
                     ((QcjStringSelect*)wdt)->addItem(query->value(0).toString(), query->value(1));
                  }
               }
               if ( init != QString::null ) 
               {
                  printf("QcjDataKeyValue::updateFieldSettings():QcjStringSelect init string = |%s|\n", (const char*)init.toLocal8Bit());
                  ((QcjStringSelect*)wdt)->initialize(init);
               }
            }
         }
      }
      printf("QcjDataKeyValue::updateFieldSettings():done with field type\n");
      fflush(stdout);
   }
   printf("QcjDataKeyValue::updateFieldSettings(): All fields Complete\n");
   fflush(stdout);
//   setReadOnly(false);
   printf("QcjDataKeyValue::updateFieldSettings(): Exit\n");
   fflush(stdout);
   return;
}

#endif

/*!
      \fn void QcjDataKeyValue::refresh()

       Reads  the  elements  in  the form with the data of the current
       form.
*/ 
void QcjDataKeyValue::refresh()
{
#ifndef QT4_DESIGNER_PLUGIN
   printf("QcjDataKeyValue::refresh(): xmldef = |%s|\n", (const char*)xmldef.toLocal8Bit());
   printf("QcjDataKeyValue::refresh(): dbTable = |%s|\n", (const char*)dbTable.toLocal8Bit());
   fflush(stdout);
   if ( ! db->isOpen() ) 
   {
      printf("QcjDataKeyValue::refresh(): QcjDatabase closed, logging in again\n");
      if ( ! db->open() )
      {
         printf("QcjDataKeyValue::refresh(): Error opeing db connection!\n");
      }
   }

   std::vector<struct QcjDataFields>::iterator it;
   for ( it = fields.begin(); it < fields.end(); ++it)
   {
      printf("QcjDataKeyValue::refresh(): Processing field: %s\n", qPrintable((*it).key));
      QString key = (*it).key;
      QString dataName = (*it).dataName;

      if ( dataName == "--ENDOFFIELDS--" ) 
         break;

      QSqlTableModel *sqlModel = fieldModels[key];
      QDataWidgetMapper *sqlMapper = fieldMappers[key];

      QString strWhere = dataName + " = '" + key + "'";
      printf("QcjDataKeyValue::refresh(): Setting model filter to: %s\n", qPrintable(strWhere));
      sqlModel->setFilter(strWhere);
      if ( ! sqlModel->select() )
         QMessageBox::warning(NULL, "Error!", "Error selecting row\n" + sqlModel->lastError().databaseText() + "\n" + 
                              sqlModel->lastError().driverText() + "\n" +
                              sqlModel->query().executedQuery(), "Okay");

      sqlMapper->setCurrentIndex(QCJ_FIRSTRECORD);

      printf("QcjDataKeyValue::refresh(): Found item |%s|\n", (const char *)sqlRecord.value(pFormDef->getIndexField(xmldef)).toString().toLocal8Bit());
      fflush(stdout);

      printf("QcjDataKeyValue::refresh(): Moving to next field\n");
      fflush(stdout);
   }
#endif
   printf("QcjDataKeyValue::refresh():Exit\n");
   fflush(stdout);
}

void QcjDataKeyValue::updateRecord()
{
   printf("QcjDataKeyValue::updateRecord():Enter()\n");
   printf("QcjDataKeyValue::update(): xmldef = |%s|\n", (const char*)xmldef.toLocal8Bit());
   fflush(stdout);

#ifndef QT4_DESIGNER_PLUGIN
   for (QMap<QString, QWidget*>::iterator it1 = formFields.begin(); it1 != formFields.end(); ++it1) 
   {
      QWidget *wdt = it1.value();
      QString name = it1.key();
      if ( ::isA(wdt, "QLineEdit") && formValidators.contains(name)) 
      {
         printf("QcjDataKeyValue::update(): field %s Is a line edit\n", qPrintable(name));
         const QValidator *val = formValidators[name];
         if ( val != NULL ) 
         {
            printf("QcjDataKeyValue::update(): have validator\n");
            QString text = ((QLineEdit*)wdt)->text();
            int pos;
            printf("QcjDataKeyValue::update(): validateing string\n");
            if ( val->validate(text, pos) == QValidator::Intermediate) 
            {
               printf("QcjDataKeyValue::update(): Validation failed, pos = %d\n", pos);
               QMessageBox::warning(NULL, "Error!", "Error- Incomplete value entered\n");
               ((QLineEdit*)wdt)->setCursorPosition(((QLineEdit*)wdt)->text().length());
               wdt->setFocus();
               return; 
            }
            else if ( val->validate(text, pos) != QValidator::Acceptable) 
            {
               printf("QcjDataKeyValue::update(): Validation failed, pos = %d\n", pos);
               QMessageBox::warning(NULL, "Error!", "Error- Invalid value Entered\n");
               wdt->setFocus();
               return; 
            }
         }
      }
   }

   beginTransaction();
   std::vector<struct QcjDataFields>::iterator it;
   for ( it = fields.begin(); it < fields.end(); ++it)
   {
      QString key = (*it).key;
      QString dataName = (*it).dataName;
      if ( dataName == "--ENDOFFIELDS--" ) 
         break;
      printf("QcjDataKeyValue::updateRecord():Submitting changes to: %s\n", qPrintable(key));
      QSqlTableModel *sqlModel = fieldModels[key];
      if ( ! sqlModel->submitAll() ) 
      {
         QMessageBox::warning(NULL, "Error!", "Error updating record: \n" + sqlModel->lastError().databaseText() + "\n" + 
                              sqlModel->lastError().driverText() + "\n" +
                              sqlModel->query().executedQuery(), "Okay");
         rollbackTransaction();
         printf("QcjDataKeyValue::updateRecord():All changes discarded!\n");
      }
      else 
      {
         commitTransaction();
         printf("QcjDataKeyValue::updateRecord():All changes submitted!\n");
      }
   }
   emit(updated());
   refresh();

#endif

   printf("QcjDataKeyValue::updateRecord():Exit()\n");
   fflush(stdout);
}

/*!
      \fn void QcjDataKeyValue::beginTransaction()

       Begins a transaction with the database.
*/ 
void QcjDataKeyValue::beginTransaction()
{
#ifndef QT4_DESIGNER_PLUGIN
   if ( ! inTransaction ) 
   {
      db->transaction();
      inTransaction = true;
   }
#endif
}

/*!
      \fn void QcjDataKeyValue::commitTransaction(bool emitUpdated)

       Commits  the  sql  transaction performed since beginTransaction
       was called.
*/ 
void QcjDataKeyValue::commitTransaction()
{
#ifndef QT4_DESIGNER_PLUGIN
   printf("QcjDataKeyValue::commitTransaction(): Enter\n");
   if ( inTransaction ) 
      db->commit();
   printf("QcjDataKeyValue::commitTransaction(): Exit\n");
#endif
}

/*!
      \fn void QcjDataKeyValue::rollbackTransaction()

       Rolls back the sql transactions performed since beginTransaction
       was called.
*/ 
void QcjDataKeyValue::rollbackTransaction()
{
#ifndef QT4_DESIGNER_PLUGIN
   if ( inTransaction ) 
      db->rollback();
   refresh();
#endif
}

/*!
      \fn void QcjDataKeyValue::setEnabled(bool b)

       Enables/Disables the form elements of the form.
*/ 
void QcjDataKeyValue::setEnabled(bool b)
{
#ifndef QT4_DESIGNER_PLUGIN
   printf("QcjDataKeyValue::setEnabled():Enter");
   fflush(stdout);
   for (unsigned x = 0; x < fields.size() - 1; x++) 
   {
      printf("QcjDataKeyValue::setEnabled(): x = %d\n", x);
      fflush(stdout);
      printf("QcjDataKeyValue::setEnabled(): setting enabled for |%s| (%d) to %d\n", (const char*)fields[x].key.toLocal8Bit(), x, b);
      fflush(stdout);
      fields[x].widget->setEnabled(b);
   }
#endif
}

/*!
      \fn static bool isA(QObject *obj, const char *type)

       Tests  the  passed  object  to  be  of  the  type  specified by
       <em>type</em>.
*/ 
static bool isA(QObject *obj, const char *type)
{
    printf("isA(): Enter\n");
    fflush(stdout);
    const QMetaObject *mobj = obj->metaObject();
    while (mobj != NULL) 
    {
        printf("isA(): className = |%s|\n", mobj->className());
        fflush(stdout);
        if ( QString(mobj->className()) == QString(type) ) 
        {
            printf("isA(): Exit(true)\n");
            fflush(stdout);
            return(true);
        }
        mobj = mobj->superClass();
    }
    printf("isA(): Exit(false)\n");
    fflush(stdout);
    return(false);
}

/*!
      \fn void QcjDataKeyValue::setReadOnly(bool b)

       Set all  of  the  editable  text fields to read only or
       writeable  depending  on  the parameter <em>b</em>, If the field
       was marked as readonly in the XML definition, it is always made
       readonly.
*/ 
void QcjDataKeyValue::setReadOnly(bool b)
{
#ifndef QT4_DESIGNER_PLUGIN
   for (unsigned x = 0; x < fields.size() - 1; x++) 
   {
      if ( ::isA(fields[x].widget, "QcjTextBlockEdit") ) 
      {
         if ( fields[x].ro == false ) 
            ((QcjTextBlockEdit*)fields[x].widget)->setReadOnly(b);
         else
            ((QcjTextBlockEdit*)fields[x].widget)->setReadOnly(true);
      }
      else if ( ::isA(fields[x].widget, "QLineEdit") ) 
      {
         if ( fields[x].ro == false ) 
            ((QLineEdit*)fields[x].widget)->setReadOnly(b);
         else
            ((QLineEdit*)fields[x].widget)->setReadOnly(true);
      }
      else if ( ::isA(fields[x].widget, "QSpinBox") )
      {
         if ( fields[x].ro == false ) 
            ((QSpinBox*)fields[x].widget)->setReadOnly(b);
         else
            ((QSpinBox*)fields[x].widget)->setReadOnly(true);
      }
   }
#endif
}

/*!
      \fn QString QcjDataKeyValue::getFieldText(QString name)

       Returns   the  value  of  the  field  named  by  the  parameter
       <em>name</em> as a QString.  This  is  what  is  shown  on  the form and not
       necessarily the value in the actual record of the database.
*/ 
QString QcjDataKeyValue::getFieldText(QString name)
{
   return( getFieldValue(name).toString() );   
}

/*!
      \fn QVariant QcjDataKeyValue::getFieldValue(QString name)

       Returns  a  QVariant  representing  the  value  of  the  field 
       named  by the parameter <em>name</em>. This is what is shown on
       the  form and not necessarily the value in the actual record of
       the database.
*/ 
QVariant QcjDataKeyValue::getFieldValue(QString name)
{
   QVariant rv;

   printf("QcjDataKeyValue::getFieldText():Enter, name = |%s|\n", (const char*)name.toLocal8Bit());
   fflush(stdout);

#ifndef QT4_DESIGNER_PLUGIN
   for (unsigned x = 0; x < fields.size() - 1; x++) 
   {
      printf("QcjDataKeyValue::getFieldText():testing field %d - |%s|\n", x, (const char*)fields[x].key.toLocal8Bit());
      fflush(stdout);
      if ( fields[x].key == name ) 
      {
         printf("QcjDataKeyValue::getFieldText():have match\n");
         fflush(stdout);
         if ( fields[x].widget != NULL ) 
         {
            if ( ::isA(fields[x].widget, "QLineEdit") ) 
            {
               printf("QcjDataKeyValue::getFieldText():is a QLineEdit\n");
               fflush(stdout);
               return( QVariant(((QLineEdit*)fields[x].widget)->text()) );
            }
            else if ( ::isA(fields[x].widget, "QcjTextBlockEdit") ) 
            {
               printf("QcjDataKeyValue::getFieldText():is a QTextEdit\n");
               fflush(stdout);
               return( QVariant(((QcjTextBlockEdit*)fields[x].widget)->text()) );
            }
            else if ( ::isA(fields[x].widget, "QComboBox") ) 
            {
               printf("QcjDataKeyValue::getFieldText():is a QComboBox\n");
               fflush(stdout);
               return( QVariant(((QComboBox*)fields[x].widget)->currentText()) );
            }
            else if ( ::isA(fields[x].widget, "QcjMoneyEdit") ) 
            {
               QString str;
               str = ((QcjMoneyEdit*)fields[x].widget)->text();
               printf("QcjDataKeyValue::getFieldText():is a QcjMoneyEdit, |%s|\n", (const char*)str.toLocal8Bit());
               fflush(stdout);
               return(QVariant(str));
            }

#ifndef NO_PHOTO_SELECT
            else if ( ::isA(fields[x].widget, "QcjPhotoEntry") ) 
            {
               printf("QcjDataKeyValue::getFieldText():is a QcjPhotoEntry\n");
               fflush(stdout);
               return( QVariant( ((QcjPhotoEntry*)fields[x].widget)->get()) );
            }
#endif

            else 
            {
               printf("QcjDataKeyValue::getFieldText():is a Something??\n");
               fflush(stdout);
            }
         }
         else 
         {
            printf("QcjDataKeyValue::getFieldText():Field has no widget\n");
            fflush(stdout);
         }
      }
   }
#endif
   return(rv);
}

/*!
      \fn void QcjDataKeyValue::setFieldText(QString name, QString str)

       Sets   the   value   of   the  field  named  by  the  parameter
       <em>name</em> to the QString parameter <em>val</em>
*/ 
void QcjDataKeyValue::setFieldText(QString name, QString str)
{
   printf("QcjDataKeyValue::setFieldText():Enter(): |%s| to |%s|\n", qPrintable(name), qPrintable(str));
   fflush(stdout);
   setFieldValue(name, QVariant(str));
   printf("QcjDataKeyValue::setFieldText():Exit()\n");
   fflush(stdout);
}

/*!
      \fn void QcjDataKeyValue::setFieldValue(QString name, QVariant val)

       Sets   the   value   of   the  field  named  by  the  parameter
       <em>name</em> to the QVariant parameter <em>val</em>
*/ 
void QcjDataKeyValue::setFieldValue(QString name, QVariant val)
{
#ifndef QT4_DESIGNER_PLUGIN
   printf("QcjDataKeyValue::setFieldValue():Enter(): %s\n", qPrintable(name));
   fflush(stdout);
   QString key;
   std::vector<struct QcjDataFields>::iterator it;
   for ( it = fields.begin(); it < fields.end(); ++it)
   {
      QString dataName = (*it).dataName;
      if ( dataName == "--ENDOFFIELDS--" ) 
         return;
      key = (*it).key;
      if ( key == name ) 
         break;
   }
   QSqlTableModel *sqlModel = fieldModels[key];
   QString fieldName = (*it).dataName;
   QSqlRecord sqlRecord = sqlModel->record();
   sqlRecord.setValue(fieldName, val);
   sqlModel->setRecord(QCJ_FIRSTRECORD, sqlRecord);
   sqlMapper.setCurrentIndex(QCJ_FIRSTRECORD);
   printf("QcjDataKeyValue::setFieldValue():Exit()\n");
   fflush(stdout);
#endif
}

/*!
      \fn void QcjDataKeyValue::setFieldEnabled(QString name, bool f)

       Enables   or   disables   the  field  named  by  the  parameter
       <em>name</em> from receving focus.
*/ 
void QcjDataKeyValue::setFieldEnabled(QString name, bool f)
{
#ifndef QT4_DESIGNER_PLUGIN
   for (unsigned x = 0; x < fields.size() - 1; x++) 
   {
      if ( fields[x].key == name ) 
      {
         if ( ::isA(fields[x].widget, "QcjStringSelect") || ::isA(fields[x].widget, "QComboBox") ) 
         {
            fields[x].widget->setEnabled(f);
            if ( f ) 
               fields[x].widget->setFocusPolicy(Qt::StrongFocus);
            else
               fields[x].widget->setFocusPolicy(Qt::NoFocus);
         }
         else 
         {
            if ( f ) 
               fields[x].widget->setFocusPolicy(Qt::StrongFocus);
            else
               fields[x].widget->setFocusPolicy(Qt::NoFocus);
         }
      }
   }
#endif
}

/*!
      \fn void QcjDataKeyValue::resetFieldEnables()

       Resets all of the fields to the appropriate state for accepting
       focus. This state is defined by the XML definition of the form.
*/ 
void QcjDataKeyValue::resetFieldEnables()
{
#ifndef QT4_DESIGNER_PLUGIN
   for (unsigned x = 0; x < fields.size() - 1; x++) 
   {
      if ( ::isA(fields[x].widget, "QcjStringSelect") || ::isA(fields[x].widget, "QComboBox") ) 
      {
         if ( fields[x].ro ) 
         {
            fields[x].widget->setEnabled(false);
            fields[x].widget->setFocusPolicy(Qt::NoFocus);
         }
         else
         {
            fields[x].widget->setEnabled(true);
            fields[x].widget->setFocusPolicy(Qt::StrongFocus);
         }
      }
      else 
      {
         if ( fields[x].ro ) 
            fields[x].widget->setFocusPolicy(Qt::NoFocus);
         else
            fields[x].widget->setFocusPolicy(Qt::StrongFocus);
      }
   }
#endif
}

/*!
      \fn QWidget *QcjDataKeyValue::getFieldWidget(QString name)

       Returns  a  pointer to the widget object for the field named by
       the parameter <em>name</em>.
*/ 
QWidget *QcjDataKeyValue::getFieldWidget(QString name)
{
#ifndef QT4_DESIGNER_PLUGIN
   printf("QcjDataKeyValue::setFieldWidget():Enter(): |%s|\n", qPrintable(name));
   for (unsigned x = 0; x < fields.size() - 1; x++) 
   {
      printf("QcjDataKeyValue::setFieldWidget():Enter(): testing against |%s|\n", qPrintable(fields[x].key));
      if ( fields[x].key == name ) 
      {
         printf("QcjDataKeyValue::setFieldWidget():Exit(): Found it!\n");
         return(fields[x].widget);
      }
   }
   printf("QcjDataKeyValue::setFieldWidget():Exit():field not found\n");
   return(0);
#endif
}

/*!
      \fn void QcjDataKeyValue::setFocus()

       Sets focus to the field specified as the focus field in the XML
       definition of the form.
*/ 
void QcjDataKeyValue::setFocus()
{
   printf("QcjDataKeyValue::setFocus(): Enter\n");
#ifndef QT4_DESIGNER_PLUGIN
   printf("QcjDataKeyValue::setFocus(): fields.size = %d\n", fields.size());
   fflush(stdout);

   if ( fields.size() > 0 ) 
   {
      for (unsigned x = 0; x < fields.size() - 1; x++) 
      {
         printf("QcjDataKeyValue::setFocus(): x = %d\n", x);
         fflush(stdout);
         printf("QcjDataKeyValue::setFocus(): Testing field |%s|\n", (const char*)fields[x].key.toLocal8Bit());
         fflush(stdout);
         if ( fields[x].focusWidget ) 
         {
            printf("QcjDataKeyValue::setFocus(): Found it!\n");
            fflush(stdout);
            fields[x].widget->setFocus();
            break;
         }
      }
   }
#endif
   printf("QcjDataKeyValue::setFocus(): Exit\n");
   fflush(stdout);
}

/*!
      \fn bool QcjDataKeyValue::validateEdits()

       Iterates  through  the  fields which are derived from QLineEdit
       and validates their contents.
*/ 
bool QcjDataKeyValue::validateEdits()
{
   bool rv = true;

#ifndef QT4_DESIGNER_PLUGIN
   for (unsigned x = 0; x < fields.size() - 1; x++) 
   {
      if ( ::isA(fields[x].widget, "QLineEdit") ) 
      {
         if ((rv = ((QLineEdit*)fields[x].widget)->hasAcceptableInput()) != true)
         {
            ((QLineEdit*)fields[x].widget)->setFocus();
            break;
         }
      }
   }
#endif
   return(rv);
}


