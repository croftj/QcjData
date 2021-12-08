#ifndef LOGREGISTERY_H
#define LOGREGISTERY_H

# include <QLoggingCategory>
# include <QMap>
# include <QSettings>
# include <QString>

#include <iostream>

/********************************************************************//*
**   @file LogRegistry.h
**   
**   PROJECT ShureQtLibrary
**   
**   Description:  This class registers a log definition giving it a
**   name,  description and the number of levels that can be set. It
**   also  is  responsible for saving and restoring logging settings
**   for each of the individual logs that are registered.
**
**   It  is  expected  to  be onle one instance of this class in the
**   application.
***********************************************************************/

namespace QcjData
{
   QLoggingCategory *log(QString category_name, unsigned int level = 1);

   typedef QMap<QString, QString> LogDescriptionMap_t;
   typedef QMap<QString, unsigned int> LogLevelsMap_t;
   typedef QMap<QString, QLoggingCategory*> LogRegisteryMap_t;

   static const QString MSG_DEBUG_TYPE       ("debug");
   static const QString MSG_INFO_TYPE        ("info");
   static const QString MSG_WARNING_TYPE     ("warning");
   static const QString MSG_CRITICAL_TYPE    ("critical");

   static const QString LOG_CONSOLE_ENABLE   ("LogConsoleEnable");
   static const QString LOG_VIEW_ENABLE      ("LogViewEnable");
   static const QString LOG_FILE_ENABLE      ("LogFileEnable");
   static const QString LOG_FILE_NAME        ("LogFileName");
   static const QString LOG_STATUS           ("LogStatus");

   class LogRegistery : public QObject 
   {
      Q_OBJECT

   public:
      LogRegistery(QObject *parent = NULL);
      void SaveLogSettings();

//      ~LogRegistery();

      /********************************************************************//*
      **   
      **   static LogRegistery* instance()
      **   
      **   Creates  an instance of this class if it does not already
      **   exists.
      **   
      **   Return: Pointer to the instance.
      ***********************************************************************/
      static LogRegistery* instance()
      {
         if ( m_instance == NULL ) 
         {
            m_instance = new LogRegistery();
         }
         return(m_instance);
      }

      /********************************************************************//*
      **   Reads  the  settings  from  QSettings for the logging and
      **   configures each of the loggers defined.
      ***********************************************************************/
      void RestoreLogSettings()
      {
         QSettings settings;
         QStringList log_info = settings.value(LOG_STATUS, "").toString().split(",");
         foreach (QString log_setting, log_info)
         {
            QStringList setting = log_setting.split(":");
            QString log_name = setting[0];
            QString cat_desc = (setting.count() > 1) ? setting[1] : "";
            QString cat_levl = (setting.count() > 2) ? setting[2] : "0";

            m_logLevels.insert(log_name, cat_levl.toInt()); 
            m_logDescriptions.insert(log_name, cat_desc); 
         }
      }

      void RegisterLog(QString log_name, unsigned int levels = 1, QString description = QString());

      /********************************************************************//*
      **
      **   QLoggingCategory *category(QString log_name, unsigned int level = 1)
      **
      **   Returns  a pre-defined logging category for use by the Qt
      **   logging infrastructure.
      **   
      **   Arguments
      **   
      **   log_name     name of the log. 
      **   level        Integer of the level
      **   
      **   Returns      pointer to the QLoggingCategory for this log.
      **   If  the  category does not exist, a default category will
      **   be returned.
      ***********************************************************************/
      QLoggingCategory *category(QString log_name, unsigned int level = 1)
      {

//         foreach (QString name, m_logMap.keys())
//         {
//            QLoggingCategory *cat = m_logMap.value(name);
//            std::cout << __FUNCTION__ << " cat: " << qPrintable(name) << ", cat->name: " << cat->categoryName() << std::endl;
//         }

         QString full_name = BuildName(log_name, level);
//         std::cout << __FUNCTION__ << " full_name: " << qPrintable(full_name) << std::endl;
         if ( m_logMap.contains(full_name) ) 
         {
//            std::cout << __FUNCTION__ << " found it!!" << std::endl;
            return(m_logMap.value(full_name));
         }
         else 
         {
            return(m_defaultCategory);
         }
      }

      
      /********************************************************************//*
      **   
      **   QString LogDescription(QString log_name)
      **   
      **   Function returning the description of the requested log.
      **   
      **   Parameters
      **   
      **   log_name Name of the log
      **   
      **   Returns QString with the description
      ***********************************************************************/
      QString LogDescription(QString log_name)
      {
         QString rv;
         if ( m_logDescriptions.contains(log_name) ) 
         {
            rv = m_logDescriptions.value(log_name);
         }
         return(rv);
      }

      /********************************************************************//*
      **   unsigned int LogLevel(QString log_name)
      **   
      **   Function  returning  the  logging level for the requested
      **   log.
      **   
      **   Parameters
      **   
      **   log_name Name of the log
      **   
      **   Returns unsigned int with the loging level
      ***********************************************************************/
      unsigned int LogLevel(QString log_name)
      {
         unsigned int rv = 1;
         if ( m_logLevels.contains(log_name) ) 
         {
            rv = m_logLevels.value(log_name);
         }
         return(rv);
      }

      
      /********************************************************************//*
      **   unsigned int LogMaximumLevel(QString log_name)
      **   
      **   Function  returning  the  maximum  logging  level for the
      **   requested log.
      **   
      **   Parameters
      **   
      **   log_name Name of the log
      **   
      **   Returns unsigned int with the loging level
      ***********************************************************************/
      unsigned int LogMaximumLevel(QString log_name)
      {
         unsigned int rv = 0;
         if ( m_logMaxLevels.contains(log_name) ) 
         {
            rv = m_logMaxLevels.value(log_name);
         }
         return(rv);
      }

      /********************************************************************//*
      **   QStringList LogList()
      **   
      **   Function returning a list of all defined log names.
      **   
      **   Parameters
      **   
      **   log_name Name of the log
      **   
      **   Returns QStringList with a list of all logs
      ***********************************************************************/
      QStringList LogList()
      {
         QStringList rv;

         foreach (QString log, m_logLevels.keys())
         {
            QStringList sl(log.split(":"));
            rv << sl[0];
         }
         return(rv);
      }


      /********************************************************************//*
      **   void  SetLogLevel(QString  log_name, unsigned int level = 1)
      **   
      **   Dunction  to  set  the  logging  level  of the named log.
      **   Parameters
      **   
      **   log_name Name of the log
      **   
      **   level Logging level
      **   
      **   Returns N/A
      ***********************************************************************/
      void SetLogLevel(QString log_name, unsigned int level = 1)
      {
         QString msg_type;
         if ( m_logLevels.contains(log_name) ) 
         {
            m_logLevels.insert(log_name, level);
         }
# if 0
         switch (type) 
         {
            case QtDebugMsg:
               msg_type = MSG_DEBUG_TYPE;
               break;

//            case QtInfoMsg:
//               msg_type = MSG_INFO_TYPE;
//               break;
//
            case QtWarningMsg:
               msg_type = MSG_WARNING_TYPE;
               break;

            case QtCriticalMsg:
               msg_type = MSG_CRITICAL_TYPE;
               break;

            case QtFatalMsg:
               msg_type = MSG_CRITICAL_TYPE;
               break;

            default:
               msg_type = MSG_DEBUG_TYPE;
               break;
         }
         QString log_base_name = log_name + ":%." + msg_type;

         for (unsigned int lvl = 1; lvl <= LogLevel(log_name); lvl++) 
         {
            QString log_descr = log_base_name;
            log_descr.replace("%", QString::number(lvl));
            if ( lvl <= level ) 
            {
               log_descr += "=true";
            }
            else 
            {
               log_descr += "=false";
            }
         }
# endif
      }

   protected:
   private:

      /********************************************************************//*
      **   QString  BuildName(QString  log_name, unsigned int level) private
      **   
      **   Function  to  build  the  full  name  of the log which is
      **   comprised of its name and level
      **   
      **   Parameters
      **   
      **   log_name Name of the log level log level
      **   
      **   Returns QString of the full name
      ***********************************************************************/

      QString BuildName(QString log_name, unsigned int level)
      {
         QString rv = log_name + ":" + QString::number(level);
         return(rv);
      }

      QLoggingCategory*    m_defaultCategory;
      LogRegisteryMap_t    m_logMap;
      LogLevelsMap_t       m_logLevels;      
      LogLevelsMap_t       m_logMaxLevels;
      LogDescriptionMap_t  m_logDescriptions;
      static LogRegistery* m_instance;
   };
};

# endif
