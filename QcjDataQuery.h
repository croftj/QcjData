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
