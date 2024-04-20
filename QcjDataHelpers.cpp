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
#include <QBuffer>
#include <QCryptographicHash>
#include <QDebug>
#include <QFileDialog>
#include <QFileInfo>
#include <QRegularExpression>
#include <QRegularExpressionMatch>
#include <QStringList>
#include "QcjDataHelpers.h"
#include "QcjDataStatics.h"

//#define NO_PHOTO_SELECT

#ifndef NO_PHOTO_SELECT
#include "QcjPhotoSelect.h"
#include "QcjLib/CameraCaptureDialog.h"
#endif

namespace
{
   QString parseInitString(QString init)
   {
      QString rv;

      QStringList sl = init.split(':');
      if (sl.count() == 1)
      {
         rv = sl[0];
      }
      else
      {
         if (sl[0] == "config")
         {
            rv = pConfig->value(sl[1]).toString();
         }
      }
      return(rv);
   }
}

QRegularExpression QcjPhoneEdit::phoneRE;
QString QcjPhoneEdit::phoneFormat;

QcjPhoneEdit::QcjPhoneEdit(QWidget *parent) : QLineEdit (parent) 
{ 
   printf("QcjPhoneEdit::QcjPhoneEdit(): here()\n"); 
   fflush(stdout); 
   if (phoneRE.pattern() == QString())
   {
      phoneRE.setPattern(pConfig->value("PhoneParse", "(\\d{3})(\\d{3})(\\d{4})").toString());
   }
   phoneFormat = pConfig->value("PhoneFormat", "(%1) %2-%3" ).toString();
   printf("QcjPhoneEdit(): pattern: %s\n", qPrintable(phoneRE.pattern()));
   printf("QcjPhoneEdit(): format: %s\n", qPrintable(phoneFormat));
}  

QcjPhoneEdit::QcjPhoneEdit(const QString &contents, QWidget *parent) : QLineEdit(contents, parent) 
{ 
   printf("QcjPhoneEdit::QcjPhoneEdit(): here()\n"); 
   fflush(stdout); 
}

QString QcjPhoneEdit::text() const
{
   QString s = QLineEdit::text();
   QString rv;

#ifndef QT4_DESIGNER_PLUGIN
   printf("QcjPhoneEdit::text(): started with |%s|\n", (const char*)s.toLocal8Bit());
   fflush(stdout);

   s.replace(QRegExp("[() -.]"), "");
   QRegularExpressionMatch match = phoneRE.match(s);
   for (int x = 1; x < match.lastCapturedIndex() + 1; x++)
   {
      if (false && rv.length() > 0)
      {
         rv += ' ';
      }
      rv += match.captured(x);
   }
   if (rv.length() == 0)
   {
      rv = s;
   }
//   s.replace(QRegExp("[\\$,]"), "");
   printf("QcjPhoneEdit::text(): returned |%s|\n", (const char*)s.toLocal8Bit());
   fflush(stdout);
#endif
   return(rv);
}

void QcjPhoneEdit::setText(const QString f) 
{
   QString str = f;
   printf("QcjPhoneEdit::SetText(): enter\n");
   fflush(stdout);
   str.replace(QRegExp("[() -.]"), "");

//   str.replace(QRegExp("[\\$,]"), "");
   QLineEdit::setText(formatPhoneNumber(str));
   printf("QcjPhoneEdit::SetText(): set to |%s|\n", qPrintable(QLineEdit::text()));
   fflush(stdout);
}

QString QcjPhoneEdit::formatPhoneNumber(QString str)
{
   QString rv(phoneFormat);
#ifndef QT4_DESIGNER_PLUGIN
   printf("QcjPhoneEdit::formatPhoneNumber(): str = %s\n", qPrintable(str));
   str.replace(QRegExp("[() -.]"), "");
   QRegularExpressionMatch match = phoneRE.match(str);
   if (match.lastCapturedIndex() > 0)
   {
      for (int x = 1; x < match.lastCapturedIndex() + 1; x++)
      {
         rv = rv.arg(match.captured(x));
      }
   }
   else
   {
      rv = str;
   }
   fflush(stdout);
   printf("QcjPhoneEdit::formatCurrency(): exit(%s)\n", qPrintable(rv));
   fflush(stdout);
#endif
   return(rv);
}

QcjMoneyEdit::QcjMoneyEdit(QWidget *parent) : QLineEdit (parent) 
{ 
   qDebug() << "creating validator";
   QRegExpValidator *v = new QRegExpValidator(QRegExp("\\$?[0-9,]*\\.?[0-9]{0,2}"), this);
   setValidator(v);
   setAlignment(Qt::AlignRight);
   connect(this, SIGNAL(editingFinished()), this, SLOT(formatText()), Qt::UniqueConnection);
   fflush(stdout); 
}

QcjMoneyEdit::QcjMoneyEdit(const QString &contents, QWidget *parent) : QLineEdit(contents, parent) 
{ 
   printf("Moneyedit::QcjMoneyEdit(): here()\n"); 
   fflush(stdout); 
}

void QcjMoneyEdit::formatText()
{
   QString txt = text();
   qDebug() << "Enter: text =" << txt;
   QLineEdit::setText(formatCurrency(txt));
   qDebug() << "exit";
}

QString QcjMoneyEdit::text() const
{
   QString s = QLineEdit::text();
#ifndef QT4_DESIGNER_PLUGIN
   printf("QcjMoneyEdit::text(): started with |%s|\n", (const char*)s.toLocal8Bit());
   fflush(stdout);

   QString exp;
   QString cur = pConfig->value("Currency", "$").toString();
   if ( cur == "$" ) 
      cur = "\\$";
   exp = "[" + QString(pLocale->groupSeparator()) + cur + "]";
   printf("QcjMoneyEdit::SetText(): exp = |%s|\n", qPrintable(exp));
   s.replace(QRegExp(exp), "");

//   s.replace(QRegExp("[\\$,]"), "");
   printf("QcjMoneyEdit::text(): returned |%s|\n", (const char*)s.toLocal8Bit());
   fflush(stdout);
#endif
   return(s);
}

void QcjMoneyEdit::setText(const QString f) 
{
   QString str = f;
   printf("QcjMoneyEdit::SetText(): enter\n");
   fflush(stdout);

   QString exp;
   QString cur = pConfig->value("Currency", "$").toString();
   if ( cur == "$" ) 
      cur = "\\$";
   exp = "[" + QString(pLocale->groupSeparator()) + cur + "]";
   printf("QcjMoneyEdit::SetText(): exp = |%s|\n", qPrintable(exp));
   str.replace(QRegExp(exp), "");

//   str.replace(QRegExp("[\\$,]"), "");
   QLineEdit::setText(formatCurrency(str));
   printf("QcjMoneyEdit::SetText(): started with |%s|\n", (const char*)formatCurrency(f).toLocal8Bit());
   fflush(stdout);
}

QString QcjMoneyEdit::formatCurrency(QString str)
{
   QString s, s1, s2;
   printf("QcjMoneyEdit::formatCurrentcy(): Enter, str = |%s|\n", qPrintable(str));
   fflush(stdout);
   str.remove("$");
   str.remove(",");
   printf("QcjMoneyEdit::formatCurrentcy(): converting |%s| to double\n", qPrintable(str));
   double f = str.toDouble();

#ifndef QT4_DESIGNER_PLUGIN
   printf("QcjMoneyEdit::formatCurrentcy(): f = %15.2f\n", f);
   fflush(stdout);
   s = pLocale->toString(f, 'f', 2);
   printf("QcjMoneyEdit::formatCurrentcy(): s = %s\n", qPrintable(s));
   s = pConfig->value("Currency", "$").toString().replace("&nbsp;", " ") + s;

/*
   s.sprintf("%15.2f", f);
   printf("QcjMoneyEdit::formatCurrentcy(): s = %s\n", (const char*)s.toLocal8Bit());
   fflush(stdout);
   QRegExp re1("([0-9])([0-9]{3}\\.)");
   re1.setMinimal(true);
   if (re1.indexIn(s) >= 0)
   {
       s1 = re1.cap(1);
       s2 = re1.cap(2);
       printf("QcjMoneyEdit::formatCurrentcy(): s1 = |%s|, s2 = |%s|\n", (const char*)s1.toLocal8Bit(), (const char*)s2.toLocal8Bit());
       fflush(stdout);
       s.replace(re1, s1 + "," + s2);
   }

   QRegExp re2("([0-9])([0-9]{3},)");
   re2.setMinimal(true);

   if (re2.indexIn(s) >= 0)
   {
       s1 = re2.cap(1);
       s2 = re2.cap(2);
       s.replace(re2, s1 + "," + s2);
   }

   if (re2.indexIn(s) >= 0)
   {
       s1 = re2.cap(1);
       s2 = re2.cap(2);
       s.replace(re2, s1 + "," + s2);
   }
   if (re2.indexIn(s) >= 0)
   {
       s1 = re2.cap(1);
       s2 = re2.cap(2);
       s.replace(re2, s1 + "," + s2);
   }
   s.replace(QRegExp("^\\s*"), "");
   s = "$" + s;
*/
   printf("QcjMoneyEdit::formatCurrency(): exit(%s)\n", qPrintable(s));
   fflush(stdout);
#endif
   return(s);
}


QString QcjStringSelect::text() const
{
   QString s = currentText();
#ifndef QT4_DESIGNER_PLUGIN
   int i = currentIndex();
   QVariant v = itemData(i);
   if (  v != QVariant::Invalid) 
   {
      s = v.toString();
   }
   if ( s == "<NULL>") 
   {
      s = "";
   }
#endif
   return(s);
}

void QcjStringSelect::setText(QString s)
{
   int i = 0;
#ifndef QT4_DESIGNER_PLUGIN
   printf("QcjStringSelect::setText(): Enter, test = |%s|\n", qPrintable(s));
   if ( s == "" ) 
   {
      s = "<NULL>";
   }
   if ((i = findData(QVariant(s))) >= 0)
   {
      printf("QcjStringSelect::setText(): Found data, index = %d\n", i);
      setCurrentIndex(i);
   }
   else if ((i = findText(s)) >= 0) 
   {
//      setSelected(i, true);
      printf("QcjStringSelect::setText(): Found text, index = %d\n", i);
      setCurrentIndex(i);
      setItemText(i, s);
   }
   else 
   {
      printf("QcjStringSelect::setText(): Adding new item\n");
      insertItem(0, s);
      setCurrentIndex(0);
      setItemText(0, s);
   }
   printf("QcjStringSelect::setText(): Exit()\n");
#endif
}

void QcjStringSelect::initialize(QString selections)
{
   if ( selections != QString() ) 
   {
      selections = parseInitString(selections);
      QStringList namesList = selections.split( "," );
      addItems( namesList );
   }
}

void QcjStringSelect::initialize(QcjDataFields *field_def)
{
   printf("QcjStringSelect::initialize(): init = |%s|\n", (const char*)field_def->init.toLocal8Bit());
   printf("QcjStringSelect::initialize(): defvalue = |%s|\n", (const char*)field_def->defvalue.toLocal8Bit());
#ifndef QT4_DESIGNER_PLUGIN
   clear();
   QString selections;
   if ( (selections = field_def->init) != QString() ) 
   {
      initialize(selections);
   }
   if (field_def->defvalue != QString())
   {
      setCurrentText(field_def->defvalue);
   }
#endif
}

QString QcjSqlModel::text() const
{
   QString s = currentText();
   printf("SqlModel::text(): currentText = |%s|\n", qPrintable(s));
#ifndef QT4_DESIGNER_PLUGIN
   int i = currentIndex();
   printf("SqlModel::text(): currentIndex = |%d|\n", i);
   QVariant v = itemData(i);
   if (  v != QVariant::Invalid) 
   {
      printf("SqlModel::text(): Have valid variant\n");
      s = v.toString();
   }
   QRegExp re("([0-9]+) - .*");
   re.setMinimal(true);
   if ( re.indexIn(s) >= 0) 
   {
      s = re.cap(1);
      printf("SqlModel::text(): re matched\n");
   }
   if ( s == "<NULL>") 
   {
      s = "";
   }
#endif
   printf("SqlModel::text(): s = |%s|\n", qPrintable(s));
   return(s);
}

void QcjSqlModel::setText(QString s)
{
   int i = 0;
#ifndef QT4_DESIGNER_PLUGIN
   printf("QcjSqlModel::setText(): s = |%s|\n", (const char*)s.toLocal8Bit());
   if ( s == "" ) 
   {
      printf("QcjSqlModel::setText(): Have NULL text\n");
      if ((i = findData("")) < 0)
      {
         insertItem(0, "");
         setCurrentIndex(0);
         setItemText(0, "");
      }
      else 
      {
         setCurrentIndex(0);
      }
   }
   else if ((i = findData(QVariant(s + " - "), Qt::MatchStartsWith)) >= 0)
   {
      printf("QcjSqlModel::setText(): Found line at %d\n", i);
      setCurrentIndex(i);
   }
   else if ((i = findText(s + " - ", Qt::MatchStartsWith)) >= 0) 
   {
//      setSelected(i, true);
      printf("QcjSqlModel::setText(): Found line at %d\n", i);
      setCurrentIndex(i);
      setItemText(i, s);
   }
   else 
   {
      printf("QcjSqlModel::setText(): String not found\n");
      if ((i = findData("")) < 0)
      {
         printf("QcjSqlModel::setText(): Inserting blank\n");
         insertItem(0, "");
         setCurrentIndex(0);
         setItemText(0, "");
      }
      else 
      {
         printf("QcjSqlModel::setText(): found blank at %d\n", i);
         setCurrentIndex(i);
      }
   }
#endif
}

void QcjSqlModel::initialize(QString s)
{
   printf("QcjSqlModel::initialize(): s = |%s|\n", (const char*)s.toLocal8Bit());
#ifndef QT4_DESIGNER_PLUGIN
   clear();
   if ( s != QString() ) 
   {
      QStringList namesList = s.split( "," );
      addItems( namesList );
   }
#endif
}



QString QcjYesNoSelect::text() const
{
   QString rv;
#ifndef QT4_DESIGNER_PLUGIN
   if ( currentText() == "Yes" ) 
      rv = "Y";
   else
      rv = "N";
#endif
   return(rv);
}

void QcjYesNoSelect::setText(QString t)
{
   QString s;
#ifndef QT4_DESIGNER_PLUGIN
   if ( t.toUpper() == "Y" ) 
      s = "Yes";
   else
      s = "No";
   int i = findText(s);
   setCurrentIndex(i);
   setItemText(i, s);
#endif
}

void QcjYesNoSelect::initialize()
{
#ifndef QT4_DESIGNER_PLUGIN
   QStringList typeList;
   typeList += "";
   typeList += "Yes";
   typeList += "No";
   clear();
   addItems(typeList);
#endif
}

int QcjTrueFalseSelect::value() const
{
   int rv;
#ifndef QT4_DESIGNER_PLUGIN
   if ( currentText() == "Yes" ) 
      rv = 1;
   else
      rv = 0;
#endif
   return(rv);
}

void QcjTrueFalseSelect::setValue(int val)
{
   QString s;
#ifndef QT4_DESIGNER_PLUGIN
   if ( val ) 
      s = "Yes";
   else
      s = "No";
   int i = findText(s);
   setCurrentIndex(i);
   setItemText(i, s);
#endif
}

void QcjTrueFalseSelect::setText(QString t)
{
   QString s;
#ifndef QT4_DESIGNER_PLUGIN
   if ( t.toUpper() == "Y" ) 
      s = "Yes";
   else
      s = "No";
   int i = findText(s);
   setCurrentIndex(i);
   setItemText(i, s);
#endif
}

void QcjTrueFalseSelect::initialize()
{
#ifndef QT4_DESIGNER_PLUGIN
   QStringList typeList;
   typeList += "";
   typeList += "Yes";
   typeList += "No";
   clear();
   addItems(typeList);
#endif
}


QString QcjTimeEntry::get() const
{
   return(time().toString("hh:mm"));
}

void QcjTimeEntry::set(QString t)
{
   setTime(QTime::fromString(t, "hh:mm"));
}

QString QcjDateEntry::get() const
{
   QString rv;
#ifndef QT4_DESIGNER_PLUGIN
   printf("QcjDateEntry::get(): Enter: %d\n", type);
   fflush(stdout);
   QDate d = date();
   switch (type) 
   {
      case MonthBeg:
         d.setDate(d.year(), d.month(), 1);
         break;

      case MonthEnd:
         d.setDate(d.year(), d.month(), d.daysInMonth());
         break;   

      case YearBeg:
         d.setDate(d.year(), 1, 1);
         break;

      case YearEnd:
         d.setDate(d.year(), 12, 31);
         break;   

      default:
         break;
   }
   rv = d.toString(Qt::ISODate);
   printf("QcjDateEntry::get(): Exit |%s|\n", (const char*)rv.toLocal8Bit());
   fflush(stdout);
#endif
   return(rv);
}

void QcjDateEntry::set(QString t)
{
   printf("QcjDateEntry::set(): Entry  |%s|\n", (const char*)t.toLocal8Bit());
#ifndef QT4_DESIGNER_PLUGIN
   QRegExp reDate("([0-9][0-9][0-9][0-9])-([0-9][0-9])-([0-9][0-9])");
   int pos;
   if ((pos = reDate.indexIn(t)) >= 0)
   {
      QString year = reDate.cap(1);
      QString mon = reDate.cap(2);
      QString day = reDate.cap(3);
      printf("QcjDateEntry::set(): %s/%s/%s\n", (const char*)mon.toLocal8Bit(), (const char*)day.toLocal8Bit(), (const char*)year.toLocal8Bit());
      QDate d(year.toInt(), mon.toInt(), day.toInt());
      setDate(d);
   }
   printf("QcjDateEntry::set(): Exit pos = %d\n", pos);
#endif
}

QString QcjTimestampEntry::get() const
{
   QString rv;
#ifndef QT4_DESIGNER_PLUGIN
   printf("QcjTimestampEntry::get(): Enter\n");
   rv = dateTime().toString(Qt::ISODate);
   printf("QcjTimestampEntry::get(): Exit |%s|\n", (const char*)rv.toLocal8Bit());
#endif
   return(rv);
}

void QcjTimestampEntry::set(QString t)
{
   printf("QcjTimestampEntry::set(): Entry  |%s|\n", (const char*)t.toLocal8Bit());
#ifndef QT4_DESIGNER_PLUGIN
   QRegExp reDate("([0-9][0-9][0-9][0-9])-([0-9][0-9])-([0-9][0-9])T([0-9][0-9]):([0-9][0-9]):([0-9][0-9])");
   int pos;
   if ((pos = reDate.indexIn(t)) >= 0)
   {
      QString year = reDate.cap(1);
      QString mon = reDate.cap(2);
      QString day = reDate.cap(3);
      QString hour = reDate.cap(4);
      QString min = reDate.cap(5);
      QString sec = reDate.cap(6);
      printf("QcjTimestampEntry::set(): %s/%s/%s\n", (const char*)mon.toLocal8Bit(), (const char*)day.toLocal8Bit(), (const char*)year.toLocal8Bit());
      QDate d(QDate(year.toInt(), mon.toInt(), day.toInt()));
      QTime t(QTime(hour.toInt(), min.toInt(), sec.toInt()));
      setDate(d);
      setTime(t);
   }
   printf("QcjTimestampEntry::set(): Exit pos = %d\n", pos);
#endif
}


QString QcjTextBlockEdit::text() const
{
   printf("QcjTextBlockEdit::text(): Enter\n");
   QString s = toPlainText();
#ifndef QT4_DESIGNER_PLUGIN
#endif
   printf("QcjTextBlockEdit::text(): exit with |%s|\n", (const char*)s.toLocal8Bit());
   return(s);
}

void QcjTextBlockEdit::setText(QString s)
{
#ifndef QT4_DESIGNER_PLUGIN
   printf("QcjTextBlockEdit::text(): Enter- setting text |%s|\n", (const char*)s.toLocal8Bit());
   setPlainText(s);
   printf("QcjTextBlockEdit::text(): exit\n");
#endif
}

#ifndef NO_PHOTO_SELECT
QString QcjPhotoEntry::md5sum() const
{
   QString sum;
   sum = QString(QCryptographicHash::hash(m_ba, QCryptographicHash::Md5).toHex().constData());
   return(sum);
}

bool QcjPhotoEntry::match(QByteArray ba) const
{
   QString sum1;
   QString sum2;
   sum1 = QString(QCryptographicHash::hash(m_ba, QCryptographicHash::Md5).constData());
   sum2 = QString(QCryptographicHash::hash(ba, QCryptographicHash::Md5).constData());
   return(sum1 == sum2);
}

QByteArray QcjPhotoEntry::get() const
{
   printf("QcjPhotoEntry::get(): Enter/Exit, ba size = %d\n", m_ba.size());
   fflush(stdout);
   qDebug() << "md5sum: " << md5sum();
   return(m_ba);
}

void QcjPhotoEntry::set(QByteArray ba)
{
   printf("QcjPhotoEntry::set(): Enter, pic size = %d\n", ba.size());
   fflush(stdout);
   m_ba = ba;
   if ( ba.size() == 0 ) 
   {
      setText("<html>Double click</html>");
   }
   else 
   {
      QPixmap pm;
      pm.loadFromData(m_ba);
      pm = pm.scaledToWidth(m_width);
      setPixmap(pm);
   }
   qDebug() << "md5sum: " << md5sum();
   printf("QcjPhotoEntry::set(): Exit\n");
   fflush(stdout);
}

void QcjPhotoEntry::setWidth(int width)
{
   m_width = width;
}

void QcjPhotoEntry::mouseDoubleClickEvent(QMouseEvent*) 
{
   printf("QcjPhotoEntry::mouseDoubleClickEvent(): Enter\n");
   fflush(stdout);
   this->setFocus();
   QcjLib::CameraCaptureDialog photoDlg(this);
   QcjPhotoSelect dlg(pConfig->value("PhotoFolder", ".").toString(), this);
   dlg.restoreState(pConfig);
   if ( m_ba.size() > 0 ) 
   {
      QPixmap pm;
      pm.loadFromData(m_ba);
      dlg.setPixmap(pm);
   }
   if ( dlg.exec() == QDialog::Accepted ) 
   {
      QString fn = dlg.fileName();
      printf("QcjPhotoEntry::mouseDoubleClickEvent(): have fn of |%s|\n", qPrintable(fn));
      QFileInfo fi(fn);
      QString path = fi.absolutePath();
      printf("QcjPhotoEntry::mouseDoubleClickEvent(): have path of |%s|, suffix |%s|\n", qPrintable(path), qPrintable(fi.suffix()));
      pConfig->setValue("PhotoFolder", path);
      QPixmap pm = dlg.pixmap();
      m_ba.clear();
      QBuffer buf(&m_ba);
      if ( pm.save(&buf, qPrintable(fi.suffix())) )
      {
         set(m_ba);
         printf("QcjPhotoEntry::mouseDoubleClickEvent(): m_ba size = %d\n", m_ba.size());
         printf("QcjPhotoEntry::mouseDoubleClickEvent(): Emitting filename\n");
         fflush(stdout);
         emit filename(fi.fileName());
         emit editingFinished();
         printf("QcjPhotoEntry::mouseDoubleClickEvent(): m_ba size = %d\n", m_ba.size());
         fflush(stdout);
         dlg.saveState(pConfig);
      }
      else
         printf("QcjPhotoEntry::mouseDoubleClickEvent(): Error saving to buffer, ignoring!!\n");
   }
   this->setFocus();
   printf("QcjPhotoEntry::mouseDoubleClickEvent(): Exit\n");
   fflush(stdout);
}
#endif
