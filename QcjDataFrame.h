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

#ifndef DATAFRAME_H
#define DATAFRAME_H

# include <QAction>
# include <QFrame>
# include "Qcj.h"
# include "QcjDataForm.h"
# include "QcjDataTable.h"
#include <stdlib.h>
#include <stdio.h>

class QcjDataFrame : public QFrame 
{
   Q_OBJECT

public:
   QcjDataFrame(QWidget *pParent = 0);

   void setDataForm(QcjDataForm *pF) 
   {
      printf("QcjDataFrame::setDataForm(): Enter, Setting form pointer to %ld\n", (unsigned long)pF);
      pForm = pF;
      if ( pForm != 0 ) 
      {
         connect(pForm, SIGNAL(updated()), this, SLOT(haveUpdated()), Qt::UniqueConnection);
      }
      printf("QcjDataFrame::setDataForm(): Exit\n");
   };

   void setDataTable(QcjDataTable *pT)
   {
      printf("QcjDataFrame::setDataTable(): Enter, setting table point to %ld\n", (unsigned long)pT);
      pTable = pT;
      if ( pTable != 0 ) 
      {
         pTable->setFocusPolicy(Qt::NoFocus);
         connect(pTable, SIGNAL(rowSelected(QSqlRecord*)), this, SLOT(haveRowSelected(QSqlRecord*)), Qt::UniqueConnection);
         connect(pTable, SIGNAL(rowActivated(QSqlRecord*)), this, SLOT(haveRowActivated(QSqlRecord*)), Qt::UniqueConnection);
         if ( pForm != 0 ) 
            connect(pTable, SIGNAL(emptyTable()), pForm, SLOT(clearForm()), Qt::UniqueConnection);
      }
      printf("QcjDataFrame::setDataTable(): Exit\n");
   };

   void enableAction(Qcj::Action act, bool f)
   {
      enabled[act] = f;
   };

   void connectButton(Qcj::Action act, QAbstractButton *btn);

   void setValidationString(QString str)
   {
      vs = str;
   };

   QString validationString()
   {
      return(vs);
   };

   bool validate();

signals:
   void actionActivated(Qcj::Action);
   void clearForm();
   void deleteRecord();
   void insertRecord();
   void nextRow();
   void updateRecord();
   void findRows();
   void prevRow();
   void frameState(Qcj::FrameState);

public slots:
   void setState(Qcj::FrameState newState);

protected slots:
   void haveActivatedAction(bool);
   void haveCancelAction(bool = false);
   void haveClearAction(bool = false);
   void haveDelAction(bool = false);
   void haveDownAction(bool = false);
   void haveEditAction(bool = false);
   void haveNewAction(bool = false);
   void haveSaveAction(bool = false);
   void haveSearchAction(bool = false);
   void haveUpAction(bool = false);

   void haveRowSelected(QSqlRecord*);
   void haveRowActivated(QSqlRecord *rec);
   void haveUpdated();

private:
   bool                    enabled[Qcj::Actions];
   QAbstractButton         *buttons[Qcj::Actions];
   QAction                 *cancelAction;
   QAction                 *activatedAction;
   QAction                 *clearAction;
   QAction                 *delAction;
   QAction                 *editAction;
   QAction                 *newAction;
   QAction                 *downAction;
   QAction                 *saveAction;
   QAction                 *searchAction;
   QAction                 *upAction;

   QString                 vs;   /* validation string */

   Qcj::FrameState         state;
   QcjDataForm             *pForm;
   QcjDataTable            *pTable;
   QMap<QString, QString>  indexMap;
};

#endif
