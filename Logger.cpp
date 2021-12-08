# include "Logger.h"
# include "LogRegistery.h"

# include  <QFile>

# include  <stdlib.h>
# include  <stdlib.h>

using namespace QcjData;

Logger* Logger::m_instance = NULL;

void myMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
   QString str(context.category);
//   std::cout << __FUNCTION__ <<  " str: " << qPrintable(str) << std::endl;
   QStringList sl = str.split(":");
   QString category = sl[0];
   unsigned int level = 1;

   QString msg_buf;
   QTextStream buf(&msg_buf);

   if ( sl.count() > 1 ) 
   {
      level = sl[1].toInt();
   }
//   std::cout << __FUNCTION__ <<  " context->category: " << context.category << ", level: " << level << std::endl;

   if ( LogRegistery::instance()->LogLevel(category) >= level ) 
   {
      buf << category << "-" << level << "::";
      switch (type)
      {
         case QtDebugMsg:
            buf << "Debug: ";
            break;

         case QtWarningMsg:
            buf << "Warning: ";
            break;

         case QtCriticalMsg:
            buf << "Critical: ";
            break;

         case QtFatalMsg:
            buf << "FATAL: " << msg << endl;
            Logger::instance()->LogMessage(msg_buf);
            abort();
      }
      buf << msg << endl;
      Logger::instance()->LogMessage(msg_buf);
   }
}

Logger::Logger(QObject *parent) : QObject(parent)
{
   QFile *out = new QFile(); 
   out->open(stdout, QIODevice::WriteOnly);
   m_consoleStream.setDevice(out);
   qInstallMessageHandler(myMessageOutput);
}

Logger::~Logger()
{
}


