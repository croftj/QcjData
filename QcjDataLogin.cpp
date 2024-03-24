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

#include <qmessagebox.h>
#include <qsettings.h>
#include <QDebug>
#include <QSqlDriver>
#include <QSqlError>
#include <QSqlQuery>
#include "QcjData/QcjDataLogin.h"

extern QSqlDatabase db;
extern QSqlDatabase *pDb;
extern QSettings *pConfig;


#define DEFAULT_DB_TYPE QPSQL
//#define DEFAULT_DB_TYPE QSQLITE

QcjDataLogin::QcjDataLogin(QWidget *p) : QDialog(p)
{
    ui.setupUi(this);
    connect(ui.ConnectBtn, SIGNAL(clicked()), this, SLOT(Login()));
    connect(ui.ExitBtn, SIGNAL(clicked()), this, SLOT(quitApp()));
    setModal(true);
}

void QcjDataLogin::Login()
{
#ifndef QT4_DESIGNER_PLUGIN 
   if ( ui.keepSettings->isChecked() ) 
   {
      printf("Saving login settings\n");
      pConfig->setValue("/DBHost", ui.HostEdit->text()); 
      pConfig->setValue("/DBPort", ui.PortEdit->text()); 
      pConfig->setValue("/DBUser", ui.UserEdit->text()); 
      pConfig->setValue("/DBName", ui.DBEdit->text()); 
      pConfig->setValue("/DBType", dbType); 
   }
   m_host = ui.HostEdit->text();
   m_port = ui.PortEdit->text(); 
   m_user = ui.UserEdit->text(); 
   m_dbName = ui.DBEdit->text(); 
   printf("QcjDataLogin::Login(): Setting parms\n");
   fflush(stdout);
//   db = QSqlDatabase::addDatabase("QOCI");
   db = QSqlDatabase::addDatabase(dbType);
   pDb = &db;

   printf("QcjDataLogin::Login(): Adding new Database\n");
   fflush(stdout);
   printf("QcjDataLogin::Login():Database name |%s|\n", (const char*)ui.DBEdit->text().toLocal8Bit());
   printf("QcjDataLogin::Login():Host |%s|\n", (const char*)ui.HostEdit->text().toLocal8Bit());
   printf("QcjDataLogin::Login():Port %d\n", ui.PortEdit->text().toInt());
   printf("QcjDataLogin::Login():User name |%s|\n", (const char*)ui.UserEdit->text().toLocal8Bit());
   printf("QcjDataLogin::Login():Password |%s|\n", (const char*)ui.PasswordEdit->text().toLocal8Bit());


   QString s = ui.UserEdit->text();
   QStringList sl = s.split("/");

   if ( sl.size() > 1 ) 
   {
      printf("QcjDataLogin::Login():Munged User name |%s|\n", (const char*)sl[0].toLocal8Bit());
      printf("QcjDataLogin::Login():Munged Password |%s|\n", (const char*)sl[1].toLocal8Bit());
      db.setUserName(sl[0]);
      db.setPassword(sl[1]);
   }
   else 
   {
      db.setUserName(ui.UserEdit->text());
      db.setPassword(ui.PasswordEdit->text());
   }

   db.setDatabaseName(ui.DBEdit->text());

   if ( ui.PortEdit->isEnabled() ) 
      db.setPort(ui.PortEdit->text().toInt());

   if ( ui.HostEdit->isEnabled() ) 
      db.setHostName(ui.HostEdit->text());


   printf("QcjDataLogin::Login(): Opening database\n");
   fflush(stdout);

   while (!db.open()) {
//      db.lastError().text("Error Connecting Database: ");
      
      printf("QcjDataLogin::Login(): Error with login\n");
      fflush(stdout);
      int resp =  QMessageBox::warning(NULL, "Error!", "Error Connecting to Database" +
                                            db.lastError().databaseText() + " - " + db.lastError().driverText(), "Retry", "Exit", "Ignore");
      if ( resp == 1 ) 
      {
         exit(0);
      }
      else if (resp == 2)
      {
         reject();
      }
      return;
   }
#endif
   printf("QcjDataLogin::Login(): Driver %s supoort transactions\n", (db.driver()->hasFeature(QSqlDriver::Transactions)) ? "does" : "DOES NOT");
   printf("QcjDataLogin::Login(): Driver %s supoort query size\n", (db.driver()->hasFeature(QSqlDriver::QuerySize)) ? "does" : "DOES NOT");
   printf("QcjDataLogin::Login(): Driver %s supoort blobs\n", (db.driver()->hasFeature(QSqlDriver::BLOB)) ? "does" : "DOES NOT");
   printf("QcjDataLogin::Login(): Driver %s supoort unicode\n", (db.driver()->hasFeature(QSqlDriver::Unicode)) ? "does" : "DOES NOT");
   printf("QcjDataLogin::Login(): Driver %s supoort prepared queries\n", (db.driver()->hasFeature(QSqlDriver::PreparedQueries)) ? "does" : "DOES NOT");
   printf("QcjDataLogin::Login(): Driver %s supoort named placeholders\n", (db.driver()->hasFeature(QSqlDriver::NamedPlaceholders)) ? "does" : "DOES NOT");
   printf("QcjDataLogin::Login(): Driver %s supoort positional placeholders\n", (db.driver()->hasFeature(QSqlDriver::PositionalPlaceholders)) ? "does" : "DOES NOT");
   printf("QcjDataLogin::Login(): Driver %s supoort last insert id\n", (db.driver()->hasFeature(QSqlDriver::LastInsertId)) ? "does" : "DOES NOT");
   accept();
}


void QcjDataLogin::initialize()
{
#ifndef QT4_DESIGNER_PLUGIN 
   ui.HostEdit->setText(pConfig->value("/DBHost", "localhost").toString());
   ui.PortEdit->setText(pConfig->value("/DBPort", "5432").toString());
   ui.UserEdit->setText(pConfig->value("/DBUser", "").toString());
   ui.DBEdit->setText(pConfig->value("/DBName", "database").toString());
   printf("QcjDataLogin::initialize(): default dbType |%s|\n", DEFAULT_DB_TYPE);
   dbType = pConfig->value("/DBType", DEFAULT_DB_TYPE).toString();

   if ( dbType == "QMYSQL" ) 
   {
      ui.PortEdit->setText("");
      ui.PortEdit->setEnabled(false);
   }
   else if ( dbType == "QOCI" ) 
   {
      ui.HostEdit->setText("");
      ui.HostEdit->setEnabled(false);
      ui.PortEdit->setText("");
      ui.PortEdit->setEnabled(false);
   }
   printf("QcjDataLogin::initialize(): dbType set to |%s|\n", (const char*)dbType.toLocal8Bit());
   fflush(stdout);
#endif
}

QSqlDatabase QcjDataLogin::database(QString connection_name)
{
   QSqlDatabase db_handle;

   if (connection_name == QString())
   {
      connection_name = "main";
   }
   qDebug() << "Adding database for connection " << connection_name;
   db_handle = QSqlDatabase::addDatabase(dbType, connection_name);
   db_handle.setUserName(ui.UserEdit->text());
   db_handle.setPassword(ui.PasswordEdit->text());
   db_handle.setDatabaseName(ui.DBEdit->text());
   if ( ui.PortEdit->isEnabled() ) 
   {
      db_handle.setPort(ui.PortEdit->text().toInt());
   }
   if ( ui.HostEdit->isEnabled() ) 
   {
      db_handle.setHostName(ui.HostEdit->text());
   }
   if (!db.open()) 
   {
      QString msg = QString("Cannot open database. Error %1")
                        .arg(db_handle.lastError().text());
      int resp =  QMessageBox::warning(NULL, "Error!", msg, "Exit");
      quitApp();
   }
   return(db_handle);
}

void QcjDataLogin::quitApp()
{
   exit(0);
}

