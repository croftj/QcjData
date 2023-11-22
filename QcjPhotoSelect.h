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

# ifndef QJCPHOTOSELECT
# define QJCPHOTOSELECT

# include <QDialog>
# include <QDir>
# include <QDirModel>
# include <QGraphicsItem>
# include <QGraphicsScene>
# include <QGraphicsView>
# include <QPixmap>
# include <QSettings>
# include <QString>
# include <QStringList>
# include "ui_QcjPhotoSelect.h"

class QcjPhotoSelect : public QDialog 
{
   Q_OBJECT

public:
   QcjPhotoSelect(QString defPath = ".", QWidget *parent = 0, Qt::WindowFlags f = Qt::WindowFlags());
   QPixmap pixmap();
   QString fileName();
   int exec();
   void saveState(QSettings *pSettings);
   void restoreState(QSettings *pSettings);

public slots:
   void setPixmap(QPixmap &pm);
   void haveAdjust();

protected slots:
   void haveActivated (const QModelIndex &index);
   void showPixmap(QPixmap &pm);

protected:
private:
   QGraphicsScene       scene;
   Ui::QcjPhotoSelect   ui;
};
# endif
