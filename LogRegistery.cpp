# include "LogRegistery.h"

# include "LogBuilder.h"

using namespace QcjData;

static LogBuilder mylog("default", 1, "Catches the undefined debug messages");

LogRegistery* LogRegistery::m_instance = NULL;

/********************************************************************//*
**   LogRegistery::LogRegistery(QObject *parent) constructor
**   
**   Function  to  build the full name of the log which is comprised
**   of its name and level
**   
**   Parameters
**   
**   parent pointer to the parent QObject
**   
**   Returns N/A
***********************************************************************/
LogRegistery::LogRegistery(QObject *parent) : QObject(parent), m_defaultCategory(new QLoggingCategory("default"))
{
}

/********************************************************************//*
**   void  LogRegistery::RegisterLog(QString  log_name, unsigned int levels, QString description)
**   
**   Function to define the actual log categories to be used for the
**   requested log
**   
**   Parameters
**   
**   log_name     Name of the log
**   
**   levels       Maximum number of levels to define for this log
**   
**   description  String describint the log.
**   
**   Returns N/A
***********************************************************************/
void LogRegistery::RegisterLog(QString log_name, unsigned int levels, QString description)
{
//   std::cout << __FUNCTION__ <<  " log_name: " << qPrintable(log_name) << std::endl;
//   std::cout << __FUNCTION__ <<  " log_name: " << std::endl;
//   m_logLevels.insert(log_name, levels);
   if ( ! m_logLevels.contains(log_name) ) 
   {
      m_logLevels.insert(log_name, 1);
   }

   m_logDescriptions.insert(log_name, description);

   m_logMaxLevels.insert(log_name, levels);
   for (unsigned int level = 1; level <= levels; level++) 
   {
      QString full_name = BuildName(log_name, level);
      char *name_buf = new char[strlen(qPrintable(full_name)) + 2];
      strncpy(name_buf, qPrintable(full_name), strlen(qPrintable(full_name)) + 1);
//      std::cout << __FUNCTION__ << " full_name: " << name_buf << std::endl;

//      std::cout << __FUNCTION__ << " full_name: " << std::endl;
      QLoggingCategory *category = new QLoggingCategory(name_buf);
//      std::cout << __FUNCTION__ <<  " cat->name(" << (unsigned long)category << "): " << category->categoryName() << " (" << (unsigned long long)(category->categoryName()) << ")" << std::endl;
      m_logMap.insert(full_name, category);
//      foreach (QString name, m_logMap.keys())
//      {
//         QLoggingCategory *cat = m_logMap.value(name);
//         std::cout << __FUNCTION__ << " cat(" << (unsigned long)cat << "): " << qPrintable(name) << ", cat->name: " << (unsigned long long)(cat->categoryName()) << std::endl;
//      }
   }
//         emit LogRegistered();
}

/********************************************************************//*
**   Function  saves the settings for each of the individual logs to
**   QSettings
**   
**   Returns N/A
***********************************************************************/
void LogRegistery::SaveLogSettings()
{
   QSettings settings;

   QString log_settings;
   foreach (QString log_name, m_logLevels.keys())
   {
      QString descr = m_logDescriptions.value(log_name);
      QString level = QString::number(m_logLevels.value(log_name));
      log_settings += (log_settings.size() > 0) ? "," : "";
      log_settings += log_name + ":" + descr + ":" + level;
   }
   settings.setValue(LOG_STATUS, log_settings);
}

QLoggingCategory *QcjData::log(QString log_name, unsigned int level)
{
   return(LogRegistery::instance()->category(log_name, level));
}



