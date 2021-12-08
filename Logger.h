#ifndef LOGGER_H
#define LOGGER_H

# include <QDebug>
# include <QFile>
# include <QFileInfo>
# include <QIODevice>
# include <QMessageLogContext>
# include <QObject>
# include <QString>
# include <QTextStream>

#include <stdio.h>
#include <stdlib.h>
#include <iostream>

namespace QcjData
{
   class Logger : QObject 
   {
      Q_OBJECT

   public:
      Logger(QObject *parent = NULL);
      ~Logger();

      static Logger *instance()
      {
         if ( m_instance == NULL ) 
         {
            m_instance = new Logger();
         }
         return(m_instance);
      }

      bool SetLogFile(QString filename)
      {
         bool rv = true;

         if ( ! filename.isEmpty() ) 
         {
            QString rename = filename + ".1";
            if ( QFileInfo::exists(filename) ) 
            {
               std::cout << __FUNCTION__ << " Renaming existing log file" << std::endl;
               QFile::rename(filename, rename);
            }

            m_logFile.setFileName(filename);
            std::cout << __FUNCTION__ << " Opeing log file" << std::endl;
            if ( (rv = m_logFile.open(QIODevice::WriteOnly)) ) 
            {
               if ( m_deviceStream.device() != NULL &&
                    m_deviceStream.device()->isOpen() ) 
               {
                  m_deviceStream.device()->close();
               }
               m_deviceStream.setDevice(&m_logFile);
            }
         }
         else if ( m_logFile.isOpen() ) 
         {
            m_logFile.close();
         }
         return(rv);
      }

      void LogMessage(QString msg)
      {
         if ( m_consoleEnable ) 
         {
            m_consoleStream << msg;
            m_consoleStream.flush();
         }
         if ( m_deviceStream.device() != NULL &&
              m_deviceStream.device()->isOpen() ) 
         {
            m_deviceStream << msg;
            m_deviceStream.flush();
         }
         emit LogEntry(msg);
      }

      void EnableConsole(bool enable)
      {
         m_consoleEnable = enable;
      }

   signals:
      void  LogEntry(QString msg);

   protected:
   private:
      void LogOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg);
      
      bool           m_consoleEnable;
      QTextStream    m_consoleStream;
      QTextStream    m_deviceStream;
      QFile          m_logFile;
      static Logger  *m_instance;
   };
};

#endif
