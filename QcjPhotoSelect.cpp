/*********************************************************************************
**
**   $Id$
**   Copyright (c) 2007 Joe Croft <joe@croftj.net>
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

# include <QCameraInfo>
# include <QDialog>
# include <QDir>
# include <QDirModel>
# include <QGraphicsItem>
# include <QGraphicsScene>
# include <QGraphicsView>
# include <QPixmap>
# include <QString>
# include <QStringList>
# include <QTransform>
# include "QcjPhotoSelect.h"

QcjPhotoSelect::QcjPhotoSelect(QString defPath, QWidget *parent, Qt::WindowFlags f) : QDialog(parent, f)
{
   QStringList nameFilters;
   ui.setupUi(this);

   printf("QcjPhotoSelect::QcjPhotoSelect(): Enter, defPath = |%s|\n", qPrintable(defPath));
   nameFilters << "*.jpg" << "*.png" << "*.gif" << "*.jpeg";
//      nameFilters"*";
   QDirModel *dm = new QDirModel(nameFilters, QDir::Files|QDir::AllDirs, QDir::DirsFirst|QDir::Name);
//      QDirModel *dm = new QDirModel(nameFilters, QDir::Files, QDir::Name);
//      QDirModel *dm = new QDirModel(nameFilters, QDir::Files|QDir::AllDirs, QDir::Name);
//      QDirModel *dm = new QDirModel;
   ui.treeView->setModel(dm);
//      ui.treeView->setRootIndex(dm->index(QDir::cleanPath(defPath)));
//   ui.treeView->setRootIndex(dm->index("/"));
   ui.treeView->setCurrentIndex(dm->index(defPath));
   ui.treeView->setExpanded(dm->index(defPath), true);
   ui.treeView->setColumnWidth(0, 256);
   ui.treeView->resizeColumnToContents(0);

   connect(ui.treeView, SIGNAL(activated(const QModelIndex &)), 
            this, SLOT(haveActivated(const QModelIndex &)), Qt::UniqueConnection);
   connect(ui.treeView, SIGNAL(clicked(const QModelIndex &)), 
            this, SLOT(haveActivated(const QModelIndex &)), Qt::UniqueConnection);
   connect(ui.adjustBtn, SIGNAL(clicked()), 
            this, SLOT(haveAdjust()), Qt::UniqueConnection);
   ui.graphicsView->setScene(&scene);
   ui.graphicsView->show();
   ui.okBtn->setEnabled(false);

   const QList<QCameraInfo> cameras = QCameraInfo::availableCameras();
   for (const QCameraInfo &cameraInfo : cameras) {
      qDebug() << "Found camera: " << cameraInfo.deviceName() << ", descr: " << cameraInfo.description();
   }
   printf("QcjPhotoSelect::QcjPhotoSelect(): Exit\n");
}

QPixmap QcjPhotoSelect::pixmap()
{
   return(((QGraphicsPixmapItem*)scene.itemAt(0, 0, QTransform()))->pixmap());
}

QString QcjPhotoSelect::fileName()
{
//   return(((QDirModel*)ui.treeView->model())->fileName(ui.treeView->currentIndex()));
   return(((QDirModel*)ui.treeView->model())->filePath(ui.treeView->currentIndex()));
};

void QcjPhotoSelect::setPixmap(QPixmap &pm)
{
   showPixmap(pm);
}

void QcjPhotoSelect::haveActivated(const QModelIndex &index) 
{
   printf("QcjPhotoSelect::haveActivated(): Enter\n");
   fflush(stdout);
   QDirModel *dm = (QDirModel*)ui.treeView->model();
   if ( ! dm->isDir(index) ) 
   {  
      printf("QcjPhotoSelect::haveActivated(): Item is not a Directory\n");
      fflush(stdout);
      QPixmap pm;
      if ( pm.load(dm->filePath(index)) )
      {
         ui.okBtn->setEnabled(true);
         showPixmap(pm);
      }
   }
   printf("QcjPhotoSelect::haveActivated(): Exit\n");
   fflush(stdout);
}

void QcjPhotoSelect::showPixmap(QPixmap &pm)
{
   printf("QcjPhotoSelect::showPixmap(): Enter\n");
   fflush(stdout);
   QGraphicsItem *item = scene.itemAt(0, 0, QTransform());
   if ( item != 0 ) 
   {
      printf("QcjPhotoSelect::showPixmap(): Removing existing item\n");
      fflush(stdout);
      scene.removeItem(item);
      delete item;
   }
   item = scene.addPixmap(pm);
   ui.graphicsView->fitInView(item, Qt::KeepAspectRatioByExpanding);
//   ui.graphicsView->fitInView(item, Qt::KeepAspectRatio);
   ui.graphicsView->show();
   printf("QcjPhotoSelect::showPixmap(): Exit\n");
   fflush(stdout);
}

int QcjPhotoSelect::exec()
{
   show();
   QGraphicsItem *item = scene.itemAt(0, 0, QTransform());
   if ( item != 0 ) 
   {
      ui.graphicsView->fitInView(item, Qt::KeepAspectRatioByExpanding);
//      ui.graphicsView->fitInView(item, Qt::KeepAspectRatio);
   }
   return(QDialog::exec());
}


void QcjPhotoSelect::haveAdjust()
{
   printf("QcjPhotoSelect::haveAdjust(): Enter\n");
   fflush(stdout);
   QGraphicsItem *item = scene.itemAt(0, 0, QTransform());
   if ( item != 0 ) 
   {
      printf("QcjPhotoSelect::haveAdjust(): Adjusting existing item\n");
      fflush(stdout);
      ui.graphicsView->fitInView(item, Qt::KeepAspectRatioByExpanding);
//      ui.graphicsView->fitInView(item, Qt::KeepAspectRatio);
   }
   printf("QcjPhotoSelect::haveAdjust(): Exit\n");
   fflush(stdout);
}

void QcjPhotoSelect::saveState(QSettings *pSettings)
{
   pSettings->beginGroup("PhotoSelect");
   pSettings->setValue("DialogSplitter", ui.splitter->saveState());
   pSettings->setValue("DialogGeometry", geometry());
   pSettings->endGroup();
}

void QcjPhotoSelect::restoreState(QSettings *pSettings)
{
   pSettings->beginGroup("PhotoSelect");
   if (pSettings->value("DialogGeometry", QRect(0, 0, 0, 0)).toRect().height() > 0)
   {
      QRect rect = pSettings->value("DialogGeometry").toRect();
      printf("QcjPhotoSelect::restoreState(): Dlg Height = %d, width = %d, top = %d, left = %d\n", rect.height(), rect.width(), rect.top(), rect.left());
      move(rect.topLeft());
      resize(rect.size());
      ui.splitter->restoreState(pSettings->value("DialogSplitter").toByteArray());
   }
   pSettings->endGroup();
}

