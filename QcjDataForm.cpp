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
//#include <q3listbox.h>
#include <qlineedit.h>
#include <qmessagebox.h>
#include <QRegExp>
//#include <q3sqlcursor.h>
#include <QBuffer>
#include <QPixmap>
#include <QSpinBox>
#include <QSqlDriver>
#include <QSqlError>
//#include <q3sqlform.h>
//#include <q3sqlpropertymap.h>
#include <qsqlrecord.h>
//Added by qt3to4:
#include <QGridLayout>
//#include <Q3SqlRecordInfo>
//#include <Q3Frame>
#include <stdio.h>
#include <qstringlist.h>
//#include <q3textedit.h>
#include "QcjData/QcjDataDelegate.h"
#include "QcjData/QcjDataHelpers.h"
#include "QcjData/QcjDataLogin.h"
#include "QcjData/QcjDataXML.h"
#endif

#include <QSqlField>
#include <QSqlQuery>
#include "QcjData/QcjDataStatics.h"

#include "QcjData/QcjDataForm.h"

#define  QCJ_FIRSTRECORD   0  // Value to use to retrieve the first record of a query

static bool isA(QObject *obj, const char *type);

/*!                                                                 
       \class QcjDataForm
       
       \brief  This  class  generates  a  QFrame containing a database
       form.
       
       The  QcjDataForm  class creates a QFrame object holding a form for
       entering  data  into a database. The definition of this form is
       defined in a &lt;form&gt; element of the applications XML file.
*/                                                                  

QcjDataForm::QcjDataForm(QWidget *pParent) :
          QFrame(pParent)
{
   printf("QcjDataForm::QcjDataForm(): Enter\n");
#ifndef QT4_DESIGNER_PLUGIN
   printf("QcjDataForm::QcjDataForm(): dbTable = |%s|\n", (const char*)dbTable.toLocal8Bit());
   fflush(stdout);
//   db = pDb;
   db = 0;
   deleteDb = false;
   ident = "";
   insertMode  = false;
   inTransaction = false;
   setFocusPolicy(Qt::StrongFocus);
   sqlModel = 0;
   xmldef = "";
   invalidData = true;
#else
   setFrameShape(QFrame::Box);
#endif
   printf("QcjDataForm::QcjDataForm(): Exit\n");
   fflush(stdout);
}

/*
QcjDataForm::QcjDataForm(QSqlDatabase *_db, const QString table, int rows, int cols, QWidget *pParent, Qt::WFlags f) :
          QFrame(pParent, f)
{
   printf("QcjDataForm::QcjDataForm(): Enter, table = |%s|\n", (const char*)table.toLocal8Bit());
   fflush(stdout);
   db = _db;
   dbTable = table;
   gl = new QGridLayout(this);
   gl->activate();
   sqlForm     = NULL;
   sqlRecord   = NULL;
   sqlCursor   = NULL;
   ident = "";
   insertMode  = false;
   inTransaction = false;
   setFocusPolicy(Qt::StrongFocus);
   printf("QcjDataForm::QcjDataForm(): Exit\n");
   fflush(stdout);
}
*/

/*!                                                                 
      \fn void QcjDataForm::writeXmlDef(QString str)
       Sets the name of the XML definition to use for this form
*/                                                                  
void QcjDataForm::writeXmlDef(QString str)
{
   printf("QcjDataForm::writeXmlDef(): Enter setting xmldef to |%s|\n", (const char*)str.toLocal8Bit());
   fflush(stdout);
   xmldef = str;
   printf("QcjDataForm::writeXmlDef(): Exit\n");
   fflush(stdout);
}


void QcjDataForm::setDatabase(QSqlDatabase *pdb)
{
#ifndef QT4_DESIGNER_PLUGIN
   printf("QcjDataForm::setDatabase(*pdb): Enter\n");
   db = new QSqlDatabase(*pdb);
   printf("QcjDataForm::setDatabase(*pdb): Calling setDatabase\n");
   deleteDb = true;
   setDatabase();
   printf("QcjDataForm::setDatabase(*pdb): Exit\n");
#endif
}

/*!                                                                 
       \fn void QcjDataForm::setDatabase()

       Sets  sets up the database for this form. If the database needs
       connected  to,  this  function  will  prompt the user for login
       information. It will also get the name of the XML definition to
       use for the form.
*/                                                                  
void QcjDataForm::setDatabase()
{
#ifndef QT4_DESIGNER_PLUGIN
   printf("QcjDataForm::setDatabase(): Enter\n");
   fflush(stdout);

   if ( pDb != 0 && db == 0 ) 
   {
      printf("QcjDataForm::setDatabase(): Using existing database\n");
      fflush(stdout);
      db = pDb;
   }

   printf("QcjDataForm::setDatabase(): Checking if we logged in yet\n");
   fflush(stdout);
   if ( db == 0 ) 
   {
      QcjDataLogin *login = new QcjDataLogin();
      login->initialize();
      login->show();
      if (! login->exec() )
      {
         printf("QcjDataForm::setDatabase(): Error with login\n");
         fflush(stdout);
         return;
      }
      delete login;
      db = pDb;
      if ( pFormDef->database() == 0 ) 
         pFormDef->setDatabase(db);
   }

   printf("QcjDataForm::setDatabase(): xmldef = |%s|\n", qPrintable(xmldef));
   fflush(stdout);

   printf("QcjDataForm::setDatabase(): retrieving table\n");
   fflush(stdout);
   dbTable = pFormDef->getTable(xmldef);
   printf("QcjDataForm::setDatabase(): got table\n");
   fflush(stdout);

   printf("QcjDataForm::setDatabase(): dbTable = |%s|\n", qPrintable(dbTable));
   fflush(stdout);
   gl = new QGridLayout(this);
   gl->setSpacing(3);
   gl->activate();
   printf("QcjDataForm::setDatabase(): Calling setFields()\n");
   fflush(stdout);
   setFields(pFormDef->getFields(xmldef, this), pConfig, pFormDef);
   printf("QcjDataForm::setDatabase(): Calling clearForm()\n");
   fflush(stdout);
   clearForm();
   printf("QcjDataForm::setDatabase(): Exit\n");
   fflush(stdout);
#endif
}


#ifndef QT4_DESIGNER_PLUGIN
void QcjDataForm::setFields(QcjDataFields *fds, QSettings *pCfg, QcjDataXML *pFDef) 
{
   printf("QcjDataForm::setFields():1: Enter\n");
   printf("QcjDataForm::setFields(): xmldef = |%s|\n", (const char*)xmldef.toLocal8Bit());
   fflush(stdout);
   const QcjDataFields *pFds = fds;
   while (! pFds->dataName.isEmpty() && pFds->dataName != QString("--ENDOFFIELDS--"))
   {
      printf("QcjDataForm::setFields(): New field |%s|\n", (const char*)pFds->dataName.toLocal8Bit());
      fflush(stdout);
      fields.push_back(*pFds);
      pFds++;
   }

   struct QcjDataFields *p = new struct QcjDataFields;
   p->dataName = QString("--ENDOFFIELDS--");
   fields.push_back(*p);
   delete p;
   _setFields(fds, pCfg, pFDef);
   printf("QcjDataForm::setFields():1: Exit\n");
   fflush(stdout);
}

void QcjDataForm::setFields(std::vector<struct QcjDataFields> fds, QSettings *pCfg, QcjDataXML *pFDef)
{
   fields = fds;
   struct QcjDataFields f[60];
//   struct QcjDataFields f[fields.size() + 1];
   unsigned x;

   printf("QcjDataForm::setFields():2: Enter\n");

   printf("QcjDataForm::setFields(): xmldef = |%s|\n", (const char*)xmldef.toLocal8Bit());
   fflush(stdout);
   for (x = 0; x < fields.size(); x++) 
      f[x] = fields[x];
   _setFields(f, pCfg, pFDef);
   printf("QcjDataForm::setFields():2: Exit\n");
   fflush(stdout);
}

void QcjDataForm::_setFields(QcjDataFields *fields, QSettings *pCfg, QcjDataXML *pFDef) 
{
   QWidget *wdt, *lastwdt = NULL;
   int count = 0;
   printf("QcjDataForm::_setFields(): Enter\n");
   fflush(stdout);

   QSizePolicy labelPolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);
   QSizePolicy wdtPolicySingl(QSizePolicy::Minimum, QSizePolicy::Maximum);
   QSizePolicy wdtPolicyMulti(QSizePolicy::MinimumExpanding, QSizePolicy::Expanding);
#ifndef NO_PHOTO_SELECT
   QSizePolicy wdtPolicyPhoto(QSizePolicy::Preferred, QSizePolicy::Preferred);
#endif
   if ( db == NULL ) 
   {
      printf("QcjDataForm::_setFields(): No QcjDatabase, Exit\n");
      return;
   }
   QSqlDriver *dbdrv = db->driver();

   printf("QcjDataForm::_setFields(): Setting models table to '%s'\n", qPrintable(dbTable));
   fflush(stdout);

   if ( sqlModel != 0 ) 
      delete sqlModel;

   /***********************************************/
   /*   Create and setup the model for the form.  */
   /*   Do  an  initial select to get the record  */
   /*   information.                              */
   /***********************************************/
   sqlModel = new QSqlTableModel(this, *db);
   sqlModel->setTable(dbTable);
   sqlModel->setEditStrategy(QSqlTableModel::OnManualSubmit);
   sqlMapper.setModel(sqlModel);
   sqlMapper.setItemDelegate(new QcjDataDelegate(this));
   printf("QcjDataForm::_setFields(): performing initial select\n");
   fflush(stdout);
//   sqlModel->select();

   printf("QcjDataForm::_setFields(): Models db name = '%s'\n", qPrintable(sqlModel->database().databaseName()));
   fflush(stdout);

   printf("QcjDataForm::_setFields(): Getting Record Info from (%ld:%s)\n", (long)dbdrv, (const char *)dbTable.toLocal8Bit());
   fflush(stdout);
   QSqlRecord recInfo = sqlModel->record();
   printf("QcjDataForm::_setFields():  Record has %d fields\n", recInfo.count());
   fflush(stdout);


   setFocusPolicy(Qt::StrongFocus);

   printf("QcjDataForm::_setFields(): Iterating field info\n");
   fflush(stdout);

   //
   // Iterate through all of the field definitions and 
   // place them into the grid.
   //
   while (! fields->dataName.isEmpty() && fields->dataName != QString("--ENDOFFIELDS--"))
   {
      count++;
      printf("QcjDataForm::_setFields(): *************** New field, count = %d\n", count);
      fflush(stdout);
      printf("QcjDataForm::_setFields(): New field |%s|\n", (const char*)fields->dataName.toLocal8Bit());
      fflush(stdout);
      if ( fields->widget == NULL ) 
      {
         printf("QcjDataForm::_setFields(): New edit field\n");
         fflush(stdout);
         wdt = new QLineEdit(this);
      }
      else 
      {
         printf("QcjDataForm::_setFields(): Existing Widget\n");
         fflush(stdout);
         wdt = fields->widget;
         printf("QcjDataForm::_setFields():Have widget\n");
         fflush(stdout);
      }
      if ( lastwdt != NULL ) 
         setTabOrder(lastwdt, wdt);
      lastwdt = wdt;
      if ( fields->ro ) 
        wdt->setFocusPolicy(Qt::NoFocus);

      //
      // Map the widget to the appropriate field of the model
      //

      printf("QcjDataForm::_setFields():adding mapping for field %s, index %d\n", qPrintable(fields->dataName), recInfo.indexOf(fields->dataName));
      fflush(stdout);
      sqlMapper.addMapping(wdt, recInfo.indexOf(fields->dataName));

      QFont f = wdt->font();
      QFontMetrics fm(f);
      int fwidth = fm.maxWidth();
      printf("QcjDataForm::_setFields():Element |%s|\n", (const char*)fields->dataName.toLocal8Bit());
      fflush(stdout);
      if ( fields->minWidth > 0 ) 
      {
         printf("QcjDataForm::_setFields():Setting min width %d\n", fwidth * fields->minWidth);
         fflush(stdout);
         wdt->setMinimumWidth(fwidth * fields->minWidth);
      }
      if ( fields->maxWidth > 0 ) 
      {
         printf("QcjDataForm::_setFields():Setting max width %d\n", fwidth * fields->maxWidth);
         fflush(stdout);
         if ( ::isA(wdt, "QcjPhotoEntry") ) 
         {
            printf("QcjDataForm::_setFields():Setting max width %d\n", fields->maxWidth);
            wdt->setMaximumWidth(fields->maxWidth);
         }
         else 
         {
            printf("QcjDataForm::_setFields():Setting max width %d\n", fwidth * fields->maxWidth);
            wdt->setMaximumWidth(fwidth * fields->maxWidth);
         }
      }
      QSqlField fi = recInfo.field(fields->dataName);
      printf("QcjDataForm::_setFields():field has max length %d\n", fi.length());
      QVariant v = fi.type();
      printf("QcjDataForm::_setFields():Field type: |%s|\n", (const char*)v.toString().toLocal8Bit());
      printf("QcjDataForm::_setFields():Widget Class: |%s|\n", wdt->metaObject()->className());
      printf("QcjDataForm::_setFields():Field type: |%d|\n", fi.typeID());
      fflush(stdout);



      if (QString(wdt->metaObject()->className()) == QString("QLineEdit") && (v != 0))
      {
         printf("QcjDataForm::_setFields():Setting fields max length to %d\n", fi.length());
         fflush(stdout);
         ((QLineEdit*)wdt)->setMaxLength(fi.length());
      }
      else if (QString(wdt->metaObject()->className()) == QString("QcjStringSelect")) 
      {
         printf("QcjDataForm::_setFields():Have strsel init = |%s|\n", qPrintable(fields->init));
         QStringList conf = fields->init.split(':');
         printf("QcjDataForm::_setFields():Split complete have %d items\n", conf.count());
         if ( fields->ro ) 
            wdt->setEnabled(false);
         if ( conf.count() >= 2 ) 
         {
            QString type = conf[0];
            QString name = conf[1];
            printf("QcjDataForm::_setFields():Name = |%s|, type = |%s|\n", (const char*)name.toLocal8Bit(), (const char*)type.toLocal8Bit());
            fflush(stdout);
            if ( pCfg != NULL && type == "config" ) 
            {
               QString def = "";
               if ( pFDef != NULL ) 
               {
                  printf("QcjDataForm::_setFields():getting defaults for |%s|\n", (const char*)name.toLocal8Bit());
                  fflush(stdout);
                  def = pFDef->getConfigurationDefault(name);
               }
               printf("QcjDataForm::_setFields(): default = |%s|\n", (const char*)def.toLocal8Bit());
               fflush(stdout);
               ((QcjStringSelect*)wdt)->initialize(pCfg->value(name, def).toString());
            }
            else if ( type == "string" )
            {
               printf("QcjDataForm::_setFields(): Initializing string select\n");
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
                  printf("QcjDataForm::_setFields():QcjStringSelect init sql = |%s|\n", (const char*)sql.toLocal8Bit());
                  fflush(stdout);
                  QSqlQuery *query = new QSqlQuery(sql);
                  for ( int x = 0; query->next(); x++ ) 
                  {
                     if ( x > 0 ) 
                        init += ",";
                     init += query->value(0).toString();
                     init += " - ";
                     init += query->value(1).toString();
                     printf("QcjDataForm::_setFields():init = |%s|\n", (const char*)init.toLocal8Bit());
                     fflush(stdout);
                  }
                  ((QcjStringSelect*)wdt)->initialize(init);
               }
            }
            else if ( type == "sqlquery" ) 
            {
               QString sql = name;
               QString init;
               printf("QcjDataForm::_setFields():QcjStringSelect init sql = |%s|\n", (const char*)sql.toLocal8Bit());
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
                     printf("QcjDataForm::_setFields():init = |%s|\n", qPrintable(init));
                     fflush(stdout);
                  }
                  else 
                  {
                     printf("QcjDataForm::_setFields():QcjStringSelect String = |%s|, data = |%s|\n", 
                            qPrintable(query->value(0).toString()), 
                            qPrintable(query->value(0).toString()));
                     ((QcjStringSelect*)wdt)->addItem(query->value(0).toString(), query->value(1));
                     fflush(stdout);
                  }
               }
               if ( ! init.isEmpty() ) 
               {
                  printf("QcjDataForm::_setFields():QcjStringSelect init string = |%s|\n", (const char*)init.toLocal8Bit());
                  fflush(stdout);
                  ((QcjStringSelect*)wdt)->initialize(init);
               }
            }
         }
      }

#ifndef NO_PHOTO_SELECT
      else if (QString(wdt->metaObject()->className()) == QString("QcjPhotoEntry")) 
      {
         printf("QcjDataForm::_setFields():have photo entry field, connecting filename\n");
         fflush(stdout);
         connect((QcjPhotoEntry*)wdt, SIGNAL(filename(QString)), this, SLOT(haveFileName(QString)));
      }
#endif
      else if (QString(wdt->metaObject()->className()) == QString("QcjYesNoSelect")) 
      {
         printf("QcjDataForm::_setFields():have yesno field\n");
         fflush(stdout);
         if ( fields->defvalue.size() == 0 ) 
         {
            printf("QcjDataForm::_setFields():Setting yesno default to \"N\"\n");
            fflush(stdout);
            fields->defvalue = "N";
         }
      }

      printf("QcjDataForm::_setFields():done with field type\n");
      fflush(stdout);
      formFields[fields->dataName] = wdt;

      if ( fields->validator != NULL ) 
      {
         formValidators[fields->dataName] = fields->validator;
      }

      if ( fields->propName != (const char*)NULL ) 
      {
         printf("QcjDataForm::_setFields(): Setting Property Name: |%s|\n", qPrintable(fields->propName));
         fflush(stdout);
         widgetProperties[fields->dataName] = fields->propName;
      }

      printf("QcjDataForm::_setFields():Default Text: |%s|\n", qPrintable(fields->defvalue));
      fflush(stdout);
      fieldDefaults[fields->dataName] = fields->defvalue;

      
      printf("QcjDataForm::_setFields(): rowSpan = %d\n", fields->rowSpan);
      fflush(stdout);
      printf("QcjDataForm::_setFields(): colSpan = %d\n", fields->colSpan);
      fflush(stdout);
      if ( fields->rowSpan <= 0) 
      {
         printf("QcjDataForm::_setFields(): Adding label- row = %d, col = %d\n", fields->row, fields->col * 2);
         fflush(stdout);
         QLabel *lbl = new QLabel(fields->label, this);
         lbl->setSizePolicy(labelPolicy);
         lbl->setAlignment(Qt::AlignTop|Qt::AlignRight);
         gl->addWidget(lbl, fields->row, fields->col * 2);
      }
      else 
      {
         int rows = (fields->rowSpan == 1) ? 0 : fields->rowSpan;
         printf("QcjDataForm::_setFields(): rows = %d\n", rows);
         fflush(stdout);
         printf("QcjDataForm::_setFields(): Adding multi row label- beg row = %d, row span = %d beg col = %d, colspan = %d\n", 
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
         printf("QcjDataForm::_setFields(): rows = %d\n", rows);
         fflush(stdout);
         int cols = (fields->colSpan <= 1) ? 1 : fields->colSpan;
         printf("QcjDataForm::_setFields(): cols = %d\n", cols);
         fflush(stdout);
         if ( fields->rowSpan > 1 ) 
         {
            printf("QcjDataForm::_setFields(): Multi Policy\n");
            wdt->setSizePolicy(wdtPolicyMulti);
         }
         else
         {
            printf("QcjDataForm::_setFields(): Single Policy\n");
            wdt->setSizePolicy(wdtPolicySingl);
         }
         printf("QcjDataForm::_setFields(): Adding multicell widget- beg row = %d, row span = %d beg col = %d, colspan = %d\n", 
                                        fields->row, rows, (fields->col * 2) + 1, cols);
         fflush(stdout);
         gl->addWidget(wdt, fields->row, (fields->col * 2) + 1, rows, cols > 1 ? ((cols - 1) * 2) + 1 : cols);
      }
      else
      {
         printf("QcjDataForm::_setFields(): Adding widget- row = %d, col = %d\n", fields->row, (fields->col * 2) + 1);
         fflush(stdout);
         wdt->setSizePolicy(wdtPolicySingl);
         gl->addWidget(wdt, fields->row, (fields->col * 2) + 1);
      }

      fieldSearch[wdt] = fields->search;

#ifndef NO_PHOTO_SELECT
      if (QString(wdt->metaObject()->className()) == QString("QcjPhotoEntry")) 
      {
         wdt->setSizePolicy(wdtPolicyPhoto);
      }
#endif

      printf("QcjDataForm::_setFields(): done with field\n");
      fflush(stdout);
      fields++;
      if (fields->dataName != "--ENDOFFIELDS--")
         printf("QcjDataForm::_setFields(): FALSE |%s|\n", (const char*)fields->dataName.toLocal8Bit());
      else
         printf("QcjDataForm::_setFields(): TRUE |%s|\n", (const char*)fields->dataName.toLocal8Bit());
      fflush(stdout);
   }
   printf("QcjDataForm::_setFields(): All fields Complete\n");
   fflush(stdout);
   setReadOnly(false);
   printf("QcjDataForm::_setFields(): Exit\n");
   fflush(stdout);
   return;
}

/*!                                                                 
      \fn void QcjDataForm::updateFieldSettings(QSettings *pCfg, QcjDataXML *pFDef) 

       Updates the fields which are string selects with  current data.
       This  is good for when new records are added to the database or
       settings are updated.
*/                                                                  
void QcjDataForm::updateFieldSettings(QSettings *pCfg, QcjDataXML *pFDef) 
{
   int count = 0;
   printf("QcjDataFormupdateFieldSettings(): Enter\n");
   fflush(stdout);

   QSqlRecord recInfo = db->record(dbTable);

   printf("QcjDataForm::updateFieldSettings(): Iterating field info\n");
   fflush(stdout);

   //
   // Iterate through all of the field definitions and 
   // place them into the grid.
   //
   std::vector<struct QcjDataFields>::iterator it;
   for ( it = fields.begin(); it < fields.end(); ++it)
   {
      count++;
      printf("QcjDataForm::updateFieldSettings(): *************** New field, count = %d\n", count);
      fflush(stdout);
      printf("QcjDataForm::updateFieldSettings(): New field |%s|\n", (const char*)(*it).dataName.toLocal8Bit());
      fflush(stdout);

      QWidget *wdt = (*it).widget;
      if (QString(wdt->metaObject()->className()) == QString("QcjStringSelect")) 
      {
         ((QcjStringSelect*)wdt)->clear();
         printf("QcjDataForm::updateFieldSettings():Have strsel init = |%s|\n", (const char*)(*it).init.toLocal8Bit());
         QStringList conf = (*it).init.split(':');
         printf("QcjDataForm::updateFieldSettings():Split complete have %d items\n", conf.count());
         if ( conf.count() >= 2 ) 
         {
            QString type = conf[0];
            QString name = conf[1];
            printf("QcjDataForm::updateFieldSettings():Name = |%s|, type = |%s|\n", (const char*)name.toLocal8Bit(), (const char*)type.toLocal8Bit());
            if ( pCfg != NULL && type == "config" ) 
            {
               QString def = "";
               if ( pFDef != NULL ) 
               {
                  printf("QcjDataForm::updateFieldSettings():getting defaults for |%s|\n", (const char*)name.toLocal8Bit());
                  def = pFDef->getConfigurationDefault(name);
               }
               printf("QcjDataForm::updateFieldSettings(): default = |%s|\n", (const char*)def.toLocal8Bit());
               ((QcjStringSelect*)wdt)->initialize(pCfg->value(name, def).toString());
            }
            else if ( type == "string" )
            {
               printf("QcjDataForm::updateFieldSettings(): Initializing string select\n");
               ((QcjStringSelect*)wdt)->initialize(name);
            }
            else if ( type == "field" ) 
            {
               QStringList sl;
               QString init;
               sl  = name.split('.');
               QString sql = "select init, " + sl[1] + " from " + sl[0] + " order by " + sl[1];
               printf("QcjDataForm::updateFieldSettings():QcjStringSelect init sql = |%s|\n", (const char*)sql.toLocal8Bit());
               QSqlQuery *query = new QSqlQuery(sql);
               for ( int x = 0; query->next(); x++ ) 
               {
                  if ( x > 0 ) 
                     init += ",";
                  init += query->value(0).toString();
                  init += " - ";
                  init += query->value(1).toString();
                  printf("QcjDataForm::updateFieldSettings():init = |%s|\n", (const char*)init.toLocal8Bit());
               }
               ((QcjStringSelect*)wdt)->initialize(init);
            }
            else if ( type == "sqlquery" ) 
            {
               QString sql = name;
               QString init;
               printf("QcjDataForm::updateFieldSettings():QcjStringSelect init sql = |%s|\n", (const char*)sql.toLocal8Bit());
               QSqlQuery *query = new QSqlQuery(sql);
               for ( int x = 0; query->next(); x++ ) 
               {
                  QVariant v;
                  if ( (v = query->value(1)) == QVariant::Invalid ) 
                  {
                     if ( x > 0 ) 
                        init += ",";
                     init += query->value(0).toString();
                     printf("QcjDataForm::updateFieldSettings():init = |%s|\n", (const char*)init.toLocal8Bit());
                  }
                  else 
                  {
                     printf("QcjDataForm::updateFieldSettings():QcjStringSelect String = |%s|, data = |%s|\n", (const char*)query->value(0).toString().toLocal8Bit(),
                                                                                                (const char*)query->value(1).toString().toLocal8Bit());
                     ((QcjStringSelect*)wdt)->addItem(query->value(0).toString(), query->value(1));
                  }
               }
               if ( ! init.isEmpty() ) 
               {
                  printf("QcjDataForm::updateFieldSettings():QcjStringSelect init string = |%s|\n", (const char*)init.toLocal8Bit());
                  ((QcjStringSelect*)wdt)->initialize(init);
               }
            }
         }
      }
      printf("QcjDataForm::updateFieldSettings():done with field type\n");
      fflush(stdout);
   }
   printf("QcjDataForm::updateFieldSettings(): All fields Complete\n");
   fflush(stdout);
//   setReadOnly(false);
   printf("QcjDataForm::updateFieldSettings(): Exit\n");
   fflush(stdout);
   return;
}

/*!
   \fn QString makeFilter()                                       
                                                                  
   Returns  a  string  which  can  be  used as a filter in a where
   statement.  The contents of the string will be derived from the
   form  elements  themselves. The contents of Edit fields will be
   surrounded  by  '%'  characters.  Other field types will be not
   have  their  contents  altered.  Any  field  name  listed in \e
   exceptions will not be included
*/
QString QcjDataForm::makeFilter(QStringList exceptions)
{
   QString opr, rv;
   QString search;

   printf("QcjDataForm::makeFilter(): Enter\n");
   fflush(stdout);
   QMap<QString, QWidget*>::iterator it = formFields.begin();
   while (it != formFields.end()) 
   {
      QString key = it.key();
      printf("QcjDataForm::makeFilter(): Have field |%s|\n", qPrintable(key));
      fflush(stdout);
      if ( ! exceptions.contains(key) ) 
      {
         printf("QcjDataForm::makeFilter(): Not in the exception list\n");
         fflush(stdout);
         QWidget *wdt = it.value();
         if ( fieldSearch.contains(wdt) ) 
         {
            search = fieldSearch[wdt];
         }
         if ( search.toLower() != "no" ) 
         {
            if ( ::isA(wdt, "QLineEdit") || wdt->property("pname").isValid() ) 
            {
               QString pname;
               if ( ! ::isA(wdt, "QLineEdit") )
                  pname = wdt->property("pname").toString();
               else
                  pname = "text";

               printf("QcjDataForm::makeFilter(): Has pname |%s|\n", qPrintable(pname));
               fflush(stdout);
               if ( wdt->property(qPrintable(pname)).isValid() ) 
               {
                  QString value;
                  QString mask;

                  value = wdt->property(qPrintable(pname)).toString();
                  if ( ! value.contains(QRegExp("^\\s+$")) ) 
                  {
                     if ( ::isA(wdt, "QLineEdit") )
                     {
                        mask = ((QLineEdit*)wdt)->inputMask();
                        printf("QcjDataForm::makeFilter(): mask = |%s|\n", qPrintable(mask));
                        mask.replace(QRegExp("[AaXxNn90Dd#HhBb><!]"), "");
                        mask.replace(QRegExp("; $"), "");
                        printf("QcjDataForm::makeFilter(): adjusted mask = |%s|\n", qPrintable(mask));
                     }
                     if ( (! ::isA(wdt, "QLineEdit") || mask != value) &&
                          value.size() > 0 )
                     {
                        printf("QcjDataForm::makeFilter(): Has value |%s|\n", qPrintable(value));
                        fflush(stdout);
                        value.replace("'", "''");
                        value = value.toLower();
                        if ( ::isA(wdt, "QLineEdit") || ::isA(wdt, "QTextEdit" ) )
                        {
                           printf("QcjDataForm::makeFilter(): is text\n");
                           fflush(stdout);
                           value = "'%" + value + "%'";
                           opr = " like ";
                        }
                        else
                        {
                           printf("QcjDataForm::makeFilter(): NOT text\n");
                           fflush(stdout);
                           value = "'" + value + "'";
                           opr = " = ";
                        }

                        if ( rv.size() > 0 ) 
                        {
                           rv += " and ";
                        }
                        rv += "lower(" + key + ")" + opr + value;
                     }
                  }
               }
            }
         }
      }
      ++it;
   }
   printf("QcjDataForm::makeFilter(): Exit |%s|\n", qPrintable(rv));
   fflush(stdout);
   return(rv);
}

/*!
   \fn void clearForm()

   This member function clears the edit fields to blank and the 
   integer and double spin boxes to 0.
*/
void QcjDataForm::clearForm()
{
   QString opr, rv;

   printf("QcjDataForm::clearForm(): Enter\n");
   fflush(stdout);

   QMap<QString, QWidget*>::iterator it = formFields.begin();
   while (it != formFields.end()) 
   {
      QString key = it.key();
      QWidget *wdt = it.value();
      printf("QcjDataForm::clearForm(): have key: %s\n", qPrintable(key));
      fflush(stdout);
      if ( ::isA(wdt, "QLineEdit") || wdt->property("pname").isValid() ) 
      {
         printf("QcjDataForm::clearForm(): here!\n");
         fflush(stdout);
         QString pname;
         if ( ! ::isA(wdt, "QLineEdit") )
            pname = wdt->property("pname").toString();
         else
            pname = "text";

         printf("QcjDataForm::clearForm(): have pname: %s\n", qPrintable(pname));
         if ( wdt->property(qPrintable(pname)).isValid() ) 
         {
            printf("QcjDataForm::clearForm(): Have a valid pname\n");
            if ( ::isA(wdt, "QLineEdit") || ::isA(wdt, "QTextEdit" ) )
            {
               printf("QcjDataForm::clearForm(): Is an edit type\n");
               wdt->setProperty(qPrintable(pname), "");
            }
            else if ( ::isA(wdt, "QSpinBox") || ::isA(wdt, "QDoubleSpinBox" ) )
            {
               printf("QcjDataForm::clearForm(): Is an spin type\n");
               wdt->setProperty(qPrintable(pname), 0);
            }
            else
               printf("QcjDataForm::clearForm(): Still don't know the type!\n");
         }
      }
      ++it;
   }
   setFocus();
   invalidData = true;
   printf("QcjDataForm::clearForm(): Exit\n");
   fflush(stdout);
}

/*!
      \fn void QcjDataForm::setValues(QSqlRecord *rec)

       Sets  the  values  of  the form elements to those in the record
       object passed in.
*/                                                                  
void QcjDataForm::setValues(QSqlRecord *rec)
{
   printf("QcjDataForm::setValues(): Enter\n");
   fflush(stdout);
   
   QMap<QString, QString> indexmap;
   QString index = pFormDef->getIndexField(xmldef);
   QStringList fields = index.split(',');
/*
   for ( QStringList::Iterator it = fields.begin(); it != fields.end(); ++it ) {
      printf("QcjDataForm::setValues():index field = |%s|\n", qPrintable((*it)));
      if ( rec->contains((*it)) ) 
      {
         printf("QcjDataForm::setValues():Removing passed in index field = |%s|\n", qPrintable((*it)));
         rec->remove(rec->indexOf((*it)));
      }
   }
*/
   printf("QcjDataForm::setValues():Have a possible of %d fields to move\n", rec->count());
   for (int x = 0; x < rec->count(); x++ ) 
   {
      QSqlField f = rec->field(x);
      QString fname = f.name();

      QStringList::Iterator it;
      for ( it = fields.begin(); it != fields.end(); ++it ) {
         printf("QcjDataForm::setValues():index field = |%s|\n", qPrintable((*it)));
         if ( fname == *it ) 
         {
            printf("QcjDataForm::setValues():Skipping index field = |%s|\n", qPrintable((*it)));
            break;
         }
      }
      if ( it == fields.end() ) 
      {
         printf("QcjDataForm::setValues():Have passed in field of |%s|, value |%s| or |%s|\n", qPrintable(fname), qPrintable(f.value().toString()), qPrintable(rec->field(x).value().toString()));
         if ( sqlRecord.contains(fname) ) 
         {  
            printf("QcjDataForm::setValues():replacing existing field = |%s|, old value |%s|, new value |%s|\n", qPrintable(fname), qPrintable(sqlRecord.field(fname).value().toString()), qPrintable(f.value().toString()));
            sqlRecord.replace(sqlRecord.indexOf(fname), f);
         }
      }
   }
   printf("QcjDataForm::setValues():Updating form\n");
   fflush(stdout);
   sqlModel->setRecord(QCJ_FIRSTRECORD, sqlRecord);
   sqlMapper.setCurrentIndex(QCJ_FIRSTRECORD);
// **************   sqlForm->readFields();
   printf("QcjDataForm::setValues(): Exit\n");
   fflush(stdout);
}

#endif

/*!
      \fn void QcjDataForm::refresh()

       Reads  the  elements  in  the form with the data of the current
       form.
*/ 
void QcjDataForm::refresh()
{
#ifndef QT4_DESIGNER_PLUGIN
   printf("QcjDataForm::refresh(): Enter, insertMode = %d\n", insertMode);
   printf("QcjDataForm::refresh(): xmldef = |%s|\n", (const char*)xmldef.toLocal8Bit());
   printf("QcjDataForm::refresh(): dbTable = |%s|\n", (const char*)dbTable.toLocal8Bit());
   fflush(stdout);
   if ( ! db->isOpen() ) 
   {
      printf("QcjDataForm::refresh(): QcjDatabase closed, logging in again\n");
      if ( ! db->open() )
      {
         printf("QcjDataForm::refresh(): Error opeing db connection!\n");
      }
   }

   if ( ! insertMode ) 
   {
      clearForm();
      printf("QcjDataForm::refresh(): where clause = |%s|\n", (const char *)strWhere.toLocal8Bit());
      fflush(stdout);

      printf("QcjDataForm::refresh(): selecting by strWhere = |%s|\n", (const char*)strWhere.toLocal8Bit());

      sqlModel->setFilter(strWhere);
      if ( ! sqlModel->select() )
         QMessageBox::warning(NULL, "Error!", "Error selecting row\n" + sqlModel->lastError().databaseText() + "\n" + 
                              sqlModel->lastError().driverText() + "\n" +
                              sqlModel->query().executedQuery(), "Okay");

      sqlRecord = sqlModel->record(QCJ_FIRSTRECORD);
      sqlMapper.setCurrentIndex(QCJ_FIRSTRECORD);

      printf("QcjDataForm::refresh(): Found item |%s|\n", (const char *)sqlRecord.value(pFormDef->getIndexField(xmldef)).toString().toLocal8Bit());
      fflush(stdout);

      printf("QcjDataForm::refresh(): Done reading fields\n");
      fflush(stdout);
   }
   invalidData = false;
#endif
   printf("QcjDataForm::refresh():Exit\n");
   fflush(stdout);
}

/*!
      \fn void QcjDataForm::refresh(QSqlRecord *r)

       Sets  the  where  clause  for  the current form using the index
       field  of  the  record,  queries  the  database and updates the
       values of the form elements to those of the new record.
*/ 
void QcjDataForm::refresh(QSqlRecord *r)
{
   printf("QcjDataForm::refresh():Enter (qsqlrecord)\n");
   printf("QcjDataForm::refresh(): xmldef = |%s|\n", (const char*)xmldef.toLocal8Bit());
   fflush(stdout);
#ifndef QT4_DESIGNER_PLUGIN
   strWhere = pFormDef->getWhereClause(xmldef, r, db);
   printf("QcjDataForm::refresh(): strWhere  = |%s|\n", (const char*)strWhere.toLocal8Bit());
   fflush(stdout);
   refresh();
   invalidData = false;
#endif
   printf("QcjDataForm::refresh():Exit (qsqlrecord)\n");
   fflush(stdout);
}

void QcjDataForm::updateRecord()
{
   printf("QcjDataForm::updateRecord():Enter()\n");
   printf("QcjDataForm::updateRecord(): xmldef = |%s|\n", (const char*)xmldef.toLocal8Bit());
   fflush(stdout);

#ifndef QT4_DESIGNER_PLUGIN
   focusNextChild(); //Move focus to the next field to ensure all changes are recognized.
   focusPreviousChild(); //Move focus back to the original field.
   if ( ! invalidData ) 
   {
      for (QMap<QString, QWidget*>::iterator it1 = formFields.begin(); it1 != formFields.end(); ++it1) 
      {
         QWidget *wdt = it1.value();
         QString name = it1.key();
         if ( ::isA(wdt, "QLineEdit") && formValidators.contains(name)) 
         {
            printf("QcjDataForm::updateRecord(): field %s Is a line edit\n", qPrintable(name));
            const QValidator *val = formValidators[name];
            if ( val != NULL ) 
            {
               printf("QcjDataForm::updateRecord(): have validator\n");
               QString text = ((QLineEdit*)wdt)->text();
               int pos;
               printf("QcjDataForm::updateRecord(): validateing string\n");
               if ( val->validate(text, pos) == QValidator::Intermediate) 
               {
                  printf("QcjDataForm::updateRecord(): Validation failed, pos = %d\n", pos);
                  QMessageBox::warning(NULL, "Error!", "Error- Incomplete value entered\n");
                  ((QLineEdit*)wdt)->setCursorPosition(((QLineEdit*)wdt)->text().length());
                  wdt->setFocus();
                  return; 
               }
               else if ( val->validate(text, pos) != QValidator::Acceptable) 
               {
                  printf("QcjDataForm::updateRecord(): Validation failed, pos = %d\n", pos);
                  QMessageBox::warning(NULL, "Error!", "Error- Invalid value Entered\n");
                  wdt->setFocus();
                  return; 
               }
            }
         }
      }

      if ( pFormDef->isAutoCommit(xmldef) ) 
      {
         bool haveError = false;
         printf("QcjDataForm::updateRecord():Submitting all pending changes for record: %s\n", qPrintable(getFieldText("ident")));
         printf("QcjDataForm::updateRecord():name field: %s\n", qPrintable(getFieldText("name")));
         if ( ! sqlModel->submitAll() ) 
         {
            if ( sqlModel->lastError().databaseText().length() > 0 ) 
            {
               QMessageBox::warning(NULL, "Error!", "Error updating record: \n" + sqlModel->lastError().databaseText() + "\n" + 
                                    sqlModel->lastError().driverText() + "\n" +
                                    sqlModel->query().executedQuery(), "Okay");
               rollbackTransaction();
               printf("QcjDataForm::updateRecord():All changes discarded!\n");
               haveError = true;
            }
            else 
            {
               printf("QcjDataForm::updateRecord():Ignoring driver error: %s\n", qPrintable(sqlModel->lastError().driverText()));
            }
         }
         if ( ! haveError ) 
         {
            commitTransaction(false);
            printf("QcjDataForm::updateRecord():All changes submitted!\n");
         }
      }

      /***********************************************/
      /*   Get  the  index  fields and their values  */
      /*   then  find  the  record just updated and  */
      /*   emit  the  signal  activeRecord with the  */
      /*   index field info                          */
      /***********************************************/

      QMap<QString, QString> indexmap;

      QString index = pFormDef->getIndexField(xmldef);
      QStringList fields = index.split(',');

      if ( ! index.isEmpty() && index != "") 
      {
         for ( QStringList::Iterator it = fields.begin(); it != fields.end(); ++it ) {
            printf("QcjDataForm::updateRecord():field = |%s|\n", (const char*)(*it).toLocal8Bit());
            QString val = sqlRecord.field((*it)).value().toString();
            indexmap[(*it)] = val;
         }
      }
      else if ( db->driverName() != "QMYSQL" )
         indexmap["id"] = sqlRecord.field("id").value().toString();
      else
         indexmap["id"] = sqlRecord.field("id").value().toString();

      if ( indexmap.size() > 0 ) 
      {
         printf("QcjDataForm::updateRecord():emitting activeRecord signal\n");
         emit updated();
         emit activeRecord(indexmap);
      }
      insertMode = false;
      refresh();
   }
   else
      printf("QcjDataForm::updateRecord():No chjanges made, record not saved!\n");

#endif

   printf("QcjDataForm::updateRecord():Exit()\n");
   fflush(stdout);
}


/*!
      \fn void QcjDataForm::delete()

       Deletes  the  current  record from the database. A query dialog
       will be presented to the user to confirm deleting the record.
*/ 
void QcjDataForm::deleteRecord()
{
   printf("QcjDataForm::deleteRecord():Enter()\n");
   printf("QcjDataForm::delete(): xmldef = |%s|\n", (const char*)xmldef.toLocal8Bit());
   fflush(stdout);

#ifndef QT4_DESIGNER_PLUGIN
   if ( ! invalidData ) 
   {
      if ( pFormDef->confirmDelete(xmldef, this) ) 
      {
         printf("QcjDataForm::delete(): Removing row\n");
         fflush(stdout);
         if ( ! sqlModel->removeRows(QCJ_FIRSTRECORD, 1) )
         {
            printf("QcjDataForm::deleteRecord():Have error\n");
            fflush(stdout);
            QMessageBox::warning(NULL, "Error!", "Error deleting Record\n" + sqlModel->lastError().databaseText(), "Okay");
            rollbackTransaction();         
         }
         else if ( pFormDef->isAutoCommit(xmldef) ) 
         {
            printf("QcjDataForm::deleteRecord():Submitting all changes\n");
            fflush(stdout);
            if ( pFormDef->isAutoCommit(xmldef) ) 
            {
               sqlModel->submitAll();
               insertMode = false;
               emit(updated());
            }
         }
   //      printf("QcjDataForm::deleteRecord():Updated %d records, db driver error = |%s|\n", x, qPrintable(sqlModel->lastError().driverText()));
   //      printf("QcjDataForm::deleteRecord():Updated %d records, database  error = |%s|\n", x, qPrintable(sqlModel->lastError().databaseText()));
         printf("QcjDataForm::deleteRecord(): Record deleted Setting insertMode to false\n");
         fflush(stdout);
         insertMode = false;
         printf("QcjDataForm::deleteRecord(): Moving to first record\n");
         sqlMapper.setCurrentIndex(QCJ_FIRSTRECORD);
      }
   }
   else
      printf("QcjDataForm::deleteRecord():Invalid Data, no delete!\n");
#endif
   printf("QcjDataForm::deleteRecord():Exit()\n");
   fflush(stdout);
}



/*!
      \fn void QcjDataForm::insertRecord()

       This   function  will  insert  a record into the table. It will
       only  work for those tables with a single field for the primary
       key or index.

       Fields which are not the index field and do not have a default defined in
       the form definition will be set to NULL.
*/ 
void QcjDataForm::insertRecord()
{
   QMap<QString, QString> fields;
   QString idx, sql;
   QString indexname;
   QString fname;
   QString fvals;


#ifndef QT4_DESIGNER_PLUGIN
   printf("QcjDataForm::insertRecord():Enter\n");
   fflush(stdout);
   printf("QcjDataForm::insertRecord(): xmldef = |%s|, drivername = '%s'\n", (const char*)xmldef.toLocal8Bit(), qPrintable(db->driverName()));
   fflush(stdout);

   /***********************************************/
   /*   If  there  is  an insert pending, update  */
   /*   the  record  to be sure it gets saved to  */
   /*   the database                              */
   /***********************************************/
   if ( insertMode == true ) 
   {
      printf("QcjDataForm::insertRecord(): Updating existing insert\n");
      fflush(stdout);
      updateRecord();
   }

   /***********************************************/
   /*   If  we are not in a transaction, start a  */
   /*   new one.                                  */
   /***********************************************/
   if ( ! inTransaction ) 
   {
      printf("QcjDataForm::insertRecord(): Starting transaction since none started\n");
      fflush(stdout);
      beginTransaction();
   }
   printf("QcjDataForm::insertRecord(): Here 1\n");
   fflush(stdout);

   indexname = pFormDef->getIndexField(xmldef);
   printf("QcjDataForm::insertRecord(): indexname = %s\n", qPrintable(indexname));
   fflush(stdout);
   /***********************************************/
   /*   If  no  index  generation was defined in  */
   /*   the form definition                       */
   /***********************************************/
   if ((idx = pFormDef->getNewIndex(xmldef)).isEmpty())
   {
      if ( db->driverName() != "QMYSQL" && db->driverName() != "QSQLITE" ) 
      {
         /***********************************************/
         /*   And if no index field was given,           */
         /*   assume a postgres sequence is used        */
         /***********************************************/
         printf("QcjDataForm::insertRecord(): Assuming postgres sequence\n");
         sql = "select nextval('" + dbTable + "_" + indexname + "_seq" + "')";

         /***********************************************/
         /*   Now fetch the value for the index of the  */
         /*   next record                               */
         /***********************************************/
         printf("QcjDataForm::insertRecord(): sql to calulate index field: |%s|\n", (const char*)sql.toLocal8Bit());
         QSqlQuery q(sql);
         if ( q.isActive() )
         {
            printf("QcjDataForm::insertRecord(): Have active query\n");
            fflush(stdout);
            if ( q.next() ) 
            {
               idx = q.value(0).toString();
            }
         }
      }
   }

   /***********************************************/
   /*   If  no  special  sql statement was given  */
   /*   for inserting an empty record             */
   /***********************************************/
   QSqlQuery q1;

   printf("QcjDataForm::insertRecord(): Setting default field values, index field = |%s|\n", qPrintable(indexname));
   if ((sql = pFormDef->getInsertQuery(xmldef, idx)).isEmpty())
   {

      /***********************************************/
      /*   Add  any  default  values defined in the  */
      /*   form.                                     */
      /***********************************************/
      QMap<QString, QString>::iterator it = fieldDefaults.begin();
      while (it != fieldDefaults.end()) 
      {
         if ( indexname != it.key() ) 
         {
            if ( it.value().size() > 0 ) 
            {
               printf("QcjDataForm::insertRecord(): have field %s = |%s|\n", (const char*)it.key().toLocal8Bit(), (const char*)it.value().toLocal8Bit());
               fflush(stdout);
               if ( fname.size() > 0 ) 
                  fname += ",";
               fname += it.key();
               if ( fvals.size() > 0 ) 
                  fvals += ",";
               fvals += ":" + it.key();
            }
         }
         else 
         {
            printf("QcjDataForm::insertRecord(): skipping index field = |%s|\n", qPrintable(indexname));
         }
         ++it;
      }
      if ( ! indexname.isEmpty() ) 
      {
         printf("QcjDataForm::insertRecord(): new index = |%s|\n", qPrintable(idx));
         if ( fname.size() > 0 ) 
            fname += ",";
         fname += indexname;
         if ( fvals.size() > 0 ) 
            fvals += ",";
         fvals += ":" + indexname;
      }

      sql = "insert into " + dbTable + " (" + fname + ") values (" + fvals + ")";
      printf("QcjDataForm::insertRecord(): sql = |%s|\n", (const char*)sql.toLocal8Bit());

      q1.prepare(sql);

      if ( ! indexname.isEmpty() ) 
         q1.bindValue(":idxname", idx);

      it = fieldDefaults.begin();
      while (it != fieldDefaults.end()) 
      {
         if ( it.value().size() > 0 ) 
            q1.bindValue(QString(":") + it.key(), getDefaultFieldValue(it.value()));
         ++it;
      }


      if ( ! indexname.isEmpty() ) 
      {
         q1.bindValue(QString(":") + indexname, idx);
         QSqlField f(indexname, QVariant::String);
         f.setValue(idx);
         strWhere = indexname + " = " + db->driver()->formatValue(f);
      }
   }

   /***********************************************/
   /*   If  an  insert query was defined for the  */
   /*   form, just prepare it                     */
   /***********************************************/
   else 
   {
      if ( !q1.prepare(sql) )
      {
         QMessageBox::warning(NULL, "Error!", "Error preparing insert statement:\n" + q1.lastError().driverText() + " - " +q1.lastError().databaseText(), "Okay");
         rollbackTransaction();         
      }
   }
   printf("QcjDataForm::insertRecord():sql = '%s'\n", (const char*)sql.toLocal8Bit());
   fflush(stdout);

   /***********************************************/
   /*   Insert the record, if any problems bitch  */
   /*   about it!                                 */
   /***********************************************/
   if ( !q1.exec() ) 
   {
      QMessageBox::warning(NULL, "Error!", "Error inserting record:\n" + q1.lastError().driverText() + " - " +q1.lastError().databaseText(), "Okay");
      rollbackTransaction();         
   }
   else 
   {

      if ( db->driverName() == "QMYSQL" || db->driverName() == "QSQLITE") 
      {
         QString sql;

         if ( db->driverName() == "QMYSQL" )
            sql = "select LAST_INSERT_ID() from " + dbTable + " limit 1";
         else
            sql = "select last_insert_rowid() from " + dbTable + " limit 1";

         printf("QcjDataForm::insertRecord(): sql = |%s|\n", (const char*)sql.toLocal8Bit());
         QSqlQuery q2;
         q2.prepare(sql);
         if ( q2.exec() )
         {
            if ( q2.next() ) 
            {
               idx = q2.record().field(0).value().toString();
               indexname = "id";
            }
         }
         else 
         {
            printf("QcjDataForm::insertRecord():Error %s - %s\n", qPrintable(q2.lastError().driverText()),  qPrintable(q2.lastError().databaseText()));
            QMessageBox::warning(NULL, "Error!", "Error, could not get key after insert: \n" + q2.lastError().driverText() + " - " + q2.lastError().databaseText(), "Okay");
            rollbackTransaction();
            printf("QcjDataForm::insertRecord():Exit\n");
            return;
         }
      }

      /***********************************************/
      /*   Otherwise,  refresh the form and fill in  */
      /*   any defaults                              */
      /***********************************************/
      QSqlField f(indexname, QVariant::String);
      f.setValue(idx);
      strWhere = indexname + " = " + db->driver()->formatValue(f);
      printf("QcjDataForm::insertRecord():where clause set to |%s|\n", (const char*)strWhere.toLocal8Bit());
      refresh();
      sqlMapper.setCurrentIndex(QCJ_FIRSTRECORD);
   }
   insertMode = true;
   setFocus();
#endif
   printf("QcjDataForm::insertRecord():Exit\n");
}


/*!
      \fn void QcjDataForm::insertRecord(QMap<QString, QString> &fields)

       Inserts  a  new record into the database. The fields defined by
       the  keys of the map <em>fields</em> will set to the associated
       values of the map.

       If any of the fields passed in through the map are considered index fields,
       no key will be generated for the record. Otherwise, a new primary key
       will be generated using the generaic rules.
*/ 
void QcjDataForm::insertRecord(QMap<QString, QString> &fields)
{
   QString idx, sql;
   QString fname;
   QString fvals;
   QString idxname;

#ifndef QT4_DESIGNER_PLUGIN
   printf("QcjDataForm::insertRecord(QMap):Enter\n");
   printf("QcjDataForm::insertRecord(): xmldef = |%s|\n", (const char*)xmldef.toLocal8Bit());

   /***********************************************/
   /*   If  an  insert  is  pending,  update the  */
   /*   record first.                             */
   /***********************************************/
   if ( insertMode == true ) 
   {
      printf("QcjDataForm::insertRecord(): Updating existing insert\n");
      updateRecord();
   }

   /***********************************************/
   /*   If   we   are   currently   not   in   a  */
   /*   transaction, start one                    */
   /***********************************************/
   if ( ! inTransaction ) 
   {
      printf("QcjDataForm::insertRecord(): Starting transaction since none started\n");
      beginTransaction();
   }

   fname = "";
   fvals = "";

   strWhere = "";

   QString indexname = pFormDef->getIndexField(xmldef);
   printf("QcjDataForm::insertRecord(): indexname = |%s|\n", qPrintable(indexname));
   QStringList sl = indexname.split(',');
   if ( indexname.size() > 0 && sl.size() > 0 ) 
   {
      printf("QcjDataForm::insertRecord() Found %d indexfields defined for the form\n", sl.size());
      fflush(stdout);

      /***********************************************/
      /*   Iterate   through  each  of  the  fields  */
      /*   defined  in  the form definition and see  */
      /*   if  any  of  them  are  included  in the  */
      /*   passed in map of fields.                  */
      /***********************************************/
      for ( QStringList::Iterator it = sl.begin(); it != sl.end(); ++it ) 
      {
         bool found_it = false;
         QMap<QString, QString>::iterator it1 = fields.begin();
         while (it1 != fields.end()) 
         {
            printf("QcjDataForm::insertRecord(): Testing field |%s| against |%s|\n", qPrintable(it1.key()), qPrintable(*it));
            fflush(stdout);

            /***********************************************/
            /*   If we have an index field, add it to the  */
            /*   where clause and show that it was found.  */
            /***********************************************/
            if ( it1.key() == (*it) ) 
            {
               QSqlField f((*it), QVariant::String);
               f.setValue(it1.value());
               if ( strWhere.size() > 0 ) 
                  strWhere += " and ";
               strWhere += (*it) + " = " + db->driver()->formatValue(f);
               found_it = true;
               idxname = *it;
               break;
            }
            ++it1;
         }

         /***********************************************/
         /*   If  the  index  wasn't  in  the  list of  */
         /*   fields  passed in the map, then it needs  */
         /*   to  be  generated,  at  least  for  some  */
         /*   database types likie postgresql           */
         /***********************************************/
         if ( ! found_it ) 
         {
            printf("QcjDataForm::insertRecord(): Did not find expected index field: |%s|\n", qPrintable(*it));
            fflush(stdout);

            idxname = indexname;
            /***********************************************/
            /*   Check  is a keygenerator was defined for  */
            /*   the form. If so, use the key it created,  */
            /*   otherwise   generate   the  key  if  the  */
            /*   database uses sequences for such things.  */
            /***********************************************/
            if ( (idx = pFormDef->getNewIndex(xmldef)).isEmpty() )
            {
               if ( db->driverName() != "QMYSQL" && db->driverName() != "QSQLITE" ) 
               {
                  printf("QcjDataForm::insertRecord(): Assuming postgres sequence\n");
                  sql = "select nextval('" + dbTable + "_" + indexname + "_seq" + "')";

                  /***********************************************/
                  /*   Now fetch the value for the index of the  */
                  /*   next record                               */
                  /***********************************************/
                  printf("QcjDataForm::insertRecord(): sql to calulate index field: |%s|\n", (const char*)sql.toLocal8Bit());
                  QSqlQuery q(sql);
                  if ( q.isActive() )
                  {
                     printf("QcjDataForm::insertRecord(): Have active query\n");
                     fflush(stdout);
                     if ( q.next() ) 
                     {
                        idx = q.value(0).toString();
                     }
                  }

                  fname = idxname;
                  fvals = ":idxname";

                  QSqlField f(idxname, QVariant::String);
                  f.setValue(idx);
                  strWhere = idxname + " = " + db->driver()->formatValue(f);
                  printf("QcjDataForm::insertRecord(): strWhere set to: |%s|\n", qPrintable(strWhere));
               }
            }

            /***********************************************/
            /*   If  the key was generated using the form  */
            /*   definition, add the index and it's value  */
            /*   to  the list of fields and add it to the  */
            /*   where clause as well.                     */
            /***********************************************/
            else 
            {
               fname = idxname;
               fvals = ":idxname";

               QSqlField f(idxname, QVariant::String);
               f.setValue(idx);
               strWhere = idxname + " = " + db->driver()->formatValue(f);
               printf("QcjDataForm::insertRecord(): strWhere set to: |%s|\n", qPrintable(strWhere));
            }
         }
      }
   }
   else
   {
      idx = pFormDef->getNewIndex(xmldef);
      if ( idx.size() > 0 )
      {
         idxname = pFormDef->getIndexField(xmldef);
         if ( idxname.size() == 0 ) 
         {
            if ( db->driverName() == "QMYSQL" || 
                 db->driverName() == "QSQLITE" ) 
               idxname = "id";
            else 
               idxname = "ident";
         }

         fname = idxname;
         fvals = ":idxname";

         QSqlField f(idxname, QVariant::String);
         f.setValue(idx);
         strWhere = idxname + " = " + db->driver()->formatValue(f);
         printf("QcjDataForm::insertRecord(): strWhere set to: |%s|\n", qPrintable(strWhere));
      }
   }

   if ( ! inTransaction ) 
   {
      printf("QcjDataForm::insertRecord(): Starting transaction since none started\n");
      beginTransaction();
   }

   if ( insertMode == true ) 
   {
      printf("QcjDataForm::insertRecord(): Updating existing insert\n");
      updateRecord();
   }

   printf("QcjDataForm::insertRecord(): Setting values for passed fields\n");
   fflush(stdout);
   QMap<QString, QString>::iterator it = fields.begin();
   while (it != fields.end()) 
   {
      printf("QcjDataForm::insertRecord(): have field %s = |%s|\n", (const char*)it.key().toLocal8Bit(), (const char*)it.value().toLocal8Bit());
      if ( fname.size() > 0 ) 
         fname += ",";
      fname += it.key();
      if ( fvals.size() > 0 ) 
         fvals += ",";
      fvals += ":" + it.key();
      ++it;
   }


   /***********************************************/
   /*   Add  any  default  values defined in the  */
   /*   form definition to the query we're about  */
   /*   to  make.  Be  sure  to  only  add those  */
   /*   fields  which were not passed in by ther  */
   /*   caller                                    */
   /***********************************************/
   printf("QcjDataForm::insertRecord(): Setting default values, index name = |%s|\n", qPrintable(idxname));
   fflush(stdout);

   QMap<QString, QString>::iterator it1 = fieldDefaults.begin();
   while (it1 != fieldDefaults.end()) 
   {
      if ( (! fields.contains(it1.key())) && idxname != it1.key() ) 
      {
         if ( it1.value().size() > 0 ) 
         {
            printf("QcjDataForm::insertRecord(): have field %s = |%s|\n", (const char*)it1.key().toLocal8Bit(), (const char*)it1.value().toLocal8Bit());
            fflush(stdout);
            if ( fname.size() > 0 ) 
               fname += ",";
            fname += it1.key();
            if ( fvals.size() > 0 ) 
               fvals += ",";
            fvals += ":" + it1.key();
         }
      }
      else 
      {
         printf("QcjDataForm::insertRecord(): skipping passed index field %s\n", (const char*)it1.key().toLocal8Bit());
         fflush(stdout);
      }
      ++it1;
   }

   printf("QcjDataForm::insertRecord(): new index = |%s|, table = |%s|\n", qPrintable(idx), qPrintable(dbTable));

   /***********************************************/
   /*   Start  generating  the  insert statement  */
   /*   using the list of fields and value names  */
   /*   we created earlier.                       */
   /***********************************************/
   sql = "insert into " + dbTable + " (" + fname + ") values (" + fvals + ")";
   printf("QcjDataForm::insertRecord(): sql = |%s|\n", (const char*)sql.toLocal8Bit());

   QSqlQuery q;
   q.prepare(sql);

   /***********************************************/
   /*   Bind  the  value  we  have in idx to the  */
   /*   field   position  named  named  for  the  */
   /*   index.                                    */
   /***********************************************/
   q.bindValue(":idxname", idx);
   printf("QcjDataForm::insertRecord(): Binding val: |%s| to field: |:indexname|\n", qPrintable(idx));
   fflush(stdout);


   /***********************************************/
   /*   Now  bind  the remaining values to their  */
   /*   respective fields.                        */
   /***********************************************/
   it = fields.begin();
   while (it != fields.end()) 
   {
      printf("QcjDataForm::insertRecord(): Binding val: |%s| to field: |%s|\n", qPrintable(it.value()), qPrintable(it.key()));
      fflush(stdout);
      q.bindValue(QString(":") + it.key(), it.value());
      ++it;
   }

   /***********************************************/
   /*   Bind any remaining default values we may  */
   /*   have for the record.                      */
   /***********************************************/
   it = fieldDefaults.begin();
   while (it != fieldDefaults.end()) 
   {
      if ( (! fields.contains(it.key())) && idxname != it.key() ) 
      {
         if ( it.value().size() > 0 ) 
         {
            printf("QcjDataForm::insertRecord(): Binding val: |%s| to field: |%s|\n", qPrintable(it.value()), qPrintable(it.key()));
            fflush(stdout);
            q.bindValue(QString(":") + it.key(), getDefaultFieldValue(it.value()));
         }
      }
      ++it;
   }

   /***********************************************/
   /*   Now  execute the query, if there are any  */
   /*   errors,  let  the user know and rollback  */
   /*   the transaction.                          */
   /***********************************************/
   if (! q.exec())
   {
      printf("QcjDataForm::insertRecord(): got error: %s - %s\n", qPrintable(q.lastError().driverText()), qPrintable(q.lastError().databaseText()));
      QMessageBox::warning(NULL, "Error!", "Error inserting record: \n" + 
                                 q.lastError().driverText() + " : " + 
                                 q.lastError().databaseText(), "Okay");   
      rollbackTransaction();         
   }

   /***********************************************/
   /*   Otherwise, if there where no errors...    */
   /***********************************************/
   else 
   {
      /***********************************************/
      /*   If  the  database  is the type wich uses  */
      /*   the row id for for its primary keys, get  */
      /*   the  key  that  was  generated  from the  */
      /*   database and add it to the where clause.  */
      /***********************************************/
      if ( (db->driverName() == "QMYSQL" || 
            db->driverName() == "QSQLITE") && 
           pFormDef->getIndexField(xmldef).size() == 0 ) 
      {
//         QString sql = "select LAST_INSERT_ID() as id from " + dbTable + " limit 1";
         QString sql;

         if ( db->driverName() == "QMYSQL" )
            sql = "select LAST_INSERT_ID() from " + dbTable + " limit 1";
         else
            sql = "select last_insert_rowid() from " + dbTable + " limit 1";

         printf("QcjDataForm::insertRecord(): sql = |%s|\n", (const char*)sql.toLocal8Bit());
         QSqlQuery q2;
         q2.prepare(sql);
         if ( q2.exec() )
         {
            if ( q2.next() ) 
            {
               idx = q2.record().field(0).value().toString();
               indexname = "id";
            }
         }

         /***********************************************/
         /*   Complain  about any errors and roll back  */
         /*   the transaction.                          */
         /***********************************************/
         else 
         {
            printf("QcjDataForm::insertRecord():Error %s - %s\n", qPrintable(q2.lastError().driverText()),  qPrintable(q2.lastError().databaseText()));
            QMessageBox::warning(NULL, "Error!", "Error, could not get key after insert: \n" + q2.lastError().driverText() + " - " + q2.lastError().databaseText(), "Okay");
            rollbackTransaction();
            printf("QcjDataForm::insertRecord():Exit\n");
            return;
         }
         QSqlField f(indexname, QVariant::String);
         f.setValue(idx);
         strWhere = indexname + " = " + db->driver()->formatValue(f);
         printf("QcjDataForm::insertRecord():where clause set to |%s|\n", (const char*)strWhere.toLocal8Bit());
      }

      refresh();
      sqlMapper.setCurrentIndex(QCJ_FIRSTRECORD);
   }

   /***********************************************/
   /*   Show  we  are  in  the  insert mode (the  */
   /*   record   needs  updating  before  we  do  */
   /*   another  operation) and set the focus to  */
   /*   the desired field.                        */
   /***********************************************/
   insertMode = true;
   setFocus();
#endif
   printf("QcjDataForm::insertRecord():Exit\n");
}

/*!
      \fn void QcjDataForm::beginTransaction()

       Begins a transaction with the database.
*/ 
void QcjDataForm::beginTransaction()
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
      \fn void QcjDataForm::commitTransaction(bool emitUpdated)

       Commits  the  sql  transaction performed since beginTransaction
       was called.
*/ 
void QcjDataForm::commitTransaction(bool emitUpdated)
{
#ifndef QT4_DESIGNER_PLUGIN
   printf("QcjDataForm::commitTransaction(): Enter\n");
   db->commit();
   insertMode = false;
   inTransaction = false;
   if ( emitUpdated ) 
      emit(updated());
   printf("QcjDataForm::commitTransaction(): Exit\n");
#endif
}

/*!
      \fn void QcjDataForm::rollbackTransaction()

       Rolls back the sql transactions performed since beginTransaction
       was called.
*/ 
void QcjDataForm::rollbackTransaction()
{
#ifndef QT4_DESIGNER_PLUGIN
   if ( inTransaction ) 
   {
      db->rollback();
      insertMode = false;
      inTransaction = false;
      emit(updated());
   }
   refresh();
#endif
}

QString QcjDataForm::getLastIdent()
{
   printf("QcjDataForm::getLastIdent():strIdent = '%s'\n", (const char*)ident.toLocal8Bit());
   return(ident);
}

/*!
      \fn void QcjDataForm::setEnabled(bool b)

       Enables/Disables the form elements of the form.
*/ 
void QcjDataForm::setEnabled(bool b)
{
#ifndef QT4_DESIGNER_PLUGIN
   printf("QcjDataForm::setEnabled():Enter");
   fflush(stdout);
   for (unsigned x = 0; x < fields.size() - 1; x++) 
   {
      printf("QcjDataForm::setEnabled(): x = %d\n", x);
      fflush(stdout);
      printf("QcjDataForm::setEnabled(): setting enabled for |%s| (%d) to %d\n", (const char*)fields[x].dataName.toLocal8Bit(), x, b);
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
      \fn void QcjDataForm::setReadOnly(bool b)

       Set all  of  the  editable  text fields to read only or
       writeable  depending  on  the parameter <em>b</em>, If the field
       was marked as readonly in the XML definition, it is always made
       readonly.
*/ 
void QcjDataForm::setReadOnly(bool b)
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
      \fn QString QcjDataForm::getFieldText(QString name)

       Returns   the  value  of  the  field  named  by  the  parameter
       <em>name</em> as a QString.  This  is  what  is  shown  on  the form and not
       necessarily the value in the actual record of the database.
*/ 
QString QcjDataForm::getFieldText(QString name)
{
   return( getFieldValue(name).toString() );   
}

/*!
      \fn QVariant QcjDataForm::getFieldValue(QString name)

       Returns  a  QVariant  representing  the  value  of  the  field 
       named  by the parameter <em>name</em>. This is what is shown on
       the  form and not necessarily the value in the actual record of
       the database.
*/ 
QVariant QcjDataForm::getFieldValue(QString name)
{
   QVariant rv;

   printf("QcjDataForm::getFieldValue():Enter, name = |%s|\n", (const char*)name.toLocal8Bit());
   fflush(stdout);

#ifndef QT4_DESIGNER_PLUGIN
   for (unsigned x = 0; x < fields.size() - 1; x++) 
   {
      printf("QcjDataForm::getFieldValue():testing field %d - |%s|\n", x, (const char*)fields[x].dataName.toLocal8Bit());
      fflush(stdout);
      if ( fields[x].dataName == name ) 
      {
         printf("QcjDataForm::getFieldValue():have match\n");
         fflush(stdout);
         if ( fields[x].widget != NULL ) 
         {
            if ( ::isA(fields[x].widget, "QLineEdit") ) 
            {
               printf("QcjDataForm::getFieldValue():is a QLineEdit\n");
               fflush(stdout);
               return( QVariant(((QLineEdit*)fields[x].widget)->text()) );
            }
            else if ( ::isA(fields[x].widget, "QcjTextBlockEdit") ) 
            {
               printf("QcjDataForm::getFieldValue():is a QTextEdit\n");
               fflush(stdout);
               return( QVariant(((QcjTextBlockEdit*)fields[x].widget)->text()) );
            }
            else if ( ::isA(fields[x].widget, "QComboBox") ) 
            {
               printf("QcjDataForm::getFieldValue():is a QComboBox\n");
               fflush(stdout);
               return( QVariant(((QComboBox*)fields[x].widget)->currentText()) );
            }
            else if ( ::isA(fields[x].widget, "QcjMoneyEdit") ) 
            {
               QString str;
               str = ((QcjMoneyEdit*)fields[x].widget)->text();
               printf("QcjDataForm::getFieldValue():is a QcjMoneyEdit, |%s|\n", (const char*)str.toLocal8Bit());
               fflush(stdout);
               return(QVariant(str));
            }
            else if ( ::isA(fields[x].widget, "QcjPhoneEdit") ) 
            {
               QString str;
               str = ((QcjPhoneEdit*)fields[x].widget)->text();
               printf("QcjDataForm::getFieldValue():is a QcjPhoneEdit, |%s|\n", (const char*)str.toLocal8Bit());
               fflush(stdout);
               return(QVariant(str));
            }

#ifndef NO_PHOTO_SELECT
            else if ( ::isA(fields[x].widget, "QcjPhotoEntry") ) 
            {
               printf("QcjDataForm::getFieldValue():is a QcjPhotoEntry\n");
               fflush(stdout);
               return( QVariant( ((QcjPhotoEntry*)fields[x].widget)->get()) );
            }
#endif

            else 
            {
               printf("QcjDataForm::getFieldValue():is a Something??\n");
               fflush(stdout);
            }
         }
         else 
         {
            printf("QcjDataForm::getFieldValue():Field has no widget\n");
            fflush(stdout);
         }
      }
   }
#endif
   printf("QcjDataForm::getFieldValue():Exit\n");
   fflush(stdout);
   return(rv);
}

/*!
      \fn void QcjDataForm::setFieldText(QString name, QString str)

       Sets   the   value   of   the  field  named  by  the  parameter
       <em>name</em> to the QString parameter <em>val</em>
*/ 
void QcjDataForm::setFieldText(QString name, QString str)
{
   printf("QcjDataForm::setFieldText():Enter(): |%s| to |%s|\n", qPrintable(name), qPrintable(str));
   fflush(stdout);
   setFieldValue(name, QVariant(str));
   printf("QcjDataForm::setFieldText():Exit()\n");
   fflush(stdout);
}

/*!
      \fn void QcjDataForm::setFieldValue(QString name, QVariant val)

       Sets   the   value   of   the  field  named  by  the  parameter
       <em>name</em> to the QVariant parameter <em>val</em>
*/ 
void QcjDataForm::setFieldValue(QString name, QVariant val)
{
#ifndef QT4_DESIGNER_PLUGIN
   printf("QcjDataForm::setFieldValue():Enter(): %s\n", qPrintable(name));
   fflush(stdout);
   sqlRecord.setValue(name, val);
   sqlModel->setRecord(QCJ_FIRSTRECORD, sqlRecord);
   sqlMapper.setCurrentIndex(QCJ_FIRSTRECORD);
   printf("QcjDataForm::setFieldValue():Exit()\n");
   fflush(stdout);
#endif
}

/*!
      \fn void QcjDataForm::initializeWidget(QString name, QString str)

       Initializes  the field specified by the parameter <em>name</em>
       to  the  value  passed  in  the parameter <em>str</em>. In most
       cases,  <em>str</em>  is the value the field is set to. In the
       case  of  a QcjStringSelect type of field, <em>str</em> represents
       the values which placed in the list of selections.

       If deamed necessary by Qt, the geometry will be ajusted accordingly.
*/ 
void QcjDataForm::initializeWidget(QString name, QString str)
{
#ifndef QT4_DESIGNER_PLUGIN
   sqlRecord.setValue(name, str);
   sqlModel->setRecord(QCJ_FIRSTRECORD, sqlRecord);
   sqlMapper.setCurrentIndex(QCJ_FIRSTRECORD);
#endif
}

/*!
      \fn void QcjDataForm::setFieldEnabled(QString name, bool f)

       Enables   or   disables   the  field  named  by  the  parameter
       <em>name</em> from receving focus.
*/ 
void QcjDataForm::setFieldEnabled(QString name, bool f)
{
#ifndef QT4_DESIGNER_PLUGIN
   for (unsigned x = 0; x < fields.size() - 1; x++) 
   {
      if ( fields[x].dataName == name ) 
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
      \fn void QcjDataForm::resetFieldEnables()

       Resets all of the fields to the appropriate state for accepting
       focus. This state is defined by the XML definition of the form.
*/ 
void QcjDataForm::resetFieldEnables()
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
      \fn QWidget *QcjDataForm::getFieldWidget(QString name)

       Returns  a  pointer to the widget object for the field named by
       the parameter <em>name</em>.
*/ 
QWidget *QcjDataForm::getFieldWidget(QString name)
{
#ifndef QT4_DESIGNER_PLUGIN
   printf("QcjDataForm::setFieldWidget():Enter(): |%s|\n", qPrintable(name));
   for (unsigned x = 0; x < fields.size() - 1; x++) 
   {
      printf("QcjDataForm::setFieldWidget():Enter(): testing against |%s|\n", qPrintable(fields[x].dataName));
      if ( fields[x].dataName == name ) 
      {
         printf("QcjDataForm::setFieldWidget():Exit(): Found it!\n");
         return(fields[x].widget);
      }
   }
   printf("QcjDataForm::setFieldWidget():Exit():field not found\n");
   return(0);
#endif
}

/*!
      \fn void QcjDataForm::setFocus()

       Sets focus to the field specified as the focus field in the XML
       definition of the form.
*/ 
void QcjDataForm::setFocus()
{
   printf("QcjDataForm::setFocus(): Enter\n");
#ifndef QT4_DESIGNER_PLUGIN
   printf("QcjDataForm::setFocus(): fields.size = %d\n", fields.size());
   fflush(stdout);

   if ( fields.size() > 0 ) 
   {
      for (unsigned x = 0; x < fields.size() - 1; x++) 
      {
         printf("QcjDataForm::setFocus(): x = %d\n", x);
         fflush(stdout);
         printf("QcjDataForm::setFocus(): Testing field |%s|\n", (const char*)fields[x].dataName.toLocal8Bit());
         fflush(stdout);
         if ( fields[x].focusWidget ) 
         {
            printf("QcjDataForm::setFocus(): Found it!\n");
            fflush(stdout);
            if ( x == 0 && fields.size() > 1) 
               fields[1].widget->setFocus(); 
            else
               fields[x - 1].widget->setFocus(); 
            fields[x].widget->setFocus();
            break;
         }
      }
   }
#endif
   printf("QcjDataForm::setFocus(): Exit\n");
   fflush(stdout);
}

/*!
      \fn bool QcjDataForm::validateEdits()

       Iterates  through  the  fields which are derived from QLineEdit
       and validates their contents.
*/ 
bool QcjDataForm::validateEdits()
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

/*!
   \fn QString QcjDataForm::findXML(QStringList app_path, QString app_name)

       Finds  the  XML file for the application named by the parameter
       <em>app_name</em>  in  the directory specified by the parameter
       <em>app_path</em> or one of the files

- $HOME/.&lt;<em>app_name</em>&gt;.xml
- $HOME/.xml/&lt;<em>app_name</em>&gt;.xml
- /opt/&lt;<em>app_name</em>&gt;/default_forms.xml
- ./&lt;<em>app_name</em>&gt;.xml
*/ 

QString QcjDataForm::findXML(QStringList app_path, QString app_name)
{
   QString home, rv;
#ifndef QT4_DESIGNER_PLUGIN
   QFile fn;
   QStringList filenames;

   home = getenv("HOME");

   QString configs = home + "/." + app_name + ".xml;";
   configs += "./" + app_name + ".xml;";
   configs += home + "/.xml/" + app_name + ".xml;";
   configs += "/opt/" + app_name + "/default_forms.xml;";
   configs += app_path.join("/") + "/" + app_name + ".xml;";
   configs += app_path.join("/") + "/../" + app_name + ".xml;";

   filenames = configs.split(";");
   for ( QStringList::Iterator it = filenames.begin(); it != filenames.end(); ++it ) {
      printf("QcjDataForm::findXML(): testing for xml config file |%s|\n", qPrintable(*it));
      if ( fn.exists(*it) ) 
      {
         rv = *it;
         break;
      }
   }
   printf("QcjDataForm::findXML(): Returning filename |%s|\n", qPrintable(rv));
#endif
   return(rv);
}

/*!
   \fn QString QcjDataForm::getDefaultFieldValue(QString defstring)
   
   This member function parses <em>defstring<em> and returns the appropriate
   value to use as the default for a field.

   If <em>defsring</em> is not special, it is used as is. Otherwise, it can have
   a specially formatted value of <em>&lt;type&gt;:&lt;value&gt; It is important that
   there are no spaces between  &lt;type&gt; the <em>:</em> and &lt;value&gt;.

   Type can be any of the following: 

   \li \c config This will use &lt;value&gt; as a configuration name and fetch it value from
   the settings.
   
   \li \c string This will use &lt;value&gt; as the value.

   \li \c sqlquery This will query the database using &lt;value&gt; as an sql statement and return
   the first value of the first record returned.

*/
QString QcjDataForm::getDefaultFieldValue(QString defstring)
{
   QString rv = defstring;

#ifndef QT4_DESIGNER_PLUGIN
   printf("QcjDataForm::getDefaultFieldValue():Enter- defstring = |%s|\n", qPrintable(defstring));
   QStringList conf = defstring.split(':');
   printf("QcjDataForm::getDefaultFieldValue():Split complete have %d items\n", conf.count());
   if ( conf.count() >= 2 ) 
   {
      QString type = conf[0];

      QString name;
      for (int cnt = 1; cnt < conf.count(); cnt++) 
      {
         name += conf[cnt];
      }

      printf("QcjDataForm::getDefaultFieldValue():Name = |%s|, type = |%s|\n", qPrintable(name), qPrintable(type));
      fflush(stdout);
      if ( pConfig != NULL && type == "config" ) 
      {
         printf("QcjDataForm::getDefaultFieldValue():config name = |%s|\n", qPrintable(name));
         rv = pConfig->value(name, pFormDef->getConfigurationDefault(name)).toString();
      }
      else if ( type == "string" )
      {
         rv = name;
      }
      else if ( type == "sqlquery" ) 
      {
         QString sql = name;
         QString init;
         printf("QcjDataForm::getDefaultFieldValue():sqlquery sql = |%s|\n", qPrintable(sql));
         fflush(stdout);
         QSqlQuery *query = new QSqlQuery(sql);
         if (query->next()) 
         {
            QVariant v;
            if ( (v = query->value(1)) == QVariant::Invalid ) 
            {
               rv = query->value(0).toString();
            }
         }
      }
   }
   printf("QcjDataForm::getDefaultFieldValue():Exit- rv = |%s|\n", qPrintable(rv));
   fflush(stdout);
#endif
   return(rv);
}

QMap<QString, QString> QcjDataForm::getIndexMap()
{
   QMap<QString, QString> rv;
#ifndef QT4_DESIGNER_PLUGIN
   printf("QcjDataForm::getIndexMap():Enter\n");
   QStringList sl = pFormDef->getIndexField(xmldef).split(",");
   QStringList::Iterator it;
   for (QStringList::Iterator it = sl.begin(); it != sl.end(); ++it) 
   {
      printf("QcjDataForm::getIndexMap(): Found indexfield |%s|, value |%s|\n", qPrintable(*it), qPrintable(getFieldText(*it)));
      rv.insert(*it, getFieldText(*it));
   }
   printf("QcjDataForm::getIndexMap():Exit\n");
#endif
   return(rv);
}

QString QcjDataForm::getIndexFilter()
{
   QString rv;
#ifndef QT4_DESIGNER_PLUGIN
   printf("QcjDataForm::getIndexFilter():Enter\n");
   QStringList sl = pFormDef->getIndexField(xmldef).split(",");
   QStringList::Iterator it;
   for (QStringList::Iterator it = sl.begin(); it != sl.end(); ++it) 
   {
      if ( rv.size() > 0 ) 
      {
         rv += " and ";
      }
      printf("QcjDataForm::getIndexFilter(): Found indexfield |%s|, value |%s|\n", qPrintable(*it), qPrintable(getFieldText(*it)));
      QSqlField f(*it, QVariant::String);
      f.setValue(getFieldText(*it));
      rv += *it + " = " + db->driver()->formatValue(f);
   }
   printf("QcjDataForm::getIndexFilter():Exit- rv = |%s|\n", qPrintable(rv));
#endif
   return(rv);
}

