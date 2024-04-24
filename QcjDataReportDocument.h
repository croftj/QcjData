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

#ifndef DATAREPORTDOCUMENT_H
#define DATAREPORTDOCUMENT_H

# include <QDomDocument>
# include <QDomElement>
# include <QMap>
# include <QPrinter>
# include <QSqlQuery>
# include <QString>
# include <QTextEdit>

class QcjDataReportDocument : public QTextDocument
{
   Q_OBJECT

public:
   QcjDataReportDocument(QObject *pParent, QString report_fn);

   QStringList                getArgumentDetails(QString name);
   QFont                      getFontDefinition();
   QVector<QString>           getRequiredArguments();
   QString                    generateReport(QMap<QString, QString> args, bool *error_occured = NULL);
   
   /*!
          \fn QString errorString()
          
          This  member  function  returns  the error string created by
          parising the XML report definition.
   */ 
   QString                    errorString() 
                              {
                                 return(errorStr);
                              };

   static QMap<QString, QString> getReportList(QString dir_path);
//   static QString getReportLabel(QString name);

protected:
   QString                    processSection(QDomElement elem);
   QString                    processColumn(QDomElement elem);
   QString                    processFieldHeader(QDomElement e);
   QString                    processHeaderFooter(QDomElement elem);
   QString                    processBody(QDomElement elem);
   QString                    processFieldData(QDomElement elem);
   QString                    getDataValue(QString source, QString name);
   QSqlQuery                  *execDataSource(QString name);
   QString                    formatDataValue(QString field);
   QString                    parseAttributes(QDomElement e, 
                                              QMap<QString, QString> *params = NULL);
   QFont                      getFontDefinition(QDomElement docElem, QFont font);

private:
   bool                       haveError;
   QSqlQuery                  *defaultDataSource;
   QString                    errorStr;
   QString                    colAttribs;
   QRegExp                    fieldFormatRE;
   QMap<QString, QSqlQuery*>  dataSources;
   QMap<QString, QString>     inputValues;
   QDomDocument               def;
};

#endif
