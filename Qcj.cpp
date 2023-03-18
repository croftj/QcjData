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
