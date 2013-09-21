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

#include "QcjPhotoFrame.h"
#include "QcjPhotoFramePlugin.h"

QcjPhotoFramePlugin::QcjPhotoFramePlugin(QObject *parent)
    : QObject(parent)
{
    initialized = false;
}

void QcjPhotoFramePlugin::initialize(QDesignerFormEditorInterface * /*core*/)
{
    if (initialized)
        return;

    initialized = true;
}

bool QcjPhotoFramePlugin::isInitialized() const
{
    return initialized;
}

QWidget *QcjPhotoFramePlugin::createWidget(QWidget *parent)
{
    return new QcjPhotoFrame(parent);
}

QString QcjPhotoFramePlugin::name() const
{
    QString rv("QcjPhotoFrame");
    printf("QcjPhotoFramePlugin::name(): rv = |%s|", qPrintable(rv));
    return QLatin1String(rv);
}

QString QcjPhotoFramePlugin::group() const
{
    QString rv("QcjData Widgets");
    printf("QcjPhotoFramePlugin::group(): rv = |%s|", qPrintable(rv));
    return QLatin1String(rv);
}

QIcon QcjPhotoFramePlugin::icon() const
{
    return QIcon();
}

QString QcjPhotoFramePlugin::toolTip() const
{
    return QString();
}

QString QcjPhotoFramePlugin::whatsThis() const
{
    return QString();
}

bool QcjPhotoFramePlugin::isContainer() const
{
    return false;
}

QString QcjPhotoFramePlugin::domXml() const
{
    QString rv("<widget class=\"QcjPhotoFrame\" name=\"photoFrame\">\n"
                  " <property name=\"geometry\">\n"
                  "  <rect>\n"
                  "   <x>0</x>\n"
                  "   <y>0</y>\n"
                  "   <width>100</width>\n"
                  "   <height>100</height>\n"
                  "  </rect>\n"
                  " </property>\n"
                  " <property name=\"toolTip\" >\n"
                  "  <string> </string>\n"
                  " </property>\n"
                  " <property name=\"xml_definition\" >\n"
                  "  <string>form_definition</string>\n"
                  " </property>\n"
                  " <property name=\"whatsThis\" >\n"
                  "  <string> </string>\n"
                  " </property>\n"
                  "</widget>\n");
    printf("QcjPhotoFramePlugin::domXml(): rv = |%s|", qPrintable(rv));
    return QLatin1String(rv);

}

QString QcjPhotoFramePlugin::includeFile() const
{
    QString rv("QcjData/QcjPhotoFrame.h");
    printf("QcjPhotoFramePlugin::includeFile(): rv = |%s|\n", qPrintable(rv));
    return QLatin1String(rv);
}

QString QcjPhotoFramePlugin::codeTemplate() const
{
    return QString();
}

Q_EXPORT_PLUGIN(QcjPhotoFramePlugin)


