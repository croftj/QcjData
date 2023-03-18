#ifndef QCJDATA_DATA_QUERY
#define QCJDATA_DATA_QUERY

#include <QMap>
#include <QSqlQuery>
#include <QString>

namespace QcjDataQuery
{
   QString insertQuery(const QString &xmldef, const QMap<QString, QString> &field_map);
   void bindFieldValues(QSqlQuery &query, const QMap<QString, QString> &field_map);
};

#endif
