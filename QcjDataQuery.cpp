#include "QcjDataQuery.h"

#include "QcjDataStatics.h"
#include "QcjLib/ButtonBoxFrame.h"
#include "QcjLib/Exceptions.h"
#include "QcjLib/Types.h"
#include "QcjLib/WidgetUtils.h"
#include <QCryptographicHash>
#include <QDebug>
#include <QSqlDriver>
#include <QSqlError>
#include <QSqlField>
#include <QSqlRecord>
#include <QStringList>

using namespace QcjLib;
using namespace WidgetUtils;

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

   QString updateQuery(const QString &xmldef, const QStringList &fields)
   {
      qDebug() << "fields: " << fields;
      QString rv = "update %table% set %bindings%";
      QString table = pFormDef->getTable(xmldef);
      QStringList bindings;
      QString indexField = pFormDef->getIndexField(xmldef);
      QStringList table_fields = pFormDef->getFieldNames(xmldef);

      foreach (QString field, fields)
      {
         qDebug() << field << " in fieldMap?";
         if (field != indexField && table_fields.contains(field))
         {
            QString field_def = field + " = :" + field;
            qDebug() << "adding field:" << field_def;
            bindings << field_def;
         }
      }
      rv = rv.replace("%table%", table);
      rv = rv.replace("%bindings%", bindings.join(", "));
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

   void updateRecord(QSqlTableModel &model, int row, const QcjLib::VariantHash &fields)
   {
      QSqlRecord rec;
      rec = model.record(row);
      qDebug() << "row = " << row << ", rec = " << rec;

      for (int idx = 0; idx < rec.count(); idx++)
      {
         if (fields.contains(rec.fieldName(idx)))
         {
            if (fields.value(rec.fieldName(idx)).type() == QVariant::StringList)
            {
               rec.setValue(idx, fields.value(rec.fieldName(idx)).toStringList().join(","));
               qDebug().noquote() << "rec field: " << rec.fieldName(idx) << "idx: " << idx
                  << ", value: " << fields.value(rec.fieldName(idx)).toStringList().join(",");
            }
            else
            {
               rec.setValue(idx, fields.value(rec.fieldName(idx)));
               qDebug().noquote() << "rec field: " << rec.fieldName(idx) << "idx: " << idx
                  << ", value: " << fields.value(rec.fieldName(idx));
            }
         }
      }
      qDebug().noquote() << "sql = " << pDb->driver()->sqlStatement(QSqlDriver::UpdateStatement,
                                                          model.tableName(),
                                                          rec,
                                                          false);
      if ( ! model.setRecord(row, rec))
      {
         QString msg = model.database().lastError().text();
         throw QcjLib::SqlException(msg);
      }
   }

   void insertRecord(QSqlTableModel &model, const VariantHash &fields)
   {
      QSqlRecord rec(model.record());
      qDebug() << "rec = " << rec;

      for (int idx = 0; idx < rec.count(); idx++)
      {
         qDebug().noquote() << "rec field: " << rec.fieldName(idx) << "idx: " << idx;
         if (fields.contains(rec.fieldName(idx)))
         {
            if (fields.value(rec.fieldName(idx)).type() == QVariant::StringList)
            {
               rec.setValue(idx, fields.value(rec.fieldName(idx)).toStringList().join(","));
            }
            else
            {
               rec.setValue(idx, fields.value(rec.fieldName(idx)));
            }
         }
         else if (rec.fieldName(idx) == "ident")
         {
            qDebug() << "have ident field, idx = " << idx;
            QString def_value;
            if (rec.field(idx).defaultValue() != QVariant())
            {
               def_value = rec.field(idx).defaultValue().toString();
            }
            else
            {
               qDebug() << "Building index query manually";
               def_value = QString("nextval('") + model.tableName() + "_"
                         + rec.fieldName(idx) + "_seq'::regclass)";
            }
            qDebug() << "def_value = " << def_value;
            QString sql = "select " + def_value;
            qDebug().noquote() << "ident query sql: " << sql;
            QSqlQuery q1(sql);
            if ( q1.isActive() )
            {
               if ( q1.next() ) 
               {
                  qDebug() << "ident (idx = " << idx << ") value: " << q1.value(0).toString();
                  rec.setValue(idx, q1.value(0).toString());
               }
            }
         }
         else if (rec.field(idx).defaultValue().isValid())
         {
            QVariant defval = rec.field(idx).defaultValue();
            rec.setValue(idx, defval);
         }
         else
         {
            qDebug() << "removing field: " << rec.fieldName(idx);
            rec.remove(idx);
            idx--;
         }
      }
      qDebug().noquote() << "sql = " << pDb->driver()->sqlStatement(QSqlDriver::InsertStatement,
                                                          model.tableName(),
                                                          rec,
                                                          false);
      if ( ! model.insertRecord(-1, rec))
      {
         QString msg = model.database().lastError().text();
         qDebug() << "insetRecord() returned error: " << msg; 
         throw QcjLib::SqlException(msg);
      }
   }

   int getLastInsertIndex(const QString &xmldef)
   {
      int idx = 0;
      QString dbTable = pFormDef->getTable(xmldef);
      QString indexname = pFormDef->getIndexField(xmldef);

      if ( pDb->driverName() == "QPSQL" ||
           pDb->driverName() == "QMYSQL" || 
           pDb->driverName() == "QSQLITE") 
      {
         QString sql;

         if ( pDb->driverName() == "QPSQL" )
            sql = "select currval('" + dbTable + "_" + indexname + "_seq" + "')";
         else if ( pDb->driverName() == "QMYSQL" )
            sql = "select LAST_INSERT_ID() from " + dbTable + " limit 1";
         else
            sql = "select last_insert_rowid() from " + dbTable + " limit 1";

         qDebug() << "sql = " << sql;
         QSqlQuery q2(*pDb);
         q2.prepare(sql);
         if (q2.exec())
         {
            if ( q2.next() ) 
            {
               idx = q2.record().field(0).value().toInt();
            }
         }
      }
      qDebug() << "idx =" << idx << ", indexname = " << indexname;
      return(idx);   
   }

   QVariantHash getPrintableData(const VariantHash &fields)
   {
      QVariantHash rv;
      foreach (QString fn, fields.keys())
      {
//         qDebug() << "fn = " << fn;
         QVariant val = fields.value(fn);
         if (static_cast<QMetaType::Type>(val.type()) == QMetaType::QByteArray)
         {
            val = QVariant(QCryptographicHash::hash(val.toByteArray(), QCryptographicHash::Md5).toHex());
         }
//         qDebug() << "Adding " << fn << " = " << val;
         rv.insert(fn, val);
      }
      return(rv);
   }

   void setFormValue(const QString &fieldName, const QVariant &value, 
                                       QWidget *parent)
   {
      qDebug() << "Searching for widget for field: " << fieldName;
      QWidget *wdt = findChildByFieldName(parent, fieldName);
      if (wdt != nullptr)
      {
         if (isA(wdt, "QcjLib::ButtonBoxFrame"))
         {
            qDebug() << "Found ButtonBoxFrame";
            QStringList allChecked = value.toString().split(",");
            dynamic_cast<QcjLib::ButtonBoxFrame*>(wdt)->checkButtons(allChecked);
         }
         else if (isA(wdt, "QCheckBox"))
         {
            qDebug() << "Found CheckBox";
            bool checked = (value.toString() == "Y") ? true : false;
            dynamic_cast<QCheckBox*>(wdt)->setChecked(checked);
         }
         else if ( isA(wdt, "QcjPhotoEntry") ) 
         {
            qDebug() << "Found " << wdt->objectName();
            double imageScale = pConfig->value("ImageAdjust", QVariant(100)).toDouble() / 100;
            int width = 512 * imageScale;
            qDebug() << "Scaled image width:" << width;
            wdt->setFixedWidth(width);
            WidgetUtils::setWidgetValue(wdt, value);
         }
         else
         {
            qDebug() << "Found " << wdt->objectName();
            WidgetUtils::setWidgetValue(wdt, value);
         }
         wdt->adjustSize();
      }
   }
}
