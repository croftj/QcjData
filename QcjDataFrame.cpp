/*********************************************************************************
**
**   $Id$
**   Copyright (c) 2007-2024 Joe Croft <joe@croftj.net>
**   
**   This file is part of QcjData Class Libraries.
**
**   QcjData Class Libraries is free software; you can redistribute it and/or modify
**   it under the terms of the GNU General Public License as published by
**   the Free Software Foundation; either version 2 of the License, or
**   (at your option) any later version.
**
**   QcjData Class Libraries is distributed in the hope that it will be useful,
**   but WITHOUT ANY WARRANTY; without even the implied warranty of
**   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**   GNU General Public License for more details.
**
**   You should have received a copy of the GNU General Public License
**   along with QcjData Class Libraries; if not, write to the Free Software
**   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
**
*********************************************************************************/

# include <QAbstractButton>
# include <QDebug>
# include <QMessageBox>

# include "QcjData/QcjDataFrame.h"
# include "QcjData/QcjDataStatics.h"

QcjDataFrame::QcjDataFrame(QWidget *pParent) :
   QFrame(pParent)
{
#ifndef QT4_DESIGNER_PLUGIN
   printf("QcjDataFrame::QcjDataFrame(): Enter\n");
   fflush(stdout);
   activatedAction = new QAction(this);
   activatedAction->setShortcut(Qt::Key_Return);
   activatedAction->setShortcutContext(Qt::WidgetWithChildrenShortcut);
   connect(activatedAction, SIGNAL(triggered(bool)), this, SLOT(haveActivatedAction(bool)), Qt::UniqueConnection);
   addAction(activatedAction);

   cancelAction = new QAction(this);
   cancelAction->setShortcut(Qt::CTRL + Qt::Key_Escape);
   cancelAction->setShortcutContext(Qt::WidgetWithChildrenShortcut);
   connect(cancelAction, SIGNAL(triggered(bool)), this, SLOT(haveCancelAction(bool)), Qt::UniqueConnection);
   addAction(cancelAction);

   clearAction = new QAction(this);
   clearAction->setShortcut(Qt::Key_F10);
   clearAction->setShortcutContext(Qt::WidgetWithChildrenShortcut);
   connect(clearAction, SIGNAL(triggered(bool)), this, SLOT(haveClearAction(bool)), Qt::UniqueConnection);
   addAction(clearAction);

   delAction = new QAction(this);
   delAction->setShortcut(QKeySequence::DeleteEndOfWord);
   delAction->setShortcutContext(Qt::WidgetWithChildrenShortcut);
   connect(delAction, SIGNAL(triggered(bool)), this, SLOT(haveDelAction(bool)), Qt::UniqueConnection);
   addAction(delAction);

   downAction = new QAction(this);
//   downAction->setShortcut(QKeySequence::MoveToNextLine);
   downAction->setShortcut(Qt::ALT + Qt::Key_Down);
   downAction->setShortcutContext(Qt::WidgetWithChildrenShortcut);
   connect(downAction, SIGNAL(triggered(bool)), this, SLOT(haveDownAction(bool)), Qt::UniqueConnection);
   addAction(downAction);

   editAction = new QAction(this);
//   editAction->setShortcut(QKeySequence::MoveToPreviousLine);
   editAction->setShortcut(QKeySequence::Open);
   editAction->setShortcutContext(Qt::WidgetWithChildrenShortcut);
   connect(editAction, SIGNAL(triggered(bool)), this, SLOT(haveEditAction(bool)), Qt::UniqueConnection);
   addAction(editAction);

   newAction = new QAction(this);
   newAction->setShortcut(QKeySequence::New);
   newAction->setShortcutContext(Qt::WidgetWithChildrenShortcut);
   connect(newAction, SIGNAL(triggered(bool)), this, SLOT(haveNewAction(bool)), Qt::UniqueConnection);
   addAction(newAction);

   saveAction = new QAction(this);
   saveAction->setShortcut(QKeySequence::Save);
   saveAction->setShortcutContext(Qt::WidgetWithChildrenShortcut);
   connect(saveAction, SIGNAL(triggered(bool)), this, SLOT(haveSaveAction(bool)), Qt::UniqueConnection);
   addAction(saveAction);

   searchAction = new QAction(this);
   searchAction->setShortcut(QKeySequence::FindNext);
   searchAction->setShortcutContext(Qt::WidgetWithChildrenShortcut);
   connect(searchAction, SIGNAL(triggered(bool)), this, SLOT(haveSearchAction(bool)), Qt::UniqueConnection);
   addAction(searchAction);

   upAction = new QAction(this);
//   upAction->setShortcut(QKeySequence::MoveToPreviousLine);
   upAction->setShortcut(Qt::ALT + Qt::Key_Up);
   upAction->setShortcutContext(Qt::WidgetWithChildrenShortcut);
   connect(upAction, SIGNAL(triggered(bool)), this, SLOT(haveUpAction(bool)), Qt::UniqueConnection);
   addAction(upAction);

   for (int x = 0; x < Qcj::Actions; x++) 
   {
      enabled[x] = true;
      buttons[x] = 0;
   }
   pForm = 0;
   pTable = 0;
   printf("QcjDataFrame::QcjDataFrame(): Exit\n");
   fflush(stdout);
#else
   setFrameShape(QFrame::Box);
#endif
}

void QcjDataFrame::connectButton(Qcj::Action act, QAbstractButton *btn)
{
#ifndef QT4_DESIGNER_PLUGIN
   printf("QcjDataFrame::connectButton(): Enter\n");
   fflush(stdout);
   switch (act) 
   {
      case Qcj::CancelAction:
         connect(btn, SIGNAL(clicked()), this, SLOT(haveCancelAction()), Qt::UniqueConnection);
         buttons[act] = btn;
         break;

      case Qcj::ClearAction:
         connect(btn, SIGNAL(clicked()), this, SLOT(haveClearAction()), Qt::UniqueConnection);
         buttons[act] = btn;
         break;

      case Qcj::DelAction:
         connect(btn, SIGNAL(clicked()), this, SLOT(haveDelAction()), Qt::UniqueConnection);
         buttons[act] = btn;
         break;
      
      case Qcj::DownAction:
         connect(btn, SIGNAL(clicked()), this, SLOT(haveDownAction()), Qt::UniqueConnection);
         buttons[act] = btn;
         break;
      
      case Qcj::EditAction:
         connect(btn, SIGNAL(clicked()), this, SLOT(haveEditAction()), Qt::UniqueConnection);
         buttons[act] = btn;
         break;
      
      case Qcj::NewAction:
         connect(btn, SIGNAL(clicked()), this, SLOT(haveNewAction()), Qt::UniqueConnection);
         buttons[act] = btn;
         break;
      
      case Qcj::SaveAction:
         connect(btn, SIGNAL(clicked()), this, SLOT(haveSaveAction()), Qt::UniqueConnection);
         buttons[act] = btn;
         break;
      
      case Qcj::SearchAction:
         connect(btn, SIGNAL(clicked()), this, SLOT(haveSearchAction()), Qt::UniqueConnection);
         buttons[act] = btn;
         break;

      case Qcj::UpAction:
         connect(btn, SIGNAL(clicked()), this, SLOT(haveUpAction()), Qt::UniqueConnection);
         buttons[act] = btn;
         break;
      
      default:
         break;
   }

   printf("QcjDataFrame::connectButton(): Exit\n");
   fflush(stdout);
#endif
}

void QcjDataFrame::haveActivatedAction(bool)
{
#ifndef QT4_DESIGNER_PLUGIN
   printf("QcjDataFrame::haveActivatedAction(): Enter\n");
   fflush(stdout);
   printf("QcjDataFrame::haveDownAction(): Action Enabled\n");
   if ( pTable != 0 ) 
   {
      pTable->activateCurrentRecord();
      qDebug() << "emitting Activated Action";
      emit actionActivated(Qcj::ActivatedAction);
   }
   printf("QcjDataFrame::haveActivatedAction(): Exit\n");
   fflush(stdout);
#endif
}

void QcjDataFrame::haveCancelAction(bool)
{
#ifndef QT4_DESIGNER_PLUGIN
   printf("QcjDataFrame::haveClearAction(): Enter\n");
   fflush(stdout);
   printf("QcjDataFrame::haveClearAction(): Exit\n");
   fflush(stdout);
#endif
   qDebug() << "emitting Cancel Action";
   emit actionActivated(Qcj::CancelAction);
}

void QcjDataFrame::haveClearAction(bool)
{
#ifndef QT4_DESIGNER_PLUGIN
   printf("QcjDataFrame::haveClearAction(): Enter\n");
   fflush(stdout);
   if ( enabled[Qcj::ClearAction] ) 
   {
      printf("QcjDataFrame::haveClearAction(): Action Enabled\n");
      if ( pForm != 0 ) 
      {
         if ( pTable != 0 ) 
         {
            pTable->setFilter("");
            pTable->selectRow(0);
         }
         pForm->clearForm();
      }
      setState(Qcj::Search);
      qDebug() << "emitting clearForm Action";
      emit clearForm();
   }
   printf("QcjDataFrame::haveClearAction(): Exit\n");
   fflush(stdout);
#endif
   qDebug() << "emitting Clear Action";
   emit actionActivated(Qcj::ClearAction);
}

void QcjDataFrame::haveDelAction(bool)
{
#ifndef QT4_DESIGNER_PLUGIN
   if ( state != Qcj::Search ) 
   {
      if ( enabled[Qcj::DelAction] ) 
      {
         if ( pForm != 0 ) 
         {
            if ( validate() ) 
            {
               pForm->deleteRecord();
               if ( pTable != 0 ) 
               {
                  pTable->seek(0);
                  pTable->refresh();
               }
               qDebug() << "emitting deleteRecord Action";
               emit deleteRecord();
            }
         }
      }
      else if ( validate() ) 
      {
         qDebug() << "emitting deleteRecord Action";
         emit deleteRecord();
      }
   }
   qDebug() << "emitting Del Action";
   emit actionActivated(Qcj::DelAction);
#endif
}

void QcjDataFrame::haveDownAction(bool)
{
#ifndef QT4_DESIGNER_PLUGIN
   if ( state != Qcj::Insert ) 
   {
      printf("QcjDataFrame::haveDownAction(): Enter\n");
      fflush(stdout);
      if ( enabled[Qcj::DownAction] ) 
      {
         printf("QcjDataFrame::haveDownAction(): Action Enabled\n");
         if ( pTable != 0 ) 
         {
            pTable->nextRow();
         }
      }
      qDebug() << "emitting nextRow Action";
      emit nextRow();
      printf("QcjDataFrame::haveDownAction(): Exit\n");
      fflush(stdout);
   }
#endif
}

/********************************************************************/
/* This slot does not do anything accept signal that the action was */
/* activated.                                                       */
/********************************************************************/
void QcjDataFrame::haveEditAction(bool)
{
#ifndef QT4_DESIGNER_PLUGIN
   qDebug() << "emitting Edit Action";
   emit actionActivated(Qcj::EditAction);
#endif
}

void QcjDataFrame::haveNewAction(bool)
{
#ifndef QT4_DESIGNER_PLUGIN
   printf("QcjDataFrame::haveNewAction(): Enter\n");
   fflush(stdout);
   if ( enabled[Qcj::NewAction] ) 
   {
      printf("QcjDataFrame::haveNewAction(): Action enabled\n");
      fflush(stdout);
      if ( pForm != 0 )
      {
         printf("QcjDataFrame::haveNewAction(): Form present\n");
         fflush(stdout);
         if ( validate() ) 
         {
            pForm->insertRecord();
            if ( pTable != 0 ) 
            {
               pTable->refresh();
               pTable->findRow(pForm->getIndexMap());
            }
            printf("QcjDataFrame::haveNewAction(): Emitting insertRecord signal\n");
            emit insertRecord();
         }
      }
      setState(Qcj::Insert);
   }
   else if ( validate() ) 
      emit insertRecord();
   printf("QcjDataFrame::haveNewAction(): Emitting NewAction signal\n");
   emit actionActivated(Qcj::NewAction);
   printf("QcjDataFrame::haveNewAction(): Exit\n");
   fflush(stdout);
#endif
}

void QcjDataFrame::haveSaveAction(bool)
{
#ifndef QT4_DESIGNER_PLUGIN
   if ( state != Qcj::Search ) 
   {
      printf("QcjDataFrame::haveSaveAction(): Enter\n");
      fflush(stdout);
      if ( enabled[Qcj::SaveAction] ) 
      {
         if ( pForm != 0 ) 
         {
            if ( validate() ) 
            {
               pForm->updateRecord();
               if ( pTable != 0 ) 
                  pTable->findRow(indexMap);
               emit updateRecord();
            }
         }
         setState(Qcj::Updated);
      }
      else if ( validate() ) 
         emit updateRecord();
      printf("QcjDataFrame::haveSaveAction(): Exit\n");
      fflush(stdout);
   }
   emit actionActivated(Qcj::SaveAction);
#endif
}

void QcjDataFrame::haveSearchAction(bool)
{
#ifndef QT4_DESIGNER_PLUGIN
   if ( state != Qcj::Insert ) 
   {
      printf("QcjDataFrame::haveSearchAction(): Enter\n");
      fflush(stdout);
      if ( pTable && pForm ) 
      {
         printf("QcjDataFrame::haveSearchAction(): Action Enabled\n");
         fflush(stdout);
         QString xmldef = pForm->readXmlDef();
         QString index = pFormDef->getIndexField(xmldef);
         QStringList sl;
         sl << index;
         QString filter = pForm->makeFilter(sl);
         pTable->setFilter(filter);
         pTable->selectRow(0);
         setState(Qcj::Updated);
      }
      emit findRows();
      printf("QcjDataFrame::haveSearchAction(): Exit\n");
      fflush(stdout);
   }
   emit actionActivated(Qcj::SearchAction);
#endif
}

void QcjDataFrame::haveUpAction(bool)
{
#ifndef QT4_DESIGNER_PLUGIN
   if ( state != Qcj::Insert ) 
   {
      printf("QcjDataFrame::haveUpAction(): Enter\n");
      fflush(stdout);
      if ( enabled[Qcj::UpAction] ) 
      {
         printf("QcjDataFrame::haveUpAction(): Action Enabled\n");
         fflush(stdout);
         if ( pTable != 0 ) 
         {
            pTable->previousRow();
         }
      }
      emit prevRow();
      printf("QcjDataFrame::haveUpAction(): Exit\n");
      fflush(stdout);
      emit actionActivated(Qcj::UpAction);
   }
#endif
}

void QcjDataFrame::setState(Qcj::FrameState newState)
{
#ifndef QT4_DESIGNER_PLUGIN
   printf("QcjDataFrame::setStates(): Enter, newState: %d\n", newState);
   fflush(stdout);
   QAbstractButton *btn;
   state = newState;

   if ( (btn = buttons[Qcj::ClearAction]) != 0 ) 
   {
      printf("QcjDataFrame::setStates(): %s button %d\n", ( state != Qcj::Insert ) ? "Enabling" : "Disabling", Qcj::ClearAction);
      fflush(stdout);
      if ( state != Qcj::Insert ) 
         btn->setEnabled(true);
      else
         btn->setEnabled(false);
   }

   if ( (btn = buttons[Qcj::DelAction]) != 0 ) 
   {
      printf("QcjDataFrame::setStates(): %s button %d\n", ( state != Qcj::Search ) ? "Enabling" : "Disabling", Qcj::DelAction);
      fflush(stdout);
      if ( state != Qcj::Search ) 
         btn->setEnabled(true);
      else
         btn->setEnabled(false);
   }

   if ( (btn = buttons[Qcj::DownAction]) != 0 ) 
   {
      printf("QcjDataFrame::setStates(): %s button %d\n", ( state != Qcj::Insert ) ? "Enabling" : "Disabling", Qcj::DownAction);
      fflush(stdout);
      if ( state != Qcj::Insert ) 
         btn->setEnabled(true);
      else
         btn->setEnabled(false);
   }

   if ( (btn = buttons[Qcj::NewAction]) != 0 ) 
   {
      printf("QcjDataFrame::setStates(): %s button %d\n", ( state != Qcj::Search ) ? "Enabling" : "Disabling", Qcj::NewAction);
      fflush(stdout);
      if ( state != Qcj::Search ) 
         btn->setEnabled(true);
      else
         btn->setEnabled(false);
   }

   if ( (btn = buttons[Qcj::SaveAction]) != 0 ) 
   {
      printf("QcjDataFrame::setStates(): %s button %d\n", ( state != Qcj::Search ) ? "Enabling" : "Disabling", Qcj::SaveAction);
      fflush(stdout);
      if ( state != Qcj::Search ) 
         btn->setEnabled(true);
      else
         btn->setEnabled(false);
   }

   if ( (btn = buttons[Qcj::SearchAction]) != 0 ) 
   {
      printf("QcjDataFrame::setStates(): %s button %d\n", ( state != Qcj::Insert ) ? "Enabling" : "Disabling", Qcj::SearchAction);
      fflush(stdout);
      if ( state != Qcj::Insert ) 
         btn->setEnabled(true);
      else
         btn->setEnabled(false);
   }

   if ( (btn = buttons[Qcj::UpAction]) != 0 ) 
   {
      printf("QcjDataFrame::setStates(): %s button %d\n", ( state != Qcj::Insert ) ? "Enabling" : "Disabling", Qcj::UpAction);
      fflush(stdout);
      if ( state != Qcj::Insert ) 
         btn->setEnabled(true);
      else
         btn->setEnabled(false);
   }

   emit frameState(state);
#endif
}

void QcjDataFrame::haveRowSelected(QSqlRecord *rec)
{
#ifndef QT4_DESIGNER_PLUGIN
   printf("QcjDataFrame::haveRowSelected(): Enter, pForm = %ld\n", (unsigned long)pForm);
   fflush(stdout);
   if ( pForm != 0 ) 
   {
      printf("QcjDataFrame::haveRowSelected(): Refreshing form\n");
      pForm->refresh(rec);
   }
   setState(Qcj::Updated);
   printf("QcjDataFrame::haveRowSelected(): Exit\n");
   fflush(stdout);
#endif
}

void QcjDataFrame::haveRowActivated(QSqlRecord *rec)
{
#ifndef QT4_DESIGNER_PLUGIN
   printf("QcjDataFrame::haveRowActivated(): Enter, pForm = %ld\n", (unsigned long)pForm);
   fflush(stdout);
   if ( pForm != 0 ) 
   {
      printf("QcjDataFrame::haveRowActivated(): Refreshing form\n");
      pForm->refresh(rec);
   }
   setState(Qcj::Updated);
   qDebug() << "emitting Activated Action, xml: " << pTable->readXmlDef();
   emit actionActivated(Qcj::ActivatedAction);
   printf("QcjDataFrame::haveRowActivated(): Exit\n");
   fflush(stdout);
#endif
}

void QcjDataFrame::haveUpdated()
{
#ifndef QT4_DESIGNER_PLUGIN
   printf("QcjDataFrame::haveUpdated(): Enter\n");
   fflush(stdout);

   if ( pForm != 0 ) 
      indexMap = pForm->getIndexMap();

   if ( pTable != 0 ) 
      pTable->refresh();

   setState(Qcj::Updated);
   printf("QcjDataFrame::haveUpdated(): Exit\n");
   fflush(stdout);
#endif
}

bool QcjDataFrame::validate()
{
   bool rv = false;

   printf("QcjDataFrame::validate(): Enter, vs length: %d\n", vs.size());
   fflush(stdout);
   if ( vs.size() > 0 && QMessageBox::warning(NULL, "Warning", vs + "\n", 
                              QMessageBox::Yes | QMessageBox::Default, 
                              QMessageBox::No | QMessageBox::Escape) == QMessageBox::No )
   {
      printf("QcjDataFrame::validate(): Form not validated, returning false\n");
      fflush(stdout);
      rv = false;
   }
   else
      rv = true;
   printf("QcjDataFrame::validate(): Exit, rv = %s\n", rv == true ? "true" : "false");
   fflush(stdout);
   return(rv);
}

