#ifndef LOGLEVELWIDGET_H
#define LOGLEVELWIDGET_H

# include <QStringList>
# include <QTableWidget>

# include "LogLevelSelect.h"
# include "LogRegistery.h"

namespace ShureQtLibrary
{
   class LogLevelWidget : public QTableWidget 
   {
      Q_OBJECT

   public:
      LogLevelWidget(QWidget *parent = NULL) : QTableWidget(parent)
      {
//         LogRegistery::instance()->RegisterLog("log_lvl_widget", 2);
         setColumnCount(3);
         QStringList header_list;
         header_list << HEADER_LOG_NAME << HEADER_LOG_DESCR << HEADER_LEVEL_SELECT;
         setHorizontalHeaderLabels(header_list);

      }

   public slots:
      void ApplySettings()
      {
         for (int row = 0; row < rowCount(); row++) 
         {
            LogLevelSelect *level_select = dynamic_cast<LogLevelSelect*>(cellWidget(row, COL_LEVEL_SELECT));
            if ( level_select != NULL ) 
            {
               unsigned int level = level_select->currentText().toUInt();
               LogRegistery::instance()->SetLogLevel(item(row, COL_LOG_NAME)->text(),  level);
            }
         }
      }

      void Initialize()
      {
         QStringList cat_names = LogRegistery::instance()->LogList();

         cat_names.sort();
         int row = 0;
//         clear();
         setColumnCount(3);
         foreach (QString cat_name, cat_names)
         {
            if ( cat_name.isEmpty() ) 
            {
               cat_name = "default";
            }
            qDebug() << __FUNCTION__ << "cat_name: " << cat_name;
            setRowCount(++row);
            insertRow(rowCount());
            
            QTableWidgetItem *item;
            Qt::ItemFlags item_flags = Qt::ItemIsEnabled;

            setCellWidget(row - 1, COL_LEVEL_SELECT, new LogLevelSelect(cat_name));
          
            item = new QTableWidgetItem(LogRegistery::instance()->LogDescription(cat_name));
            item->setFlags(item_flags);
            setItem(row - 1, COL_LOG_DESCR, item);

            item = new QTableWidgetItem(cat_name);
            item->setFlags(item_flags);
            setItem(row - 1, COL_LOG_NAME, item);
         }
      }

   protected:
   private:
      static const int  COL_LEVEL_SELECT;
      static const int  COL_LOG_DESCR;
      static const int  COL_LOG_NAME;

      static const QString  HEADER_LEVEL_SELECT;
      static const QString  HEADER_LOG_DESCR;
      static const QString  HEADER_LOG_NAME;
   };
}
#endif

