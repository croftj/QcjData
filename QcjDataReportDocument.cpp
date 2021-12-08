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
# include <QApplication>
# include <QDir>
# include <QDomDocument>
# include <QFile>
# include <QtGui>
# include <QMessageBox>
# include <QSqlError>
# include <QSqlField>
# include <QSqlQuery>
# include <QSqlRecord>
# include <QString>
# include <QStringList>
# include "QcjData/QcjDataReportDocument.h"
# include "QcjData/QcjDataStatics.h"

/*!
       \class QcjDataReportDocument
       
       \brief  The  QcjDataReportDocument  class  parses an XML report
       definition and builds an html representation of it.

       This class parses and XML report definition and buulds an html report from it. In doing so it will
       execute the appropriate sql queries to retrieve the data from the database. 
*/ 

/*!
       \fn QcjDataReportDocument::QcjDataReportDocument(QObject *parent, QString report_fn)
       
       This  is  the  constructor  for  the  class.  The  parameter \a
       report_fn  is  the  name  of  the  XML  file  to  parse for the
       definition  of the report. If any errors are encountered in the
       file they will be saved in the variable errorStr which can then
       be  retrieved  using the member function errorString(). A non 0
       length  string  returned  by  the function errorSrting() is the
       only indication of an error that will be made.
*/ 
QcjDataReportDocument::QcjDataReportDocument(QObject *parent, QString report_fn) : QTextDocument(parent)
{
   int line, col;
   QString err;

   errorStr.clear();
   haveError = false;

   QDomDocument doc( "report" );
   def = doc;
   fieldFormatRE.setPattern("::(field|config|input):(date|mon|month|dow|dayofweek|string|text|integer|int|money|float|truefalse|yesno):([a-z0-9_.]+):");
   QFile file( report_fn );
   if ( !file.open( QIODevice::ReadOnly ) )
   {
      
//      printf("QcjDataXML::QcjDataXML(): Could not open file\n");

      errorStr = "Error opening report: " + report_fn + "The error reported as: " + file.errorString();
      return;
   }
   if ( !def.setContent( &file, &err, &line, &col ) ) {
//      printf("QcjDataXML::QcjDataXML(): Error in setContent file\n");
      file.close();
      errorStr = "Error parsing report: " + report_fn + "The error reported as: " + err + " found on line: " + QString::number(line) + ", column: " + QString::number(col);
      return;
   }
//   printf("QcjDataXML::QcjDataXML(): Exit normaly\n");
   file.close();
}

/*!
       \fn QMap<QString, QString> QcjDataReportDocument::getReportList(QString dir_path)
       
       This  member  function  will  return  a map of the names of the
       reports  found  in  the  directory  named  by  the parameter \a
       dir_path. Any file ending with the extension \a .xml or \a .rpt
       will   be  looked  at.  If  it  has  a  top  level  element  of
       <em>%lt;report%gt;</em>  it  will  be considered to be a report
       and  the name of the report and it's file name will be returned
       in  the map. The report name will be the key of the map and the
       filename will be the value portioon of the returned map.
*/ 
QMap<QString, QString> QcjDataReportDocument::getReportList(QString dir_path)
{
   QMap<QString, QString> rv;
   int line, col;
   QString err;
   QDir dir;

   if ( ! dir.exists(dir_path) ) 
   {
      QMessageBox::critical(NULL, "Report Parser", "Directory path '" + dir_path + "' does not exist");
      return(rv);
   }

   dir.setFilter(QDir::Files);
   QStringList filters;
   filters << "*.xml";
   filters << "*.rpt";
   dir.setPath(dir_path);
   QFileInfoList reports = dir.entryInfoList();
   dir.setNameFilters(filters);
   for (int i = 0; i < reports.size(); ++i) 
   {
      QString error_str;

      QFileInfo fileInfo = reports.at(i);
      if ( fileInfo.suffix().toLower() == "xml" ||
           fileInfo.suffix().toLower() == "rpt" ) 
      {
         QString fn = fileInfo.absoluteFilePath();
         QDomDocument doc( "report" );
         QFile file( fn );

         if ( !file.open( QIODevice::ReadOnly ) )
            continue;

         if ( !doc.setContent( &file, &err, &line, &col ) ) {
            file.close();
            error_str = "Error parsing report doc: " + fn + "The error reported as: " + err + " found on line: " + QString::number(line) + ", column: " + QString::number(col);
            QMessageBox::critical(NULL, "Report Parser", error_str);
            continue;
         }
         file.close();
         QDomElement e = doc.documentElement();
         if ( !e.isNull() && e.tagName() == "report" && e.attribute("name") != QString() ) 
         {
            QString rptName = e.attribute("name");
            rv[rptName] = fn;
         }
      }
   }
   return(rv);
}


/*!
       \fn QVector<QString> QcjDataReportDocument::getRequiredArguments()
       
       This  member  function returns a vector containing the names of
       the arguments defined within the report.
*/ 
QVector<QString> QcjDataReportDocument::getRequiredArguments()
{
   QVector<QString> rv;

   QDomElement docElem = def.documentElement();
   QDomNode n = docElem.firstChild();
   while ( !n.isNull() ) 
   {
      QDomElement e = n.toElement(); // try to convert the node to an element.
      if ( !e.isNull() && 
           e.tagName() == "input" && 
           e.attribute("name") != QString::null )
      {
         rv << e.attribute("name");
      }
      n = n.nextSibling();
   }
   return(rv);
}

/*!
       \fn QStringList QcjDataReportDocument::getArgumentDetails(QString name)
       
       This member function returns a list of the details defining the
       input argument passed in by name with the parameter \a name.
       
       The list will hold the following details
       
       \li \c name The name of the argument. \li \c label The label to
       present  to  the user for the argument. \li \c type The type of
       the  argument. \li \c default The default value to be presented
       to  the  user. \li \c filter The file filter to be used for the
       argument type \a file \li \c min The Minimum value to allow for
       the  argument  types  \a  interger  or \a double \li \c max The
       Maximum value to allow for the argument types \a interger or \a
       double
       
       The list will only have the parameters \a filter, \a min and \a
       max when appropriate for the argument type.
*/ 
QStringList QcjDataReportDocument::getArgumentDetails(QString name)
{
   QStringList rv;
   
   QDomElement docElem = def.documentElement();
   QDomNode n = docElem.firstChild();
   while ( !n.isNull() ) 
   {
      QDomElement e = n.toElement(); // try to convert the node to an element.
      if ( !e.isNull() && e.tagName() == "input" && 
           e.attribute("name") != QString::null && 
           e.attribute("name") == name ) 
      {
         QString type = "";
         rv << e.attribute("name");

         if ( e.hasAttribute("label") )
            rv << e.attribute("label");
         else
            rv << QString("");

         if ( e.hasAttribute("type") ) 
         {
            type = e.attribute("type");
            rv << e.attribute("type");
         }
         else
            rv << QString("");

         if ( e.hasAttribute("default") ) 
            rv << e.attribute("default");
         else
            rv << QString("");

         if ( type == "file" ) 
         {
            if ( e.hasAttribute("filter") ) 
               rv << e.attribute("filter");
            else
               rv << QString("*");
         }
         else if ( type == "integer" || type == "double") 
         {
            if ( e.hasAttribute("min") ) 
               rv << e.attribute("min");
            else
               rv << QString("min");

            if ( e.hasAttribute("max") ) 
               rv << e.attribute("max");
            else
               rv << QString("max");
         }
         return(rv);
      }
      n = n.nextSibling();
   }
   return(rv);
}


QSqlQuery *QcjDataReportDocument::execDataSource(QString name)
{
   printf("QcjDataReportDocument::execDataSource(): Enter, name = |%s|\n", qPrintable(name));
   fflush(stdout);
   QDomElement docElem = def.documentElement();
   QDomNode n = docElem.firstChild();
   while ( !n.isNull() ) 
   {
      QDomElement e = n.toElement(); // try to convert the node to an element.
      printf("QcjDataReportDocument::execDataSource(): have element |%s| with name |%s|\n", qPrintable(e.tagName()), qPrintable(e.attribute("name")));
      fflush(stdout);
      if ( !e.isNull() && e.tagName() == "data_source" && 
           e.attribute("name") != QString::null && 
           e.attribute("name") == name ) 
      {
         printf("QcjDataReportDocument::execDataSource(): found desired datasource\n");
         fflush(stdout);
         QRegExp re("::(input|config|field):([a-z0-9_.]+):");
         if ( dataSources.contains(name) )
         {
            printf("QcjDataReportDocument::execDataSource(): Found existing data source\n");
            fflush(stdout);
            QSqlQuery *q = dataSources[name];
            dataSources.remove(name);
            if ( defaultDataSource == q ) 
            {
               printf("QcjDataReportDocument::execDataSource(): removing data source as default\n");
               fflush(stdout);
               defaultDataSource = NULL;
            }
            delete q;
         }
         printf("QcjDataReportDocument::execDataSource(): Creating new data source\n");
         fflush(stdout);
         QSqlQuery *q = new QSqlQuery;
         QDomNode cdata = n.firstChild();
         if ( cdata.isText() ) 
         {
            QString sql = cdata.nodeValue();
            printf("QcjDataReportDocument::execDataSource(): have sql of |%s|\n", qPrintable(sql));
            fflush(stdout);
            QStringList varNames;
            QStringList values;
            while ( re.indexIn(sql) >= 0 ) 
            {
               QString source = re.cap(1);
               QString varname = re.cap(2);
               QString value = getDataValue(source, varname);
               printf("QcjDataReportDocument::execDataSource(): source = |%s|\n", qPrintable(source));
               printf("QcjDataReportDocument::execDataSource(): varname = |%s|\n", qPrintable(varname));
               printf("QcjDataReportDocument::execDataSource(): value = |%s|\n", qPrintable(value));
               fflush(stdout);
               QString bindName = varname;
               bindName = bindName.replace(".", "_");
//               sql.replace(QString("::" + source + ":" + varname + ":"), QString(":" + bindName));
               sql.replace(QString("::" + source + ":" + varname + ":"), QString( "'" + value.replace("'", "''") + "'"));
//               varNames << QString(":" + bindName);
//               values << value;
            }
            printf("QcjDataReportDocument::execDataSource(): sql now looks like |%s|\n", qPrintable(sql));
            q->prepare(sql);
/*
            for (int i = 0; i < varNames.size(); ++i)
            {
               printf("QcjDataReportDocument::execDataSource(): binding value |%s| to var |%s|\n", qPrintable(values.at(i)), qPrintable(varNames.at(i)));
               fflush(stdout);
               q->bindValue(varNames.at(i), values.at(i));
            }
*/
            if ( ! q->exec() )
            {
               printf("QcjDataReportDocument::execDataSource(): Error occured |%s|, |%s|\n", qPrintable(q->lastError().driverText()), qPrintable(q->lastError().databaseText()));
               fflush(stdout);
               errorStr = "QcjDatabase Error: " + q->lastError().driverText() + " - " + q->lastError().databaseText();
               haveError = true;
               return(NULL);
            }
            printf("QcjDataReportDocument::execDataSource(): setting |%s| (0x%lx) for default data source \n", qPrintable(name), (unsigned long)q);
            dataSources[name] = q;
            defaultDataSource = q;
            if ( q->at() == QSql::BeforeFirstRow )
            {
               printf("QcjDataReportDocument::execDataSource(): Moving to first row of data\n");
               fflush(stdout);
               q->next();
            }
            return(q);
         }
      }
      n = n.nextSibling();
   }
   printf("QcjDataReportDocument::execDataSource(): Enter\n");
   fflush(stdout);
   return(NULL);
}

QString QcjDataReportDocument::getDataValue(QString source, QString name)
{
   QString rv;
   printf("QcjDataReportDocument::getDataValue(): Enter source = |%s|, name = |%s|\n", qPrintable(source), qPrintable(name));
   if ( source == "config" ) 
      rv = pConfig->value(name).toString();
   else if ( source == "input" )
      rv = inputValues[name];
   else if ( source == "field" )
   {
      QStringList sl = name.split(".");
      if ( sl.size() > 1 ) 
      {
         printf("QcjDataReportDocument::formatDataValue(): Testing for datasource |%s|\n", qPrintable(sl[0]));
         fflush(stdout);
         if ( dataSources.contains(sl[0]) ) 
         {
            printf("QcjDataReportDocument::getDataValue(): Getting value from data source |%s|\n", qPrintable(sl[0]));
            fflush(stdout);
            rv = dataSources[sl[0]]->record().value(sl[1]).toString();
         }
      }
      else 
      {
         printf("QcjDataReportDocument::formatDataValue(): Getting value from default datasource\n");
         fflush(stdout);
         rv = defaultDataSource->record().value(name).toString();
      }
   }
   printf("QcjDataReportDocument::getDataValue(): returning value |%s|\n", qPrintable(rv));
   fflush(stdout);
   return(rv);
}

QString QcjDataReportDocument::formatDataValue(QString field)
{
   printf("QcjDataReportDocument::formatDataValue(): Enter field = |%s|\n", qPrintable(field));
   QString rv;
   if ( fieldFormatRE.indexIn(field) == 0 ) 
   {
      printf("QcjDataReportDocument::formatDataValue(): have valid field spec\n");
      fflush(stdout);
      QVariant val;
      QString source = fieldFormatRE.cap(1);
      QString format = fieldFormatRE.cap(2);
      QString fullname = fieldFormatRE.cap(3);

      if ( source == "config" ) 
      {
         printf("QcjDataReportDocument::formatDataValue(): have config spec\n");
         fflush(stdout);
         val = pConfig->value(fullname).toString();
      }
      else if ( source == "input" ) 
      {
         printf("QcjDataReportDocument::formatDataValue(): have input spec\n");
         fflush(stdout);
         val = QVariant(inputValues[fullname]);
      }
      else if ( source == "field" ) 
      {
         printf("QcjDataReportDocument::formatDataValue(): have field spec\n");
         fflush(stdout);
         QStringList sl = fullname.split(".");
         QSqlQuery *q;
         QString field;
         if ( sl.count() > 1 ) 
         {
            printf("QcjDataReportDocument::formatDataValue(): using data source |%s|\n", qPrintable(sl[0]));
            fflush(stdout);
            q = dataSources[sl[0]];
            field = sl[1];
         }
         else
         {
            printf("QcjDataReportDocument::formatDataValue(): using default data source\n");
            fflush(stdout);
            q = defaultDataSource;
            field = sl[0];
         }
         printf("QcjDataReportDocument::formatDataValue(): fetching value for field |%s| from data source 0x%lx\n", qPrintable(field), (unsigned long)q);
         fflush(stdout);
         val = q->record().value(field);
         printf("QcjDataReportDocument::formatDataValue(): Got it!\n");
         fflush(stdout);
      }

      printf("QcjDataReportDocument::formatDataValue(): Formatting data\n");
      fflush(stdout);
      if ( format == "time" ) 
         rv = val.toTime().toString("hh:mm:ss A");
      if ( format == "time24" ) 
         rv = val.toTime().toString("HH:mm:ss");
      if ( format == "date" ) 
         rv = val.toDate().toString();
      if ( format == "datetime" ) 
         rv = val.toDateTime().toString();
      else if ( format == "string" || format == "text" )
         rv = val.toString();
      else if ( format == "mon" || format == "month" ) 
      {
         int m = val.toInt();
         switch (m) 
         {
            case 1:
               rv = (format == "mon") ? "Jan" : "January";
            case 2:
               rv = (format == "mon") ? "Feb" : "February";
            case 3:
               rv = (format == "mon") ? "Mar" : "March";
            case 4:
               rv = (format == "mon") ? "Apr" : "April";
            case 5:
               rv = (format == "mon") ? "May" : "May";
            case 6:
               rv = (format == "mon") ? "Jun" : "June";
            case 7:
               rv = (format == "mon") ? "Jul" : "July";
            case 8:
               rv = (format == "mon") ? "Aug" : "Aug";
            case 9:
               rv = (format == "mon") ? "Sep" : "September";
            case 10:
               rv = (format == "mon") ? "Oct" : "October";
            case 11:
               rv = (format == "mon") ? "Nov" : "Novemeber";
            case 12:
               rv = (format == "mon") ? "Dec" : "December";
            default:
               rv = val.toString();
         }
      }
      else if ( format == "integer" || format == "int")
         rv.setNum(val.toInt());
      else if ( format == "float" )
         rv.setNum(val.toDouble());
      else if ( format == "money" )
      {
         rv = QcjMoneyEdit::formatCurrency(val.toString()).rightJustified(12).replace(" ", "&nbsp; ");
      }
      else if ( format == "truefalse" || format == "yesno" )
      {
         QString val1 = val.toString();
         rv = ( val1 == "Y" || val1 == "y" || val1.toInt() > 0 ) ? "Yes" : "No";
      }
   }
   printf("QcjDataReportDocument::formatDataValue(): returning formatted value |%s|\n", qPrintable(rv));
   fflush(stdout);
   return(rv);
}

/*!
       \fn QString QcjDataReportDocument::generateReport(QMap<QString, QString> args, bool *error_occured)
       
       This  member  function  build  the  report  using the arguments
       passed in the map parameter \a args. If an error is encountered
       while  building  the  report,  the  parameter  pointed to by \a
       error_occured  will  be  set  to  \a true if the pointer is not
       NULL.
*/ 
QString QcjDataReportDocument::generateReport(QMap<QString, QString> args, bool *error_occured)
{
   QString bodyAttribs;
   QMap<QString, QString> attribMap;
   QString body;

   QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
   inputValues = args;
   haveError = false;
   if ( error_occured != NULL ) 
   {
      *error_occured = false;
   }
   printf("QcjDataReportDocument::generateReport(): page height = %f, width = %f\n", pageSize().height(), pageSize().width());
   printf("QcjDataReportDocument::generateReport(): height = %f, width = %f\n", size().height(), size().width());

   QDomElement docElem = def.documentElement();
   printf("QcjDataReportDocument::generateReport(): Document element named |%s|\n", qPrintable(docElem.tagName()));
   attribMap.insert("name", "");
   bodyAttribs = parseAttributes(docElem, &attribMap);
   body = "<html>";
   body += "<!-- process generate report enter -->\n";
   body += QString("<body " + bodyAttribs + ">\n");

   bool no_banner = false;
   QDomNode n = docElem.firstChild();
   while ( !n.isNull() ) 
   {
      QDomElement e = n.toElement(); // try to convert the node to an element.
      printf("QcjDataReportDocument::generateReport(): body: have element |%s|\n", qPrintable(e.tagName()));
      fflush(stdout);
      if ( !no_banner && !e.isNull() && e.tagName() == "banner" )
      {
         QMap<QString, QString> bannerAttribs;
         bannerAttribs.clear();
         bannerAttribs.insert("img", "");
         QString sectAttribs = parseAttributes(e, &bannerAttribs);
         body += "   <div>\n";
         body += "      <center>\n";
         body += "         <img src=\"" + bannerAttribs["img"] + "\"" + sectAttribs + " />\n";
         body += "      </center>\n";
         body += "   </div>\n";
         
      }
      if ( !e.isNull() && e.tagName() == "section" )
      {
         no_banner = true;
         printf("QcjDataReportDocument::generateReport(): found section\n");
         fflush(stdout);
         body += processSection(e);
      }
      printf("QcjDataReportDocument::generateReport(): moving to next node\n");
      fflush(stdout);
      n = n.nextSibling();
   }
   body += "</body>";
   body += "<!-- process generate report exit -->\n";
   body += "</html>";
//   setHtml(body);
   if ( error_occured != NULL ) 
   {
      *error_occured = haveError;
   }
   QApplication::restoreOverrideCursor();
   return(body);
//   return(toHtml("utf-8"));
}

QString QcjDataReportDocument::processSection(QDomElement elem)
{
   printf("QcjDataReportDocument::processSection(): found section\n");
   fflush(stdout);
   QString sectAttribs;
   QMap<QString, QString> attribMap;
   QString body = "";
   QSqlQuery *ds = 0;
   int cols = 0;

   attribMap.clear();
   attribMap.insert("mode", "single");
   attribMap.insert("data_source", "");
   attribMap.insert("font", "");
   attribMap.insert("color", "");
   attribMap.insert("size", "");
   sectAttribs = parseAttributes(elem, &attribMap);

   QMapIterator<QString, QString> it(attribMap);
   while (it.hasNext()) 
   {
      it.next();
      printf("QcjDataReportDocument::processSection(): attribMap: |%s| = |%s|\n", qPrintable(it.key()), qPrintable(it.value()));
      fflush(stdout);
 
   }

   if ( attribMap["data_source"].size() > 0 && 
        attribMap["mode"] == "multiple" ) 
   {
      ds = defaultDataSource;
      if ( ds->at() == QSql::BeforeFirstRow )
      {
         printf("QcjDataReportDocument::processSection(): Moving to first row of data\n");
         fflush(stdout);
         ds->next();
      }
   }

   body += "   <div " + sectAttribs + ">\n";
   if ( attribMap["font"].size() > 0 ||
        attribMap["size"].size() > 0 ||
        attribMap["color"].size() > 0 ) 
   {
      body += "<font ";
      if ( attribMap["font"].size() > 0  ) 
         body += "family=\"" + attribMap["font"] + "\" ";
      if ( attribMap["size"].size() > 0  ) 
         body += "size=\"" + attribMap["size"] + "\" ";
      if ( attribMap["color"].size() > 0  ) 
         body += "color=\"" + attribMap["color"] + "\" ";
      body += ">\n";
   }

   /***********************************************/
   /*   Now process the elements of the section   */
   /***********************************************/
   while (1) 
   {
      body += "<!-- process section enter -->\n";
//      body += "   <p style=\"margin-top: 10px;\">\n";
//      body += "<table border=\"3\" " + sectAttribs + ">\n";
      body += "<table " + sectAttribs + ">\n";
//      body += "      <tr>\n";

      QDomNode n = elem.firstChild();
      while ( !n.isNull() ) 
      {
         QDomElement e = n.toElement(); // try to convert the node to an element.
         printf("QcjDataReportDocument::processSection(): section: have child element |%s|\n", qPrintable(e.tagName()));
         fflush(stdout);

         if ( !e.isNull() && e.tagName() == "column" )
         {
            if ( cols == 0 ) 
            {
               body += "<tr>\n";
            }
            cols++;         
            body += "<!-- processSection(): have column -->\n";
            QMap<QString, QString> colAttribMap;
            colAttribMap.clear();
            colAttribMap.insert("data_source", "");
            colAttribs = parseAttributes(e, &colAttribMap);
//            body += "         <th " + colAttribs + ">\n";
            printf("QcjDataReportDocument::processSection(): Processsing column\n");
            fflush(stdout);
            body += processColumn(e);
//            body += "      </tr>\n";
//            body += "         </th>\n";
         }
         if ( !e.isNull() && e.tagName() == "banner" )
         {
            QMap<QString, QString> bannerAttribs;
            bannerAttribs.clear();
            bannerAttribs.insert("img", "");
            QString sectAttribs = parseAttributes(e, &bannerAttribs);
            body += "   <div>\n";
            body += "      <center>\n";
            body += "         <img src=\"" + bannerAttribs["img"] + "\"" + sectAttribs + " />\n";
            body += "      </center>\n";
            body += "   </div>\n";
            
         }
         else if ( e.tagName() == "body" )
         {
            body += "<!-- processSection(): have body -->";
//            body += "         <td>\n";
            printf("QcjDataReportDocument::processSection(): Processsing body\n");
            fflush(stdout);
            body += processBody(e);
//            body += "         </td>\n";
         }
         else if ( e.tagName() == "header" ||
                   e.tagName() == "footer" )
         {
            printf("QcjDataReportDocument::processSection(): Processsing header\n");
            fflush(stdout);
//***            body += "<th>\n";
            body += "<!-- processSection(): have header or footer -->\n";
//            body += "</tr><tr>\n";
//            body += "<td>\n";
            body += processHeaderFooter(e);
//            if ( e.tagName() == "header" )
//            {
//               body += "</td>\n";
//               body += "</tr>\n<tr>\n";
//            }
         }
         n = n.nextSibling();
      }
      printf("QcjDataReportDocument::processSection(): done processing section element\n");
      fflush(stdout);
//      body += "      </tr>\n";
      if ( cols > 0 ) 
      {
         if ( cols == 0 ) 
         {
            body += "<tr>\n";
         }
      }
      body += "</table>\n";
//      body += "   </p>\n";
      body += "<!-- process section exit -->\n";

      if ( ds != 0 && attribMap["mode"] == "multiple" ) 
      {
         printf("QcjDataReportDocument::processSection(): Moving to next record in data source using query |%s|\n", qPrintable(ds->executedQuery()));
         fflush(stdout);
         if ( ! ds->next() ) 
            break;
         cols = 0;
         printf("QcjDataReportDocument::processSection(): Have next record in data source\n");
         fflush(stdout);
      }
      else
      {
         printf("QcjDataReportDocument::processSection(): Not multiple mode for datasource, quitting\n");
         fflush(stdout);
         break;
      }
      printf("QcjDataReportDocument::processSection(): bottom of while\n");
      fflush(stdout);
   }
   if ( attribMap["font"].size() > 0 ||
        attribMap["size"].size() > 0 ||
        attribMap["color"].size() > 0 ) 
   {
      body += "</font>\n";
   }
   body += "   </div>\n";
   return(body);
}

QString QcjDataReportDocument::processColumn(QDomElement elem)
{
   
   printf("QcjDataReportDocument::processColumn(): Enter\n");
   fflush(stdout);
   QMap<QString, QString> attribMap;
   QString attribs;
   QString body = "";

   attribMap.insert("font", "");
   attribMap.insert("size", "");
   attribMap.insert("color", "");
   attribs = parseAttributes(elem, &attribMap);

   QMapIterator<QString, QString> it(attribMap);
   while (it.hasNext()) 
   {
      it.next();
      printf("QcjDataReportDocument::processColumn(): attribMap: |%s| = |%s|\n", qPrintable(it.key()), qPrintable(it.value()));
      fflush(stdout);
 
   }


   body += "<!-- process column enter -->\n";
//   body += "         <td>\n";
   if ( attribMap["font"].size() > 0 ||
        attribMap["size"].size() > 0 ||
        attribMap["color"].size() > 0 ) 
   {
      body += "<font ";
      if ( attribMap["font"].size() > 0  ) 
         body += "family=\"" + attribMap["font"] + "\" ";
      if ( attribMap["size"].size() > 0  ) 
         body += "size=\"" + attribMap["size"] + "\" ";
      if ( attribMap["color"].size() > 0  ) 
         body += "color=\"" + attribMap["color"] + "\" ";
      body += ">\n";
   }
   body += "<td " + attribs + ">\n";
//   body += "<table border=\"2\" " + attribs + ">\n";
   body += "<table width=\"100%\">\n";

   /***********************************************/
   /*   Now process the elements of the section   */
   /***********************************************/
   QDomNode n = elem.firstChild();
   while ( !n.isNull() ) 
   {
      QDomElement e = n.toElement(); // try to convert the node to an element.
      printf("QcjDataReportDocument::processColumn(): have child element |%s|\n", qPrintable(e.tagName()));
      fflush(stdout);

      if ( !e.isNull() && (e.tagName() == "header" || e.tagName() == "footer") )
      {
         printf("QcjDataReportDocument::processColumn(): Processsing header/footer\n");
         fflush(stdout);
         body += processHeaderFooter(e);
      }
      else if ( e.tagName() == "body" )
      {
         printf("QcjDataReportDocument::processColumn(): Processsing body\n");
         fflush(stdout);
         body += processBody(e);
      }
      n = n.nextSibling();
   }
   body += "      </table>\n";
   if ( attribMap["font"].size() > 0 ||
        attribMap["size"].size() > 0 ||
        attribMap["color"].size() > 0 ) 
   {
      body += "</font>\n";
   }
   body += "   </td>\n";
   body += "<!-- process column exit -->\n";
   return(body);
}

QString QcjDataReportDocument::processHeaderFooter(QDomElement elem)
{
   
   printf("QcjDataReportDocument::processHeaderFooter(): Enter\n");
   fflush(stdout);
   bool bold = false;
   QMap<QString, QString> attribMap;

   QString attribs;
   QString body = "";
   attribMap.insert("font", "");
   attribMap.insert("size", "");
   attribMap.insert("color", "");
   attribMap.insert("bold", "no");
   attribs = parseAttributes(elem, &attribMap);

   QMapIterator<QString, QString> it(attribMap);
   while (it.hasNext()) 
   {
      it.next();
      printf("QcjDataReportDocument::processHeaderFooter(): attribMap: |%s| = |%s|\n", qPrintable(it.key()), qPrintable(it.value()));
      fflush(stdout);
 
   }

   if ( attribMap["bold"] == "yes" ) 
      bold = true;

   body += "<!-- process header/footer enter -->\n";
   if ( elem.tagName() == "header" )
      body += "<thead>\n";
   else
      body += "<tfoot>\n";
   body += "<tr " + attribs + ">\n";
   body += "   <td " + attribs + ">\n";
   QDomNode cdata = elem.firstChild();
   if ( cdata.isText() ) 
   {
      QString row = cdata.nodeValue();
      printf("QcjDataReportDocument::processHeaderFooter(): Row = |%s|\n", qPrintable(row));
      while (fieldFormatRE.indexIn(row) >= 0) 
      {
         QString spec = fieldFormatRE.cap(1);
         QString type = fieldFormatRE.cap(2);
         QString name = fieldFormatRE.cap(3);
         QString field = "::" + spec + ":" + type + ":" + name + ":";
         printf("QcjDataReportDocument::processHeaderFooter(): Replacing field spec |%s| with |%s|\n", qPrintable(field), qPrintable(formatDataValue(field)));
         fflush(stdout);
         row.replace(field, formatDataValue(field));
      }
      printf("QcjDataReportDocument::processHeaderFooter(): Adding row to body |%s|\n", qPrintable(row));
      fflush(stdout);
      row.trimmed();
      if ( attribMap["font"].size() > 0 ||
           attribMap["size"].size() > 0 ||
           attribMap["color"].size() > 0 ) 
      {
         body += "<font ";
         if ( attribMap["font"].size() > 0  ) 
            body += "family=\"" + attribMap["font"] + "\" ";
         if ( attribMap["size"].size() > 0  ) 
            body += "size=\"" + attribMap["size"] + "\" ";
         if ( attribMap["color"].size() > 0  ) 
            body += "color=\"" + attribMap["color"] + "\" ";
         body += ">\n";
      }
      if ( bold ) 
         body += "<b>";
      body += row;
      if ( bold ) 
         body += "</b>";
      if ( attribMap["font"].size() > 0 ||
           attribMap["size"].size() > 0 ||
           attribMap["color"].size() > 0 ) 
      {
         body += "</font>\n";
      }
   }
   else 
   {
      printf("QcjDataReportDocument::processHeaderFooter(): Element not text, processing as body\n");
      body += processBody(elem);
   }
   body += "   </td>\n";
   body += "</tr>\n";
   if ( elem.tagName() == "header" )
      body += "</thead>\n";
   else
      body += "</tfoot>\n";
   printf("QcjDataReportDocument::processHeaderFooter(): Exit\n");
   fflush(stdout);
   body += "<!-- process header/footer exit -->\n";
   return(body);
}

QString QcjDataReportDocument::processBody(QDomElement elem)
{
   
   printf("QcjDataReportDocument::processBody(): Enter\n");
   fflush(stdout);
   QMap<QString, QString> attribMap;

   QString attribs;
   QString body = "";
   body += "<!-- process body  enter -->\n";

   attribMap.clear();
   attribMap.insert("mode", "multi");
   attribMap.insert("font", "");
   attribMap.insert("size", "");
   attribMap.insert("color", "");
   attribMap.insert("bold", "no");
   attribs = parseAttributes(elem, &attribMap);
   printf("QcjDataReportDocument::processBody(): Parsed attributes\n");
   fflush(stdout);

   QMapIterator<QString, QString> it(attribMap);
   while (it.hasNext()) 
   {
      it.next();
      printf("QcjDataReportDocument::processBody(): attribMap: |%s| = |%s|\n", qPrintable(it.key()), qPrintable(it.value()));
      fflush(stdout);
 
   }


   printf("QcjDataReportDocument::processBody(): found mode of |%s|\n", qPrintable(attribMap["mode"]));
   fflush(stdout);

   /***********************************************/
   /*   Start by processing the headers for each  */
   /*   field, this will set the field widths     */
   /***********************************************/
   if ( attribMap["font"].size() > 0 ||
        attribMap["size"].size() > 0 ||
        attribMap["color"].size() > 0 ) 
   {
      body += "<font ";
      if ( attribMap["font"].size() > 0  ) 
         body += "family=\"" + attribMap["font"] + "\" ";
      if ( attribMap["size"].size() > 0  ) 
         body += "size=\"" + attribMap["size"] + "\" ";
      if ( attribMap["color"].size() > 0  ) 
         body += "color=\"" + attribMap["color"] + "\" ";
      body += ">\n";
   }
//   body += "<table border=\"1\" " + attribs + ">\n";
//***   body += "   <tr>\n";

   body += "<tbody>\n";
   body += "<tr " + attribs + ">\n";
   QDomNode n = elem.firstChild();
   while ( ! n.isNull() ) 
   {
      printf("QcjDataReportDocument::processBody(): converting node to element \n");
      fflush(stdout);
      QDomElement e = n.toElement(); // try to convert the node to an element.
      if ( e.tagName() == "field" ) 
      {
         printf("QcjDataReportDocument::processBody(): processing field header\n");
         fflush(stdout);
         body += processFieldHeader(e);
         printf("QcjDataReportDocument::processBody(): processing field header, done\n");
         fflush(stdout);
      }
      printf("QcjDataReportDocument::processBody(): moving to next child node\n");
      fflush(stdout);
      n = n.nextSibling();
   }
   printf("QcjDataReportDocument::processBody(): Done with fields\n");
   fflush(stdout);

   body += "</tr>\n";

   printf("QcjDataReportDocument::processBody(): moving to first row if not there defaultDataSource = |%ld|\n", (long)defaultDataSource);
   fflush(stdout);
   if ( defaultDataSource->at() == QSql::BeforeFirstRow )
   {
      printf("QcjDataReportDocument::processBody(): Moving to first row of data\n");
      fflush(stdout);
      defaultDataSource->next();
   }
   while ( defaultDataSource->at() != QSql::AfterLastRow ) 
   {
      printf("QcjDataReportDocument::processBody(): Processing row of data, mode = |%s|\n", qPrintable(attribMap["mode"]));
      fflush(stdout);
      body += "<tr " + attribs + ">\n";
      n = elem.firstChild();
      while ( ! n.isNull() ) 
      {
         QDomElement e = n.toElement(); // try to convert the node to an element.
         body += processFieldData(e);
         n = n.nextSibling();
      }
      body += "</tr>\n";
      if ( attribMap["mode"] == "single" ) 
      {
         printf("QcjDataReportDocument::processBody(): Single mode, breaking out\n");
         fflush(stdout);
         break;
      }
      else
      {
         printf("QcjDataReportDocument::processBody(): Fetching next record from db\n");
         fflush(stdout);
         if ( ! defaultDataSource->next() )
            break;
      }
   }
//   body += "</table>\n";
   body += "</tbody>\n";
   if ( attribMap["font"].size() > 0 ||
        attribMap["size"].size() > 0 ||
        attribMap["color"].size() > 0 ) 
   {
      body += "</font>\n";
   }
   printf("QcjDataReportDocument::processBody(): Exit\n");
   fflush(stdout);
   body += "<!-- process body  exit -->\n";
   return(body);
}

QString QcjDataReportDocument::processFieldHeader(QDomElement elem)
{
   printf("QcjDataReportDocument::processFieldHeader(): Enter\n");
   fflush(stdout);
   QMap<QString, QString> attribMap;

   QString attribs;
   QString body = "";
   attribMap.clear();
   attribMap.insert("font", "");
   attribMap.insert("size", "");
   attribMap.insert("color", "");
   attribMap.insert("bold", "no");
   attribs = parseAttributes(elem, &attribMap);
   printf("QcjDataReportDocument::processFieldData(): Parsed attributes, have %d attribs\n", attribMap.size());
   fflush(stdout);

   QMapIterator<QString, QString> it(attribMap);
   while (it.hasNext()) 
   {
      it.next();
      printf("QcjDataReportDocument::processFieldHeader(): attribMap: |%s| = |%s|\n", qPrintable(it.key()), qPrintable(it.value()));
      fflush(stdout);
 
   }

   printf("QcjDataReportDocument::processFieldHeader(): found mode of |%s|\n", qPrintable(attribMap["mode"]));
   fflush(stdout);

   body += "<!-- process field header enter -->\n";
   QDomNode n = elem.firstChild();
   while ( ! n.isNull() ) 
   {
      QDomElement e = n.toElement(); // try to convert the node to an element.
      printf("QcjDataReportDocument::processFieldHeader(): have tag |%s|\n", qPrintable(e.tagName()));
      fflush(stdout);
      if ( e.tagName() == "header" ) 
      {
         QMap<QString, QString> dataAttribMap;
         dataAttribMap = attribMap;
         QString dataAttribs = parseAttributes(e, &dataAttribMap);
         body += "<td " + dataAttribs + ">\n";
         if ( dataAttribMap["font"].size() > 0 ||
              dataAttribMap["size"].size() > 0 ||
              dataAttribMap["width"].size() > 0 ||
              dataAttribMap["color"].size() > 0 ) 
         {
            body += "<font ";
            if ( dataAttribMap["font"].size() > 0  ) 
               body += "family=\"" + dataAttribMap["font"] + "\" ";
            if ( dataAttribMap["size"].size() > 0  ) 
               body += "size=\"" + dataAttribMap["size"] + "\" ";
            if ( dataAttribMap["width"].size() > 0  ) 
               body += "width=\"" + dataAttribMap["size"] + "\" ";
            if ( dataAttribMap["color"].size() > 0  ) 
               body += "color=\"" + dataAttribMap["color"] + "\" ";
            body += ">\n";
         }
         body += "<b>\n";
         body += e.text() + "\n";
         body += "</b>\n";
         if ( dataAttribMap["font"].size() > 0 ||
              dataAttribMap["size"].size() > 0 ||
              dataAttribMap["color"].size() > 0 ) 
         {
            body += "</font>\n";
         }
         body += "</td>\n";
      }
      n = n.nextSibling();
   }
   body += "<!-- process field header exit -->\n";
   printf("QcjDataReportDocument::processFieldHeader(): Exit\n");
   fflush(stdout);
   return(body);
}


QString QcjDataReportDocument::processFieldData(QDomElement elem)
{
   printf("QcjDataReportDocument::processFieldData(): Enter\n");
   fflush(stdout);
   bool bold = false;
   QMap<QString, QString> attribMap;

   QString attribs;
   QString body = "";
   attribMap.clear();
   attribMap.insert("font", "");
   attribMap.insert("size", "");
   attribMap.insert("color", "");
   attribMap.insert("bold", "no");
   attribs = parseAttributes(elem, &attribMap);
   printf("QcjDataReportDocument::processFieldData(): Parsed attributes, have %d attribs\n", attribMap.size());
   fflush(stdout);

   QMapIterator<QString, QString> it(attribMap);
   while (it.hasNext()) 
   {
      it.next();
      printf("QcjDataReportDocument::processFieldData(): attribMap: |%s| = |%s|\n", qPrintable(it.key()), qPrintable(it.value()));
      fflush(stdout);
 
   }

   if ( attribMap.contains("bold") ) 
   {
      printf("QcjDataReportDocument::processFieldData(): have bold attrib\n");
      if ( attribMap["bold"].toLower() == "yes" ) 
      {
         printf("QcjDataReportDocument::processFieldData(): bold attrib = yes\n");
         bold = true;
      }
   }

   printf("QcjDataReportDocument::processFieldData(): found mode of |%s|\n", qPrintable(attribMap["mode"]));
   fflush(stdout);

   body += "<!-- process field data enter -->\n";
   QDomNode n = elem.firstChild();
   while ( ! n.isNull() ) 
   {
      QDomElement e = n.toElement(); // try to convert the node to an element.
      printf("QcjDataReportDocument::processFieldData(): have tag |%s|\n", qPrintable(e.tagName()));
      fflush(stdout);
      if ( e.tagName() == "data" ) 
      {
         QString row = e.text();
         printf("QcjDataReportDocument::processFieldData(): type = |%d|\n", n.nodeType());
         printf("QcjDataReportDocument::processFieldData(): Row = |%s|\n", qPrintable(row));

         QMap<QString, QString> dataAttribMap;
         dataAttribMap = attribMap;
         dataAttribMap.insert("data_source", "");
         QString dataAttribs = parseAttributes(e, &dataAttribMap);
         body += "<td " + dataAttribs + ">\n";
         if ( dataAttribMap["font"].size() > 0 ||
              dataAttribMap["size"].size() > 0 ||
              dataAttribMap["color"].size() > 0 ) 
         {
            body += "<font ";
            if ( dataAttribMap["font"].size() > 0  ) 
               body += "family=\"" + dataAttribMap["font"] + "\" ";
            if ( dataAttribMap["size"].size() > 0  ) 
               body += "size=\"" + dataAttribMap["size"] + "\" ";
            if ( dataAttribMap["color"].size() > 0  ) 
               body += "color=\"" + dataAttribMap["color"] + "\" ";
            body += ">\n";
         }
         if ( bold ) 
            body += "<b>";
         
         while (fieldFormatRE.indexIn(row) >= 0) 
         {
            QString spec = fieldFormatRE.cap(1);
            QString type = fieldFormatRE.cap(2);
            QString name = fieldFormatRE.cap(3);
            QString field = "::" + spec + ":" + type + ":" + name + ":";
            printf("QcjDataReportDocument::processFieldData(): Replacing field spec |%s| with |%s|\n", qPrintable(field), qPrintable(formatDataValue(field)));
            fflush(stdout);
            row.replace(field, formatDataValue(field));
         }
         printf("QcjDataReportDocument::processFieldData(): Adding row |%s| to body\n", qPrintable(row));
         fflush(stdout);
         row.trimmed();
         body += row + "\n";
         if ( bold ) 
            body += "</b>";
         if ( dataAttribMap["font"].size() > 0 ||
              dataAttribMap["size"].size() > 0 ||
              dataAttribMap["color"].size() > 0 ) 
         {
            body += "</font>\n";
         }
         body += "</td>";
      }
      n = n.nextSibling();
   }
   body += "<!-- process field data exit -->\n";
   printf("QcjDataReportDocument::processFieldData(): exit\n");
   fflush(stdout);
   return(body);
}

/**********************************************************************/
/*   This  functin  parses  out  the attributes of the element e and  */
/*   sets  the  data_source  if  any,  and  repackages the remaining  */
/*   attributes into a string.                                        */
/**********************************************************************/
QString QcjDataReportDocument::parseAttributes(QDomElement e, QMap<QString, QString> *params)
{
   QString rv;

   printf("QcjDataReportDocument::parseAttributes(): Enter\n");
   fflush(stdout);
   QDomNamedNodeMap sectAttribs = e.attributes();
   for (int x = 0; x < sectAttribs.count(); x++) 
   {
      QDomAttr attr = sectAttribs.item(x).toAttr();
      printf("QcjDataReportDocument::parseAttributes(): have attribute |%s|\n", qPrintable(attr.name()));
      fflush(stdout);
      if ( attr.name() == "data_source" ) 
      {
         printf("QcjDataReportDocument::parseAttributes(): Have data source of |%s|\n", qPrintable(attr.value()));
         fflush(stdout);
         if ( attr.value().size() > 0 ) 
         {
            params->insert(attr.name(), attr.value());
            QStringList dsNames = attr.value().split(",");
            for (int x = 0; x < dsNames.count(); x++) 
            {
               printf("QcjDataReportDocument::parseAttributes(): executing data source |%s|\n", qPrintable(dsNames.at(x)));
               fflush(stdout);
               execDataSource(dsNames.at(x));
            }
         }
      }
      else if ( params != NULL && params->contains(attr.name()) )
      {
         printf("QcjDataReportDocument::parseAttributes(): inserting |%s| into |%s|\n", qPrintable(attr.value()), qPrintable(attr.name()));
         fflush(stdout);
         params->insert(attr.name(), attr.value());
      }
      else 
      {
         printf("QcjDataReportDocument::parseAttributes(): adding %s=\"%s\"\n", qPrintable(attr.name()), qPrintable(attr.value()));
         fflush(stdout);
         rv += QString(" " + attr.name() + "=\"" + attr.value() + "\"");
      }
   }
   printf("QcjDataReportDocument::parseAttributes(): Exit\n");
   fflush(stdout);
   return(rv);
}
