#ifndef LOGDIALOG_H
#define LOGDIALOG_H

# include <QDialog>
# include <QSettings>

# include "LogRegistery.h"
# include "LogLevelWidget.h"

# include "ui_logdialog.h"

namespace QcjData
{
   class LogDialog : public QDialog
   {
      Q_OBJECT

   public:
      LogDialog(QWidget *parent = NULL) : 
         QDialog(parent, Qt::Dialog | Qt::WindowMinMaxButtonsHint)
      {
         m_ui.setupUi(this);
         m_ui.logTableWidget->Initialize();

         connect(m_ui.fileCheckBox, SIGNAL(stateChanged(int)), this, SLOT(SlotSetFileEnable(int)));
         connect(m_ui.browseBtn,    SIGNAL(clicked()),         this, SLOT(SlotBrowseFiles()));

         QSettings settings;
         m_ui.consoleCheckBox->setChecked(settings.value(LOG_CONSOLE_ENABLE, true).toBool());
         m_ui.viewCheckBox->setChecked(settings.value(LOG_VIEW_ENABLE, true).toBool());
         m_ui.fileCheckBox->setChecked(settings.value(LOG_FILE_ENABLE, true).toBool());
         m_ui.fileNameEdit->setText(settings.value(LOG_FILE_NAME, "./wilmaadmin.log").toString());
      }

      static bool isViewEnabled()
      {
         QSettings settings;
         return(settings.value(LOG_VIEW_ENABLE, true).toBool());
      }

      static void InitializeLogger();

   public slots:
      void accept();
      void SlotSetFileEnable(int check_state);
      void SlotBrowseFiles();

   protected:
   private:
      Ui::LogDialog  m_ui;
   };
}

#endif
