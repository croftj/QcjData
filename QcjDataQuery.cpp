#include "QcjDataQuery.h"

#include "QcjDataStatics.h"
#include <QDebug>
#include <QStringList>

namespace QcjDataQuery
{
   QString insertQuery(const QString &xmldef, const QMap<QString, QString> &field_map)
   {
      qDebug() << "field_map: " << field_map;
      QString rv = "insert into %table% (%fields%) values (%bindings%)";
      QStringList table_fields = pFormDef->getFieldNames(xmldef);
      qDebug() << "table_fields = " << table_fields;
      QString table = pFormDef->getTable(xmldef);
      QStringList insertFields;
      QStringList insertBindings;

      foreach (QString field, table_fields)
      {
         qDebug() << field << " in fieldMap?";
         if (field_map.contains(field))
         {
            qDebug() << "adding field";
            insertFields << field;
            insertBindings << ":" + field;
         }
      }
      rv = rv.replace("%table%", table);
      rv = rv.replace("%fields%", insertFields.join(","));
      rv = rv.replace("%bindings%", insertBindings.join(","));
      return(rv);
   }

   void bindFieldValues(QSqlQuery &query, const QMap<QString, QString> &field_map)
   {
      qDebug() << "field_map: " << field_map;
      foreach (QString field, field_map.keys())
      {
         qDebug() << "binding field " << field << "w/ " << field_map.value(field);
         query.bindValue(":" + field, field_map.value(field));
      }
   }
};
