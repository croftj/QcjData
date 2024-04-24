/*********************************************************************************
**
**   $Id$
**   Copyright (c) 2005-2024 Joe Croft <joe@croftj.net>
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
************************o*********************************************************/
#ifndef QCJDATA_DATA_QUERY
#define QCJDATA_DATA_QUERY
#include "QcjLib/Types.h"

#include <QMap>
#include <QSqlQuery>
#include <QSqlTableModel>
#include <QString>

namespace QcjDataQuery
{
   QString insertQuery(const QString &xmldef, const QMap<QString, QString> &field_map);
   void bindFieldValues(QSqlQuery &query, const QMap<QString, QString> &field_map);
   void insertRecord(QSqlTableModel &model, const QcjLib::VariantHash &fields);
   void setFormValue(const QString &fieldName, const QVariant &value, QWidget *parent);
   void updateRecord(QSqlTableModel &model, int row, const QcjLib::VariantHash &fields);
   void updateRecord(const QString &tableName, const QcjLib::VariantHash &fields);
   QString updateQuery(const QString &xmldef, const QStringList &fields);
   int getLastInsertIndex(const QString &xmldef);
   QVariantHash getPrintableData(const QcjLib::VariantHash &fields);
}

#endif
