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

#include "QcjDataTable.h"
#include "QcjDataTablePlugin.h"

QcjDataTablePlugin::QcjDataTablePlugin(QObject *parent)
    : QObject(parent)
{
    initialized = false;
}

void QcjDataTablePlugin::initialize(QDesignerFormEditorInterface * /*core*/)
{
    if (initialized)
        return;

    initialized = true;
}

bool QcjDataTablePlugin::isInitialized() const
{
    return initialized;
}

QWidget *QcjDataTablePlugin::createWidget(QWidget *parent)
{
    return new QcjDataTable(parent);
}

QString QcjDataTablePlugin::name() const
{
    return QLatin1String("QcjDataTable");
}

QString QcjDataTablePlugin::group() const
{
    return QLatin1String("QcjData Widgets");
}

QIcon QcjDataTablePlugin::icon() const
{
    return QIcon();
}

QString QcjDataTablePlugin::toolTip() const
{
    return QString();
}

QString QcjDataTablePlugin::whatsThis() const
{
    return QString();
}

bool QcjDataTablePlugin::isContainer() const
{
    return false;
}

QString QcjDataTablePlugin::domXml() const
{
/*
    return QLatin1String("<widget class=\"QcjDataTable\" name=\"QcjDataTable\">\n"
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
                         "  <string> </string>\n"
                         " </property>\n"
                         " <property name=\"whatsThis\" >\n"
                         "  <string> </string>\n"
                         " </property>\n"
                         "</widget>\n");
*/
    return QLatin1String("<widget class=\"QcjDataTable\" name=\"dataTable\">\n"
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
                         "  <string>table_definition</string>\n"
                         " </property>\n"
                         " <property name=\"whatsThis\" >\n"
                         "  <string> </string>\n"
                         " </property>\n"
                         "</widget>\n");
/*
    return QLatin1String("<widget class=\"WorldTimeClock\" name=\"worldTimeClock\">\n"
                         " <property name=\"geometry\">\n"
                         "  <rect>\n"
                         "   <x>0</x>\n"
                         "   <y>0</y>\n"
                         "   <width>100</width>\n"
                         "   <height>100</height>\n"
                         "  </rect>\n"
                         " </property>\n"
                         "</widget>\n");
*/
}

QString QcjDataTablePlugin::includeFile() const
{
    return QLatin1String("QcjData/QcjDataTable.h");
}

QString QcjDataTablePlugin::codeTemplate() const
{
    return QString();
}

Q_EXPORT_PLUGIN(QcjDataTablePlugin)
