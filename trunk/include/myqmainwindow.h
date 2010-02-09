/**********************************************************************
 * File:        myqmainwindow.h 
 * Description: The MyQMainWindow class provides the main application 
 * window.
 * Author:      Roberto Benitez Monje
 * Created:     Mon Jan 04 2010
 **********************************************************************/
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QBasicTimer>
#include <QComboBox>
#include <QHBoxLayout>
#include <QMainWindow>
#include <QProgressDialog>
#include <QSpinBox>

#include "myqgraphicsview.h"
#include "myqtextedit.h"
#include "myqtreewidget.h"
#include "readerthread.h"

namespace qtessnamespace
{
    
    /**
      * La clase principal. Tiene como base el objeto de QT ventana
      * de aplicacion.
      * @author Roberto Benitez Monje
    */
    class MyQMainWindow : public QMainWindow
    {
	Q_OBJECT
    
    public:
	/**
	  * Constructor de MyQMainWindow
	  */
	MyQMainWindow();
	
    private slots:
	/**
	  * Muestra un mensaje en una nueva ventana sobre el programa.
	  */
	void about();
	
	/**
	  * Cambia el dibujo de la accion de enhance, el de la bombilla. Tambien pone a
	  * true o false dependiendo de si esta marcado o no la accion enhance.
	  */
	void enhance();
	
	/**
	  * Oculta todos los items de las página en el visor
	  */
	void hideAllPageItems();	
	
	/**
	  * Es el slot que recibe los items del thread que realiza OCR.
	  * Crea el QGraphicsItem en el visor. Va mostrando las palabras
	  * recibidas y cambia de pagina en caso de ser necesario.
	  * @param treeItem Item del arbol creado en OCR
	  * @param type Tipo de componente: 0 letra, 1 palabra, 2 fila, 3 parrafo
	  * @param pageNumber Para identificar a que documento corresponde
	  */
	void newItem(QTreeWidgetItem *treeItem,int type,int pageNumber, int left, int top,
		     int right, int bottom);
	
	/**
	  * El thread de OCR le envia a este slot por donde va y un mensaje de texto
	  * informativo para que se muestre en la ventana de progreso.
	  * @param state Valor que se corresponde con la barra de progreso
	  * @param info Cadena de texto para informar en que parte del proceso está.
	  */
	void newState(int state,const QString &info);
	
	/**
	  * Borra cualquier dato guardado de una previa imagen abierta, incluyendo archivos
	  * temporales creados. Lee la imagen, crea los ficheros temporales.
	  */
	void open();
	    
	/**
	  * Se ocultan los items del visor de la pagina actual. Se cambia de pagina en el visor.
	  * Se muestran los items de la pagina nueva.
	  * @param page Página a la cual se quiere cambiar.
	  */    
	void pageChanged(int page);
	
	/**
	  * Funcion que se encarga de liberar los datos del arbol, visor de imagen y editor
	  * de texto. Crear la ventana de progreso, que muestra por donde se va en el OCR.
	  * Enviar al thread de OCR las distintas paginas, y por último decile que empiece.
	  */
	void read();    
	
	/**
	  * Hace visible todos los items de las página en el visor
	  */
	void showAllPageItems();
	
	/**
	  * Funcion que se encarga de recibir la señal de termino del thread que hace OCR.
	  * Borra la ventana de progreso. Configura los dockets que se ven. Oculta las
	  * palabras que se hicieron visibles durante el OCR.
	  */
	void threadIsFinished();
	
	/**
	  * Cuando se clica sobre un item del arbol, si no pertenece a la pagina
	  * actual automaticamente se realiza un cambio de pagina a la suya. El
	  * QGraphicsRectItem asociado se señala en el visor durante un tiempo
	  * determinado por timer.
	  * @param item Item del arbol que ha sido clicado con el ratón.
	  * @param column Columna del item clicado.
	  */
	void treeItemClicked(QTreeWidgetItem * item,int column);
	
	/**
	  * Cuando un item del arbol se desmarca, el rectItem asociado se oculta.
	  * Si se marca, el rectItem se muestra solo si se encuentra en la pagina
	  * actual.
	  * @param item Objeto del arbol que ha sido marcado o desmarcado.
	  */
	void treeItemChanged(QTreeWidgetItem * item);
	
	
    
    protected:
	/**
	  * Funcion que se ejecuta cuando se cierra el programa. Esta
	  * implementado que se guarde la posicion y tamaño de la ventana
	  * @param e Evento de cierre que contiene informacion.
	  */
	virtual void closeEvent(QCloseEvent *e);
	
	/**
	  * Esta funcion maneja eventos de tiempo. Concretamente se encarga de
	  * mirar el tiempo cuando se clica un item del arbol y la visualizacion
	  * del item asociado en el visor.
	  * @param e Evento de QTimer que contiene informacion.
	  */
	void timerEvent(QTimerEvent *event);
    private:
	/**
	  * Funcion que devuelve el item del visor asociado al elemento del árbol
	  * @param treeItem Item del arbol
	  * @return el QGraphicsItem asociado al item del árbol
	  */
	QGraphicsRectItem *getRect(QTreeWidgetItem* treeItem);
	
	/**
	  * Oculta los items del visor de la pagina pasada como argumento
	  * @param page Pagina donde se quiere ocultar los QGraphicsItems
	  */
	void hidePageItems(int page);
	
	/**
	  * Esta funcion se encarga de leer la imagen. Llama a una funcion de forma asincrona
	  * que procesa la imagen para que al final se tenga imagenes en escala de grises.
	  */
	int readImage(const QString &fileName);
	
	/**
	  * Borra del disco duro los archivos temporales creados.
	  */
	bool removeTempFile();
	
	/**
	  * Muestra los items del visor de la pagina pasada como argumento
	  * @param page Pagina donde se quiere mostrar los QGraphicsItems
	  */
	void showPageItems(int page);
	
	/**
	  * Esta funcion busca los lenguajes de tesseract disponibles y los añade a la caja
	  * de lenguajes lanCombo.
	  */
	void updateLangs();
	
	/*** Objetos en el uso de clicar un item del arbol ***/
	    QBasicTimer timer;    
	    QGraphicsRectItem *clickedItem;
	    bool clickedItemIsVisible;
	    int step;			// Momento del timer
	
	bool enhanceIsEnabled;
	
	/// Para excluir objetos en el cambio de pagina
	bool readDone;
	
	int pagesNumber;    
	int currentPage;    
	QByteArray fileName;
	
	/// Correspondencia entre lenguaje de entrada tesseract con la palabra de idioma
	/// que aparece en la caja de idiomas
	QHash<QString,QString> hashlangtessdata; 
	
	/// Tamaño de las imagenes que nos devuelve el thread que procesa la imagen
	QList<QString> tamanyos;
	
	/// Guarda las palabras que se van mostrando durante el OCR para luego ocultarlas.
	QList<QGraphicsRectItem *> wordsRect; //to hide words rect
	
	QProgressDialog *progress;		// Ventana de progreso para proceso OCR
	QStringList mytesseractlangs;  
	
	QVector<QGraphicsRectItem *> pagesRect;
	
	MyQGraphicsView *mainView;
	MyQTextEdit *textEdit;
	MyQTreeWidget *treeWidget;
	ReaderThread thread;		// Thread que realiza el OCR
	
	QString lastOpenDirectory;
	/*****************************/
	/*** OBJETOS DE LA INTERFAZ **/
	/*****************************/
	    void createActions();
	    
	    /** Crea loss DockWidgets. Estos son los paneles que contienen al árbol, al
	      * visor de imagenes y al editor de texto en la ventana principal.
	      */    
	    void createDockWidgets();
	    
	    void createMenus();
	    void createToolbars();
		
	    /** Guarda el estado de la aplicacion posicion, tamaño de ventana e idioma
	      * escogido para OCR.
	      */
	    void readSettings();
	    
	    /**
	      * Esta funcion devuelve la aplicacion a un estado anterior.
	      */
	    void writeSettings();
	    
	    QAction *aboutAct;
	    QAction *aboutQtAct;
	    QAction *exitAct;
	    QAction *hideAllPagesItems;
	    QAction *openAct;
	    QAction *printAct;    
	    QAction *enhancementAct;
	    QAction *readAct;			// Accion que dispara el OCR
	    QAction *showAllPagesItems;
	    QAction *zoomInAct;
	    QAction *zoomOutAct;
	    QAction *fitToWindowAct;
	    QAction *normalSizeAct;
	
	    QComboBox *langCombo;
	
	    QDockWidget *mainViewDock;
	    QDockWidget *textEditDock;
	    QDockWidget *treeWidgetDock;
	
	    QMenu *fileMenu;
	    QMenu *viewMenu;
	    QMenu *helpMenu;
	
	    QSpinBox *spinBox;
	    
	    QToolBar *basicToolBar;
	    QToolBar *enhancementToolBar;
	    QToolBar *infoToolBar;
	    QToolBar *zoomToolBar;
	
    };
}
#endif
