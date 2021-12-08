#ifndef LOGBUILDER_H
#define LOGBUILDER_H

# include <QDebug>

# include "LogRegistery.h"

namespace QcjData
{
   class LogBuilder 
   {
   public:
      LogBuilder(QString cat_name, unsigned int levels = 1, QString description = QString())
      {
         LogRegistery::instance()->RegisterLog(cat_name, levels, description);
      }

   private:
   protected:
   };
}

#endif
