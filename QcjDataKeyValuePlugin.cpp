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

#include "QcjDataKeyValue.h"
#include "QcjDataKeyValuePlugin.h"

QcjDataKeyValuePlugin::QcjDataKeyValuePlugin(QObject *parent)
    : QObject(parent)
{
    initialized = false;
}

void QcjDataKeyValuePlugin::initialize(QDesignerFormEditorInterface * /*core*/)
{
    if (initialized)
        return;

    initialized = true;
}

bool QcjDataKeyValuePlugin::isInitialized() const
{
    return initialized;
}

QWidget *QcjDataKeyValuePlugin::createWidget(QWidget *parent)
{
    return new QcjDataKeyValue(parent);
}

QString QcjDataKeyValuePlugin::name() const
{
    return QLatin1String("QcjDataKeyValue");
}

QString QcjDataKeyValuePlugin::group() const
{
    return QLatin1String("QcjData Widgets");
}

QIcon QcjDataKeyValuePlugin::icon() const
{
    return QIcon();
}

QString QcjDataKeyValuePlugin::toolTip() const
{
    return QString();
}

QString QcjDataKeyValuePlugin::whatsThis() const
{
    return QString();
}

bool QcjDataKeyValuePlugin::isContainer() const
{
    return false;
}

QString QcjDataKeyValuePlugin::domXml() const
{
    return QLatin1String("<widget class=\"QcjDataKeyValue\" name=\"dataForm\">\n"
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
}

QString QcjDataKeyValuePlugin::includeFile() const
{
    return QLatin1String("QcjData/QcjDataKeyValue.h");
}

QString QcjDataKeyValuePlugin::codeTemplate() const
{
    return QString();
}

Q_EXPORT_PLUGIN(QcjDataKeyValuePlugin)

