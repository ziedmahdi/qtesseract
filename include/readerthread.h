/**********************************************************************
 * File:        readerthread.h 
 * Description: The ReaderThread class provides a thread to process OCR
 * in a Image.
 * Author:      Roberto Benitez Monje
 * Created:     Mon Jan 04 2010
 **********************************************************************/
#ifndef READERTHREAD_H
#define READERTHREAD_H


#ifdef __cplusplus
#include "baseapi.h"
#include "tesstext.h"
using namespace tesseract;
using namespace qtessnamespace;
#include "pageres.h" //block,row, werd_res, etc.
#include "allheaders.h" //lib leptonica
    
#include <QGraphicsRectItem>
#include <QTextDocument>
#include <QThread>
#include <QTreeWidgetItem>
#include <QWaitCondition>

class Container;

    /**
      Es la clase que realiza el proceso OCR.
      * @author Roberto Benitez Monje
    */
    class ReaderThread : public QThread, private TessBaseAPI
    {
    public:
    
	Q_OBJECT
    
    public:
	/**
	  * Constructor de ReaderThread
	  * @param parent El QWidget padre
	  */
	ReaderThread(QObject *parent = 0);
	//~ReaderThread();
    
	/**
	  * Añade a la cola de trabajo la pagina pageNumber del archivo fileName.
	  * Los items de arbol se añaden a partir de treePage.
	  * @param fileName Nombre del archivo de imagen que se quiere procesar.
	  * @param pageNumber Numero de pagina de la imagen
	  * @param treePage, item del arbol asociado a la pagina
	  */
	void read(const QByteArray &fileName,int pageNumber,QTreeWidgetItem *treePage);
	
	/**
	  * Ordena a la instancia de esta clase que comience el proceso OCR.
	  * @param enhance Si se aplica al OCR metodos extras o no.
	  * @param language Lenguaje para hacer OCR.
	  * @param dpiQTextEditX Resolucion del editor de texto. Para calcular cuantos
	  * espacios en blanco entre palabras corresponden en la imagen.
	  */
	void start(bool enhance,QByteArray language, float dpiQTextEditX);
    
    public slots:
    
    signals:
	/**
	  * Señal que recibe la clase principal MyQMainWindow. Manda resultado de
	  * tesseract por componente.
	  * @param treeItem Item del árbol asociado al componente
	  * @param type Tipo de componente (letra, palabra, linea, parrafo)
	  * @param pageNumber Numero de pagina
	  * @param left Coordenada izquierda del rectangulo que recuadra al componente
	  * @param top Coordenada superior.
	  * @param right Coordenada derecha
	  * @param bottom Coordenada inferior
	  */
	void newItem(QTreeWidgetItem *treeItem,int type,int pageNumber, int left, int top,
		     int right, int bottom);
	/**
	  * Actualiza la ventana de progreso
	  * @param state Valor de la barra de progreso
	  * @param info Cadena de texto de informacion
	  */
	void newState(int state,const QString &info);
	
	/**
	  * Actualiza la barra de la ventana de progreso
	  * @param value Valor de la barra de progreso
	  */
	void setValue(int value);
	
	/**
	  * Envia el documento al editor de texto
	  * @param pageNumber Identifica que pagina es
	  * @param doc Puntero 
	  */
	void writedDocument(int pageNumber, QTextDocument *doc);
    
    
    private:
	//Tesseract
	/// Directorio donde se encuentra la carpeta tessdata sin acabar en barra.	
	QByteArray dataDir;
	
	/// Colores para los items del arbol.
	QBrush block0Brush,block1Brush,block2Brush;
	QBrush row0Brush,row1Brush,row2Brush;
	QBrush word0Brush,word1Brush,word2Brush;
	QBrush letter0Brush,letter1Brush,letter2Brush;
	
	void createBrushes();
	
	/**
	  * Explora el resultado de tesseract para crear los items del arbol
	  * @param treePage Item del árbol padre, del cual cuelgan los que se
	  * van a crear
	  * @param height Altura de la pagina. Para cambiar el sentido de la
	  * coordenada y.
	  * @param pageNumber Numero de la pagina que se está procesando
	  */
	int getTree(QTreeWidgetItem *treePage,int height,int pageNumber);
	
	/**
	  * Realiza el proceso de OCR para una pagina.
	  * @param c Contiene un contenedor con lo necesario para procesar una pagina
	  */
	int process(Container *c);
	
	/**
	  * Es el punto de comienzo del thread. Cuando se le llama con start, el thread
	  * ejecuta esta funcion.
	  */
	void run();
	
	
	QTreeWidgetItem *insertWord(QTreeWidgetItem *treeRow,int wordCount,WERD_RES *wordres,
				    int height, int pageNumber);
	QTreeWidgetItem *insertRow(QTreeWidgetItem *treeBlock,int rowCount,ROW *row,int height
				   ,int pageNumber);
	QTreeWidgetItem *insertBlock(QTreeWidgetItem *treePage,int blockCount,BLOCK *block,
				     int height, int pageNumber);
	void insertLetters(QTreeWidgetItem *treeWord,WERD_RES *word,int height, int pageNumber);
    
	/// Contador que suma las letras de las filas
	int blockCharsCount;
	
	bool enhance;
	QByteArray language;
	float dpiCoeffX;
    };
    
    class Container
    {
	public:
	    Container(QByteArray fileName,int pageNumber,QTreeWidgetItem *treePage);
    
	    QByteArray fileName;
	    int pageNumber;
	    QTreeWidgetItem *treePage;
    };
    //! [0]
#endif
    
#endif
