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

#ifndef DATAXML_H
#define DATAXML_H

#include <qdom.h>
#include <qstring.h>
#include <qwidget.h>
#include <map>
#include <QSqlDatabase>
#include <QSqlRecord>
#include "QcjDataForm.h"

class QcjDataXML 
{
public:
   QcjDataXML(QString);
   bool                             confirmDelete(QString name, QcjDataForm *form);
   std::vector<struct QcjDataFields>   getFields(QString name, QWidget *parent);
   int                              getCols(QString name);
   QString        getConfigurationDefault(QString name);
   QStringList    getConfigurationDetails(QString config, QString name);
   QString        getConfigurationHelp(QString config);
   QStringList    getConfigurationItemNames(QString config);
   QString        getConfigurationName(int id);
   QStringList    getConfigurationNames();
   QString        getConfigurationTitle(QString config);

   /*!
          \fn QDomDocument *getDomDocument() 

          Returns  a  pointer  to  the  QDomDocument  object  for this
          object.
   */ 
   QDomDocument   *getDomDocument() 
   {
      return(&def);  
   };
   QString                 getFieldLabel(QString name, QString field);
   QString                 getFieldLabelColor(QString name);
   QString                 getFieldLabelBgColor(QString name);
   QStringList             getFieldNames(QString name);
   QString                 getFieldType(QString name, QString field);
   QString                 getFormTitle(QString name);
   QString                 getHelpBlock(QString blockName);
   int                     getHorzSpacing(QString name);
   QString                 getIndexField(QString name);
   QString                 getInsertQuery(QString name, QString index);
   int                     getLeftMargin(QString name);
   QString                 getNewIndex(QString name);
   QVariant                getResource(QString blockName);
   QMap<QString, QVariant> getResourceMap(QString blockName);
   QStringList             getResourceNames(QString group = QString::null);
   int                     getRows(QString name);
   QStringList             getSortFields(QString name);
   QString                 getTable(QString name);
   int                     getTopMargin(QString name);
   int                     getVertSpacing(QString name);
   QString                 getWhereClause(QString name, QSqlRecord *rec, QSqlDatabase *db);
   bool                    isAutoCommit(QString name);
   bool                    resourceHasProperties(QString blockName);
   void                    setConfigurationMenuId(QString config, int id);
   void                    setDatabase(QSqlDatabase *pdb)
                           {
                              db = pdb;
                           };
   QSqlDatabase            *database()
                           {
                              return(db);
                           };
private:
   QDomDocument def;
   QSqlDatabase *db;
   std::map<int, QString> Configurations;
protected:
};

#endif

