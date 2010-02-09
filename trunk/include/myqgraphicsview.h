/**********************************************************************
 * File:        myqgraphicsview.h 
 * Description: The MyQGraphicsView class provides a image viewer for
 ** displaying the result of Tesseract.
 * Author:      Roberto Benitez Monje
 * Created:     Mon Jan 04 2010
 **********************************************************************/
#ifndef MYQGraphicsView_H
#define MYQGraphicsView_H

#include <QGraphicsView>
#include <QPixmapCache>

#include <QHash>
#include "zoomarea.h"
namespace qtessnamespace
{    
    /**
      * La clase que visualiza la imagen. Tambien muestra resultado de
      * Tesseract
      * @author Roberto Benitez Monje
    */
    class MyQGraphicsView : public QGraphicsView
    {
	Q_OBJECT
    public:
	/**
	  * Constructor de MyQGraphicsView
	  * @param parent El QWidget padre
	  */
	MyQGraphicsView(QWidget *parent = 0);
    
	/**
	  * Inserta un QGraphicsRectItem en el escenario
	  * @param rect Posicion y dimensiones del item rectangulo
	  */
	QGraphicsRectItem* insertPageRectItem(const QRect &rect);
	
	/**
	  * Extrae de una lista de cadenas el tamaño de cada imagen.
	  * Cada cadena contiene pagina, tamaño y resolucion de una imagen.
	  * @param v es la lista de cadenas, cada una es de un campo.
	  */
	void insertPagesInfo(const QList<QString> &v);
    
    public slots:
	
	/**
	  * Establece la vista para ver la imagen ajustada a la ventana
	  */
	void fitToWindow();
	
	/**
	  * Establece la vista para ver la imagen en tamaño original
	  */
	void normalSize();
    
	/**
	  * Cambia la imagen de fondo por la que se pasa por entrada,
	  * restablece la vista y los zooms guardados
	  * @param fileName Nombre fisico del fichero que contiene la imagen
	  * @param page Indice de pagina en TIFF multipaginas. page > 0
	  */
	void pageChanged(const QByteArray &fileName, int page);
    
	/**
	  * Destruye y asigna nuevo escenario y objeto ZoomArea.
	  */
	void removeRectItemList();
    
	/**
	  * Resetea tanto el visor como el escenario
	  */
	void reset();
    
	/**
	  * Restaura los zooms previamente guardados. Cuando se realiza el OCR se
	  * muestra en el visor por donde va. Esto cambiaría la pila de zooms guardados.
	  * Esta funcion esta ligada a saveAndLock().
	  */
	void restoreAndUnlock();
    
	/**
	  * Bloquea el guardado de zooms. Cuando se realiza el OCR se muestra
	  * en el visor por donde va. Esto cambiaría la pila de zooms guardados.
	  * Esta funcion bloquea los zooms contra modificaciones de la vista.
	  * Esta funcion esta ligada a restoreAndUnlock().
	  */
	void saveAndLock();
	
	/**
	  * Devuelve la matriz de estado de la vista.
	  */
	QMatrix viewMatrix();
    
	/**
	  * Aumenta en un 25% la vista de imagen.
	  */
	void zoomIn();
    
	/**
	  * Disminuye en un 25% la vista de imagen.
	  */
	void zoomOut();
    protected:
	/**
	  * Dibuja el fondo usando un QPainter, debajo de todo item.
	  * @param painter Clase de pintado de bajo nivel.
	  * @param rect Rectangulo del escenario que se va apintar.
	  */
	void drawBackground (QPainter *painter, const QRectF &rect);
	
	/**
	  * Cuando se hace doble clic sobre zoomarea se hace zoom en ese area.
	  * @param event Evento que se recibe cuando se hace doble clic con raton
	  */
	void mouseDoubleClickEvent(QMouseEvent * event);
	
	/**
	  * Implementa el inicio de creacion de zoomarea y su ajuste de tamaño.
	  * Tambien la eliminacion de zoomarea.
	  * @param event Evento que se recibe cuando se pulsa un boton del raton
	  */
	void mousePressEvent(QMouseEvent *event);
	
	/**
	  * Cuando el raton se mueve por creacion o cambio de tamaño de zoomarea.
	  * Tambien cuando se mueve con zoomarea en el escenario en otro caso.
	  * @param event Evento que se recibe cuando se mueve el raton
	  */
	void mouseMoveEvent(QMouseEvent *event);
	
	/**
	  * Vuelve al estado inicial del visor. Sirve tambien para terminar la creacion
	  * de zoomarea.
	  * @param event Evento que se recibe cuando se suelta un boton del raton.
	  */
	void mouseReleaseEvent(QMouseEvent *event);
    
    private:
	
	/**
	  * Lee la imagen del archivo temporal.
	  * @param fileName Nombre del fichero temporal que contiene la imagen.
	  * @param page Numero de pagina >= 1
	  */
	QPixmap loadImage(const QByteArray &fileName,int page);
	
	QPixmap loadImage(const QByteArray &fileName,int page, QSize size);
	
	void removeImage(const QByteArray &fileName, int page);
	
	QByteArray fileName;
	
	/// Almacena los zoomareas para las distintas paginas.
	QHash< int,ZoomArea *> zoomAreaHash;
	
	/// Almacena el estado del visor para cada pagina.
	QHash< int,QMatrix> matrixHash;
	
	/// Guarda los tamaños de las imagenes
	QList<QSizeF> sizeHash;
	
	/// Fondo del escenario
	QPixmap pixmap;
	
	/// Devuelve si no se esta realizando OCR.
	bool notReading;
	
	/// Ultima pagina visitada.
	int currentPage;
	
	/// ZoomArea de la pagina actual.
	ZoomArea *zoomarea;
    };
}
#endif // MYQGraphicsView_H
