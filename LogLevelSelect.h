#ifndef LOGLEVELSELECT_H
#define LOGLEVELSELECT_H

# include <QComboBox>
# include <QVariant>
# include <QWidget>

# include "LogRegistery.h"

namespace ShureQtLibrary
{
   class LogLevelSelect : public QComboBox 
   {
      Q_OBJECT

   public:
      LogLevelSelect(QString cat_name, QWidget *parent = NULL) : QComboBox(parent)
      {
         addItem("Off", QVariant(0));
         unsigned int levels = LogRegistery::instance()->LogMaximumLevel(cat_name);
         for (unsigned int level = 1;  level <= levels; level++) 
         {
            addItem(QString::number(level), QVariant(level));
         }
         setCurrentIndex(LogRegistery::instance()->LogLevel(cat_name));
      }

   protected:
   private:
   };
}

#endif
