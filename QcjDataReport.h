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

#ifndef DATAREPORT_H
#define DATAREPORT_H

# include <QGridLayout>
# include <QMap>
# include <QPrinter>
# include <QSqlQuery>
# include <QString>
# include <QTextEdit>

# include "ui_QcjDataReport.h"
# include "QcjDataReportDocument.h"

class QcjDataReport : public QDialog
{
   Q_OBJECT

public:
   QcjDataReport(QWidget *pParent = NULL);
   bool     setReportFileName(QString fn, QMap<QString, QString> args);
   QString  errorString();
   static QMap<QString, QString> getReportList(QString dir_path);
   static QString findReportsPath(QStringList app_path, QString app_name);

public slots:
   void     previewReport();
   void     printReport();
//   void     done();

protected:

private:
   QMap<QString, QString>  inputArgs;
   QGridLayout             *gridLayout;
   QcjDataReportDocument   *report;
   Ui::QcjDataReport       ui;
};

#endif
