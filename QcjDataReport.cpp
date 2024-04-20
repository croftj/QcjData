/*********************************************************************************
**
**   $Id$
**   Copyright (c) 2005 Joe Croft <joe@croftj.net>
**   
o**   This file is part of QcjData Class Libraries.
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
# include <QDebug>
# include <QDir>
# include <QDomDocument>
# include <QFile>
# include <QLabel>
# include <QMessageBox>
# include <QPrintDialog>
# include <QPrinter>
# include <QSqlField>
# include <QSqlQuery>
# include <QSqlRecord>
# include <QString>
# include <QStringList>
# include <QVector>
# include "QcjData/QcjDataReport.h"
#include "QcjData/Qcjdoubleentry.h"
#include "QcjData/Qcjfileentry.h"
#include "QcjData/Qcjintegerentry.h"
#include "QcjData/Qcjsinglelineentry.h"
#include "QcjData/Qcjtruefalse.h"
#include "QcjData/Qcjyesnoentry.h"
#include "QcjData/QcjDataConfigure.h"
#include "QcjData/QcjDataStatics.h"

/*!
       \class QcjDataReport
       
       \brief  QcjDataReport  Class builds a dialog box and displays a
       report
       
       QcjDataReport  Class  is  used  for displaying and printing reports
       from  within an application. It derives it's widget definitions
       and the contents of report from an XML file.
*/ 


/*!
       \fn QcjDataReport::QcjDataReport(QWidget *parent)
       
       Constructor for the QcjDataReport object.
*/ 
QcjDataReport::QcjDataReport(QWidget *parent) : QDialog(parent)
{
    ui.setupUi(this);
    report = NULL;
    ui.printBtn->setEnabled(false);
    connect(ui.cancelBtn, SIGNAL(clicked()), this, SLOT(reject()), Qt::UniqueConnection);
    connect(ui.printBtn, SIGNAL(clicked()), this, SLOT(printReport()), Qt::UniqueConnection);
    connect(ui.previewBtn, SIGNAL(clicked()), this, SLOT(previewReport()), Qt::UniqueConnection);
}

/*!
       \fn bool QcjDataReport::setReportFileName(QString fn, QMap<QString, QString> args)
       
       This  member  function  sets  the  XML  file to be read for the
       report  definition.  If  the  parameter \a args is an empty map
       (has  zero elements), any arguments required by the report will
       be  derived  from the report definition and appropriate widgets
       will be placed on the dialog to accept the user's input.

       If the parameter \a args has elements in it, they will be used to
       as the input to the report
*/
bool QcjDataReport::setReportFileName(QString fn, QMap<QString, QString> args)
{
   int row;

   printf("QcjDataReport::setReportFileName(): Enter, fn = |%s|\n", qPrintable(fn));
   qDebug() << "Enter, fn = " << fn << ", args = " << args;
   fflush(stdout);
   if ( report != NULL ) 
      delete report;
   printf("QcjDataReport::setReportFileName(): Creating report Document\n");
   fflush(stdout);
   report = new QcjDataReportDocument(this, fn);
   printf("QcjDataReport::setReportFileName(): Parse Errors = |%s|\n", qPrintable(report->errorString()));
   fflush(stdout);
   if ( report->errorString().size() > 0 ) 
   {
      QMessageBox::critical(NULL, "Report Parser", report->errorString());
      return(false);
   }

   if ( args.size() == 0 ) 
   {
      printf("QcjDataReport::setReportFileName(): Getting Report Arguments\n");
      fflush(stdout);
      QVector<QString> rptArgs;
      rptArgs = report->getRequiredArguments();
      QVectorIterator<QString> it1(rptArgs);
      printf("QcjDataReport::setReportFileName(): Building dialog\n");
      fflush(stdout);
      QGridLayout *gridLayout = new QGridLayout(ui.inputFrame);
      for ( row = 0; it1.hasNext(); row++) 
      {
         QString name = it1.next();
         printf("QcjDataReport::setReportFileName(): Getting details for argument |%s|\n", qPrintable(name));
         fflush(stdout);
         QStringList parms = report->getArgumentDetails(name);
         printf("QcjDataReport::setReportFileName(): Got them\n");
         fflush(stdout);
         if ( parms.count() >= 4 ) 
         {
            QString name      = parms[0];
            QString label     = parms[1];
            QString type      = parms[2];
            QString defvalue  = parms[3];

            QString name_type = "report::" + name + "::" + type;
            QLabel *lbl = new QLabel(label, ui.inputFrame);
            lbl->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);
            printf("QcjDataReport::setReportFileName(): Adding label to Layout\n");
            fflush(stdout);
            ((QGridLayout*)gridLayout)->addWidget(lbl, row, 0);
            printf("QcjDataReport::setReportFileName(): done\n");
            fflush(stdout);

            printf("QcjDataReport::setReportFileName(): name: |%s|, label = |%s|, type = |%s|\n", qPrintable(name), qPrintable(label), qPrintable(type));
            if ( type.toLower() == "lineedit" || type.toLower() == "password") 
            {
               printf("QcjDataReport::setReportFileName(): creating lineedit\n");
               fflush(stdout);
               QLineEdit *wdt = new QLineEdit(ui.inputFrame);
               if ( type.toLower() == "password") 
                  wdt->setEchoMode(QLineEdit::Password);
               printf("QcjDataReport::setReportFileName(): creating lineedit\n");
               fflush(stdout);
               wdt->setObjectName(name_type);
               wdt->setText(defvalue);
               ((QGridLayout*)gridLayout)->addWidget(wdt, row, 1);
            }
            else if ( type.toLower() == "double" ) 
            {
               QcjDoubleEntry *wdt = new QcjDoubleEntry(ui.inputFrame);
               wdt->setObjectName(name_type);
               wdt->setLabel(label);
               wdt->setText(defvalue);
               QString min = parms[4];
               QString max = parms[5];
               if ( min != "min" ) 
                  wdt->setMin(min.toDouble());
               if ( min != "max" ) 
                  wdt->setMin(max.toDouble());
               ((QGridLayout*)gridLayout)->addWidget(wdt, row, 1);
            }
            else if ( type.toLower() == "integer" ) 
            {
               QcjIntegerEntry *wdt = new QcjIntegerEntry(ui.inputFrame);
               wdt->setObjectName(name_type);
               wdt->setLabel(label);
               wdt->setText(defvalue);
               QString min = parms[4];
               QString max = parms[5];
               if ( min != "min" ) 
                  wdt->setMin(min.toInt());
               if ( min != "max" ) 
                  wdt->setMin(max.toInt());
               ((QGridLayout*)gridLayout)->addWidget(wdt, row, 1);
            }
            else if ( type.toLower() == "yesno" ) 
            {
               QcjYesNoEntry *wdt = new QcjYesNoEntry(ui.inputFrame);
               wdt->setObjectName(name_type);
               wdt->setLabel(label);
               wdt->setText(defvalue);
               ((QGridLayout*)gridLayout)->addWidget(wdt, row, 1);
            }
            else if ( type.toLower() == "truefalse" ) 
            {
               QcjTrueFalse *wdt = new QcjTrueFalse(ui.inputFrame);
               wdt->setObjectName(name_type);
               wdt->setLabel(label);
               wdt->setText(defvalue);
               ((QGridLayout*)gridLayout)->addWidget(wdt, row, 1);
            }
            else if ( type.toLower() == "date" ||
                      type.toLower() == "monthend" ||
                      type.toLower() == "monthbeg" ||
                      type.toLower() == "yearend" ||
                      type.toLower() == "yearbeg" ) 
            {
               QcjDateEntry *wdt;
               if ( type == "monthbeg" ) 
                  wdt = new QcjDateEntry(QcjDateEntry::MonthBeg, ui.inputFrame);
               else if ( type == "monthend" ) 
                  wdt = new QcjDateEntry(QcjDateEntry::MonthEnd, ui.inputFrame);
               else if ( type == "yearbeg" ) 
                  wdt = new QcjDateEntry(QcjDateEntry::YearBeg, ui.inputFrame);
               else if ( type == "yearend" ) 
                  wdt = new QcjDateEntry(QcjDateEntry::YearEnd, ui.inputFrame);
               else
                  wdt = new QcjDateEntry(ui.inputFrame);
               wdt->setObjectName(name_type);
               if ( defvalue.toLower() == "today" || defvalue.toLower() == "now" ) 
               {
                  wdt->setDate(QDate::currentDate());
               }
               if ( defvalue.toLower() == "tomorrow" ) 
               {
                  wdt->setDate(QDate::currentDate().addDays(1));
               }
               if ( defvalue.toLower() == "yesterday" ) 
               {
                  wdt->setDate(QDate::currentDate().addDays(-1));
               }
               ((QGridLayout*)gridLayout)->addWidget(wdt, row, 1);
            }
         }
      }
   }
   else
   {
      inputArgs = args;
   }
   return(true);
}

/*!
       \fn void QcjDataReport::printReport()
       
       This  member  function  presents the user with a printer dialog
       then  prints  the  report  using the selections provided by the
       user.
*/ 
void QcjDataReport::printReport()
{
   printf("QcjDataReport::printReport(): Enter\n");
   fflush(stdout);

   QTextEdit *editor = static_cast<QTextEdit*>(ui.textBrowser);
   QTextDocument *document = editor->document();
   QPrinter printer;
   printf("QcjDataReport::printReport(): document page height = %f, width = %f\n", document->pageSize().height(), document->pageSize().width());
   printf("QcjDataReport::printReport(): document height = %f, width = %f\n", document->size().height(), document->size().width());
   fflush(stdout);
   printer.setFullPage(true);

   QPrintDialog *dialog = new QPrintDialog(&printer, this);
   dialog->setWindowTitle(tr("Print Document"));

   if (dialog->exec() != QDialog::Accepted)
      return;

   printf("QcjDataReport::printReport(): printer page rect = %d, width = %d\n", printer.pageRect().height(), printer.pageRect().width());
   printf("QcjDataReport::printReport(): printer paper rect height = %d, width = %d\n", printer.paperRect().height(), printer.paperRect().width());

   printf("QcjDataReport::printReport(): printing document\n");
   fflush(stdout);
   document->print(&printer);

   printf("QcjDataReport::printReport(): Exit\n");
   fflush(stdout);
}

/*!
       \fn void QcjDataReport::previewReport()
       
       This  mmeber function builds the report using any of the user's
       inputs   provided   and   presents   the   final  report  in  a
       QTextBrowser. It also enables the report to be printed.
*/ 
void QcjDataReport::previewReport()
{
   QRegExp re("^report::.*");
   QFrame *wframe;
   QString value;
   QMap<QString, QString> inputValues;

   if ( inputArgs.size() == 0 ) 
   {
      printf("QcjDataReport::previewReport(): Enter\n");
      if ((wframe = QObject::findChild<QFrame*>(QString("inputFrame"))) != NULL) 
      {
         QList<QObject*> childlist = wframe->children();
         QListIterator<QObject*> it(childlist);
         QObject *obj;

         printf("QcjDataReport::previewReport(): Found widget frame\n");
         fflush(stdout);
         while (it.hasNext()) 
         {
            obj = it.next();
            QString object = obj->objectName();
            printf("QcjDataReport::previewReport(): found child widget name = |%s|\n", qPrintable(obj->objectName()));
            if ( ! object.contains(re) ) 
               continue;
            QStringList parmdef = object.split("::");
            QString name = parmdef[1];
            QString type = parmdef[2];
            printf("QcjDataReport::previewReport(): Saving parm name = |%s|, type = |%s|\n", qPrintable(name), qPrintable(type));
            fflush(stdout);
            if ( type.toLower() == "lineedit" || type.toLower() == "password") 
               value = ((QcjSingleLineEntry*)obj)->text();
            else if ( type.toLower() == "double" ) 
               value = ((QcjDoubleEntry*)obj)->text();
            else if ( type.toLower() == "integer" ) 
               value = ((QcjIntegerEntry*)obj)->text();
            else if ( type.toLower() == "yesno" ) 
               value = ((QcjYesNoEntry*)obj)->text();
            else if ( type.toLower() == "truefalse" ) 
               value = ((QcjTrueFalse*)obj)->text();
            else if ( type.toLower() == "date" ||
                      type.toLower() == "monthend" ||
                      type.toLower() == "monthbeg" ||
                      type.toLower() == "yearend" ||
                      type.toLower() == "yearbeg" ) 
               value = ((QcjDateEntry*)obj)->get();
            printf("QcjDataReport::previewReport(): Saving parm name = |%s|, value = |%s|\n", qPrintable(name), qPrintable(value));
            fflush(stdout);
            inputValues[name] = value;
         }
      }
   }
   else 
   {
      inputValues = inputArgs;
   }
   printf("QcjDataReport::previewReport(): Generating report\n");
   fflush(stdout);
   bool error_occured;

   QString reportStr = report->generateReport(inputValues, &error_occured);
   ui.textBrowser->setCurrentFont(report->getFontDefinition());

   if ( error_occured ) 
   {
      QMessageBox::critical(NULL, "Report Execution Error", report->errorString());
   }
   else 
   {
      printf("QcjDataReport::previewReport(): Font string: %s\n", qPrintable(ui.textBrowser->font().toString()));
      printf("QcjDataReport::previewReport(): Got report = \n|%s|\n", qPrintable(reportStr));
      fflush(stdout);
      if ( ui.plainTextCheckbox->isChecked() ) 
         ui.textBrowser->setPlainText(reportStr);
      else
         ui.textBrowser->setHtml(reportStr);
      ui.printBtn->setEnabled(true);
   }
}

QString QcjDataReport::findReportsPath(QStringList app_path, QString app_name)
{
   QString home, rv;
#ifndef QT4_DESIGNER_PLUGIN
   QDir dn;
   QStringList filenames;

   home = getenv("HOME");

   QString configs = home + "/." + app_name + "/reports;";
   configs += app_path.join("/") + "/reports;";
   configs += app_path.join("/") + "/../reports;";
   configs += "/opt/" + app_name + "/reports;";
   configs += app_path.join("/");

   filenames = configs.split(";");
   for ( QStringList::Iterator it = filenames.begin(); it != filenames.end(); ++it ) {
      printf("QcjDataReport::findReportsPath(): testing for reports directory |%s|\n", qPrintable(*it));
      dn.setPath(*it);
      if ( dn.isReadable() ) 
      {
         rv = *it;
         break;
      }
   }
#endif
   printf("QcjDataReport::findReportsPath(): returing path |%s|\n", qPrintable(rv));
   return(rv);
}


