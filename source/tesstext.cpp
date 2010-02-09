#include "include/tesstext.h"
#include <QFontMetricsF>
#include <QTextBlock>
#include <QTextDocument>
#include <QtDebug>


namespace qtessnamespace
{

    QString getBlanks(QTextCursor cursor,WERD_RES *word,float dpiCoeffX)
    {
        QString blanksString(" ");
        int blanks = word->word->space();
        if (blanks > 1) {
            QFontMetricsF metrics(cursor.charFormat().font());
            float wordPos = dpiCoeffX*(word->word->bounding_box().left()) -
                            cursor.block().blockFormat().leftMargin();
            QString aux = cursor.block().text();
            float currentPos = metrics.width(aux);
            blanks = 0;
            while (currentPos < wordPos) {
                aux = aux.append(" ");
                currentPos = metrics.width(aux);
                blanks++;
            }
            blanksString = blanksString.repeated(blanks);
        }
        return blanksString;
    }
#include <QStringList>
#include "allheaders.h"

QByteArray getTempFileName(QByteArray fileName, int page)
{
    QByteArray tempName(fileName);
    tempName.append(QString("[%1].temp").arg(page+1));
    return tempName;
}
    float pointToPixel(float point, float dpi)
    {
        return ((point) * (dpi) / ptPerInch + 0.5);
    }

    float pixelToPoint(float pixel, float dpi)
    {
        return ((pixel) * ptPerInch / (dpi) + 0.5);
    }

    QString printTBOX(TBOX box,int height, bool eol)
    {
        if (eol)
            return QString ("Bounding box=(%1,%2)->(%3,%4)\n").arg(box.left())
                .arg(height - box.top()).arg(box.right()).arg(height - box.bottom());
        else
            return QString ("Bounding box=(%1,%2)->(%3,%4)").arg(box.left())
                .arg(height - box.top()).arg(box.right()).arg(height - box.bottom());
    }

    QString printWORD(WERD_RES *wordres,int height)
    {
        WERD *word = wordres->word;
        QString aux;
        BITS16 flags;                //flags about word
        flags.set_bit(W_SEGMENTED,word->flag(W_SEGMENTED));
        flags.set_bit(W_ITALIC,word->flag(W_ITALIC));
        flags.set_bit(W_BOL,word->flag(W_BOL));
        flags.set_bit(W_EOL,word->flag(W_EOL));
        flags.set_bit(W_NORMALIZED,word->flag(W_NORMALIZED));
        flags.set_bit(W_POLYGON,word->flag(W_POLYGON));
        flags.set_bit(W_LINEARC,word->flag(W_LINEARC));
        flags.set_bit(W_DONT_CHOP,word->flag(W_DONT_CHOP));
        flags.set_bit(W_REP_CHAR,word->flag(W_REP_CHAR));
        flags.set_bit(W_FUZZY_SP,word->flag(W_FUZZY_SP));
        flags.set_bit(W_FUZZY_NON,word->flag(W_FUZZY_NON));
        aux.append(QString("Blanks= %1\n").arg(word->space()));
        aux.append(printTBOX(word->bounding_box(),height,true));
        aux.append(QString("Correct= %1\n")
                    .arg(wordres->best_choice->unichar_string().string()));
        aux.append(QString("Flags = %1 = 0%2\n").arg(flags.val)
                  .arg(flags.val,0,8));
        aux.append (QString("   W_SEGMENTED = %1\n")
                  .arg(word->flag(W_SEGMENTED) ? "TRUE" : "FALSE "));
        aux.append (QString("   W_ITALIC = %1\n")
                  .arg(word->flag(W_ITALIC) ? "TRUE" : "FALSE "));
        aux.append (QString("   W_BOL = %1\n")
                  .arg(word->flag(W_BOL) ? "TRUE" : "FALSE "));
        aux.append (QString("   W_EOL = %1\n")
                  .arg(word->flag(W_EOL) ? "TRUE" : "FALSE "));
        aux.append (QString("   W_NORMALIZED = %1\n")
                  .arg(word->flag(W_NORMALIZED) ? "TRUE" : "FALSE "));
        aux.append (QString("   W_POLYGON = %1\n")
                  .arg(word->flag(W_POLYGON) ? "TRUE" : "FALSE "));
        aux.append (QString("   W_LINEARC = %1\n")
                  .arg(word->flag(W_LINEARC) ? "TRUE" : "FALSE "));
        aux.append (QString("   W_DONT_CHOP = %1\n")
                  .arg(word->flag(W_DONT_CHOP) ? "TRUE" : "FALSE "));
        aux.append (QString("   W_REP_CHAR = %1\n")
                  .arg(word->flag(W_REP_CHAR) ? "TRUE" : "FALSE "));
        aux.append (QString("   W_FUZZY_SP = %1\n")
                  .arg(word->flag(W_FUZZY_SP) ? "TRUE" : "FALSE "));
        aux.append (QString("   W_FUZZY_NON = %1\n")
                  .arg(word->flag(W_FUZZY_NON) ? "TRUE" : "FALSE "));
        aux.append(QString("Rejected cblob count = %1")
                    .arg(word->rej_cblob_list()->length()));
        return aux;

    }
    
    QString myPixRead(const QString &list)
    {
	QString imageInfo;
	l_int32    d = 0;
	l_uint32 pxres = 0,pyres = 0;
	l_float32  scalex = 0.0,scaley = 0.0;
	PIX *pixs = NULL, *pix1 = NULL, *pixd = NULL;
    
	QStringList list2 = list.split(",");
	if (list2.count() != 2) {
		 return QObject::tr("Failed to read QStringList");
	     }
	const QByteArray aux1 = list2.at(0).toUtf8(); //aux1 and aux2 must be not delete or change
	const char *fileName = aux1.constData();
	const QByteArray aux2 = list2.at(1).toUtf8();
	int page = aux2.toInt(0,10);
	if (page == 0) {
	    if ((pixs = pixRead(fileName)) == NULL) {
		 QString msg = QObject::tr("Failed to open image %1").arg(fileName);
		 return msg;
	     }
	}
	else {
	    if ((pixs = pixReadTiff(fileName, page)) == NULL) {
		 QString msg = QObject::tr("Failed to open subimage %1 %2")
				 .arg(fileName)
				 .arg(page);
		 return msg;
	    }
	}
    
    
	d = pixGetDepth(pixs);
	if (d != 1 && d != 2 && d != 4 && d != 8 && d != 16 && d != 32) {
	     QString msg = QObject::tr("depth image must be 1,2,4,8,16 or 32");
	     return msg;
	}
    
	pixGetResolution(pixs,&pxres,&pyres);
	imageInfo = QString("%1,%2,%3,%4,%5").arg(page).arg(pixs->w).arg(pixs->h).arg(pxres)
										    .arg(pyres);
	if (pxres && pyres) {
	    scalex = 300.0 / pxres;
	    scaley = 300.0 / pyres;
	    pix1 = pixScale(pixs,scalex,scaley);
	    if (pix1->d > 1) {
		pixd = pixConvertTo8(pix1,FALSE);
		pixDestroy(&pix1);
	    }
	    else
		pixd = pix1;
	}
	else
	    if (pixs->d > 1)
		pixd = pixConvertTo8(pixs,FALSE);
	    else
		pixd = pixClone(pixs);
	pixDestroy(&pixs);
	
	/* ------------------ Image file format types -------------- */
	/*  
	 *  The IFF_UNKNOWN flag is used to write the file out in
	 *  a compressed and lossless format; namely, IFF_TIFF_G4
	 *  for d = 1 and IFF_PNG for everything else.
	 */
	QByteArray fileOut(getTempFileName(aux1,page));
	if (pixWrite(fileOut.constData(),pixd,IFF_DEFAULT)) {
	     QString msg = QObject::tr("pix not written to file");
	     return msg;
	}
	pixDestroy(&pixd);
	return imageInfo;
    }

    QString printROW(ROW *row,int height)
    {
        QString aux;
        aux.append(QString("Ascrise= %1\n").arg(row->ascenders()));
        aux.append(printTBOX(row->bounding_box(),height,true));
        aux.append(QString("Descdrop= %1\n").arg(row->descenders()));
        aux.append(QString("Kerning= %1\n").arg(row->kern()));
        aux.append(QString("Spacing= %1\n").arg(row->space()));
        aux.append(QString("Xheight= %1").arg(row->x_height()));
        return aux;
    }

    QString printBLOCK(BLOCK *block,int height)
    {
        QString aux;
        aux.append(printTBOX(block->bounding_box(),height,true));
        aux.append(QString("Filename= %1").arg(block->name()));
        aux.append(QString("Fixed_pitch= %1\n").arg(block->fixed_pitch()));
        aux.append(QString("Kerning= %1\n").arg(block->kern()));
        aux.append(QString("Proportional= %1\n").arg(block->prop() ? "TRUE" : "FALSE"));
        aux.append(QString("Spacing= %1").arg(block->space()));
        return aux;
    }

    //baseapi.cpp:1353
    float rating_to_cost(float rating) {
      rating = 100 + 5*rating;
      // cuddled that to save from coverage profiler
      // (I have never seen ratings worse than -100,
      //  but the check won't hurt)
      if (rating < 0) rating = 0;
      return rating;
    }

    float rowPointSize(ROW *row,int resolution)
    {
        float ptsize = pixelToPoint(rowPixelSize(row), resolution);
        return ptsize;
    }

    float rowPixelSize(ROW *row)
    {
        float ptsize = row->x_height () + row->ascenders () - row->descenders ();
        return ptsize;
    }

    QTextCharFormat werdFormat(WERD_RES *word,ROW_RES *row,int resolution)
    {
        QTextCharFormat newFormat;

        float ptsize;
        bool italic = false;
	int res = resolution;
	if ( resolution == 0 )
	    res = 300;
        // x_heigh = altura de la letra tipo o,x..a..No b ni j.
        // ascenders = tamaño de las letras altas
        // descenders = tamaño de las letras bajas
        // medidas en pixeles
        ptsize = rowPointSize(row->row,res);
        if (ptsize < 6)
            newFormat.setFontPointSize(6);
        else
            newFormat.setFontPointSize(ptsize);
        italic = word->italic > 0 || (word->italic == 0 && row->italic > 0);
        newFormat.setFontItalic(italic);
        if (word->bold > 0 || (word->bold == 0 && row->bold > 0)) {
            newFormat.setFontWeight(QFont::Bold);
        }
        else
            newFormat.setFontWeight(QFont::Normal);
        /*
        QFont font;
        newFormat.setFont(font); //Tira error al guardar en ODF
        */

        return newFormat;
    }

    bool werdIsBOL(WERD_RES *word)
    {
        return word->word->flag(W_BOL);
    }


    bool werdIsEOL(WERD_RES *word)
    {
        return word->word->flag(W_EOL);
    }


}
