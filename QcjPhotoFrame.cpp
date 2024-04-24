/*********************************************************************************
**
**   $Id$
**   Copyright (c) 2005-2024 Joe Croft <joe@croftj.net>
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
# include <QApplication>
# include <QBuffer>
# include <QGraphicsPixmapItem>
# include <QGraphicsTextItem>
# include <QSqlQuery>
#ifndef QT4_DESIGNER_PLUGIN
# include "QcjData/QcjDataHelpers.h"
# include "QcjData/QcjDataLogin.h"
# include "QcjData/QcjDataStatics.h"
#endif
# include "QcjData/QcjPhotoFrame.h"

void QcjPhotoScene::mousePressEvent(QGraphicsSceneMouseEvent *ev)
{
   printf("QcjPhotoFrame::mousePressedEvent(): Enter\n");
   printf("QcjPhotoFrame::mousePressedEvent(): lastPressed = %ld, item = %ld\n", (unsigned long)lastPressed, (unsigned long)itemAt(ev->buttonDownScenePos(ev->button())));
   lastPressed = itemAt(ev->buttonDownScenePos(ev->button()));
   pressedTimer.start(500);
   printf("QcjPhotoFrame::mousePressedEvent(): Exit\n");
   fflush(stdout);
}

void QcjPhotoScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *ev)
{
   printf("QcjPhotoFrame::mouseReleaseEvent(): Enter\n");
   printf("QcjPhotoFrame::mouseReleaseEvent(): lastPressed = %ld, item = %ld\n", (unsigned long)lastPressed, (unsigned long)itemAt(ev->buttonDownScenePos(ev->button())));
   if ( lastPressed == itemAt(ev->buttonDownScenePos(ev->button())) ) 
   {
      printf("QcjPhotoFrame::mouseReleaseEvent(): emitting signal!\n");
      emit clicked(lastPressed);
      lastPressed = NULL;
      pressedTimer.stop();
   }
   printf("QcjPhotoFrame::mouseReleaseEvent(): Exit\n");
   fflush(stdout);
}

void QcjPhotoScene::mouseDoubleClickedEvent(QGraphicsSceneMouseEvent *ev)
{
   printf("QcjPhotoFrame::mouseDoubleClickEvent(): Enter\n");
   printf("QcjPhotoFrame::mouseDoubleClickEvent(): lastPressed = %ld, item = %ld\n", (unsigned long)lastPressed, (unsigned long)itemAt(ev->buttonDownScenePos(ev->button())));
   if ( lastPressed == itemAt(ev->buttonDownScenePos(ev->button())) ) 
   {
      printf("QcjPhotoFrame::mouseDoubleClickEvent(): emitting signal!\n");
      emit doubleClicked(lastPressed);
      lastPressed = NULL;
      pressedTimer.stop();
   }
   printf("QcjPhotoFrame::mouseDoubleClickEvent(): Exit\n");
   fflush(stdout);
}

void QcjPhotoScene::clearPressed()
{
   printf("QcjPhotoFrame::clearPressed(): Enter\n");
   lastPressed = NULL;
   printf("QcjPhotoFrame::clearPressed(): Exit\n");
   fflush(stdout);
}

QcjPhotoFrame::QcjPhotoFrame(QWidget *pParent) :
#ifndef QT4_DESIGNER_PLUGIN
          QGraphicsView(pParent)
#else
          QFrame(pParent)
#endif
{
#ifndef QT4_DESIGNER_PLUGIN
   printf("QcjPhotoFrame::QcjPhotoFrame(): Enter\n");
   currentScene = NULL;
   pQuery = NULL;

//   connect(this, SIGNAL(beforeInsert(QSqlRecord *)), this, SLOT(nextIdent(QSqlRecord *)));
//   connect((const QObject*)(horizontalHeader()), SIGNAL(sectionClicked(int)), this, SLOT(sortBy(int)));
   printf("QcjPhotoFrame::QcjPhotoFrame(): Exit\n");
   fflush(stdout);
#else
   setFrameShape(QFrame::Box);
#endif
}

void QcjPhotoFrame::setDatabase()
{
#ifndef QT4_DESIGNER_PLUGIN
   printf("QcjPhotoFrame::setDatabase(): Enter, xmldef = |%s|\n", qPrintable(xmldef));
   dbTable = pFormDef->getTable(xmldef);
   printf("QcjPhotoFrame::setDatabase(): Enter, table = |%s|\n", qPrintable(dbTable));
   fflush(stdout);
   if ( pDb == 0 ) 
   {
      printf("QcjPhotoFrame::setDatabase(): No database yet, go login\n");
      QcjDataLogin *login = new QcjDataLogin();
      login->initialize();

      if (!login->exec()) 
      {
         return;
      }
   }
   printf("QcjPhotoFrame::setDatabase(): Calling setFields\n");
   setFields(pFormDef->getFields(xmldef, this));
   printf("QcjPhotoFrame::setDatabase(): Refreshing table\n");
//    refresh();

   printf("QcjPhotoFrame::setDatabase(): Exit\n");
   fflush(stdout);
#endif
}

void QcjPhotoFrame::writeXmlDef(QString str)
{
    printf("QcjPhotoFrame::writeXmlDef(): Enter: str = %s\n", qPrintable(str));
    fflush(stdout);
    xmldef = str;
    printf("QcjPhotoFrame::writeXmlDef(): Exit\n");
    fflush(stdout);
}

void QcjPhotoFrame::setFields(std::vector<struct QcjDataFields> fds)
{
#ifndef QT4_DESIGNER_PLUGIN
   printf("QcjPhotoFrame::setFields(): Enter\n");
   printf("QcjPhotoFrame::setFields(): xmldef = |%s|\n", qPrintable(xmldef));
   fflush(stdout);
   fieldDefs = fds;
   printf("QcjPhotoFrame::setFields(): exit\n");
   fflush(stdout);
#endif
}

void QcjPhotoFrame::refresh()
{
#ifndef QT4_DESIGNER_PLUGIN
   double leftMargin = pFormDef->getLeftMargin(xmldef);
   if ( leftMargin <= 0 ) 
      leftMargin = 10;

   double topMargin = pFormDef->getTopMargin(xmldef);
   if ( topMargin <= 0 ) 
      topMargin = 10;

   double vSpacing = pFormDef->getVertSpacing(xmldef);
   if ( vSpacing <= 0 ) 
      vSpacing = 10;

   double hSpacing = pFormDef->getHorzSpacing(xmldef);
   if ( hSpacing <= 0 ) 
      hSpacing = 10;

   double vertOfs = topMargin;
   double horzOfs = leftMargin;
   double maxColVert = 0;

   int cols = pFormDef->getCols(xmldef);
   int col = 0;

   if ( currentScene != NULL ) 
   {
      photoItemLists.clear();
      delete currentScene;
   }

   currentScene = new QcjPhotoScene(this);
   connect(currentScene, SIGNAL(clicked(QGraphicsItem*)), this, SLOT(itemClicked(QGraphicsItem*)), Qt::UniqueConnection);
   connect(currentScene, SIGNAL(doubleClicked(QGraphicsItem*)), this, SLOT(itemDoubleClicked(QGraphicsItem*)), Qt::UniqueConnection);

   QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
   /***********************************************/
   /*   Build  up the list of fields to get from  */
   /*   the database. It is assumed one of these  */
   /*   is a picture                              */
   /***********************************************/
   QStringList fnames = pFormDef->getFieldNames(xmldef);
   QString fields = "";
   QStringListIterator it1(fnames);
   while (it1.hasNext()) 
   {
      if ( fields.size() > 0 ) 
         fields += ", ";
      fields += it1.next();
      printf("QcjPhotoFrame::refresh(): fields = |%s|\n", qPrintable(fields));
   }

   /***********************************************/
   /*   Build up the fields to sort by            */
   /***********************************************/
   QStringList snames = pFormDef->getSortFields(xmldef);
   QString sort = "";
   QStringListIterator it2(snames);
   while (it2.hasNext()) 
   {
      if ( sort.size() > 0 ) 
         sort += ", ";
      sort += it2.next();
   }

   /***********************************************/
   /*   Now  build  up the required query needed  */
   /*   to get the photos                         */
   /***********************************************/
   QString sql = "select " + fields + " from " + dbTable;
   if ( whereClause.size() > 0 ) 
      sql += " where " + whereClause;
   if ( sort.size() > 0 ) 
      sql += " order by " + sort;
   printf("QcjPhotoFrame::refresh(): sql = |%s|\n", qPrintable(sql));

   if ( pQuery != NULL ) 
      delete pQuery;
   pQuery = new QSqlQuery(sql);
   while (pQuery->next()) 
   {
      printf("QcjPhotoFrame::refresh(): Have photo record\n");
      QList<QGraphicsItem*> itemList;
      QGraphicsItem *item;
      double vertColOfs = vertOfs;
      double minWidth = 0.0;
      QString tool_tip;
      for (int x = 0; x < (int)fieldDefs.size() - 1; x++) 
      {
         QRectF rect;
         QcjDataFields *pfd = &fieldDefs[x];
         QString type = pFormDef->getFieldType(xmldef, pfd->dataName);
         QString name = pfd->dataName;
         QString label = pfd->label;
         
         printf("QcjPhotoFrame::refresh(): working on field %d, dbname = |%s|, type = |%s|\n",
            x, qPrintable(name), qPrintable(type));

         if ( minWidth < pfd->minWidth ) 
            minWidth = (double)(pfd->minWidth);
            printf("QcjPhotoFrame::refresh(): minWidth %f\n", minWidth);

         if ( type.toLower() == "tooltip" ) 
         {
            tool_tip = pQuery->record().value(name).toString();
         }
         if ( type.toLower() != "image" ) 
         {
            QString text = pQuery->record().value(name).toString();
            printf("QcjPhotoFrame::refresh(): field Text = |%s|\n", qPrintable(text));
            if ( type.toLower() == "money" ) 
               text = QcjMoneyEdit::formatCurrency(text);
            item = currentScene->addText(text);
            ((QGraphicsTextItem*)item)->setTextWidth(pfd->minWidth);
            rect = ((QGraphicsTextItem*)item)->boundingRect();
         }
         else 
         {
            QBuffer iBuf;
            iBuf.open(QBuffer::ReadWrite);

            QPixmap pm;
            if ( pm.loadFromData(pQuery->record().value(name).toByteArray()) )
            {
               printf("QcjPhotoFrame::refresh(): scaling image to width of %d\n", pfd->minWidth);
               pm  = pm.scaledToWidth(pfd->minWidth);
               item = currentScene->addPixmap(pm);
               rect = ((QGraphicsPixmapItem*)item)->boundingRect();
            }
            else
            {
               printf("QcjPhotoFrame::refresh(): Error reading image, ignoring\n");
               continue;
            }
         }
         if (type != "tooltip")
         {
            item->setFlags(QGraphicsItem::ItemIsSelectable);
            itemList << item;
         }

         printf("QcjPhotoFrame::refresh(): Moving picture to point %f, %f\n", horzOfs, vertColOfs);
         item->setPos(horzOfs, vertColOfs);
         vertColOfs += rect.height() + 3;
         printf("QcjPhotoFrame::refresh(): item width = %f, item height = %f, Adjusted vertColOfs to %f\n", rect.width(), rect.height(), vertColOfs);
      }

      /************************************************************/
      /* Set the tool tip for each item in the record if defined. */
      /************************************************************/
      if ( ! tool_tip.isEmpty())
      {
         foreach (item, itemList)
         {
            item->setToolTip(tool_tip);
         }
      }

      photoItemLists << itemList;

      if ( vertColOfs > maxColVert ) 
         maxColVert = vertColOfs;
      printf("QcjPhotoFrame::refresh(): maxColVert = %f\n", maxColVert);

      horzOfs += minWidth + hSpacing;
      printf("QcjPhotoFrame::refresh(): horzOfs = %f, hSpacing = %f, minWidth = %f\n", horzOfs, minWidth, hSpacing);

      col++;
      if ( col >= cols ) 
      {
         printf("QcjPhotoFrame::refresh(): Done with column %d\n", col);
         
         col = 0;
         vertOfs += maxColVert + vSpacing;
         horzOfs = leftMargin;
         printf("QcjPhotoFrame::refresh(): horzOfs = %f, vertOfs = %f, vSpacing = %f, maxColVert = %f\n", horzOfs, vertOfs, vSpacing, maxColVert);
      }
   }
   setScene(currentScene);
   currentScene->update();
   QApplication::restoreOverrideCursor();
#endif
}

void QcjPhotoFrame::selectRow(int new_row)
{
   printf("QcjPhotoFrame::selectRow(): Enter new_row = %d\n", new_row);
   fflush(stdout);

   if ( currentScene == NULL || pQuery == NULL ) 
      return;

   currentScene->clearSelection();
   printf("QcjPhotoFrame::selectRow(): table refreshed\n");
   fflush(stdout);
   printf("QcjPhotoFrame::selectRow():Seeking new_row\n");
   fflush(stdout);
   if ( pQuery->seek(new_row) ) 
   {
      printf("QcjPhotoFrame::selectRow():Fetching record\n");
      fflush(stdout);
      rec = pQuery->record();
      printf("QcjPhotoFrame::selectRow(): Selecting row\n");
      fflush(stdout);
      current_row = new_row;
      QListIterator<QGraphicsItem*> it(photoItemLists[current_row]);
      while (it.hasNext()) 
      {
         it.next()->setSelected(true);
      }
      emit rowSelected(&rec);
   }
   printf("QcjPhotoFrame::selectRow(): Exit new_row = %d\n", new_row);
   fflush(stdout);
   return;
}

void QcjPhotoFrame::itemClicked(QGraphicsItem *item)
{
   printf("QcjPhotoFrame::itemClicked(): Enter item = %ld\n", (unsigned long)item);
   fflush(stdout);
   if ( currentScene == NULL || pQuery == NULL ) 
      return;
   QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
   if ( pQuery->first() )
   {
      QVectorIterator<QList<QGraphicsItem*> > it(photoItemLists);
      int row = 0;
      while (it.hasNext()) 
      {
         printf("QcjPhotoFrame::itemClicked(): Testing for item in db row %d\n", row);
         fflush(stdout);
         QListIterator<QGraphicsItem*> it1(it.next());
         while (it1.hasNext()) 
         {
            QGraphicsItem *ritem = it1.next();
            printf("QcjPhotoFrame::itemClicked(): Testing ritem %ld against item %ld\n", (unsigned long)ritem, (unsigned long)item);
            fflush(stdout);
            if ( ritem == item ) 
            {
               printf("QcjPhotoFrame::itemClicked(): have match\n");
               fflush(stdout);
               currentScene->clearSelection();
               it1.toFront();
               while (it1.hasNext()) 
                  it1.next()->setSelected(true);
               rec = pQuery->record();
               emit rowSelected(&rec);
               it.toBack();
               break;
            }
         }
         row++;
         if ( ! pQuery->next() ) 
            break;
      }
   }
   currentScene->update();
   QApplication::restoreOverrideCursor();
   printf("QcjPhotoFrame::itemClicked(): exit\n");
   fflush(stdout);
}

void QcjPhotoFrame::itemDoubleClicked(QGraphicsItem *item)
{
   printf("QcjPhotoFrame::itemDoubleClicked(): Enter item = %ld\n", (unsigned long)item);
   fflush(stdout);
   if ( currentScene == NULL || pQuery == NULL ) 
      return;
   QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
   if ( pQuery->first() )
   {
      QVectorIterator<QList<QGraphicsItem*> > it(photoItemLists);
      int row = 0;
      while (it.hasNext()) 
      {
         printf("QcjPhotoFrame::itemDoubleClicked(): Testing for item in db row %d\n", row);
         fflush(stdout);
         QListIterator<QGraphicsItem*> it1(it.next());
         while (it1.hasNext()) 
         {
            QGraphicsItem *ritem = it1.next();
            printf("QcjPhotoFrame::itemDoubleClicked(): Testing ritem %ld against item %ld\n", (unsigned long)ritem, (unsigned long)item);
            fflush(stdout);
            if ( ritem == item ) 
            {
               printf("QcjPhotoFrame::itemDoubleClicked(): have match\n");
               fflush(stdout);
               currentScene->clearSelection();
               it1.toFront();
               while (it1.hasNext()) 
                  it1.next()->setSelected(true);
               rec = pQuery->record();
               emit doubleClicked(&rec);
               it.toBack();
               break;
            }
         }
         row++;
         if ( ! pQuery->next() ) 
            break;
      }
   }
   currentScene->update();
   QApplication::restoreOverrideCursor();
   printf("QcjPhotoFrame::itemDoubleClicked(): exit\n");
   fflush(stdout);
}

