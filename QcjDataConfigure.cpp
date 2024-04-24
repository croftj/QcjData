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

#include <QtGui>
#include <QObject>

#include "QcjData/Qcjdoubleentry.h"
#include "QcjData/Qcjfileentry.h"
#include "QcjData/Qcjintegerentry.h"
#include "QcjData/Qcjsinglelineentry.h"
#include "QcjData/Qcjyesnoentry.h"
#include "QcjData/QcjDataConfigure.h"
#include "QcjData/QcjDataStatics.h"

/*! \class QcjDataConfigure
                                                               
    \brief  This  class  generates  a  dialog  for  configuring  an
    application
    
    The QcjDataConfigure class creates a configuration dialog to accept
    application  specific settings from the user. The definition of
    the  dialog  is  specified  by  &lt;configuration&gt; tags in the xml
    document of the application.

*/                                                                   



/*!                                                                 
   Creates  a  new  QcjDataConfigure  dialog object. This object must
   then be initialized then executed using exec()
*/                                                                  
QcjDataConfigure::QcjDataConfigure(QWidget *p) :
    QDialog(p)
{
    ui.setupUi(this);
}

/*!                                                                 
   Initializes  the  dialog box using the configuration definition
   named by the parameter \em config.
*/                                                                  
void QcjDataConfigure::initialize( QString config )
{
 
#ifndef QT4_DESIGNER_PLUGIN
   connect(ui.saveBtn, SIGNAL(clicked()),
           this, SLOT(save()), Qt::UniqueConnection);
   connect(ui.cancelBtn, SIGNAL(clicked()),
           this, SLOT(reject()), Qt::UniqueConnection);

   QString help = pFormDef->getConfigurationHelp(config);
   if ( help.size() == 0 ) 
   {
      help = "These values of the items to the right will be saved to be used now and in the future.";
   }
   ui.helpEdit->setText(help);
   qDebug() << "help: " << help;

   setWindowTitle(pFormDef->getConfigurationTitle(config));
   QStringList fields = pFormDef->getConfigurationItemNames(config);

   QGridLayout *layout = new QGridLayout(ui.widgetFrame);

   int row;
   QStringList::iterator it;
   for (row = 0, it = fields.begin(); it != fields.end(); it++, row++) 
   {
      printf("QcjDataConfigure::initialize(): getting details for entry |%s|\n", qPrintable(*it));
      QStringList parms = pFormDef->getConfigurationDetails(config, *it);
      printf("QcjDataConfigure::initialize(): found %d parms\n", parms.count());
      if ( parms.count() >= 4 ) 
      {
         QString name      = parms[0];
         QString label     = parms[1];
         QString type      = parms[2];
         QString defvalue  = pConfig->value(name, parms[3]).toString();
         QString name_type = "conf::" + name + "::" + type;
         printf("QcjDataConfigure::initialize(): name: |%s|, label = |%s|, type = |%s|\n", qPrintable(name), qPrintable(label), qPrintable(type));
         if ( type.toLower() == "lineedit" || type.toLower() == "password") 
         {
            printf("QcjDataConfigure::initialize(): creating lineedit\n");
            fflush(stdout);
            QcjSingleLineEntry *wdt = new QcjSingleLineEntry(ui.widgetFrame);
            if ( type.toLower() == "password") 
               wdt->setEchoMode(QLineEdit::Password);
            printf("QcjDataConfigure::initialize(): creating lineedit\n");
            fflush(stdout);
            wdt->setObjectName(name_type);
            printf("QcjDataConfigure::initialize(): creating lineedit\n");
            fflush(stdout);
            wdt->setLabel(label);
            printf("QcjDataConfigure::initialize(): creating lineedit\n");
            fflush(stdout);
            wdt->setText(defvalue);
            printf("QcjDataConfigure::initialize(): creating lineedit\n");
            fflush(stdout);
            QLabel *lbl = new QLabel(label, ui.widgetFrame);
            printf("QcjDataConfigure::initialize(): creating lineedit\n");
            fflush(stdout);
            lbl->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);
            printf("QcjDataConfigure::initialize(): creating lineedit\n");
            fflush(stdout);
            layout->addWidget(lbl, row, 0);
            layout->addWidget(wdt, row, 1);
            printf("QcjDataConfigure::initialize(): creating lineedit\n");
            fflush(stdout);
         }
         else if ( type.toLower() == "double" ) 
         {
            QcjDoubleEntry *wdt = new QcjDoubleEntry(ui.widgetFrame);
            wdt->setObjectName(name_type);
            wdt->setLabel(label);
            wdt->setText(defvalue);
            QString min = parms[4];
            QString max = parms[5];
            if ( min != "min" ) 
               wdt->setMin(min.toDouble());
            if ( min != "max" ) 
               wdt->setMin(max.toDouble());
            QLabel *lbl = new QLabel(label, ui.widgetFrame);
            layout->addWidget(lbl, row, 0);
            layout->addWidget(wdt, row, 1);
         }
         else if ( type.toLower() == "integer" ) 
         {
            QcjIntegerEntry *wdt = new QcjIntegerEntry(ui.widgetFrame);
            wdt->setObjectName(name_type);
            wdt->setLabel(label);
            wdt->setText(defvalue);
            QString min = parms[4];
            QString max = parms[5];
            if ( min != "min" ) 
               wdt->setMin(min.toInt());
            if ( min != "max" ) 
               wdt->setMin(max.toInt());
            QLabel *lbl = new QLabel(label, ui.widgetFrame);
            layout->addWidget(lbl, row, 0);
            layout->addWidget(wdt, row, 1);
         }
         else if ( type.toLower() == "yesno" ) 
         {
            QcjYesNoEntry *wdt = new QcjYesNoEntry(ui.widgetFrame);
            wdt->setObjectName(name_type);
            wdt->setLabel(label);
            wdt->setText(defvalue);
//            wdt->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
            QLabel *lbl = new QLabel(label, ui.widgetFrame);
            layout->addWidget(lbl, row, 0);
            layout->addWidget(wdt, row, 1);
            wdt->adjustSize();
         }
         else if ( type.toLower() == "file" )
         {
            QString filter;
            printf("QcjDataConfigure::initialize(): creating QcjFileEntry\n");
            fflush(stdout);
            QcjFileEntry *wdt = new QcjFileEntry(ui.widgetFrame);
            printf("QcjDataConfigure::initialize(): QcjFileEntry created wdt = %ld\n", (long)wdt);
            fflush(stdout);
            wdt->setObjectName(name_type);
            wdt->setLabel(label);
            wdt->setText(defvalue);
            printf("QcjDataConfigure::initialize(): Setting filter\n");
            fflush(stdout);
            if ( parms.size() >= 5 ) 
               filter = parms[4];
            else
               filter = "*";
            printf("QcjDataConfigure::initialize(): filter = |%s|\n", qPrintable(filter));
            fflush(stdout);
            wdt->setFilter(filter);
            printf("QcjDataConfigure::initialize(): Setting type to file\n");
            fflush(stdout);
            wdt->setType("file");
            printf("QcjDataConfigure::initialize(): creating QLabel\n");
            fflush(stdout);
            QLabel *lbl = new QLabel(label, ui.widgetFrame);
            printf("QcjDataConfigure::initialize(): Adding QLabel to layout\n");
            fflush(stdout);
            layout->addWidget(lbl, row, 0);
            printf("QcjDataConfigure::initialize(): Adding QcjFileEntry to layout\n");
            fflush(stdout);
            layout->addWidget(wdt, row, 1);
            printf("QcjDataConfigure::initialize(): done initializing QcjFileEntry widget\n");
            fflush(stdout);
         }
         else if ( type.toLower() == "directory" )
         {
            QcjFileEntry *wdt = new QcjFileEntry(ui.widgetFrame);
            wdt->setObjectName(name_type);
            wdt->setLabel(label);
            wdt->setText(defvalue);
            wdt->setType("directory");
            QLabel *lbl = new QLabel(label, ui.widgetFrame);
            layout->addWidget(lbl, row, 0);
            layout->addWidget(wdt, row, 1);
         }
      }
   }
//   adjustSize();
//   updateGeometry();
//   ui.widgetFrame->updateGeometry();
   printf("QcjDataConfigure::initialize(): expandingDirections() = %d\n", (int)layout->expandingDirections());
   fflush(stdout);
   save();
#endif
}

/*!
   \fn void QcjDataConfigure::save()

   Saves  the  values  entered  in by the user to the applications
   settings.
*/                                                                  
void QcjDataConfigure::save()
{
#ifndef QT4_DESIGNER_PLUGIN
   QRegExp re("^conf::.*");
   QFrame *wframe;

   if ((wframe = QObject::findChild<QFrame*>(QString("widgetFrame"))) != NULL) 
   {
      QList<QObject*> childlist = wframe->children();
      QListIterator<QObject*> it(childlist);
      QObject *obj;

      printf("QcjDataConfigure::save(): Enter\n");
      fflush(stdout);
      while (it.hasNext()) 
      {
         obj = it.next();
         QString object = obj->objectName();
         printf("QcjDataConfigure::save(): childname = |%s|\n", qPrintable(obj->objectName()));
         if ( ! object.contains(re) ) 
            continue;

         printf("QcjDataConfigure::save(): object name = |%s|\n", qPrintable(obj->objectName()));
         fflush(stdout);
         QStringList parmdef = object.split("::");
         QString name = parmdef[1];
         QString type = parmdef[2];
         printf("QcjDataConfigure::save(): Saving parm name = |%s|, type = |%s|\n", qPrintable(name), qPrintable(type));
         fflush(stdout);
         if ( type.toLower() == "lineedit" || type.toLower() == "password") 
            pConfig->setValue(name, ((QcjSingleLineEntry*)obj)->text());
         else if ( type.toLower() == "double" ) 
            pConfig->setValue(name, ((QcjDoubleEntry*)obj)->text());
         else if ( type.toLower() == "integer" ) 
            pConfig->setValue(name, ((QcjIntegerEntry*)obj)->text());
         else if ( type.toLower() == "file" || type.toLower() == "directory" )
            pConfig->setValue(name, ((QcjFileEntry*)obj)->text());
         else if ( type.toLower() == "yesno" ) 
         {
            if (((QcjYesNoEntry*)obj)->text() == "Yes")
               pConfig->setValue(name, "Y");
            else
               pConfig->setValue(name, "N");
         }
      }
      pConfig->sync();
      
   }
#endif
   printf("QcjDataConfigure::save(): Exit\n");
   fflush(stdout);
   accept();
}

/*!                                          
   Refreshes the dialogs form entry objects
   to    the    current   values   in   the
   applications settings
*/                                           
void QcjDataConfigure::refresh()
{
#ifndef QT4_DESIGNER_PLUGIN
   QRegExp re("^conf::.*");
   QFrame *wframe;

   if ((wframe = findChild<QFrame*>(QString("widgetFrame"))) != NULL) 
   {
      QList<QObject*> childlist = wframe->children();
      QListIterator<QObject*> it(childlist);
      QObject *obj;

      printf("QcjDataConfigure::refresh(): Enter\n");
      fflush(stdout);
      while (it.hasNext()) 
      {
         obj = it.next();
         QString object = obj->objectName();
         printf("QcjDataConfigure::refresh(): childname = |%s|\n", qPrintable(obj->objectName()));
         if ( ! object.contains(re) ) 
            continue;

         printf("QcjDataConfigure::refresh(): object name = |%s|\n", qPrintable(obj->objectName()));
         fflush(stdout);
         QStringList parmdef = object.split("::");
         QString name = parmdef[1];
         QString type = parmdef[2];
         printf("QcjDataConfigure::refresh(): Saving parm name = |%s|, type = |%s|\n", qPrintable(name), qPrintable(type));
         fflush(stdout);

         QString defvalue  = pConfig->value(name).toString();

         if ( type.toLower() == "lineedit" || type.toLower() == "password") 
            ((QcjSingleLineEntry*)obj)->setText(defvalue);
         else if ( type.toLower() == "double" ) 
            ((QcjDoubleEntry*)obj)->setText(defvalue);
         else if ( type.toLower() == "integer" ) 
            ((QcjIntegerEntry*)obj)->setText(defvalue);
         else if ( type.toLower() == "yesno" ) 
            ((QcjYesNoEntry*)obj)->setText(defvalue);
         else if ( type.toLower() == "file" || type.toLower() == "directory" )
            ((QcjFileEntry*)obj)->setText(defvalue);
      }
      pConfig->sync();
      
   }
#endif
   printf("QcjDataConfigure::refresh(): Exit\n");
   fflush(stdout);
   accept();
}

