/***************************** ****************************************************
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
#include <QLocale>
#include <QSettings>
#include "QcjDataXML.h"
#include <stdlib.h>
#include <stdio.h>

/*!
      \file QcjDataStatics.h
        
       \brief Convenient  objects  to  be  used by the application. These are
       used by all of the QcjData Classes.

       This file has declarations for global variables which are used by all of the QcjData Classes 
       and the application as well.

       Two of the objects need to be initialized by the main application. 

       \li pFormDef Should point to the XML definition of the application.
       \li pConfig Should point to the QSettings object of the application.

      Here is a generic example of code to perform the initialization which can be used 
      in most applications.

      \code

      #include "QcjData/QcjDataStatics.h"
      #include "QcjData/QcjDataXML.h"

      QStringList appPath;
      QString appName;
      QString invoiceReportFn;
      QcjDataConfigure *pConfigDlg;
      QMap<int, QString> menuMap;

      int main( int argc, char ** argv )
      {
         QString reportPath;

         // Create the application object
         QApplication a( argc, argv );

         // Get it's path information
         appPath = a.arguments().at(0).split(QRegExp("[/\\\\]"));
         QFileInfo appInfo(a.arguments().at(0));

         // Get it's name
         appName = appInfo.baseName();

         // Get the directory where it resides
         appPath.removeLast();

         // Create a QSettings object to retrieve the configuration information from
         // Note the Company name, you will want to change this
         pConfig = new QSettings("CompanyName", appName);

         // Create the XML object searching the likely files where xml file may reside.
         pFormDef = new QcjDataXML(pConfig->readEntry("XMLConfig", QcjDataForm::findXML(appPath, appName)));

         // Create and initialize the QcjDataConfigure dialog which the user can configure the application with.
         pConfigDlg = new QcjDataConfigure;
         pConfigDlg->initialize("app");
         .
         .
         .
      \endcode

*/ 
extern QcjDataXML *pFormDef;
extern QSettings *pConfig;
extern QSqlDatabase db;
extern QSqlDatabase *pDb;
extern QLocale *pLocale;

