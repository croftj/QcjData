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
#include "Qcj.h"

static QStringList littleWords = {"a", "and", "at", "but", "by", "for", "in", "of", "on", "or", "the", "to"};

/**************************************************************************/
/* This function will capitalize the words of a string to make it a title */
/**************************************************************************/
QString Qcj::makeTitle(QString& string)
{
   QString rv;
   QStringList list = string.toLower().split(QRegExp("\\s+"));
   int count = 0;
   foreach (QString word, list)
   {
      if ( count == 0 || ((count + 1) == list.length()) || (! littleWords.contains(word)))
      {
         word.replace(0, 1, word[0].toUpper());
         rv.append(word + ' ');
      }
      else
      {
         rv.append(word + ' ');
      }
      count++;
   }
   return(rv.trimmed());
}
