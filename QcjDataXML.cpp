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
#include <qfile.h>
#include <qlineedit.h>
#include <QDebug>
#include <QMessageBox>
#include <qregexp.h>
#include <qsettings.h>
#include <qtextedit.h>
#include <qvalidator.h>
#include <QSpinBox>
#include <QSqlField>
#include <QSqlDriver>
#include <QSqlQuery>
// #include "confdefs.h"
//#include <values.h>
#include "QcjDataXML.h"
#include "QcjDataStatics.h"
#include "QcjLib/WidgetUtils.h"

#ifndef MININT
#define MININT ((-2^32)+1)
#endif
#ifndef MAXINT
#define MAXINT ((2^32)-1)
#endif
#ifndef MINFLOAT
#define MINFLOAT -1.0e+99
#endif
#ifndef MAXFLOAT
#define MAXFLOAT 1.0e+99
#endif

/*!
       \class QcjDataXML
       
       \brief Object holding the xml definition for the application.
       
       The  QcjDataXML  class  parses  and retrieves variuous information
       needed  by the application which is defined in the XML document
       for the application.
       
       One  of these objects is created for the application and all of
       the components in the application will reference it to retrieve
       the required information from the document.
       
       In  most  cases,  a  parameter  called  <em>name</em>  will  be
       required  by  the member functions to determin which definition
       to  use  from  within the XML document. To reduce the amount of
       redundant  information in this document, the parameter will not
       be explained for each call.
*/ 

/*!
      \fn QcjDataXML::QcjDataXML(QString dfn)

       Opens  the XML file specified by the parameter <em>dfn</em> and
       parses  it.  If there are any errors in the document, a warning
       will  be presented to the user indicating the type of error and
       where it was encountered in the document.
*/ 
QcjDataXML::QcjDataXML(QString dfn)
{
#ifndef QT4_DESIGNER_PLUGIN
   QString err, error_str;
   int line, col;

//   printf("QcjDataXML::QcjDataXML(): Enter dfn = |%s|\n", qPrintable(dfn));
   QDomDocument doc( "cjiforms" );
   def = doc;
   QFile file( dfn );
   if ( !file.open( QIODevice::ReadOnly ) )
   {
      
   //      printf("QcjDataXML::QcjDataXML(): Could not open file\n");
      return;
   }
   if ( !def.setContent( &file, &err, &line, &col ) ) {
//      printf("QcjDataXML::QcjDataXML(): Error in setContent file\n");
      file.close();
      error_str = "Error parsing xml doc: " + dfn + "The error reported as: " + err + " found on line: " + QString::number(line) + ", column: " + QString::number(col);
      QMessageBox::critical(NULL, "XML Parser", error_str);
      exit(0);
   }
//   printf("QcjDataXML::QcjDataXML(): Exit normaly\n");
   file.close();
   db = 0;
   if ( pLocale == 0 ) 
      pLocale = new QLocale();
#endif
}

/*!
      \fn QcjDataFieldDef QcjDataXML::getFieldDefinition(QString xml_def, QString field_name)

       Iterates  through  the  xml  definition  searching for the form
       section  named by the parameter <em>name</em>. Once the section
       is  named, appropriate widgets will be created in the case of a
       form  and  a  vector of field definitions will be generated for
       use by the calling object.
       
       The generated vector will be returned to the caller.
*/ 
QcjDataFieldDef QcjDataXML::getFieldDef(QString xml_def, QString field_name) const
{
   QcjDataFieldDef field_def;
#ifndef QT4_DESIGNER_PLUGIN
   qDebug() << "Entry, looking for" << field_name;
   QDomElement docElem = def.documentElement();

   QDomNode n = docElem.firstChild();
   while ( !n.isNull() ) 
   {
      QDomElement e = n.toElement(); // try to convert the node to an element.
//      qDebug() << "have element type = " << e.tagName() << ", name = " << e.attribute("name");
      if ( !e.isNull() && e.tagName() == "form" && e.attribute("name") != QString() && e.attribute("name") == xml_def ) 
      {
         qDebug() << "Have found form!";
         QString focusName = e.attribute("focus");
         QString keyField = e.attribute("key_field");
         QString valueField = e.attribute("value_field");
         QDomNode n1 = n.firstChild();
         for ( int idx = 0; !n1.isNull(); idx++ ) 
         {
            QDomElement e1 = n1.toElement(); // try to convert the node to an element.
            qDebug() <<  "searching for field labeled: " << e1.attribute("label");
            if ( !e1.isNull() && e1.tagName() == "field" && e1.attribute("label") == field_name)
            {
               field_def.focusWidget = false;
               field_def.valueName = valueField;
               field_def.dataName = keyField;
               field_def.validator = nullptr;

               qDebug() << "attribute dataName = " << e1.attribute("dataName");
               field_def.dataName = e1.attribute("dataName");
               if ( field_def.dataName == focusName ) 
                  field_def.focusWidget = true;
               else
                  field_def.focusWidget = false;

               qDebug() << "attribute key = " << e1.attribute("key");
               field_def.key = e1.attribute("key");
               if ( field_def.key == focusName ) 
                  field_def.focusWidget = true;
               else
                  field_def.focusWidget = false;

               qDebug() << "attribute label = " << e1.attribute("label");
               field_def.label = e1.attribute("label");

               qDebug() << "attribute row = " << e1.attribute("row");
               field_def.row = e1.attribute("row").toInt();

               qDebug() << "attribute col = " << e1.attribute("col");
               field_def.col = e1.attribute("col").toInt();

               qDebug() << "attribute rowspan = " << e1.attribute("rowspan");
               field_def.rowSpan = e1.attribute("rowspan").toInt();

               qDebug() << "attribute colspan = " << e1.attribute("colspan");
               field_def.colSpan = e1.attribute("colspan").toInt();

               qDebug() << "attribute min_width = " << e1.attribute("min_width");
               field_def.minWidth = e1.attribute("min_width").toInt();

               qDebug() << "attribute max_width = " << e1.attribute("max_width");
               field_def.maxWidth = e1.attribute("max_width").toInt();

               qDebug() << "attribute read_only = " << e1.attribute("read_only");
               field_def.ro = e1.attribute("read_only").toInt();

               qDebug() << "attribute options = " << e1.attribute("options");
               field_def.options = e1.attribute("options");

               qDebug() << "attribute format = " << e1.attribute("format");
               field_def.format = e1.attribute("format");

               qDebug() << "attribute align = " << e1.attribute("align");
               field_def.align = e1.attribute("align");

               qDebug() << "attribute color = " << e1.attribute("color");
               field_def.color = e1.attribute("color");

               qDebug() << "attribute bgcolor = " << e1.attribute("bgcolor");
               field_def.bgcolor = e1.attribute("bgcolor");

               qDebug() << "attribute width = " << e1.attribute("width");
               field_def.width = e1.attribute("width");

               qDebug() << "attribute height = " << e1.attribute("height");
               field_def.height = e1.attribute("height");

               qDebug() << "attribute init = " << e1.attribute("init");
               field_def.init = e1.attribute("init");

               qDebug() << "attribute default = " << e1.attribute("default");
               field_def.defvalue = e1.attribute("default");

               qDebug() << "attribute search = " << e1.attribute("search");
               field_def.search = e1.attribute("search");

               qDebug() << "attribute type = " << e1.attribute("type");
               field_def.fieldType = e1.attribute("type");

               // No further need to go on!
               break;
            }
            n1 = n1.nextSibling();
         }
      }
      n = n.nextSibling();
   }
#endif
   return(field_def);
}

/*!
      \fn std::vector<struct QcjDataFields> QcjDataXML::getFields(QString name, QWidget *parent)

       Iterates  through  the  xml  definition  searching for the form
       section  named by the parameter <em>name</em>. Once the section
       is  named, appropriate widgets will be created in the case of a
       form  and  a  vector of field definitions will be generated for
       use by the calling object.
       
       The generated vector will be returned to the caller.
*/ 
std::vector<struct QcjDataFields> QcjDataXML::getFields(QString name, QWidget *parent)
{
   std::vector<struct QcjDataFields> rv;
#ifndef QT4_DESIGNER_PLUGIN
   struct QcjDataFields *p;
   qDebug() << "Entry, looking for" << name;
   QDomElement docElem = def.documentElement();

   QDomNode n = docElem.firstChild();
   while ( !n.isNull() ) 
   {
      QDomElement e = n.toElement(); // try to convert the node to an element.
//      qDebug() << "have element type = " << e.tagName() << ", name = " << e.attribute("name");
      if ( !e.isNull() && e.tagName() == "form" && e.attribute("name") != QString() && e.attribute("name") == name ) 
      {
//         qDebug() << "Have match!";
         QString focusName = e.attribute("focus");
         QString keyField = e.attribute("key_field");
         QString valueField = e.attribute("value_field");
//         printf("QcjDataXML::getFields():Found form definition e |%s|\n", qPrintable(e.attribute("name"));
         QDomNode n1 = n.firstChild();
         for ( int idx = 0; !n1.isNull(); idx++ ) 
         {
            QDomElement e1 = n1.toElement(); // try to convert the node to an element.
//            printf("QcjDataXML::getFields():Testing for dataName attribute in element of |%s|\n", qPrintable(e1.tagName()));
//            if ( !e1.isNull() && e1.tagName() == "field" && e.attribute("dataName") != NULL )
            if ( !e1.isNull() && e1.tagName() == "field" )
            {
//               printf("QcjDataXML::getFields():Found form definition e1 |%s|!\n", qPrintable(e1.tagName()));
               printf("QcjDataXML::getFields():Creating QcjDataField for |%s|!\n", qPrintable(e1.attribute("dataName")));
               p = new QcjDataFieldDef;
               p->focusWidget = false;
               p->valueName = valueField;
               p->dataName = keyField;
               p->validator = NULL;
               p->ro = false;
               if ( e1.attribute("dataName") != QString() ) 
               {
                  qDebug() << "attribute dataName = " << e1.attribute("dataName");
                  p->dataName = e1.attribute("dataName");
                  if ( p->dataName == focusName ) 
                     p->focusWidget = true;
                  else
                     p->focusWidget = false;
               }
               if ( e1.attribute("key") != QString() ) 
               {
//                  printf("QcjDataXML::getFields():Attribute dataName = |%s|\n", qPrintable(e1.attribute("dataName")));
                  fflush(stdout);
                  p->key = e1.attribute("key");
                  if ( p->key == focusName ) 
                     p->focusWidget = true;
                  else
                     p->focusWidget = false;
               }
               if ( e1.attribute("label") != QString() ) 
               {
                  qDebug() << "attribute label = " << e1.attribute("label");
                  p->label = e1.attribute("label");
               }
               if ( e1.attribute("row") != QString() ) 
               {
                  qDebug() << "attribute row = " << e1.attribute("row");
                  p->row = e1.attribute("row").toInt();
               }
               if ( e1.attribute("col") != QString() ) 
               {
                  qDebug() << "attribute col = " << e1.attribute("col");
                  p->col = e1.attribute("col").toInt();
               }
               if ( e1.attribute("rowspan") != QString() ) 
               {
                  qDebug() << "attribute rowspan = " << e1.attribute("rowspan");
                  p->rowSpan = e1.attribute("rowspan").toInt();
               }
               if ( e1.attribute("colspan") != QString() ) 
               {
                  qDebug() << "attribute colspan = " << e1.attribute("colspan");
                  p->colSpan = e1.attribute("colspan").toInt();
               }
               if ( e1.attribute("min_width") != QString() ) 
               {
                  qDebug() << "attribute min_width = " << e1.attribute("min_width");
                  p->minWidth = e1.attribute("min_width").toInt();
               }
               if ( e1.attribute("max_width") != QString() ) 
               {
                  qDebug() << "attribute max_width = " << e1.attribute("max_width");
                  p->maxWidth = e1.attribute("max_width").toInt();
               }
               if ( e1.attribute("read_only") != QString() ) 
               {
                  qDebug() << "attribute read_only = " << e1.attribute("read_only");
                  p->ro = e1.attribute("read_only").toInt();
               }
               if ( e1.attribute("options") != QString() ) 
               {
                  qDebug() << "attribute options = " << e1.attribute("options");
                  p->options = e1.attribute("options");
               }
               if ( e1.attribute("format") != QString() ) 
               {
                  qDebug() << "attribute format = " << e1.attribute("format");
                  p->format = e1.attribute("format");
               }
               if ( e1.attribute("align") != QString() ) 
               {
                  qDebug() << "attribute align = " << e1.attribute("align");
                  p->align = e1.attribute("align");
               }
               if ( e1.attribute("color") != QString() ) 
               {
                  qDebug() << "attribute color = " << e1.attribute("color");
                  p->color = e1.attribute("color");
               }
               if ( e1.attribute("bgcolor") != QString() ) 
               {
                  qDebug() << "attribute bgcolor = " << e1.attribute("bgcolor");
                  p->bgcolor = e1.attribute("bgcolor");
               }
               if ( e1.attribute("width") != QString() ) 
               {
                  qDebug() << "attribute width = " << e1.attribute("width");
                  p->width = e1.attribute("width");
               }
               if ( e1.attribute("height") != QString() ) 
               {
                  qDebug() << "attribute height = " << e1.attribute("height");
                  p->height = e1.attribute("height");
               }
               if ( e1.attribute("default") != QString() ) 
               {
                  qDebug() << "attribute default = " << e1.attribute("default");
                  p->defvalue = e1.attribute("default");
               }
               if ( e1.attribute("init") != QString() ) 
               {
                  qDebug() << "attribute init = " << e1.attribute("init");
                  p->init = e1.attribute("init");
               }
               if ( e1.attribute("search") != QString() ) 
               {
                  qDebug() << "attribute search = " << e1.attribute("search");
                  p->search = e1.attribute("search");
               }
               if ( e1.attribute("type") != QString() ) 
               {
                  qDebug() << "attribute type = " << e1.attribute("type");
                  p->fieldType = e1.attribute("type");
               }
               if ( e1.hasAttribute("min") )
               {
                  qDebug() << "attribute min = " << e1.attribute("min");
                  p->minValue = e1.attribute("min");
               }

               if ( e1.hasAttribute("max") )
               {
                  qDebug() << "attribute max = " << e1.attribute("max");
                  p->maxValue = e1.attribute("max");
                  qDebug() << "attribute max = " << p->maxValue;
               }

               if ( e1.hasAttribute("decimals") )
               {
                  qDebug() << "attribute decimals = " << e1.attribute("decimals");
                  p->decimals = e1.attribute("decimals");
               }

               if ( e1.hasAttribute("step") )
               {
                  qDebug() << "attribute step = " << e1.attribute("step");
                  p->stepValue = e1.attribute("step");
               }

               if ( e1.hasAttribute("suffix") )
               {
                  qDebug() << "attribute suffix = " << e1.attribute("suffix");
                  p->suffix = e1.attribute("suffix");
               }

               if ( parent != NULL ) 
               {
                  QString type = e1.attribute("type");
                  printf("QcjDataXML::getFields():****** Attempting to create widget for type |%s|\n", qPrintable(type));
                  if (type == "lineedit")
                  {
                     printf("QcjDataXML::getFields():Creating QLineEdit\n");
                     fflush(stdout);
                     p->widget = new QLineEdit(parent);
                     if ( e1.hasAttribute("mask") )
                     {
                        printf("QcjDataXML::getFields():Setting mask to |%s|\n", qPrintable(e1.attribute("mask")));
                        ((QLineEdit*)p->widget)->setInputMask(e1.attribute("mask"));
                     }
                     if ( e1.hasAttribute("validate") )
                     {
                        printf("QcjDataXML::getFields():Setting validator to |%s|\n", qPrintable(e1.attribute("validate")));
                        QRegExpValidator *validator = new QRegExpValidator(QRegExp(e1.attribute("validate")), 0);
                        p->validator = validator;
                     }
                     if ( e1.hasAttribute("echo") )
                     {
                        QString echoMode = e1.attribute("echo");
                        QLineEdit::EchoMode mode = QLineEdit::Normal;
                        if ( echoMode.toLower() == "normal" || echoMode.toLower() == "yes") 
                           mode = QLineEdit::Normal;
                        else if ( echoMode.toLower() == "noecho" || echoMode.toLower() == "no" || echoMode.toLower() == "hide")
                           mode = QLineEdit::NoEcho;
                        else if ( echoMode.toLower() == "password" || echoMode.toLower() == "mask" || echoMode.toLower() == "obscure")
                           mode = QLineEdit::Password;

                        printf("QcjDataXML::getFields():Setting echo mode to %d\n", mode);
                        ((QLineEdit*)p->widget)->setEchoMode(mode);
                     }
                     p->propName = "text";
                  }

                  else if (type == "textedit")
                  {
                     printf("QcjDataXML::getFields():Creating QTextEdit\n");
                     fflush(stdout);
                     p->widget = new QcjTextBlockEdit(parent);
                     p->propName = "text";
                  }

                  else if (type == "strsel")
                  {
                     printf("QcjDataXML::getFields():Creating QcjStringSelect\n");
                     fflush(stdout);
                     p->init = e1.attribute("init");
                     p->widget = new QcjStringSelect(parent);
                     dynamic_cast<QcjStringSelect*>(p->widget)->initialize(p);
                     p->propName = "strsel";
                  }

                  else if (type == "sqlmodel")
                  {
//                     printf("QcjDataXML::getFields():Creating QcjStringSelect\n");
                     fflush(stdout);
                     p->init = "";
                     p->widget = new QcjSqlModel(parent);
                     p->propName = "sqlmodel";
                  }

                  else if (type == "integer")
                  {
                     printf("QcjDataXML::getFields():Creating Integer\n");
                     fflush(stdout);
//                     p->widget = new QLineEdit(parent);

                     p->widget = new QcjSpinBox(parent);
                     if ( e1.hasAttribute("min") )
                        ((QcjSpinBox*)p->widget)->setMinimum(e1.attribute("min").toInt());
                     else
                        ((QcjSpinBox*)p->widget)->setMaximum(MININT);
                     if ( e1.hasAttribute("max") )
                        ((QcjSpinBox*)p->widget)->setMaximum(e1.attribute("max").toInt());
                     else
                        ((QcjSpinBox*)p->widget)->setMaximum(MAXINT);
                     if ( e1.hasAttribute("suffix") )
                        ((QcjSpinBox*)p->widget)->setSuffix(e1.attribute("suffix"));
                     p->propName = "value";
                  }

                  else if (type == "double")
                  {
                     printf("QcjDataXML::getFields():Creating double\n");
                     fflush(stdout);

                     p->widget = new QcjDoubleSpinBox(parent);

                     if ( e1.hasAttribute("decimals") )
                     {
                        printf("DataXML::getFields():Setting decimals to |%d|\n", e1.attribute("decimals").toInt());
                        ((QcjDoubleSpinBox*)p->widget)->setDecimals(e1.attribute("decimals").toInt());
                     }
                     else
                        ((QcjDoubleSpinBox*)p->widget)->setDecimals(3);

                     if ( e1.hasAttribute("min") )
                     {
                        printf("DataXML::getFields():Setting min to |%f|\n", e1.attribute("min").toFloat());
                        ((QcjDoubleSpinBox*)p->widget)->setMinimum(e1.attribute("min").toFloat());
                     }
                     else
                        ((QcjDoubleSpinBox*)p->widget)->setMinimum(MINFLOAT);

                     if ( e1.hasAttribute("max") )
                     {
                        printf("DataXML::getFields():Setting max to |%f|\n", e1.attribute("max").toFloat());
                        ((QcjDoubleSpinBox*)p->widget)->setMaximum(e1.attribute("max").toFloat());
                     }
                     else 
                        ((QcjDoubleSpinBox*)p->widget)->setMaximum(MAXFLOAT);

                     if ( e1.hasAttribute("suffix") )
                        ((QcjDoubleSpinBox*)p->widget)->setSuffix(e1.attribute("suffix"));
            
                     ((QcjDoubleSpinBox*)p->widget)->setValue(0.0);
                     p->propName = "value";
                  }

                  else if (type == "yesno")
                  {
                     printf("QcjDataXML::getFields():Creating YesNo\n");
                     fflush(stdout);
                     p->widget = new QcjYesNoSelect(parent);
                     p->propName = "yesno";
                  }

                  else if (type == "truefalse")
                  {
                     printf("QcjDataXML::getFields():Creating TrueFalse\n");
                     fflush(stdout);
                     p->widget = new QcjTrueFalseSelect(parent);
                     p->propName = "truefalse";
                  }

                  else if (type == "checkbox")
                  {
                     printf("QcjDataXML::getFields():Creating Checkbox\n");
                     fflush(stdout);
                     QString style = "yesno";
                     if ( e1.hasAttribute("style") )
                        style = qPrintable(e1.attribute("style"));
                     if ( style.toLower() == "truefalse" ) 
                     {
                        printf("QcjDataXML::getFields(): creating tf checkbox\n");
                        p->widget = new QcjCheckBoxTF(parent);
                        p->propName = "checkboxtf";
                     }
                     else
                     {
                        printf("QcjDataXML::getFields(): creating yn checkbox\n");
                        p->widget = new QcjCheckBoxYN(parent);
                        p->propName = "checkboxyn";
                     }
                  }

                  else if (type == "money")
                  {
                     printf("QcjDataXML::getFields():Creating QcjMoneyEdit\n");
                     fflush(stdout);
                     printf("QcjDataXML::getFields():making widget\n");
                     fflush(stdout);
                     p->widget = new QcjMoneyEdit(parent);
                     printf("QcjDataXML::getFields():setting property\n");
                     fflush(stdout);
                     p->propName = "money";
                     printf("QcjDataXML::getFields():done\n");
                     fflush(stdout);
//                     p->propName = "fred";
                  }
                  else if (type == "phone")
                  {
                     printf("QcjDataXML::getFields():Creating QcjPhoneEntry\n");
                     fflush(stdout);
                     p->widget = new QcjPhoneEdit(parent);
                     p->propName = "phone";
                  }
                  else if (type == "date")
                  {
                     printf("QcjDataXML::getFields():Creating QcjDateEntry\n");
                     fflush(stdout);
                     p->widget = new QcjDateEntry(parent);
                     p->propName = "date";
                  }
                  else if (type == "time")
                  {
                     printf("QcjDataXML::getFields():Creating QcjTimeEntry\n");
                     fflush(stdout);
                     p->widget = new QcjTimeEntry(parent);
                     p->propName = "timestamp";
                  }
                  else if (type == "timestamp")
                  {
                     printf("QcjDataXML::getFields():Creating QcjTimestampEntry\n");
                     fflush(stdout);
                     p->widget = new QcjTimestampEntry(parent);
                     p->propName = "timestamp";
                  }

#ifndef NO_PHOTO_SELECT
                  else if (type == "photo")
                  {
                     printf("QcjDataXML::getFields():Creating PhotoEntry\n");
                     fflush(stdout);
                     p->widget = new QcjPhotoEntry(parent);
//                     p->widget->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);
                     p->propName = "photo";
                  }
#endif

                  else
                     printf("QcjDataXML::getFields():No such type defined!\n");
                  if ( p->widget != 0 ) 
                  {
//                     QPalette wPalette = p->widget->palette();
//                     wPalette.setColor(QPalette::Active, QPalette::Base, QColor("#8080ff"));
//                     p->widget->setPalette(wPalette);
                  }
               }
               printf("QcjDataXML::getFields():Assigning datafield to vector pos %d\n", idx);
               fflush(stdout);
               rv.push_back(*p);
               printf("QcjDataXML::getFields():Deleting datafield\n");
               fflush(stdout);
               delete p;
            }
//            printf("QcjDataXML::getFields():Moving to next datafield\n");
            fflush(stdout);
            n1 = n1.nextSibling();
//            printf("QcjDataXML::getFields():n1 = %d!\n", n1.isNull());
            fflush(stdout);
         }
         break;
      }
//      else if ( e.attribute("name") != QString()) 
//         printf("QcjDataXML::getFields():skipping form definition |%s|!\n", qPrintable(e.attribute("name")));
//      else
//         printf("QcjDataXML::getFields():skipping unnamed definition\n");
      n = n.nextSibling();
   }
   p = new struct QcjDataFields;
   p->dataName = QString("--ENDOFFIELDS--");
   rv.push_back(*p);
   delete p;
#endif
   return(rv);
}

/*!
      \fn QcjDataFieldMap QcjDataXml::getFieldsMap(QString xmldef, QWidget *parent)

       Calls getFields to return a vector of field definitions if found.
       It will then convert the vector of fields definitions into a QcjDataFieldMap
       with the key being the field name.
       
       Returns a QcjDataFieldMap of the field defintions.
*/ 
QcjDataFieldMap QcjDataXML::getFieldsMap(QString config, QWidget *parent)
{
   QcjDataFieldMap rv;
   QcjDataFieldStdVector fds = pFormDef->getFields(config, nullptr);

   qDebug() << "fds count: " << fds.size();
   for (unsigned int idx = 0; idx < fds.size(); idx++)
   {
      QcjDataFields field_def = fds.at(idx);
      rv.insert(field_def.dataName, field_def);
   }
   return(rv);
}

/*!
      \fn QString QcjDataXML::getTable(QString name)

       Returns  the name of the table for the definition named by the parameter <em>name</em>
*/ 
QString QcjDataXML::getTable(QString name)
{
   QString rv;
#ifndef QT4_DESIGNER_PLUGIN
   printf("QcjDataXML::getTable():Entry, looking for |%s|!\n", qPrintable(name));
   fflush(stdout);
   QDomElement docElem = def.documentElement();

   QDomNode n = docElem.firstChild();
   while ( !n.isNull() ) 
   {
      QDomElement e = n.toElement(); // try to convert the node to an element.
      if ( !e.isNull() && e.tagName() == "form" && e.attribute("name") != QString() && e.attribute("name") == name ) 
      {
//         printf("QcjDataXML::getTable():Found form definition e |%s|\n", qPrintable(e.attribute("name")));
         rv = e.attribute("table_name");
      }
//      else if ( e.attribute("name") != QString()) 
//         printf("QcjDataXML::getTable():skipping form definition |%s|!\n", qPrintable(e.attribute("name")));
//      else
//         printf("QcjDataXML::getTable():skipping unnamed definition\n");
      n = n.nextSibling();
   }
#endif
   printf("QcjDataXML::getTable():Exit\n");
   fflush(stdout);
   return(rv);
}

/*!
   \fn QMap<QString, QString> QcjDataXML::getTableNames(QString name)

   Returns a map with the dataName and label for each field of a table.
*/
QMap<QString, QString> QcjDataXML::getTableNames(QString name)
{
   QMap<QString, QString> rv;

#ifndef QT4_DESIGNER_PLUGIN
   qDebug() << __FUNCTION__ << "(): Entry, looking for " << name;
   QDomElement docElem = def.documentElement();

   QDomNode n = docElem.firstChild();
   while ( !n.isNull() ) 
   {
      QDomElement e = n.toElement(); // try to convert the node to an element.
      if ( !e.isNull() && e.tagName() == "form" && e.attribute("name") != QString() && e.attribute("name") == name ) 
      {
         printf("QcjDataXML::getTableNames():Found form definition e |%s|\n", qPrintable(e.attribute("name")));
         QDomNode n1 = n.firstChild();
         for ( int idx = 0; !n1.isNull(); idx++ ) 
         {
            QDomElement e1 = n1.toElement(); // try to convert the node to an element.
            printf("QcjDataXML::getTableNames():Testing for dataName attribute in element of |%s|\n", qPrintable(e1.tagName()));
//            if ( !e1.isNull() && e1.tagName() == "field" && e.attribute("dataName") != QString() )
            if ( !e1.isNull() && e1.tagName() == "field" )
            {
               printf("QcjDataXML::getTableNames(): Found form definition e1 |%s|!\n", qPrintable(e1.tagName()));
               if ( e1.attribute("dataName") != QString() ) 
               {
                  printf("QcjDataXML::getTableNames(): Attribute dataName = |%s|\n", qPrintable(e1.attribute("dataName")));
                  fflush(stdout);
                  QString data_name = e1.attribute("dataName");
                  QString label = e1.attribute("label");
                  rv.insert(data_name, label);
               }
            }
            printf("QcjDataXML::getTableNames():Moving to next datafield\n");
            fflush(stdout);
            n1 = n1.nextSibling();
            printf("QcjDataXML::getTableNames():n1 = %d!\n", n1.isNull());
            fflush(stdout);
         }
         break;
      }
      else if ( e.attribute("name") != QString()) 
         printf("QcjDataXML::getTableNames():skipping form definition |%s|!\n", qPrintable(e.attribute("name")));
      else
         printf("QcjDataXML::getTableNames():skipping unnamed definition\n");
      n = n.nextSibling();
   }
#endif
   qDebug() << "Exiting rv = " << rv;
   return(rv);
}

/*!
      \fn QString QcjDataXML::getNewIndex(QString name)

       Creates a new index value to be used in an insert statement for
       the table defined in the XML definition.
*/ 
QString QcjDataXML::getNewIndex(QString name)
{
   QString rv = QString();

#ifndef QT4_DESIGNER_PLUGIN
   printf("QcjDataXML::getNewIndex():Entry, looking for |%s|!\n", qPrintable(name));
   QDomElement docElem = def.documentElement();

   QDomNode n = docElem.firstChild();
   while ( !n.isNull() ) 
   {
      QDomElement e = n.toElement(); // try to convert the node to an element.
      if ( !e.isNull() && e.tagName() == "form" && e.attribute("name") != QString() && e.attribute("name") == name ) 
      {
         QDomNode n1 = n.firstChild();
         for ( int idx = 0; !n1.isNull(); idx++ ) 
         {
            QDomElement e1 = n1.toElement(); // try to convert the node to an element.
            if ( !e1.isNull() && e1.tagName() == "key_generator" )
            {
               QDomNode cdata = n1.firstChild();
               if ( cdata.isText() ) 
               {
                  QString sql = cdata.nodeValue();
                  QSqlQuery q1(sql);
                  if ( q1.isActive() )
                  {
                     printf("QcjDataXML::getNewIndex(): Have active query\n");
                     fflush(stdout);
                     if ( q1.next() ) 
                     {
                        rv = q1.value(0).toString();
                        break;
                     }
                  }
               }
            }
//            printf("QcjDataXML::getNewIndex():Moving to next datafield\n");
            fflush(stdout);
            n1 = n1.nextSibling();
//            printf("QcjDataXML::getNewIndex():n1 = %d!\n", n1.isNull());
            fflush(stdout);
         }
         break;
      }
//      else if ( e.attribute("name") != QString()) 
//         printf("QcjDataXML::getNewIndex():skipping form definition |%s|!\n", qPrintable(e.attribute("name")));
//      else
      n = n.nextSibling();
   }
#endif
   printf("QcjDataXML::getNewIndex(): Exit(%s)\n", qPrintable(rv));
   return(rv);
}

/*!
      \fn QString QcjDataXML::getInsertQuery(QString name, QString index)

       If  an  <em>nsert_query</em>  section  is found for a form, the
       text   for   that   tag   will   be   returned.  The  parameter
       <em>index</em>  will be inserted into the string where ever the
       string ":index:" is found.
*/ 
QString QcjDataXML::getInsertQuery(QString name, QString index)
{
   QString rv;

#ifndef QT4_DESIGNER_PLUGIN
   printf("QcjDataXML::getInsertQuery():Entry, looking for |%s|!\n", qPrintable(name));
   QDomElement docElem = def.documentElement();

   QDomNode n = docElem.firstChild();
   while ( !n.isNull() ) 
   {
      QDomElement e = n.toElement(); // try to convert the node to an element.
      if ( !e.isNull() && e.tagName() == "form" && e.attribute("name") != QString() && e.attribute("name") == name ) 
      {
         QDomNode n1 = n.firstChild();
         for ( int idx = 0; !n1.isNull(); idx++ ) 
         {
            QDomElement e1 = n1.toElement(); // try to convert the node to an element.
            if ( !e1.isNull() && e1.tagName() == "insert_template" )
            {
               QDomNode cdata = n1.firstChild();
               if ( cdata.isText() ) 
               {
                  rv = cdata.nodeValue();
                  QRegExp re(":index:");
                  while (rv.contains(re)) 
                     rv.replace(":index:", index);
                  break;
               }
            }
            printf("QcjDataXML::getInsertQuery():Moving to next datafield\n");
            fflush(stdout);
            n1 = n1.nextSibling();
//            printf("QcjDataXML::getInsertQuery():n1 = %d!\n", n1.isNull());
            fflush(stdout);
         }
         break;
      }
//      else if ( e.attribute("name") != QString()) 
//         printf("QcjDataXML::getInsertQuery():skipping form definition |%s|!\n", qPrintable(e.attribute("name")));
//      else
//         printf("QcjDataXML::getInsertQuery():skipping unnamed definition\n");
      n = n.nextSibling();
   }
#endif
   return(rv);
}

/*!
      \fn QString QcjDataXML::getIndexField(QString name)

       Returns  a string with the comma seperated names defined in the
       form as the index fields.
*/ 
QString QcjDataXML::getIndexField(QString name)
{
   QString rv;
#ifndef QT4_DESIGNER_PLUGIN
//   printf("QcjDataXML::getTable():Entry, looking for |%s|!\n", qPrintable(name));
   QDomElement docElem = def.documentElement();

   QDomNode n = docElem.firstChild();
   while ( !n.isNull() ) 
   {
      QDomElement e = n.toElement(); // try to convert the node to an element.
      if ( !e.isNull() && e.tagName() == "form" && e.attribute("name") != QString() && e.attribute("name") == name ) 
      {
//         printf("QcjDataXML::getTable():Found form definition e |%s|\n", qPrintable(e.attribute("name")));
         rv = e.attribute("index");
         break;
      }
//      else if ( e.attribute("name") != QString()) 
//         printf("QcjDataXML::getTable():skipping form definition |%s|!\n", qPrintable(e.attribute("name")));
//      else
//         printf("QcjDataXML::getTable():skipping unnamed definition\n");
      n = n.nextSibling();
   }
   if ( rv.size() == 0 ) 
   {
      if ( db == 0 ) 
         db = pDb;

      if ( db->driverName() == "QMYSQL" || db->driverName() == "QSQLITE" ) 
      {
         rv = "id";
      }
      else 
      {
         rv = "ident"; // getTable(name) + "_ident_seq";
      }
   }

#endif
   return(rv);
}

/*!
      \fn QString QcjDataXML::getWhereClause(QString name, QSqlRecord *rec, QSqlDatabase *db) 

       Returns  a  properly  formated  where  clause (without the word
       "where")   for   the   record   pointed  to  by  the  parameter
       <em>rec</em>  and  the  database  pointed  to  by the parameter
       <em>db</em>.
*/ 
QString QcjDataXML::getWhereClause(QString name, QSqlRecord *rec, QSqlDatabase *myDb) 
{
   QString where;

   QString index = getIndexField(name);
   printf("QcjDataXML::getWhereClause(): myDb = %ld\n", (long)myDb);
   fflush(stdout);
   if ( myDb == 0 ) 
      myDb = db;

   if ( index.size() == 0 ) 
   {

      if ( myDb->driverName() == "QMYSQL" || myDb->driverName() == "QSQLITE" ) 
      {
         index = "id";
      }
      else 
      {
         index = getTable(name);
      }
   }
   printf("QcjDataXML::getWhereClause():index = |%s|\n", qPrintable(index));
   fflush(stdout);
   QStringList fields = index.split(',');
   printf("QcjDataXML::getWhereClause():fields has %d elements\n", fields.size());
   fflush(stdout);
   for ( QStringList::Iterator it = fields.begin(); it != fields.end(); ++it ) {
      printf("QcjDataXML::getWhereClause():field = |%s|\n", qPrintable((*it)));
      fflush(stdout);
      QVariant v = rec->value((*it));
      printf("QcjDataXML::getWhereClause():value = |%s|\n", qPrintable(v.toString()));
      fflush(stdout);
      if ( v == QVariant::Invalid ) 
      {
         printf("QcjDataXML::getWhereClause(): Invalid value\n");
         fflush(stdout);
         if ( QMessageBox::warning(NULL, "Missing Field", "Field '" + v.toString() + "' is missing from the XML definition " + name,
                                          "Ignore", "Exit") == 1  ) 
            exit(0);
      }
      if ( where.size() > 0 ) 
         where += QString(" and ");
      printf("QcjDataXML::getWhereClause(): Fetching field from record\n");
      fflush(stdout);
      QSqlField f = rec->field((*it));
      printf("QcjDataXML::getWhereClause(): myDb = %ld\n", (long)myDb);
      fflush(stdout);
      printf("QcjDataXML::getWhereClause(): Formatting value of field, driver = %ld\n", (long)myDb->driver());
      fflush(stdout);
      QString s = myDb->driver()->formatValue(f);
      printf("QcjDataXML::getWhereClause(): Adding field to where clause\n");
      fflush(stdout);
      where += (*it) + " = " + s;
   }
   printf("QcjDataXML::getWhereClause():where = |%s|\n", qPrintable(where));
   fflush(stdout);
   return(where);
}

/*!
      \fn int QcjDataXML::getCols(QString name)

       Returns the "col" attribute for the form
*/ 
int QcjDataXML::getCols(QString name)
{
   int rv = -1;
   
#ifndef QT4_DESIGNER_PLUGIN
//   printf("QcjDataXML::getCols():Entry, looking for |%s|!\n", qPrintable(name));
   QDomElement docElem = def.documentElement();

   QDomNode n = docElem.firstChild();
   while ( !n.isNull() ) 
   {
      QDomElement e = n.toElement(); // try to convert the node to an element.
      if ( !e.isNull() && e.tagName() == "form" && e.attribute("name") != QString() && e.attribute("name") == name ) 
      {
//         printf("QcjDataXML::getCols():Found form definition e |%s|\n", qPrintable(e.attribute("name")));
         rv = e.attribute("cols").toInt();
      }
//      else if ( e.attribute("name") != QString()) 
//         printf("QcjDataXML::getCols():skipping form definition |%s|!\n", qPrintable(e.attribute("name")));
//      else
//         printf("QcjDataXML::getCols():skipping unnamed definition\n");
      n = n.nextSibling();
   }
#endif
   return(rv);
}

/*!
      \fn int QcjDataXML::getLeftMargin(QString name)

       Returns  the  "leftmargin" attribute for the form.
*/ 
int QcjDataXML::getLeftMargin(QString name)
{
   int rv = -1;
   
#ifndef QT4_DESIGNER_PLUGIN
//   printf("QcjDataXML::getLeftMargin():Entry, looking for |%s|!\n", qPrintable(name));
   QDomElement docElem = def.documentElement();

   QDomNode n = docElem.firstChild();
   while ( !n.isNull() ) 
   {
      QDomElement e = n.toElement(); // try to convert the node to an element.
      if ( !e.isNull() && e.tagName() == "form" && e.attribute("name") != QString() && e.attribute("name") == name ) 
      {
//         printf("QcjDataXML::getLeftMargin():Found form definition e |%s|\n", qPrintable(e.attribute("name")));
         rv = e.attribute("leftmargin").toInt();
      }
//      else if ( e.attribute("name") != QString()) 
//         printf("QcjDataXML::getLeftMargin():skipping form definition |%s|!\n", qPrintable(e.attribute("name")));
//      else
//         printf("QcjDataXML::getLeftMargin():skipping unnamed definition\n");
      n = n.nextSibling();
   }
#endif
   return(rv);
}

/*!
      \fn int QcjDataXML::getTopMargin(QString name)

       Returns the "topmargin" attribute for the form.
*/ 
int QcjDataXML::getTopMargin(QString name)
{
   int rv = -1;
   
#ifndef QT4_DESIGNER_PLUGIN
//   printf("QcjDataXML::getTopMargin():Entry, looking for |%s|!\n", qPrintable(name));
   QDomElement docElem = def.documentElement();

   QDomNode n = docElem.firstChild();
   while ( !n.isNull() ) 
   {
      QDomElement e = n.toElement(); // try to convert the node to an element.
      if ( !e.isNull() && e.tagName() == "form" && e.attribute("name") != QString() && e.attribute("name") == name ) 
      {
//         printf("QcjDataXML::getTopMargin():Found form definition e |%s|\n", qPrintable(e.attribute("name")));
         rv = e.attribute("topmargin").toInt();
      }
//      else if ( e.attribute("name") != QString()) 
//         printf("QcjDataXML::getTopMargin():skipping form definition |%s|!\n", qPrintable(e.attribute("name")));
//      else
//         printf("QcjDataXML::getTopMargin():skipping unnamed definition\n");
      n = n.nextSibling();
   }
#endif
   return(rv);
}

/*!
      \fn int QcjDataXML::getHorzSpacing(QString name)

       Returns the "horzspacing" attribute of the form.
*/ 
int QcjDataXML::getHorzSpacing(QString name)
{
   int rv = -1;
   
#ifndef QT4_DESIGNER_PLUGIN
//   printf("QcjDataXML::getHorzSpacing():Entry, looking for |%s|!\n", qPrintable(name));
   QDomElement docElem = def.documentElement();

   QDomNode n = docElem.firstChild();
   while ( !n.isNull() ) 
   {
      QDomElement e = n.toElement(); // try to convert the node to an element.
      if ( !e.isNull() && e.tagName() == "form" && e.attribute("name") != QString() && e.attribute("name") == name ) 
      {
//         printf("QcjDataXML::getHorzSpacing():Found form definition e |%s|\n", qPrintable(e.attribute("name")));
         rv = e.attribute("horzspacing").toInt();
      }
//      else if ( e.attribute("name") != QString()) 
//         printf("QcjDataXML::getHorzSpacing():skipping form definition |%s|!\n", qPrintable(e.attribute("name")));
//      else
//         printf("QcjDataXML::getHorzSpacing():skipping unnamed definition\n");
      n = n.nextSibling();
   }
#endif
   return(rv);
}

/*!
      \fn int QcjDataXML::getVertSpacing(QString name)

       Returns the "vertspacing" attribute of the form.
*/ 
int QcjDataXML::getVertSpacing(QString name)
{
   int rv = -1;
   
#ifndef QT4_DESIGNER_PLUGIN
//   printf("QcjDataXML::getVertSpacing():Entry, looking for |%s|!\n", qPrintable(name));
   QDomElement docElem = def.documentElement();

   QDomNode n = docElem.firstChild();
   while ( !n.isNull() ) 
   {
      QDomElement e = n.toElement(); // try to convert the node to an element.
      if ( !e.isNull() && e.tagName() == "form" && e.attribute("name") != QString() && e.attribute("name") == name ) 
      {
//         printf("QcjDataXML::getVertSpacing():Found form definition e |%s|\n", qPrintable(e.attribute("name")));
         rv = e.attribute("vertspacing").toInt();
      }
//      else if ( e.attribute("name") != QString()) 
//         printf("QcjDataXML::getVertSpacing():skipping form definition |%s|!\n", qPrintable(e.attribute("name")));
//      else
//         printf("QcjDataXML::getVertSpacing():skipping unnamed definition\n");
      n = n.nextSibling();
   }
#endif
   return(rv);
}

/*!
      \fn int QcjDataXML::getRows(QString name)

       Returns the "row" attribute for the form.
*/ 
int QcjDataXML::getRows(QString name)
{
   int rv = -1;
   
#ifndef QT4_DESIGNER_PLUGIN
//   printf("QcjDataXML::getRows():Entry, looking for |%s|!\n", qPrintable(name));
   QDomElement docElem = def.documentElement();

   QDomNode n = docElem.firstChild();
   while ( !n.isNull() ) 
   {
      QDomElement e = n.toElement(); // try to convert the node to an element.
      if ( !e.isNull() && e.tagName() == "form" && e.attribute("name") != QString() && e.attribute("name") == name ) 
      {
//         printf("QcjDataXML::getRows():Found form definition e |%s|\n", qPrintable(e.attribute("name")));
         rv = e.attribute("rows").toInt();
      }
//      else if ( e.attribute("name") != QString()) 
//         printf("QcjDataXML::getRows():skipping form definition |%s|!\n", qPrintable(e.attribute("name")));
//      else
//         printf("QcjDataXML::getRows():skipping unnamed definition\n");
      n = n.nextSibling();
   }
#endif
   return(rv);
}

/*!
      \fn bool QcjDataXML::confirmDelete(QString name, QcjDataForm *form)

       Builds  an appropriate QMessageBox for the form and queries the
       user  if they want to delete the current record. It will return
       true if the user affirms the delete, otherwise it returns false
*/ 
bool QcjDataXML::confirmDelete(QString name, QcjDataForm *form)
{
   bool rv = false;

#ifndef QT4_DESIGNER_PLUGIN
   printf("QcjDataXML::confirmDelete():Entry, looking for |%s|!\n", qPrintable(name));
   fflush(stdout);
   QDomElement docElem = def.documentElement();

   QDomNode n = docElem.firstChild();
   while ( !n.isNull() ) 
   {
      QDomElement e = n.toElement(); // try to convert the node to an element.
      if ( !e.isNull() && e.tagName() == "form" && e.attribute("name") != QString() && e.attribute("name") == name ) 
      {
         QDomNode n1 = n.firstChild();
         for ( int idx = 0; !n1.isNull(); idx++ ) 
         {
            QDomElement e1 = n1.toElement(); // try to convert the node to an element.
            if ( !e1.isNull() && e1.tagName() == "delete_confirmation" )
            {
               QDomNode cdata = n1.firstChild();
               if ( cdata.isText() ) 
               {
                  QString str = cdata.nodeValue();
                  printf("QcjDataXML::confirmDelete():Have deleteConfirmation string: |%s|\n", qPrintable(str));
                  fflush(stdout);
                  QRegExp re(":field:(\\w+):");
                  int ofs;
                  while ((ofs = re.indexIn(str, 0)) >= 0) 
                  {
                     printf("QcjDataXML::confirmDelete():Found re text in string |%s|\n", qPrintable(re.cap(0)));
                     QString name = re.cap(1);
                     if ( name.size() > 0 ) 
                     {
                        printf("QcjDataXML::confirmDelete():Found field name of |%s|\n", qPrintable(name));
                        fflush(stdout);
                        QString val = form->getFieldText(name);
                        printf("QcjDataXML::confirmDelete():replacing field |%s| with |%s|\n", qPrintable(name), qPrintable(val));
                        str.replace(":field:" + name + ":", val);
                     }
                     else 
                     {
                        printf("QcjDataXML::confirmDelete():Field not found!\n");
                        fflush(stdout);
                        break;
                     }
                  }

                  int x;
                  if ((x = QMessageBox::warning(NULL, "Delete Transaction", str, QMessageBox::Yes | QMessageBox::Default, 
                                          QMessageBox::No | QMessageBox::Escape)) == QMessageBox::Yes) 
                  {
//                     printf("QcjDataXML::confirmDelete():Have delete confirmation, x = %d\n", x);
                     rv = true;
                  }
                  else 
                  {
//                     printf("QcjDataXML::confirmDelete():Have delete denial, x = %d\n", x);
                     rv = false;
                  }
               }
            }
//            printf("QcjDataXML::confirmDelete():Moving to next datafield\n");
            fflush(stdout);
            n1 = n1.nextSibling();
//            printf("QcjDataXML::confirmDelete():n1 = %d!\n", n1.isNull());
            fflush(stdout);
         }
         break;
      }
//      else if ( e.attribute("name") != QString()) 
//         printf("QcjDataXML::confirmDelete():skipping form definition |%s|!\n", qPrintable(e.attribute("name")));
//      else
//         printf("QcjDataXML::confirmDelete():skipping unnamed definition\n");
      n = n.nextSibling();
   }
#endif
   return(rv);
}

QVariant QcjDataXML::getFieldDefault(const QcjDataFields &field_def)
{
   QString defval = field_def.defvalue;

   if (defval.isEmpty())
   {
      if (defval.startsWith("config:") || defval.startsWith("sqlquery:"))
      {
         QStringList sl = defval.split(":");
         if (sl[0] == "config")
         {
            defval = pFormDef->getConfigurationDefault(sl[1]);
         }
         else if (sl[0] == "sqlquery")
         {
            QString sql = sl[1];
            printf("QcjDataForm::getDefaultFieldValue():sqlquery sql = |%s|\n", qPrintable(sql));
            fflush(stdout);
            QSqlQuery *query = new QSqlQuery(sql);
            if (query->next()) 
            {
               if ( (query->value(0)) == QVariant::Invalid ) 
               {
                  defval = query->value(0).toString();
               }
            }
         }
      }
      if (field_def.fieldType == "integer")
      {
         defval = "0";
      }
      if (field_def.fieldType == "money" || field_def.fieldType == "double")
      {
         defval = "0.00";
      }
   }
   return(stringToVariant(field_def, defval));
}

/*!
      \fn QStringList QcjDataXML::getFieldNames(QString name)

       Returns a list of names defined by the form.
*/ 
QStringList QcjDataXML::getFieldNames(QString name)
{
   QStringList rv;
   
#ifndef QT4_DESIGNER_PLUGIN
   printf("QcjDataXML::getFieldNames():Entry, looking for |%s|!\n", qPrintable(name));
   QDomElement docElem = def.documentElement();

   QDomNode n = docElem.firstChild();
   while ( !n.isNull() ) 
   {
      QDomElement e = n.toElement(); // try to convert the node to an element.
      if ( !e.isNull() && e.tagName() == "form" && e.attribute("name") != QString() && e.attribute("name") == name ) 
      {
         printf("QcjDataXML::getFieldNames():Found form definition e |%s|\n", qPrintable(e.attribute("name")));
         QDomNode n1 = n.firstChild();
         for ( int idx = 0; !n1.isNull(); idx++ ) 
         {
            QDomElement e1 = n1.toElement(); // try to convert the node to an element.
            printf("QcjDataXML::getFieldNames():Testing for dataName attribute in element of |%s|\n", qPrintable(e1.tagName()));
//            if ( !e1.isNull() && e1.tagName() == "field" && e.attribute("dataName") != QString() )
            if ( !e1.isNull() && e1.tagName() == "field" )
            {
               printf("QcjDataXML::getFieldNames(): Found form definition e1 |%s|!\n", qPrintable(e1.tagName()));
               if ( e1.attribute("dataName") != QString() ) 
               {
                  printf("QcjDataXML::getFieldNames(): Attribute dataName = |%s|\n", qPrintable(e1.attribute("dataName")));
                  fflush(stdout);
                  rv << e1.attribute("dataName");
               }
            }
            printf("QcjDataXML::getFieldNames():Moving to next datafield\n");
            fflush(stdout);
            n1 = n1.nextSibling();
            printf("QcjDataXML::getFieldNames():n1 = %d!\n", n1.isNull());
            fflush(stdout);
         }
         break;
      }
      else if ( e.attribute("name") != QString()) 
         printf("QcjDataXML::getFieldNames():skipping form definition |%s|!\n", qPrintable(e.attribute("name")));
      else
         printf("QcjDataXML::getFieldNames():skipping unnamed definition\n");
      n = n.nextSibling();
   }
#endif
   qDebug() << "Exiting rv = " << rv;
   return(rv);
}

/*!
      \fn QStringList QcjDataXML::getFieldGrouping()
      
      Returns a list of the quick_search fields for the named form that 
      are in the target group.
*/
QStringList QcjDataXML::getFieldGrouping(QString name, QString target_group)
{
   QStringList rv;
   
#ifndef QT4_DESIGNER_PLUGIN
   printf("QcjDataXML::getFieldGroupings():Entry, looking for |%s|, target_group = |%s|!\n", qPrintable(name), qPrintable(target_group));
   QDomElement docElem = def.documentElement();

   QDomNode n = docElem.firstChild();
   while ( !n.isNull() ) 
   {
      QDomElement e = n.toElement(); // try to convert the node to an element.
      if ( !e.isNull() && e.tagName() == "form" && e.attribute("name") != QString() && e.attribute("name") == name ) 
      {
         printf("QcjDataXML::getFieldGroupings():Found form definition e |%s|\n", qPrintable(e.attribute("name")));
         QDomNode n1 = n.firstChild();
         for ( int idx = 0; !n1.isNull(); idx++ ) 
         {
            QDomElement e1 = n1.toElement(); // try to convert the node to an element.
            printf("QcjDataXML::getFieldGroupings():Testing for dataName attribute in element of |%s|\n", qPrintable(e1.tagName()));
//            if ( !e1.isNull() && e1.tagName() == "field" && e.attribute("dataName") != QString() )
            if ( !e1.isNull() && e1.tagName() == "quick_search")
            {
               QDomNode n2 = n1.firstChild();
               for ( int idx = 0; !n2.isNull(); idx++ ) 
               {
                  QDomElement e2 = n2.toElement(); // try to convert the node to an element.
                  printf("QcjDataXML::getFieldGroupings():Testing for dataName attribute in element of |%s|\n", qPrintable(e2.tagName()));
                  if ( !e2.isNull() && e2.tagName() == "field")
                  {
                     printf("QcjDataXML::getFieldGroupings(): Found form definition e2 |%s|!\n", qPrintable(e2.tagName()));
                     if ( e2.attribute("dataName") != QString() && e2.attribute("group") == target_group) 
                     {
                        printf("QcjDataXML::getFieldGroupings(): Attribute dataName = |%s|\n", qPrintable(e2.attribute("dataName")));
                        fflush(stdout);
                        rv << e2.attribute("dataName");
                     }
                  }
                  n2 = n2.nextSibling();
               }
               printf("QcjDataXML::getFieldGroupings():n2 = %d!\n", n2.isNull());
               fflush(stdout);
            }
            printf("QcjDataXML::getFieldGroupings():Moving to next datafield\n");
            fflush(stdout);
            n1 = n1.nextSibling();
            printf("QcjDataXML::getFieldGroupings():n1 = %d!\n", n1.isNull());
            fflush(stdout);
         }
         break;
      }
      else if ( e.attribute("name") != QString()) 
         printf("QcjDataXML::getFieldGroupings():skipping form definition |%s|!\n", qPrintable(e.attribute("name")));
      else
         printf("QcjDataXML::getFieldGroupings():skipping unnamed definition\n");
      n = n.nextSibling();
   }
#endif
   printf("QcjDataXML::getFieldGroupings():Exit\n");
   return(rv);
}

/*!
      \fn QStringList QcjDataXML::getSortFields(QString name)

       Returns  a  list of names of fields to sort on. This is derived
       from  the  comma seperated list of names which are the value of
       the "sort_fields" attribute for the form.
*/ 
QStringList QcjDataXML::getSortFields(QString name)
{
   QStringList rv;
   
#ifndef QT4_DESIGNER_PLUGIN
//   printf("QcjDataXML::getSortFields():Entry, looking for |%s|!\n", qPrintable(name));
   QDomElement docElem = def.documentElement();

   QDomNode n = docElem.firstChild();
   while ( !n.isNull() ) 
   {
      QDomElement e = n.toElement(); // try to convert the node to an element.
      if ( !e.isNull() && e.tagName() == "form" && e.attribute("name") != QString() && e.attribute("name") == name ) 
      {
//         printf("QcjDataXML::getSortFields():Found form definition e |%s|\n", qPrintable(e.attribute("name")));
         if ( e.hasAttribute("sort_fields") ) 
         {
//            printf("QcjDataXML::getSortFields():Found sortFields |%s|\n", qPrintable(e.attribute("sort_fields")));
            if ( ! e.attribute("sort_fields").contains(',') ) 
               rv << e.attribute("sort_fields");
            else 
               rv = e.attribute("sort_fields").split(',');
         }
         break;
      }
      n = n.nextSibling();
   }
#endif
   return(rv);
}

/*!
      \fn QString QcjDataXML::getSearchXlate()
      
      Returns a list of the quick_search fields for the named form that 
      are in the target group.
*/
QString QcjDataXML::getSearchXlate(QString name, QString field)
{
   QString rv;
#ifndef QT4_DESIGNER_PLUGIN
   printf("QcjDataXML::getSearchXlate():Entry, looking for |%s|, field = |%s|!\n", qPrintable(name), qPrintable(field));
   QDomElement docElem = def.documentElement();

   QDomNode n = docElem.firstChild();
   while ( !n.isNull() ) 
   {
      QDomElement e = n.toElement(); // try to convert the node to an element.
      if ( !e.isNull() && e.tagName() == "form" && e.attribute("name") != QString() && e.attribute("name") == name ) 
      {
         printf("QcjDataXML::getSearchXlate():Found form definition e |%s|\n", qPrintable(e.attribute("name")));
         QDomNode n1 = n.firstChild();
         for ( int idx = 0; !n1.isNull(); idx++ ) 
         {
            QDomElement e1 = n1.toElement(); // try to convert the node to an element.
            printf("QcjDataXML::getSearchXlate():Testing for dataName attribute in element of |%s|\n", qPrintable(e1.tagName()));
//            if ( !e1.isNull() && e1.tagName() == "field" && e.attribute("dataName") != QString() )
            if ( !e1.isNull() && e1.tagName() == "quick_search")
            {
               QDomNode n2 = n1.firstChild();
               for ( int idx = 0; !n2.isNull(); idx++ ) 
               {
                  QDomElement e2 = n2.toElement(); // try to convert the node to an element.
                  printf("QcjDataXML::getSearchXlate():Testing for dataName attribute in element of |%s|\n", qPrintable(e2.tagName()));
                  if ( !e2.isNull() && e2.tagName() == "field")
                  {
                     printf("QcjDataXML::getSearchXlate(): Found form definition e2 |%s|!\n", qPrintable(e2.tagName()));
                     if ( e2.attribute("dataName") == field)
                     {
                        printf("QcjDataXML::getSearchXlate(): Found field |%s|, xlate = |%s|\n", qPrintable(field), qPrintable(e2.attribute("xlate")));
                        fflush(stdout);
                        rv = e2.attribute("xlate");
                     }
                  }
                  n2 = n2.nextSibling();
               }
               printf("QcjDataXML::getSearchXlate():n2 = %d!\n", n2.isNull());
               fflush(stdout);
            }
            printf("QcjDataXML::getSearchXlate():Moving to next datafield\n");
            fflush(stdout);
            n1 = n1.nextSibling();
            printf("QcjDataXML::getSearchXlate():n1 = %d!\n", n1.isNull());
            fflush(stdout);
         }
         break;
      }
      else if ( e.attribute("name") != QString()) 
         printf("QcjDataXML::getSearchXlate():skipping form definition |%s|!\n", qPrintable(e.attribute("name")));
      else
         printf("QcjDataXML::getSearchXlate():skipping unnamed definition\n");
      n = n.nextSibling();
   }
#endif
   printf("QcjDataXML::getSearchXlate():Exit\n");
   return(rv);
}

/*!
      \fn QString QcjDataXML::getFormTitle(QString name)
       Retuens the "title" attribute of a form.
*/ 
QString QcjDataXML::getFormTitle(QString name)
{
   QString rv;
   
#ifndef QT4_DESIGNER_PLUGIN
//   printf("QcjDataXML::getSortFields():Entry, looking for |%s|!\n", qPrintable(name));
   QDomElement docElem = def.documentElement();

   QDomNode n = docElem.firstChild();
   while ( !n.isNull() ) 
   {
      QDomElement e = n.toElement(); // try to convert the node to an element.
      if ( !e.isNull() && e.tagName() == "form" && e.attribute("name") != QString() && e.attribute("name") == name ) 
      {
//         printf("QcjDataXML::getSortFields():Found form definition e |%s|\n", qPrintable(e.attribute("name")));
         if ( e.hasAttribute("title") ) 
         {
//            printf("QcjDataXML::getSortFields():Found sortFields |%s|\n", qPrintable(e.attribute("title")));
            rv = e.attribute("title");
         }
         break;
      }
      n = n.nextSibling();
   }
#endif
   return(rv);
}

/*!
      \fn bool QcjDataXML::isAutoCommit(QString name)
       Returns  true  if  the  "auto_commit" attribute is one of "on",
       "yes" or true".
*/ 
bool QcjDataXML::isAutoCommit(QString name)
{
   bool rv = false;
   
#ifndef QT4_DESIGNER_PLUGIN
   printf("QcjDataXML::isAutoCommit():Entry, looking for |%s|!\n", qPrintable(name));
   QDomElement docElem = def.documentElement();

   QDomNode n = docElem.firstChild();
   while ( !n.isNull() ) 
   {
      QDomElement e = n.toElement(); // try to convert the node to an element.
      if ( !e.isNull() && e.tagName() == "form"  && e.attribute("name") == name ) 
      {
         printf("QcjDataXML::isAutoCommit():found form!\n");
         if ( e.hasAttribute("auto_commit") ) 
         {
            printf("QcjDataXML::isAutoCommit():auto_commit = |%s|\n", qPrintable(e.attribute("auto_commit")));
            if ( e.attribute("auto_commit").toLower() == "on" || 
                 e.attribute("auto_commit").toLower() == "true" ||
                 e.attribute("auto_commit").toLower() == "yes"
               ) 
            {
               rv = true;
            }
            break;
         }
      }
      n = n.nextSibling();
   }
#endif
   return(rv);
}


/*!
      \fn QString QcjDataXML::getFieldLabel(QString name, QString field)
       Returns  the  "lable"  of  the field specified by the parameter
       <em>field</e> for the form.
*/ 
QString QcjDataXML::getFieldLabel(QString name, QString field)
{
   QString rv;
   
#ifndef QT4_DESIGNER_PLUGIN
//   printf("QcjDataXML::getFieldLabel():Entry, looking for |%s|\n", qPrintable(name));
   QDomElement docElem = def.documentElement();

   QDomNode n = docElem.firstChild();
   while ( !n.isNull() ) 
   {
      QDomElement e = n.toElement(); // try to convert the node to an element.
      if ( !e.isNull() && e.tagName() == "form" && e.attribute("name") != QString() && e.attribute("name") == name ) 
      {
//         printf("QcjDataXML::getFieldLabel():Found form definition e |%s|\n", qPrintable(e.attribute("name")));
         QDomNode n1 = n.firstChild();
         for ( int idx = 0; !n1.isNull(); idx++ ) 
         {
            QDomElement e1 = n1.toElement(); // try to convert the node to an element.
//            printf("QcjDataXML::getFieldLabel():Testing for dataName attribute in element of |%s|\n", qPrintable(e1.tagName()));
//            if ( !e1.isNull() && e1.tagName() == "field" && e.attribute("dataName") != NULL )
            if ( !e1.isNull() && e1.tagName() == "field" )
            {
//               printf("QcjDataXML::getFieldLabel():Found form definition e1 |%s|!\n", qPrintable(e1.tagName()));
               fflush(stdout);
               if ( e1.attribute("dataName") != QString() ) 
               {
//                  printf("QcjDataXML::getFieldLabel():Attribute dataName = |%s|\n", qPrintable(e1.attribute("dataName")));
                  fflush(stdout);
                  if (e1.attribute("dataName") == field) 
                  {
                     if ( e1.attribute("label") != QString() ) 
                     {
//                        printf("QcjDataXML::getFieldLabel():Attribute label = |%s|\n", qPrintable(e1.attribute("label")));
                        fflush(stdout);
                        rv = e1.attribute("label");
                        break;
                     }
                  }
               }
            }
//            printf("QcjDataXML::getFieldLabel():Moving to next datafield\n");
            fflush(stdout);
            n1 = n1.nextSibling();
//            printf("QcjDataXML::getFieldLabel():n1 = %d!\n", n1.isNull());
            fflush(stdout);
         }
         break;
      }
//      else if ( e.attribute("name") != QString()) 
//         printf("QcjDataXML::getFieldLabel():skipping form definition |%s|!\n", qPrintable(e.attribute("name")));
//      else
//         printf("QcjDataXML::getFieldLabel():skipping unnamed definition\n");
      n = n.nextSibling();
      fflush(stdout);
   }
//   printf("QcjDataXML::getFieldLabel():exit(%s)\n", qPrintable(rv));
#endif
   return(rv);
}

/*!
      \fn QString QcjDataXML::getFieldLabelColor(QString name)

       Returns  the  "color" attribute, which specifies the text color
       of the field labels, for the forms.
*/ 
QString QcjDataXML::getFieldLabelColor(QString name)
{
   QString rv;
   
#ifndef QT4_DESIGNER_PLUGIN
//   printf("QcjDataXML::getFieldLabel():Entry, looking for |%s|\n", qPrintable(name));
   QDomElement docElem = def.documentElement();

   QDomNode n = docElem.firstChild();
   while ( !n.isNull() ) 
   {
      QDomElement e = n.toElement(); // try to convert the node to an element.
      if ( !e.isNull() && e.tagName() == "form" && e.attribute("name") != QString() && e.attribute("name") == name ) 
      {
//         printf("QcjDataXML::getFieldLabel():Found form definition e |%s|\n", qPrintable(e.attribute("name")));
         QDomNode n1 = n.firstChild();
         for ( int idx = 0; !n1.isNull(); idx++ ) 
         {
            QDomElement e1 = n1.toElement(); // try to convert the node to an element.
//            printf("QcjDataXML::getFieldLabel():Testing for dataName attribute in element of |%s|\n", qPrintable(e1.tagName()));
//            if ( !e1.isNull() && e1.tagName() == "field" && e.attribute("dataName") != NULL )
            if ( !e1.isNull() && e1.tagName() == "label" )
            {
               if ( e1.attribute("color") != QString() ) 
               {
//                        printf("QcjDataXML::getFieldLabel():Attribute label = |%s|\n", qPrintable(e1.attribute("label")));
                  fflush(stdout);
                  rv = e1.attribute("color");
                  break;
               }
            }
//            printf("QcjDataXML::getFieldLabel():Moving to next datafield\n");
            fflush(stdout);
            n1 = n1.nextSibling();
//            printf("QcjDataXML::getFieldLabel():n1 = %d!\n", n1.isNull());
            fflush(stdout);
         }
         break;
      }
//      else if ( e.attribute("name") != QString()) 
//         printf("QcjDataXML::getFieldLabel():skipping form definition |%s|!\n", qPrintable(e.attribute("name")));
//      else
//         printf("QcjDataXML::getFieldLabel():skipping unnamed definition\n");
      n = n.nextSibling();
      fflush(stdout);
   }
//   printf("QcjDataXML::getFieldLabel():exit(%s)\n", qPrintable(rv));
#endif
   return(rv);
}

/*!
      \fn QString QcjDataXML::getFieldLabelBgColor(QString name)

       Returns the "bgcolor" attribute, which specified the background
       color of the field's label, for the form.
*/ 
QString QcjDataXML::getFieldLabelBgColor(QString name)
{
   QString rv;
   
#ifndef QT4_DESIGNER_PLUGIN
//   printf("QcjDataXML::getFieldLabel():Entry, looking for |%s|\n", qPrintable(name));
   QDomElement docElem = def.documentElement();

   QDomNode n = docElem.firstChild();
   while ( !n.isNull() ) 
   {
      QDomElement e = n.toElement(); // try to convert the node to an element.
      if ( !e.isNull() && e.tagName() == "form" && e.attribute("name") != QString() && e.attribute("name") == name ) 
      {
//         printf("QcjDataXML::getFieldLabel():Found form definition e |%s|\n", qPrintable(e.attribute("name")));
         QDomNode n1 = n.firstChild();
         for ( int idx = 0; !n1.isNull(); idx++ ) 
         {
            QDomElement e1 = n1.toElement(); // try to convert the node to an element.
//            printf("QcjDataXML::getFieldLabel():Testing for dataName attribute in element of |%s|\n", qPrintable(e1.tagName()));
//            if ( !e1.isNull() && e1.tagName() == "field" && e.attribute("dataName") != NULL )
            if ( !e1.isNull() && e1.tagName() == "label" )
            {
               if ( e1.attribute("bgcolor") != QString() ) 
               {
//                        printf("QcjDataXML::getFieldLabel():Attribute label = |%s|\n", qPrintable(e1.attribute("label")));
                  fflush(stdout);
                  rv = e1.attribute("bgcolor");
                  break;
               }
            }
//            printf("QcjDataXML::getFieldLabel():Moving to next datafield\n");
            fflush(stdout);
            n1 = n1.nextSibling();
//            printf("QcjDataXML::getFieldLabel():n1 = %d!\n", n1.isNull());
            fflush(stdout);
         }
         break;
      }
//      else if ( e.attribute("name") != QString()) 
//         printf("QcjDataXML::getFieldLabel():skipping form definition |%s|!\n", qPrintable(e.attribute("name")));
//      else
//         printf("QcjDataXML::getFieldLabel():skipping unnamed definition\n");
      n = n.nextSibling();
      fflush(stdout);
   }
//   printf("QcjDataXML::getFieldLabel():exit(%s)\n", qPrintable(rv));
#endif
   return(rv);
}

/*!
      \fn QString QcjDataXML::getFieldType(QString name, QString field)

       Returns  the  stype  of  the  field  specified by the parameter
       <em>field</em>.
*/ 
QString QcjDataXML::getFieldType(QString name, QString field)
{
   QString rv;
   
#ifndef QT4_DESIGNER_PLUGIN
//   printf("QcjDataXML::getFieldType():Entry, looking for |%s|\n", qPrintable(name));
   QDomElement docElem = def.documentElement();

   QDomNode n = docElem.firstChild();
   while ( !n.isNull() ) 
   {
      QDomElement e = n.toElement(); // try to convert the node to an element.
      if ( !e.isNull() && e.tagName() == "form" && e.attribute("name") != QString() && e.attribute("name") == name ) 
      {
//         printf("QcjDataXML::getFieldType():Found form definition e |%s|\n", qPrintable(e.attribute("name")));
         QDomNode n1 = n.firstChild();
         for ( int idx = 0; !n1.isNull(); idx++ ) 
         {
            QDomElement e1 = n1.toElement(); // try to convert the node to an element.
//            printf("QcjDataXML::getFieldType():Testing for dataName attribute in element of |%s|\n", qPrintable(e1.tagName()));
//            if ( !e1.isNull() && e1.tagName() == "field" && e.attribute("dataName") != NULL )
            if ( !e1.isNull() && e1.tagName() == "field" )
            {
//               printf("QcjDataXML::getFieldType():Found form definition e1 |%s|!\n", qPrintable(e1.tagName()));
               fflush(stdout);
               if ( e1.attribute("dataName") != QString() ) 
               {
//                  printf("QcjDataXML::getFieldType():Attribute dataName = |%s|\n", qPrintable(e1.attribute("dataName")));
                  fflush(stdout);
                  if (e1.attribute("dataName") == field) 
                  {
                     if ( e1.attribute("type") != QString() ) 
                     {
//                        printf("QcjDataXML::getFieldType():Attribute label = |%s|\n", qPrintable(e1.attribute("type")));
                        fflush(stdout);
                        rv = e1.attribute("type");
                        break;
                     }
                  }
               }
            }
//            printf("QcjDataXML::getFieldType():Moving to next datafield\n");
            fflush(stdout);
            n1 = n1.nextSibling();
//            printf("QcjDataXML::getFieldType():n1 = %d!\n", n1.isNull());
            fflush(stdout);
         }
         break;
      }
//      else if ( e.attribute("name") != QString()) 
//         printf("QcjDataXML::getFieldType():skipping form definition |%s|!\n", qPrintable(e.attribute("name")));
//      else
//         printf("QcjDataXML::getFieldType():skipping unnamed definition\n");
      n = n.nextSibling();
      fflush(stdout);
   }
//   printf("QcjDataXML::getFieldType():exit(%s)\n", qPrintable(rv));
#endif
   return(rv);
}

/*!
      \fn QStringList QcjDataXML::getConfigurationNames()

       Returns a list of names for each configuration section found in
       the XML file.
*/ 
QStringList QcjDataXML::getConfigurationNames()
{
   QStringList rv;
   
#ifndef QT4_DESIGNER_PLUGIN
//   printf("QcjDataXML::getConfigurationNames():Entry!\n");
   QDomElement docElem = def.documentElement();

   QDomNode n = docElem.firstChild();
   while ( !n.isNull() ) 
   {
      QDomElement e = n.toElement(); // try to convert the node to an element.
      if ( !e.isNull() && e.tagName() == "configuration" && e.hasAttribute("name") )
      {
//         printf("QcjDataXML::getConfigurationNames():Adding name: |%s|\n", qPrintable(e.attribute("name")));
         rv << e.attribute("name");
      }
      n = n.nextSibling();
   }
#endif
   return(rv);
}

/*!
      \fn QString QcjDataXML::getConfigurationTitle(QString config)

       Returns  the  "title"  attribute  for the configuration section
       specified by the parameter <em>config</em>.
*/ 
QString QcjDataXML::getConfigurationTitle(QString config)
{
   QString rv;
   
#ifndef QT4_DESIGNER_PLUGIN
//   printf("QcjDataXML::getConfigurationTitle():Entry, looking for |%s|\n", qPrintable(config));
   QDomElement docElem = def.documentElement();

   QDomNode n = docElem.firstChild();
   while ( !n.isNull() ) 
   {
      QDomElement e = n.toElement(); // try to convert the node to an element.
      if ( !e.isNull() && e.tagName() == "configuration" && e.attribute("name") != QString() && e.attribute("name") == config ) 
      {
         if ( e.hasAttribute("title") ) 
         {
//            printf("QcjDataXML::getConfigurationTitle():Found title |%s|\n", qPrintable(e.attribute("title")));
            return(e.attribute("title"));
         }
      }
      n = n.nextSibling();
   }
#endif
   return(QString(""));
}

/*!
      \fn void QcjDataXML::setConfigurationMenuId(QString config, int id)

       Associates  the  configuration  section  name speciofied by the
       parameter  <em>config</em> with the numeric id specified by the
       parameter <em>id</em>.
*/ 
void QcjDataXML::setConfigurationMenuId(QString config, int id)
{
   Configurations[id] = config;
}

/*!
      \fn QStringList QcjDataXML::getConfigurationDetails(QString config, QString name)

       Returns  a list of details for the configuration item specified
       by  the  parameter  <em>name</em>  of the configuration section
       specifide by the parameter <em>config</em>.

       The list comprises, in this order the following attributes:

       \li \c label The label for the configuration item.
       \li \c type The entry widget type of the item
       \li \c default The default values
       \li \c filter The text filter or constraint for the entry widget.
       \li \c min The minimum value for the numeric configuration items.
       \li \c max The maximum value for the numeric configuration items.
*/ 
QStringList QcjDataXML::getConfigurationDetails(QString config, QString name)
{
   QStringList rv;
   
#ifndef QT4_DESIGNER_PLUGIN
//   printf("QcjDataXML::getConfigurationDetails():Entry, looking for |%s.%s|!\n", qPrintable(config), qPrintable(name));
   QDomElement docElem = def.documentElement();

   QDomNode n = docElem.firstChild();
   while ( !n.isNull() ) 
   {
      QDomElement e = n.toElement(); // try to convert the node to an element.
      if ( !e.isNull() && e.tagName() == "configuration" && e.hasAttribute("name")  && e.attribute("name") == config)
      {
         QDomNode n1 = n.firstChild();
         for ( int idx = 0; !n1.isNull(); idx++ ) 
         {
            QDomElement e1 = n1.toElement(); // try to convert the node to an element.
            if ( !e1.isNull() && e1.tagName() == "entry" )
            {
               if ( e1.hasAttribute("name") )
               {  
                  QString type = "";
//                  printf("QcjDataXML::getConfigurationDetails():Testing Entry name |%s|!\n", qPrintable(e1.attribute("name"));
                  if ( e1.attribute("name") == name) 
                  {
                     rv << e1.attribute("name");

                     if ( e1.hasAttribute("label") )
                        rv << e1.attribute("label");
                     else
                        rv << QString("");

                     if ( e1.hasAttribute("type") ) 
                     {
                        type = e1.attribute("type");
                        rv << e1.attribute("type");
                     }
                     else
                        rv << QString("");

                     if ( e1.hasAttribute("default") ) 
                        rv << e1.attribute("default");
                     else
                        rv << QString("");

                     if ( type == "filter" ) 
                     {
                        if ( e1.hasAttribute("filter") ) 
                           rv << e1.attribute("filter");
                        else
                           rv << QString("*");
                     }
                     else if ( type == "integer" || type == "double") 
                     {
                        if ( e1.hasAttribute("min") ) 
                           rv << e1.attribute("min");
                        else
                           rv << QString("min");

                        if ( e1.hasAttribute("max") ) 
                           rv << e1.attribute("max");
                        else
                           rv << QString("max");
                     }
                  }
               }
            }
            n1 = n1.nextSibling();
         }
      }
      n = n.nextSibling();
   }
#endif
   return(rv);
}

/*!
      \fn QStringList QcjDataXML::getConfigurationItemNames(QString config)

       Returns  a  list  of  names  of  each configuration item in the
       configuration    section    specified    by    the    parameter
       <em>config</em>.
*/ 
QStringList QcjDataXML::getConfigurationItemNames(QString config)
{
   QStringList rv;

#ifndef QT4_DESIGNER_PLUGIN
//   printf("QcjDataXML::getConfigurationItemNames():Entry, looking for |%s|\n", qPrintable(config));
   QDomElement docElem = def.documentElement();

   QDomNode n = docElem.firstChild();
   while ( !n.isNull() ) 
   {
      QDomElement e = n.toElement(); // try to convert the node to an element.
      if ( !e.isNull() && e.tagName() == "configuration" && e.hasAttribute("name")  && e.attribute("name") == config)
      {
         QDomNode n1 = n.firstChild();
         for ( int idx = 0; !n1.isNull(); idx++ ) 
         {
            QDomElement e1 = n1.toElement(); // try to convert the node to an element.
//            printf("QcjDataXML::getConfigurationItemNames():Looking for entry, tagname = %s\n", qPrintable(e1.tagName()));
            if ( !e1.isNull() && e1.tagName() == "entry" )
            {
               if ( e1.hasAttribute("name") ) 
                  rv << e1.attribute("name");
            }
            n1 = n1.nextSibling();
         }
      }
      n = n.nextSibling();
   }
#endif
   return(rv);
}

/*!
      \fn QString QcjDataXML::getConfigurationDefault(QString name)

       Returns  the  default  value  to use for the configuration item
       spacified by the parameter <em>name</em>.
       
       This forces a caveate, all configuration items must have unique
       names even if they are in seperate configuration sections.
*/ 
QString QcjDataXML::getConfigurationDefault(QString name)
{
   printf("QcjDataXML::getConfigurationDefault():Entry, looking for |%s|\n", qPrintable(name));
#ifndef QT4_DESIGNER_PLUGIN
   QDomElement docElem = def.documentElement();

   QDomNode n = docElem.firstChild();
   while ( !n.isNull() ) 
   {
      QDomElement e = n.toElement(); // try to convert the node to an element.
      if ( !e.isNull() && e.tagName() == "configuration" )
      {
         QDomNode n1 = n.firstChild();
         for ( int idx = 0; !n1.isNull(); idx++ ) 
         {
            QDomElement e1 = n1.toElement(); // try to convert the node to an element.
            if ( !e1.isNull() && e1.tagName() == "entry" )
            {
               printf("QcjDataXML::getConfigurationDefault():found entry, name = |%s|\n", qPrintable(e1.attribute("name")));
               if ( e1.hasAttribute("name") && e1.attribute("name") == name) 
               {
                  if ( e1.hasAttribute("default") ) 
                  {
                     printf("QcjDataXML::getConfigurationDefault():found default returning: |%s|\n", qPrintable(e1.attribute("default")));
                     return(e1.attribute("default"));
                  }
                  else
                     return(QString(""));
               }
            }
            n1 = n1.nextSibling();
         }
      }
      n = n.nextSibling();
   }
#endif
   return(QString(""));
}

/*!
      \fn QString QcjDataXML::getConfigurationName(int id)

       Returns the "name" attribute for the configuration section with
       the id matching the parameter <em>id</em>.
       
       \sa QcjDataXML::setConfigurationMenuId(QString, int)
*/ 
QString QcjDataXML::getConfigurationName(int id)
{
   return(Configurations[id]);
}

/*!
      \fn QString QcjDataXML::getConfigurationHelp(QString config)

       Returns the help string for the configuration section specified
       by the parameter <em>config</em>.
*/ 
QString QcjDataXML::getConfigurationHelp(QString config)
{
   QString rv;
//   printf("QcjDataXML::getConfigurationDefault():Entry, looking for |%s|\n", qPrintable(name));
#ifndef QT4_DESIGNER_PLUGIN
   QDomElement docElem = def.documentElement();

   rv = "<dl>";
   QDomNode n = docElem.firstChild();
   while ( !n.isNull() ) 
   {
      QDomElement e = n.toElement(); // try to convert the node to an element.
      if ( !e.isNull() && e.tagName() == "configuration" && e.hasAttribute("name")  && e.attribute("name") == config )
      {
         QDomNode n1 = n.firstChild();
         for ( int idx = 0; !n1.isNull(); idx++ ) 
         {
            QDomElement e1 = n1.toElement(); // try to convert the node to an element.
            if ( !e1.isNull() && e1.tagName() == "entry" )
            {
               QString tag = e1.attribute("label");
               printf("QcjDataXML::getConfigurationDefault():found entry, name = |%s|\n", qPrintable(e1.attribute("name")));
               QDomNode n2 = n1.firstChild();
               for ( int idx = 0; !n2.isNull(); idx++ ) 
               {
                  QDomElement e2 = n2.toElement(); // try to convert the node to an element.
                  if ( !e2.isNull() && e2.tagName() == "help" )
                  {
                     printf("QcjDataXML::getConfigurationDefault():found help for item %s\n", qPrintable(e1.attribute("name")));
                     QDomNode cdata = n2.firstChild();
                     if ( cdata.isText() ) 
                     {
                        rv += "<dt><b>" + tag + "</b></dt><dd>";
                        rv += cdata.nodeValue();
                        rv += "</dd>\n";
                     }
                  }
                  n2 = n2.nextSibling();
               }
            }
            n1 = n1.nextSibling();
         }
      }
      n = n.nextSibling();
   }
   rv += "</dl>";
#endif
   return(rv);
}

QString QcjDataXML::getFieldXlate(QString name, QString field)
{
   QString rv;
   
#ifndef QT4_DESIGNER_PLUGIN
//   printf("QcjDataXML::getFieldXlate():Entry, looking for |%s|\n", qPrintable(name));
   QDomElement docElem = def.documentElement();

   QDomNode n = docElem.firstChild();
   while ( !n.isNull() ) 
   {
      QDomElement e = n.toElement(); // try to convert the node to an element.
      if ( !e.isNull() && e.tagName() == "form" && e.attribute("name") != QString() && e.attribute("name") == name ) 
      {
//         printf("QcjDataXML::getFieldXlate():Found form definition e |%s|\n", qPrintable(e.attribute("name")));
         QDomNode n1 = n.firstChild();
         for ( int idx = 0; !n1.isNull(); idx++ ) 
         {
            QDomElement e1 = n1.toElement(); // try to convert the node to an element.
//            printf("QcjDataXML::getFieldXlate():Testing for dataName attribute in element of |%s|\n", qPrintable(e1.tagName()));
//            if ( !e1.isNull() && e1.tagName() == "field" && e.attribute("dataName") != NULL )
            if ( !e1.isNull() && e1.tagName() == "field" )
            {
//               printf("QcjDataXML::getFieldXlate():Found form definition e1 |%s|!\n", qPrintable(e1.tagName()));
               fflush(stdout);
               if ( e1.attribute("dataName") != QString() ) 
               {
//                  printf("QcjDataXML::getFieldXlate():Attribute dataName = |%s|\n", qPrintable(e1.attribute("dataName")));
                  fflush(stdout);
                  if (e1.attribute("dataName") == field) 
                  {
                     if ( e1.attribute("xlate") != QString() ) 
                     {
//                        printf("QcjDataXML::getFieldXlate():Attribute label = |%s|\n", qPrintable(e1.attribute("label")));
                        fflush(stdout);
                        rv = e1.attribute("xlate");
                        break;
                     }
                  }
               }
            }
//            printf("QcjDataXML::getFieldXlate():Moving to next datafield\n");
            fflush(stdout);
            n1 = n1.nextSibling();
//            printf("QcjDataXML::getFieldXlate():n1 = %d!\n", n1.isNull());
            fflush(stdout);
         }
         break;
      }
//      else if ( e.attribute("name") != QString()) 
//         printf("QcjDataXML::getFieldXlate():skipping form definition |%s|!\n", qPrintable(e.attribute("name")));
//      else
//         printf("QcjDataXML::getFieldXlate():skipping unnamed definition\n");
      n = n.nextSibling();
      fflush(stdout);
   }
//   printf("QcjDataXML::getFieldXlate():exit(%s)\n", qPrintable(rv));
#endif
   return(rv);
}

/*!
      \fn QString QcjDataXML::getHelpBlock(QString blockName)

       Returns  the  string  for  the  help  section  specified by the
       parameter <em>blockName</em>.
*/ 
QString QcjDataXML::getHelpBlock(QString blockName)
{
   QString rv;
   printf("QcjDataXML::getHelpBlock():Entry, looking for |%s|\n", qPrintable(blockName));
#ifndef QT4_DESIGNER_PLUGIN
   QDomElement docElem = def.documentElement();

   QDomNode n = docElem.firstChild();
   while ( !n.isNull() ) 
   {
      QDomElement e = n.toElement(); // try to convert the node to an element.
      printf("QcjDataXML::getHelpBlock():have element type %s, name %s\n", qPrintable(e.tagName()), qPrintable(e.attribute("name")));
      if ( !e.isNull() && e.tagName() == "help" && e.hasAttribute("name")  && e.attribute("name") == blockName )
      {
         printf("QcjDataXML::getHelpBlock():found help for item %s\n", qPrintable(e.attribute("name")));
         QDomNode cdata = n.firstChild();
         if ( cdata.isText() ) 
         {
            rv += cdata.nodeValue();
            rv += "<p>\n";
         }
      }
      n = n.nextSibling();
   }
#endif
   printf("QcjDataXML::getHelpBlock():Exit\n");
   return(rv);
}

/*!
      \fn QVariant QcjDataXML::getResource(QString blockName)

       Returns  the  variant for the resource section specified by the
       parameter <em>blockName</em>.
*/ 
QVariant QcjDataXML::getResource(QString blockName)
{
   QString rv;
//   printf("QcjDataXML::getConfigurationDefault():Entry, looking for |%s|\n", qPrintable(name));
#ifndef QT4_DESIGNER_PLUGIN
   QDomElement docElem = def.documentElement();

   QDomNode n = docElem.firstChild();
   while ( !n.isNull() ) 
   {
      QDomElement e = n.toElement(); // try to convert the node to an element.
      if ( !e.isNull() && e.tagName() == "resource" && e.hasAttribute("name")  && e.attribute("name") == blockName )
      {
         printf("QcjDataXML::getConfigurationDefault():found help for item %s\n", qPrintable(e.attribute("name")));
         QDomNode cdata = n.firstChild();
         if ( cdata.isText() ) 
         {
            rv += cdata.nodeValue();
         }
      }
      n = n.nextSibling();
   }
#endif
   return(QVariant(rv));
}

/*!
   \fn QMap<QString, QVariant> QcjDataXML::getResourceMap(QString blockName)

   Returns a map of all property items for the requested resource
*/
QMap<QString, QVariant> QcjDataXML::getResourceMap(QString blockName)
{
   QMap<QString, QVariant> rv;
#ifndef QT4_DESIGNER_PLUGIN
   printf("QcjDataXML::getResourceMap():Entry, looking for resource |%s|\n", qPrintable(blockName));
   QDomElement docElem = def.documentElement();

   QDomNode n = docElem.firstChild();
   while ( !n.isNull() ) 
   {
      QDomElement e = n.toElement(); // try to convert the node to an element.
      if ( !e.isNull() && e.tagName() == "resource" && e.hasAttribute("name")  && e.attribute("name") == blockName )
      {
         printf("QcjDataXML::getResourceMap():Found resource\n");
         fflush(stdout);
         if (e.hasAttribute("group"))
         {
            rv.insert("group", e.attribute("group"));
         }
         QDomNode prop = n.firstChild();
         while ( ! prop.isNull() ) 
         {
            printf("QcjDataXML::getResourceMap():Checking for proprty tags\n");
            fflush(stdout);
            QDomElement e1 = prop.toElement(); // try to convert the node to an element.
            printf("QcjDataXML::getResourceMap():Have node: %s\n", qPrintable(e1.tagName()));
            fflush(stdout);
            if ( !e1.isNull() && e1.tagName() == "property" && e1.hasAttribute("name") )
            {
               QString name = e1.attribute("name");
               QString value;
               if ( e1.hasAttribute("value") )
                  value = e1.attribute("value");
               else 
               {
                  printf("QcjDataXML::getConfigurationDefault():found help for item %s\n", qPrintable(e1.attribute("name")));
                  QDomNode cdata = prop.firstChild();
                  if ( cdata.isText() ) 
                  {
                     value = cdata.nodeValue();
                  }
               }
               printf("QcjDataXML::getResourceMap():Adding value of |%s| for name |%s|\n", qPrintable(value), qPrintable(name));
               fflush(stdout);
               rv.insert(name, QVariant(value));
            }
            prop = prop.nextSibling();
         }
         break;
      }
      n = n.nextSibling();
   }
#endif
   printf("QcjDataXML::getResourceMap():Exit\n");
   fflush(stdout);
   return(rv);
}

/*!
   \fn QStringList QcjDataXML::getResourceNames(QString groupName = QString())

   Returns a list of resource names in the requested group. 
   
   If the group is QString() then it will return a list of all resource names 
   found.
*/
QStringList QcjDataXML::getResourceNames(QString group)
{
   QStringList rv;
#ifndef QT4_DESIGNER_PLUGIN
   printf("QcjDataXML::getResourceNames():Entry, looking for group |%s|\n", qPrintable(group));
   QDomElement docElem = def.documentElement();

   QDomNode n = docElem.firstChild();
   while ( ! n.isNull() ) 
   {
      QDomElement e = n.toElement(); // try to convert the node to an element.
      printf("QcjDataXML::getResourceNames():Have node, tag is |%s|, group is |%s|\n", qPrintable(e.tagName()), qPrintable(e.attribute("group")));
      fflush(stdout);
      if ( !e.isNull() && e.tagName() == "resource" && 
           ( group == QString() || 
             ( e.hasAttribute("group") && 
               e.attribute("group") == group ) )
         )
      {
         printf("QcjDataXML::getResourceNames():Adding name: |%s|\n", qPrintable(e.attribute("name")));
         fflush(stdout);
         rv.append(e.attribute("name"));
      }
      n = n.nextSibling();
   }
#endif
   printf("QcjDataXML::getResourceNames():Exiting\n");
   fflush(stdout);
   return(rv);
}

/*!
   \fn bool QcjDataXML::resourceHasProperties(QString blockName)

   Returns wether or not a specified resource has properties defined for it or not.
   If the resource specified by <em>blockName</em> has any property elements, this function
   will return \e true, otherwise it will return <em>false</em>
*/
bool QcjDataXML::resourceHasProperties(QString blockName)
{
#ifndef QT4_DESIGNER_PLUGIN
   QDomElement docElem = def.documentElement();

   QDomNode n = docElem.firstChild();
   while ( !n.isNull() ) 
   {
      QDomElement e = n.toElement(); // try to convert the node to an element.
      if ( !e.isNull() && e.tagName() == "resource" && e.hasAttribute("name")  && e.attribute("name") == blockName )
      {
         QDomNode prop = n.firstChild();
         while ( ! prop.isNull() ) 
         {
            QDomElement e1 = prop.toElement(); // try to convert the node to an element.
            if ( !e1.isNull() && e1.tagName() == "property" && e1.hasAttribute("name") )
            {
               return(true);
            }
            prop = prop.nextSibling();
         }
      }
      n = n.nextSibling();
   }
#endif
   return(false);
}

QVariant QcjDataXML::stringToVariant(const QcjDataFields &field_def, const QString &value)
{
   QVariant rv;
   QString type = field_def.fieldType.toLower();

   /*********/
   /* Bools */
   /*********/
   if (type == "bool" || type == "yesno")
   {
      if (value.toLower() == "t" || value.toLower() == "true" ||
          value.toLower() == "y" || value.toLower() == "yes")
         return(QVariant("Y"));
      else
         return(QVariant("N"));
   }

   /****************/
   /* Real Numbers */
   /****************/
   if (type == "double" || type == "money")
      return(QVariant(value.toDouble()));


   /************/
   /* Integers */
   /************/
   if (type == "integer")
      return(QVariant(value.toInt()));

   /*********/
   /* Dates */
   /*********/
   if (type == "date")
      return(QVariant(value));

   /**************************************/
   /* Binary Data (no string equivalent) */
   /**************************************/
   if (type == "image" || type == "photo")
      return(QVariant());
   
   /***************/
   /* String data */
   /***************/
   return(QVariant(value));
}

/*!
 *    \fn xlateRecordToForm
 *
 *    This function will return a VariantHash of the passed in record of values
 *    with the key set to the dataName for each field found in the input hash 
 *    with the label of the field'
*/
QcjLib::VariantHash QcjDataXML::xlateRecordToForm(const QString &config, const QSqlRecord &rec, bool labels) const
{
   QVariantHash rv;

   qDebug() << "enter: config = " << config;
   QcjDataFieldStdVector fds = pFormDef->getFields(config, nullptr);

   qDebug() << "fds count: " << fds.size();
   for (unsigned int idx = 0; idx < fds.size(); idx++)
   {
      QcjDataFields field_def = fds.at(idx);
      qDebug() << "field_def[" << idx << "]: label: " << field_def.label
                                      << " field name: " << field_def.dataName;
      if (rec.contains(field_def.dataName))
      {
         QVariant value = rec.value(field_def.dataName);
         if (labels)
         {
            qDebug() << "Adding value: " << value << " for " << field_def.dataName
                     << " to form element (label): " << field_def.label;
            rv.insert(field_def.label, value);
         }
         else
         {
            qDebug() << "Adding value: " << value << " for " << field_def.dataName
                     << " to form element (field): " << field_def.dataName;
            rv.insert(field_def.dataName, value);
         }
      }
   }

   /**************************************************************************/
   /* Add any fields in rec that were not defined in the field definitions.  */
   /**************************************************************************/
   for (int idx = 0; idx < rec.count(); idx++)
   {
      QString name = rec.fieldName(idx);
      if ( ! rv.contains(name))
      {
         qDebug() << "Adding extra field: " << name;
         rv.insert(name, rec.value(name));
      }
   }

   return(rv);
}

/*!
 *    \fn xlateFormFromFields
 *
 *    This function will return a VariantHash of the passed in Hash of fields
 *    with the key set to the dataName for each field found in the input hash 
 *    with the label of the field'
*/
QcjLib::VariantHash QcjDataXML::xlateFormToFields(const QString &config, const QcjLib::VariantHash &item) const
{
   QVariantHash rv;
   qDebug() << "enter: config = " << config;
   qDebug() << "item keys:" << item.keys();
   std::vector<struct QcjDataFields> fds = pFormDef->getFields(config, nullptr);

   qDebug() << "fds count: " << fds.size();
   for (unsigned int idx = 0; idx < fds.size(); idx++)
   {
      QcjDataFields field_def = fds.at(idx);
      qDebug() << "field_def[" << idx << "]: label: " << field_def.label
                                      << " field name: " << field_def.dataName;
      if (item.contains(field_def.label))
      {
         QString value;
         if (field_def.fieldType == "photo")
         {
            value = QString(item.value(field_def.label).toByteArray().toBase64().data());
         }
         else if (field_def.fieldType == "money")
         {
            value = item.value(field_def.label).toString().remove("$");
#if 0
            static QRegularExpression re("\\D*(\\d*\\.\\d{,2})\\D*");
            QRegularExpressionMatch match = re.match(item.value(field_def.label).toString());
            if (match.hasMatch())
            {
               value = match.captured(1);
            }
            else
            {
               value = "0.00";
            }
#endif
         }
         else if (field_def.fieldType != "yesno")
         {
            QVariant val = item.value(field_def.label);
            if (val.type() == QVariant::StringList)
            {
               value = val.toStringList().join(",");
            }
            else
            {
               value = val.toString();
            }
            QString fmt = field_def.format;
            char pad = ' ';
            if ( !  fmt.isEmpty())
            {
               if (fmt.startsWith("9"))
               {
                  pad = '0';
               }
               value = value.rightJustified(fmt.length(), pad);
            }
         }
         else
         {
            value = item.value(field_def.label).toString();
         }

         if (value.isEmpty())
         {
            value = pFormDef->getFieldDefault(field_def).toString();
         }
         qDebug() << "Adding value(" << field_def.label
//                  << ") of " << item.value(field_def.label)
                  << "to field: " << field_def.dataName;
         rv.insert(field_def.dataName, value);
      }
   }

   /**************************************************************************/
   /* Add any fields in item that were not defined in the field definitions. */
   /**************************************************************************/
   foreach (QString name, item.keys())
   {
      if ( ! rv.contains(name))
      {
         rv.insert(name, item.value(name));
      }
   }

   return(rv);
}
