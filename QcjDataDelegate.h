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

#ifndef QCJDATADELEGATE_H
#define QCJDATADELEGATE_H

# include <QAbstractItemModel>
# include <QItemDelegate>
# include <QModelIndex>
# include <stdio.h>

/*!
   \class QcjDataDelegate

   \brief This class acts as the editor delegate for the QcjDataForm

   The QcjDataDelegate class will act as the delegate for any object with a property 
   named "pname". If that property is found and it has a valid value, the property
   named by pname will be used as the value to pass between the widget and model.
*/
class QcjDataDelegate : public QItemDelegate
{
   Q_OBJECT

public:
   QcjDataDelegate(QObject *parent) : QItemDelegate(parent)
   {
   };

   void setEditorData(QWidget *edt, const QModelIndex &index) const
   {
      printf("QcjDataDelegate::setEditorData(): Enter, column %d\n", index.column());
      const QMetaObject *mobj = edt->metaObject();
      while (mobj != NULL) 
      {
         printf("QcjDataDelegate::setEditorData(): className = |%s|\n", mobj->className());
         fflush(stdout);
         mobj = mobj->superClass();
      }
      if ( edt->property("pname").isValid() ) 
      {
         printf("QcjDataDelegate::setEditorData(): name = %s\n", qPrintable(edt->property("pname").toString()));
         QString pname = edt->property("pname").toString();
         if ( edt->property(qPrintable(pname)).isValid() ) 
         {
            printf("QcjDataDelegate::setEditorData(): current value = %s\n", qPrintable(edt->property(qPrintable(pname)).toString()));
            printf("QcjDataDelegate::setEditorData(): new value = %s\n", qPrintable(index.data().toString()));
            edt->setProperty(qPrintable(pname), index.data());
            printf("QcjDataDelegate::setEditorData(): Exit\n");
            return;
         }
      }
      QItemDelegate::setEditorData(edt, index);
      printf("QcjDataDelegate::setEditorData(): Exit\n");
   };

   void setModelData(QWidget *edt, QAbstractItemModel *model, const QModelIndex &index) const
   {
      printf("QcjDataDelegate::setModelData(): Enter, column %d\n", index.column());
      const QMetaObject *mobj = edt->metaObject();
      while (mobj != NULL) 
      {
         printf("QcjDataDelegate::setModelData(): className = |%s|\n", mobj->className());
         fflush(stdout);
         mobj = mobj->superClass();
      }
      if ( edt->property("pname").isValid() ) 
      {
         printf("QcjDataDelegate::setModelData(): name = %s\n", qPrintable(edt->property("pname").toString()));
         QString pname = edt->property("pname").toString();
         if ( edt->property(qPrintable(pname)).isValid() ) 
         {
            QVariant value = edt->property(qPrintable(pname));
            printf("QcjDataDelegate::setModelData(): value = %s\n", qPrintable(edt->property(qPrintable(pname)).toString()));
            QString pname = edt->property("pname").toString();
            if ( value.isValid() ) 
            {
               printf("QcjDataDelegate::setModelData(): Setting value\n");
               model->setData(index, value);
               printf("QcjDataDelegate::setModelData(): Exit\n");
               return;
            }
         }
      }
      QItemDelegate::setModelData(edt, model, index);
      printf("QcjDataDelegate::setModelData(): Exit\n");
   };
};

#endif
