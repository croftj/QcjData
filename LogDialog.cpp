# include "LogDialog.h"

# include <QFile>
# include <QFileDialog>
# include <QMessageBox>

#include <stdio.h>
#include <stdlib.h>
#include <iostream>

# include "Logger.h"

using namespace QcjData;

void LogDialog::accept()
{
   bool valid = true;
   if ( m_ui.fileCheckBox->isChecked() ) 
   {
      QFile log(m_ui.fileNameEdit->text());
      if ( ! Logger::instance()->SetLogFile(m_ui.fileNameEdit->text()) ) 
      {
         QMessageBox::critical(NULL, "Error Opening Log File", "File '" 
                                   + m_ui.fileNameEdit->text()
                                   + " could not be opened");
         valid = false;
      }
   }
   else 
   {
      Logger::instance()->SetLogFile("");
   }

   if ( valid ) 
   {
      Logger::instance()->EnableConsole(m_ui.consoleCheckBox->isChecked());

      QSettings settings;
      settings.setValue(ShureQtLibrary::LOG_CONSOLE_ENABLE, m_ui.consoleCheckBox->isChecked());
      settings.setValue(ShureQtLibrary::LOG_VIEW_ENABLE,    m_ui.viewCheckBox->isChecked());
      settings.setValue(ShureQtLibrary::LOG_FILE_ENABLE,    m_ui.fileCheckBox->isChecked());
      settings.setValue(ShureQtLibrary::LOG_FILE_NAME,      m_ui.fileNameEdit->text());

      qDebug() << __FUNCTION__ << "Saving settings";
      m_ui.logTableWidget->ApplySettings();
      LogRegistery::instance()->SaveLogSettings();

      QDialog::accept();
   }
}

void LogDialog::SlotSetFileEnable(int)
{
   m_ui.fileNameEdit->setVisible(m_ui.fileCheckBox->isChecked());
   m_ui.browseBtn->setVisible(m_ui.fileCheckBox->isChecked());
}


void LogDialog::SlotBrowseFiles()
{
   m_ui.fileNameEdit->setText(QFileDialog::getOpenFileName(this, "Log File", m_ui.fileNameEdit->text()));
}

void LogDialog::InitializeLogger()
{
   QSettings settings;
   if ( settings.value(ShureQtLibrary::LOG_FILE_ENABLE, true).toBool() )
   {
      Logger::instance()->SetLogFile(settings.value(ShureQtLibrary::LOG_FILE_NAME, "").toString());
   }

   bool console_enable = settings.value(ShureQtLibrary::LOG_CONSOLE_ENABLE, true).toBool();
   std::cout << "console_enable: " << (int)console_enable << std::endl;
   Logger::instance()->EnableConsole(console_enable);
}

