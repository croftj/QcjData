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

#ifndef DATAHELPERS_H
#define DATAHELPERS_H

#include <qcombobox.h>
//#include <q3datetimeedit.h>
//#include <q3frame.h>
#include <qlayout.h>
#include <qlineedit.h>
#include <qsettings.h>
//#include <q3sqlcursor.h>
#include <qsqldatabase.h>
//#include <q3sqleditorfactory.h>
//#include <q3sqlform.h>
#include <qsqlrecord.h>
#include <qstring.h>
#include <qtextedit.h>
#include <QLabel>
#include <QWidget>
#include <QFrame>
#include <QCheckBox>
#include <QDateEdit>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <stdlib.h>
#include <stdio.h>


/*!
       This  structure holds the information for a single element on a
       form.  It  is  used  internally  by  the QcjDataForm and QcjDataTable
       classes to hold the information needed to represent the data in
       the database for the particular form or table widget.
*/ 
typedef struct QcjDataFields 
{
   QString     dataName;
   QString     label;
   QString     key;
   QString     valueName;
   int         row;
   int         col;
   int         colSpan;
   int         rowSpan;
   int         minWidth;
   int         maxWidth;
   bool        ro;
   QString     init;
   QString     defvalue;
   QString     fieldType;
   QString     propName;
   QString     options;
   QString     format;
   QString     align;
   QString     color;
   QString     bgcolor;
   QString     width;
   QString     height;
   QString     search;
   QString     minValue;
   QString     maxValue;
   QString     stepValue;
   QString     decimals;
   QString     suffix;
   bool        focusWidget;
   QValidator  *validator;
   QWidget     *widget;
} QcjDataFieldDef;

typedef QList<QcjDataFieldDef> QcjDataFieldList;
typedef QHash<QString, QcjDataFieldDef> QcjDataFieldMap;
typedef std::vector<QcjDataFieldDef> QcjDataFieldStdVector;

class QcjStringSelect : public QComboBox 
{
   Q_OBJECT               
   Q_PROPERTY( QString strsel READ text WRITE setText );
   Q_PROPERTY( QString pname READ getPName WRITE setPName );

public:
   QcjStringSelect(QWidget * parent = 0) : QComboBox(parent) { };
   QcjStringSelect(QString strsel, QWidget * parent = 0) : QComboBox(parent) { initialize(strsel); };
   QString text() const;
   void setText(QString t);
   void setPName(QString name)
   {
      m_pname = name;
   };

   QString getPName()
   {
      if ( m_pname == QString() ) 
         m_pname = "strsel";
      return(m_pname);
   };

public slots:
   void initialize(QcjDataFields *field_def);
   void initialize(QString selections);

private:
   QString m_pname;
};

class QcjSqlModel : public QComboBox 
{
   Q_OBJECT               
   Q_PROPERTY( QString sqlmodel READ text WRITE setText );
   Q_PROPERTY( QString pname READ getPName WRITE setPName );

public:
   QcjSqlModel(QWidget * parent = 0) : QComboBox(parent) { };
   QcjSqlModel(QString strsel, QWidget * parent = 0) : QComboBox(parent) { initialize(strsel); };
   QString text() const;
   void setText(QString t);
   void setPName(QString name)
   {
      m_pname = name;
   };

   QString getPName()
   {
      if ( m_pname == QString() ) 
         m_pname = "sqlmodel";
      return(m_pname);
   };

public slots:
   void initialize(QString s);

private:
   QString m_pname;
};

class QcjYesNoSelect : public QComboBox 
{
   Q_OBJECT               
   Q_PROPERTY( QString yesno READ text WRITE setText );
   Q_PROPERTY( QString pname READ getPName WRITE setPName );

public:
   QcjYesNoSelect(QWidget * parent = 0) : QComboBox(parent) { initialize(); };
   QString text() const;
   void setText(QString t);
   void setPName(QString name)
   {
      m_pname = name;
   };

   QString getPName()
   {
      if ( m_pname == QString() ) 
         m_pname = "yesno";
      return(m_pname);
   };

public slots:
   void initialize();

private:
   QString m_pname;
};

class QcjTrueFalseSelect : public QComboBox 
{
   Q_OBJECT               
   Q_PROPERTY( int truefalse READ value WRITE setValue );
   Q_PROPERTY( QString pname READ getPName WRITE setPName );

public:
   QcjTrueFalseSelect(QWidget * parent = 0) : QComboBox(parent) { initialize(); };
   int value() const;
   void setValue(int i);
   void setText(QString t);
   void setPName(QString name)
   {
      m_pname = name;
   };

   QString getPName()
   {
      if ( m_pname == QString() ) 
         m_pname = "truefalse";
      return(m_pname);
   };

public slots:
   void initialize();
private:
   QString m_pname;
};

class QcjCheckBoxTF : public QCheckBox 
{
   Q_OBJECT               
   Q_PROPERTY( int checkboxtf READ value WRITE setValue );
   Q_PROPERTY( QString pname READ getPName WRITE setPName );

public:
   QcjCheckBoxTF(QWidget * parent = 0) : QCheckBox(parent) { };

   int value() const
   {
      int rv;
      if ( isChecked() ) 
         rv = 1;
      else
         rv = 0;
      return(rv);
   };

   void setValue(int val)
   {
      if ( val )
         setChecked(true);
      else
         setChecked(false);
   };

   void setPName(QString name)
   {
      m_pname = name;
   };

   QString getPName()
   {
      if ( m_pname == QString() ) 
         m_pname = "checkboxtf";
      return(m_pname);
   };

private:
   QString m_pname;
};

class QcjCheckBoxYN : public QCheckBox 
{
   Q_OBJECT               
   Q_PROPERTY( QString checkboxyn READ value WRITE setValue );
   Q_PROPERTY( QString pname READ getPName WRITE setPName );

public:
   QcjCheckBoxYN(QWidget * parent = 0) : QCheckBox(parent) { };
   QString value() const
   {
      QString rv;
      if ( isChecked() ) 
         rv = "Y";
      else
         rv = "N";
      return(rv);
   };

   void setValue(QString s)
   {
      if ( s.toLower() == "y" )
         setChecked(true);
      else
         setChecked(false);
   };

   void setPName(QString name)
   {
      m_pname = name;
   };

   QString getPName()
   {
      if ( m_pname == QString() ) 
         m_pname = "checkboxyn";
      return(m_pname);
   };
private:
   QString m_pname;
};

class QcjPhoneEdit : public QLineEdit
{
   Q_OBJECT
   Q_PROPERTY( QString phone READ text WRITE setText );
   Q_PROPERTY( QString pname READ getPName WRITE setPName );

public:
   QcjPhoneEdit ( QWidget * parent);
   QcjPhoneEdit ( const QString & contents, QWidget * parent);
   QString text() const;
   static QString formatPhoneNumber(QString f);
   void setPName(QString name)
   {
      m_pname = name;
   };

   QString getPName()
   {
      if ( m_pname == QString() ) 
         m_pname = "phone";
      return(m_pname);
   };

public slots:
   void setText(const QString);

private:
   QString m_pname;
   static QRegularExpression phoneRE;
   static QString phoneFormat;
};


class QcjMoneyEdit : public QLineEdit
{
   Q_OBJECT
   Q_PROPERTY( QString money READ text WRITE setText );
   Q_PROPERTY( QString pname READ getPName WRITE setPName );

public:
   QcjMoneyEdit ( QWidget * parent);
   QcjMoneyEdit ( const QString & contents, QWidget * parent);
   QString text() const;
   static QString formatCurrency(QString f);
   void setPName(QString name)
   {
      m_pname = name;
   };

   QString getPName()
   {
      if ( m_pname == QString() ) 
         m_pname = "money";
      return(m_pname);
   };

public slots:
   void formatText();
   void setText(const QString);

private:
   QString m_pname;
};

class QcjTextBlockEdit : public QTextEdit
{
   Q_OBJECT
   Q_PROPERTY( QString text READ text WRITE setText );
   Q_PROPERTY( QString pname READ getPName WRITE setPName );

public:
   QcjTextBlockEdit ( QWidget * parent) : QTextEdit(parent)
   {
      setTabChangesFocus(true);
   };

   QcjTextBlockEdit ( const QString & contents, QWidget * parent) : QTextEdit(parent) 
   { 
      setTabChangesFocus(true);
      setPlainText(contents);
   };

   QString text() const;
   void setPName(QString name)
   {
      m_pname = name;
   };

   QString getPName()
   {
      if ( m_pname == QString() ) 
         m_pname = "text";
      return(m_pname);
   };

public slots:
   void setText(const QString);

private:
   QString m_pname;
};

typedef enum qcj_date_type
{
   Date,
   MonthBeg,
   MonthEnd,
   YearBeg,
   YearEnd
} QcjDateType;

class QcjDateEntry : public QDateEdit 
{
   Q_OBJECT               
   Q_PROPERTY( QString date READ get WRITE set );
   Q_PROPERTY( QString pname READ getPName WRITE setPName );

public:
   typedef enum
   {
      Date,
      MonthBeg,
      MonthEnd,
      YearBeg,
      YearEnd
   } QcjDateType;

   QcjDateEntry(QcjDateType t, QWidget * parent = 0) : QDateEdit(parent) 
   {
      printf("QcjDateEntry::QcjDateEntry(): Enter: type = %d\n", t);
      setDateType(t);
   };

   QcjDateEntry(QWidget * parent = 0) : QDateEdit(parent) 
   { 
      type = Date; 
   };
   QString get() const;
   void set(QString);
   void setPName(QString name)
   {
      m_pname = name;
   };

   QString getPName()
   {
      if ( m_pname == QString() ) 
         m_pname = "date";
      return(m_pname);
   };

   void setDateType(QcjDateType t)
   {
      type = t;
      switch (type) 
      {
         case Date:
            setDisplayFormat("D MMM yyyy");
            break;   
   
         case MonthBeg:
         case MonthEnd:
            setDisplayFormat("MMM yyyy");
            break;   
   
         case YearBeg:
         case YearEnd:
            setDisplayFormat("yyyy");
            break;   
   
         default:
            break;
      }
   };

   QcjDateType dateType()
   {
      return(type);
   };

private:
   QcjDateType type;
   QString     m_pname;
};

class QcjTimeEntry : public QTimeEdit 
{
   Q_OBJECT               
   Q_PROPERTY( QString date READ get WRITE set );
   Q_PROPERTY( QString pname READ getPName WRITE setPName );

public:
   QcjTimeEntry(QWidget * parent = 0) : QTimeEdit(parent) 
   {
   };

   QString get() const;
   void set(QString);
   void setPName(QString name)
   {
      m_pname = name;
   };

   QString getPName()
   {
      if ( m_pname == QString() ) 
         m_pname = "time";
      return(m_pname);
   };

private:
   QString     m_pname;
};

class QcjTimestampEntry : public QDateTimeEdit 
{
   Q_OBJECT               
   Q_PROPERTY( QString timestamp READ get WRITE set );
   Q_PROPERTY( QString pname READ getPName WRITE setPName );

public:
   QcjTimestampEntry(QWidget * parent = 0) : QDateTimeEdit(parent) { };
   QString get() const;
   void set(QString);
   void setPName(QString name)
   {
      m_pname = name;
   };

   QString getPName()
   {
      if ( m_pname == QString() ) 
         m_pname = "timestamp";
      return(m_pname);
   };
private:
   QString m_pname;
};

class QcjSpinBox : public QSpinBox
{
   Q_OBJECT               
   Q_PROPERTY( int value READ get WRITE set );
   Q_PROPERTY( QString pname READ getPName WRITE setPName );

public:
   QcjSpinBox(QWidget * parent = 0) : QSpinBox(parent) { };
   int get() const 
   {
      return(value());
   };

   void set(int val)
   {
      setValue(val);
   };

   void setPName(QString name)
   {
      m_pname = name;
   };

   QString getPName()
   {
      if ( m_pname == QString() ) 
         m_pname = "value";
      return(m_pname);
   };
private:
   QString m_pname;
};

class QcjDoubleSpinBox : public QDoubleSpinBox
{
   Q_OBJECT               
   Q_PROPERTY( double value READ get WRITE set );
   Q_PROPERTY( QString pname READ getPName WRITE setPName );

public:
   QcjDoubleSpinBox(QWidget * parent = 0) : QDoubleSpinBox(parent) { };
   double get() const 
   {
      return(value());
   };

   void set(double val)
   {
      setValue(val);
   };

   void setPName(QString name)
   {
      m_pname = name;
   };

   QString getPName()
   {
      if ( m_pname == QString() ) 
         m_pname = "value";
      return(m_pname);
   };
private:
   QString m_pname;
};

#ifndef NO_PHOTO_SELECT
class QcjPhotoEntry : public QLabel 
{
   Q_OBJECT               
   Q_PROPERTY( QByteArray photo READ get WRITE set )
   Q_PROPERTY( QString pname READ getPName WRITE setPName );

public:
   QcjPhotoEntry(QWidget *parent = 0) : QLabel(parent) 
   {
      setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);
      setText("<html>Double click</html>");
      m_pname = "photo";
   };

   bool        match(QByteArray ba) const;
   QString     md5sum() const;
   QByteArray  get() const;
   void        set(QByteArray);
   void        setWidth(int width);

   void setPName(QString name)
   {
      m_pname = name;
   };

   QString getPName()
   {
      if ( m_pname == QString() ) 
         m_pname = "photo";
      return(m_pname);
   };

signals:
   void filename(QString);
   void editingFinished();

protected:
   void mouseDoubleClickEvent (QMouseEvent *evt);

private:
   QByteArray     m_ba;
   QString        m_pname;
   int            m_width;
};
#endif

#endif
