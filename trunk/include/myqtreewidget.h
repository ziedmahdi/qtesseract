/**********************************************************************
 * File:        myqtreewidget.h 
 * Description: The MyQTreeWidget class provides a tree view to view
 * Tesseract result.
 * Author:      Roberto Benitez Monje
 * Created:     Mon Jan 04 2010
 **********************************************************************/
#ifndef MYQTREEWIDGET_H
#define MYQTREEWIDGET_H

#include <QTreeWidget>
#include "rect.h"
#include "tesstext.h"

namespace qtessnamespace
{

    /**
      La clase del arbol donde se muestra el resultado de tesseract
      * @author Roberto Benitez Monje
    */
    class MyQTreeWidget : public QTreeWidget
    {
      Q_OBJECT
	
    public:
	/**
	  * Constructor de MyQTreeWidget
	  * @param parent El QWidget padre
	  */
	MyQTreeWidget(QWidget *parent = 0);
    
	/**
	  * Devuelve el QTreeWidgetItem pagina del item
	  * @param item Es el item descendiente
	  */
	QTreeWidgetItem *getPage(QTreeWidgetItem *item);
	
	/**
	  * Devuelve el QTreeWidgetItem de la pagina number
	  * @param number Es el numero de pagina
	  */
	QTreeWidgetItem *getPage(int number);
    
	/**
	  * Inserta un item pagina en el arbol
	  * @param fileName Nombre del fichero al que corresponde
	  * la página
	  * @param page Numero de pagina al que corresponde la
	  * página
	  */
	QTreeWidgetItem * insertPage(const char*fileName, int page);
	
	/**
	  * Inserta los campos de informacion a los items página.
	  * @param v es la lista de cadenas, cada una es de un campo.
	  */
	void insertPagesInfo(const QList<QString> &v);
    
	bool itemIsPage(QTreeWidgetItem *item);
	bool itemIsWord(QTreeWidgetItem *item);
	bool itemIsRow(QTreeWidgetItem *item);
	bool itemIsBlock(QTreeWidgetItem *item);
	bool itemIsLetter(QTreeWidgetItem *item);
	
	/**
	  * Devuelve el numero que le corresponde a un item dentro
	  * del documento.
	  */
	int getCount(QTreeWidgetItem *item);
	
	QAction *actionSelectAll;
	QAction *actionAllLetters;
	QAction *actionAllWords;
	QAction *actionAllRows;
	QAction *actionAllBlocks;
	QAction *actionAllPage;
	QAction *actionSelectAllD;
	QAction *actionAllLettersD;
	QAction *actionAllWordsD;
	QAction *actionAllRowsD;
	QAction *actionAllBlocksD;
	QAction *actionAllPageD;
	
	int itemsNumber;
    public slots:
	void deselectAll();
	
	/** Deselecciona todos los items letra descendientes al que
	  * se ha seleccionado con el ratón al hacer clic con él.
	  */    
	void deselectAllLettersSinceItem();
	void deselectAllWordsSinceItem();
	void deselectAllRowsSinceItem();
	void deselectAllBlocksSinceItem();
	void deselectAllSincePageItem();
	
	/**
	  Marca o desmarca todos los items del arbol
	  * @param check True, se marca los items. False, se desmarcan
	  */
	void selectAll(bool check);
	
	void selectAll();
	void selectAllSincePageItem();
	void selectAllLettersSinceItem();
	void selectAllWordsSinceItem();
	void selectAllRowsSinceItem();
	void selectAllBlocksSinceItem();
    private:
	void createActions();
	void createBrushes();
	void createHeader();
	
	/** Marca o desmarca todos los items de la pagina del item clicado
	  * con el raton
	  * @param check True, se marca los items. False, se desmarcan
	  */
	void selectAllSincePageItem(bool check);
	
	/**
	  * Devuelve en una lista todos los items de un tipo introducido como argumento
	  * de la pagina del item seleccionado. .
	  * @param list Para que la funcion sea recurrente
	  * @param level Cuando vale 0 se cogen los items del nivel actual. 4 para pagina
	  * , 3 para parrafo, 2 para linea, 1 para palabra, 0 para letras
	  */
	QList<QTreeWidgetItem *> selectItemsByLevel(QList<QTreeWidgetItem *> list, int level);
	
	/** Marca o desmarca los descendientes de item de un tipo
	  * @param item Item padre del cual se quiere buscar en sus hijos
	  * @param type Tipo de componente (pagina, parrafo, linea, palabra o letra)
	  * @param check True, se marca los items. False, se desmarcan
	  */
	void selectAllItemsSinceItemByType(QTreeWidgetItem *item,int type,bool check);
	
	QBrush page0Brush,page1Brush,page2Brush;
	
    };
}
#endif // MYQTREEWIDGET_H
