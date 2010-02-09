#ifndef TESSTEXT_H
#define TESSTEXT_H

#include "pageres.h"
#include <QString>
#include <QTextCursor>
#include <QTextCharFormat>

namespace qtessnamespace
{
    //Es mejor asi que con #Define para que el compilador sepa el tipo
    static const int LETTERv = 0;
    static const int WORDv = 1;
    static const int ROWv = 2;
    static const int BLOCKv = 3;
    static const int PAGEv = 4;


    /// Tamaño minimo de imagen que merece la pena analizar con tesseract
    static const int kMinRectSize = 10; //min size image
    
    /// Puntos por pulgada
    static const float ptPerInch = 72.27;

    /** Devuelve el nombre del fichero temporal a partir de la direccion de la
      * imagen y de la pagina.
      * @param fileName Direccion de la imagen
      * @param page Subimagen/pagina de la imagen
      */
    QByteArray getTempFileName(QByteArray fileName, int page);
    
    /** Es la funcion que procesa la imagen para generar los archivos temporales.
      * Son imagenes en escala de grises
      * @param direccion de imagen y numero en una cadena.
      */
    QString myPixRead(const QString &list);


/* Return a pixel size (integer) corresponding to POINT size (double)
   on resolution DPI.  */
    float pointToPixel(float point, float dpi);

/* Return a point size corresponding to POINT size (integer)
   on resolution DPI. Note that though point size is a double, we expect
   it to be rounded to an int, so we add 0.5 here. If the desired value
   is tenths of points (as in xfld specs), then the pixel size should
   be multiplied BEFORE the conversion to avoid magnifying the error.  */
    float pixelToPoint(float pixel, float dpi);

    /**
      * Devuelve un QString con espacios en blanco, que se corresponden
      * a los de la imagen
      * @param cursor QTextCursor del documento para recoger la fuente
      * actual y el texto de la linea escrito hasta ahora.
      * @param word Componente del resultado de tesseract para extraer
      * el numero de espacios y la caja que recuadra la palabra.
      * @param dpiCoeffX Coeficiente del editor de texto para contar
      * con exactitud los espacios necesarios
      */
    QString getBlanks(QTextCursor cursor,WERD_RES *word,float dpiCoeffX);
    
    QString printBLOCK(BLOCK *block,int height);
    QString printROW(ROW *row,int height);
    QString printWORD(WERD_RES *wordres,int height);
    QString printTBOX(TBOX box,int height, bool eol);

    /** Calcula la fiabilidad a partir de la tasacion hecha por Tesseract 
      * para una palabra o letra
      * @param rating Valor que se quiere convertir
      * @return Porcentaje de acierto
      */
    float rating_to_cost(float rating);
    
    /**
      * Calcula el tamaño de la fuente a partir de un ROW de Tesseract y la
      * resolucion de la imagen
      * @param row ROW de donde se saca la informacion de Tesseract
      * @param resolution Resolucion vertical de la imagen
      * @return Tamaño de la fuente en puntos por pulgada
      */
    float rowPointSize(ROW *row,int resolution);
    
    /**
      * Calcula el tamaño de la fuente en pixeles a partir de un ROW de
      * Tesseract y la resolucion de la imagen
      * @param row ROW de donde se saca la informacion de Tesseract
      * @return Tamaño de la fuente en pixeles
      */
    float rowPixelSize(ROW *row);

    QTextCharFormat werdFormat(WERD_RES *word,ROW_RES *row,int resolution);

    /**
      * Devuelve si la palabra word es la primera de la linea
      * @param word Word palabra de la que se quiere saber
      * @return True Si, si es cierto. No, si es falso
      */
    bool werdIsBOL(WERD_RES *word);

    /**
      * Devuelve si la palabra word es la última de la linea
      * @param word Word palabra de la que se quiere saber
      * @return True Si, si es cierto. No, si es falso
      */
    bool werdIsEOL(WERD_RES *word);
}

#endif // TESSTEXT_H
