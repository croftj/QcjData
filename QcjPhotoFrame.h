/*********************************************************************************
**
**   $Id$
**   Copyright (c) 2005 Joe Croft <joe@croftj.net>
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
#ifndef QCJPHOTOFRAME_H
#define QCJPHOTOFRAME_H

#include <vector>

# include <QFrame>
# include <QGraphicsScene>
# include <QGraphicsSceneMouseEvent>
# include <QGraphicsView>
# include <QMap>
# include <QSettings>
# include <QSqlQuery>
# include <QSqlRecord>
# include <QString>
# include <QTimer>


#ifndef QT4_DESIGNER_PLUGIN
# include "QcjData/QcjDataXML.h"
# include "QcjData/QcjDataHelpers.h"
#endif

# ifdef QT4_DESIGNER_PLUGIN
struct QcjDataFields 
{
   int   x;
};
# endif

class QcjPhotoScene : public QGraphicsScene
{
   Q_OBJECT

public:
   QcjPhotoScene(QObject *parent = 0) : QGraphicsScene(parent)
   {
      connect(&pressedTimer, SIGNAL(timeout()), this, SLOT(clearPressed()));
   };

protected:
   void mousePressEvent(QGraphicsSceneMouseEvent *ev);
   void mouseReleaseEvent(QGraphicsSceneMouseEvent *ev);

protected slots:
   void clearPressed();

signals:
   void clicked(QGraphicsItem*);

private:
   QGraphicsItem  *lastPressed;
   QTimer         pressedTimer;
};

#ifndef QT4_DESIGNER_PLUGIN
class QcjPhotoFrame : public QGraphicsView
#else
class QcjPhotoFrame : public QFrame
#endif
{
   Q_OBJECT
   Q_PROPERTY (QString xml_definition READ readXmlDef WRITE writeXmlDef)

public:
   QcjPhotoFrame(QWidget *parent);

   void setDatabase();
   void setFields(std::vector<struct QcjDataFields> fds);
   QString readXmlDef() const { return(xmldef); };
   void writeXmlDef(QString s);
   void setFilter(QString filter)
   {
      whereClause = filter;
   };

   void find(QMap<QString, QString>) {};

signals:
   void rowSelected(QSqlRecord *rec);

public slots:
   void refresh();
   void selectRow(int row);

protected slots:
   void itemClicked(QGraphicsItem*);

protected:
   void _setFields(const QcjDataFields *fields);

private:
   QString                          dbTable;
   QString                          whereClause;
   QSqlQuery                        *pQuery;
   QcjPhotoScene                    *currentScene;
   int                              current_row;
   std::vector<struct QcjDataFields>   fieldDefs;
   QVector<QList<QGraphicsItem*> >  photoItemLists;
   QSqlRecord                       rec;
   QString                          xmldef;
};

#endif
