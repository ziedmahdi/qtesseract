/**********************************************************************
 * File:        myqmainwindow.cpp 
 * Description: The MyQMainWindow class provides the main application 
 * window.
 * Author:      Roberto Benitez Monje
 * Created:     Mon Jan 04 2010
 **********************************************************************/
#include <QAction>
#include <QApplication>
#include <QCloseEvent>
#include <QDir>
#include <QDockWidget>
#include <QFile>
#include <QFileDialog>
#include <QFutureWatcher>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QSettings>
#include <QtConcurrentMap>

#include "include/myqmainwindow.h"

#include "tiffio.h"	    //TIFF library file

#include "tesseractmain.h"  // Tesseract Library file




#include <QScrollBar>



namespace qtessnamespace
{
    
    MyQMainWindow::MyQMainWindow()
    {        
	this->currentPage = -1; //initizialize to imposible page
	this->enhanceIsEnabled = false;
	this->fileName = 0;
	this->pagesNumber = 0;
	this->readDone = false;
    
	this->mainView = new MyQGraphicsView();
	this->treeWidget = new MyQTreeWidget(this);
	this->textEdit = new MyQTextEdit(this);
    
	this->createActions();
	this->createMenus();
	this->createToolbars();
	this->createDockWidgets();	//depende de createmenus
	this->readSettings();
	this->setWindowTitle(tr("QTesseract"));
	
	connect(treeWidget, SIGNAL(itemChanged(QTreeWidgetItem *, int))
		, this, SLOT(treeItemChanged(QTreeWidgetItem *)));
	connect(treeWidget, SIGNAL(itemClicked(QTreeWidgetItem *, int))
		, this, SLOT(treeItemClicked(QTreeWidgetItem *,int)));
	connect(textEditDock, SIGNAL(visibilityChanged(bool))
		, this->textEdit, SLOT(updateWindow(bool)));
    
	connect(&thread
		, SIGNAL(newItem(QTreeWidgetItem *, int, int, int, int, int, int))
		, this, SLOT(newItem(QTreeWidgetItem *, int, int, int, int, int, int))
		, Qt::QueuedConnection);
	connect(&thread, SIGNAL(newState(int, QString)), this, SLOT(newState(int, QString))
		, Qt::QueuedConnection);
	connect(&thread, SIGNAL(writedDocument(int, QTextDocument*)), textEdit
		, SLOT(insertDocument(int, QTextDocument*)), Qt::QueuedConnection);
	connect(&thread, SIGNAL(finished()), this, SLOT(threadIsFinished())
		, Qt::QueuedConnection);
	
	this->textEdit->updateWindow(false);    
    }
    
    /***************************************************************/
    /**************************  SLOTS  ****************************/
    /***************************************************************/
    
    void MyQMainWindow::about()
    {
	QMessageBox::about(this, tr("QTesseract"),
		tr("<p>The <b>QTesseract</b> is a Qt4 front-end for Tesseract-OCR"
		   "</p><p>Frontline Author: Roberto Benitez Monje "
		   "<roberto_benitez@telefonica.net</p>"));
    }
    
    void MyQMainWindow::enhance()
    {
	if ( this->enhanceIsEnabled ) {
	    enhancementAct->setIcon(QIcon(":/images/LightOff.png"));
	    this->enhanceIsEnabled = false;
	}
	else {
	    enhancementAct->setIcon(QIcon(":/images/LightOn.png"));
	    this->enhanceIsEnabled = true;
	}
    }
    
    void MyQMainWindow::newItem(QTreeWidgetItem *treeItem, int type
				, int pageNumber, int left, int top
				, int right, int bottom)
    {
	QGraphicsRectItem *pageRect = 0;
	QGraphicsRectItem *itemRect;
	pageRect = pagesRect.at(pageNumber);
	if ( pageNumber != this->currentPage ) {
	    pageRect->setVisible(true);
	    pageRect->setVisible(false);
	    if ( this->pagesNumber == 1 )
		this->pageChanged(1);
	    else
		this->spinBox->setValue(pageNumber);
	}
	int scaledLeft = (left - 1) * this->mainView->matrix().m11();
	int scaledTop = (top - 1) * this->mainView->matrix().m22();
	int scaledRight = (right + 1) * this->mainView->matrix().m11();
	int scaledBottom = (bottom + 1) * this->mainView->matrix().m22();
	//itemRect = new QGraphicsRectItem(scaledLeft, scaledTop, scaledRight
	//				 , scaledBottom, pageRect
	//				 , this->mainView->scene());
	itemRect = new QGraphicsRectItem(left, top, right
					 , bottom, pageRect
					 , this->mainView->scene());
	itemRect->setData(0,pageNumber);
	itemRect->setData(1,reinterpret_cast< int>(treeItem));
	itemRect->setToolTip(treeItem->toolTip(0));
	treeItem->setData(0,Qt::UserRole,pageNumber);
	treeItem->setData(1,Qt::UserRole,reinterpret_cast< int>(itemRect));
	switch ( type ) {
	    case LETTERv:
		itemRect->setZValue(3);
		itemRect->setPen(QPen(Qt::gray));
		itemRect->setVisible(false);
		break;
	    case WORDv:
		itemRect->setZValue(2);
		itemRect->setPen(QPen(Qt::green));
		itemRect->setVisible(true);
		this->wordsRect << itemRect;
		this->mainView->centerOn(itemRect);
		break;
	    case ROWv:
		itemRect->setZValue(1);
		itemRect->setPen(QPen(Qt::red));
		itemRect->setVisible(false);
		break;
	    case BLOCKv:
		itemRect->setZValue(0);
		itemRect->setPen(QPen(Qt::blue));
		itemRect->setVisible(false);
		break;
	}
	this->treeWidget->itemsNumber++;
	return;
    }
    
    void MyQMainWindow::newState(int state, const QString &info)
    {
	if ( this->progress ) {
	    this->progress->setValue(state);
	    this->progress->setLabelText(info);
	}
    }
    
    void MyQMainWindow::open()
    {
	QString fileName;
    #if 1
	QFileDialog dialog(this);
	dialog.setFileMode(QFileDialog::ExistingFile);
	fileName = dialog.getOpenFileName(this, tr("Open File")
					  , this->lastOpenDirectory);
	if ( ! fileName.isEmpty() ) {
	    QImage image(fileName);
	    if ( image.isNull() ) {
		QMessageBox::information(this, tr("Image Viewer"),
					 tr("Cannot load %1.").arg(fileName));
		this->fileName = QByteArray();
		return;
	    }
	    this->lastOpenDirectory = QFileInfo(fileName).path();
	}
    #elif 1
	fileName = "/home/rmonje/QTESSERACT/TestImages/phototest.jpg";///home/benitez/QTESSERACT/TestImages/ImagenesA4.tif";///home/benitez/Cristie.jpg"; //
    #else
	fileName = "/home/rmonje/QTESSERACT/TestImages/ImagenesA4-0.jpeg";
    #endif
	QApplication::setOverrideCursor(Qt::WaitCursor);
	this->removeTempFile();    
	this->pagesNumber = readImage(fileName);
	this->mainView->insertPagesInfo(this->tamanyos);
	
	this->textEditDock->hide();
	this->treeWidgetDock->hide();
	this->mainViewDock->show();
	
	this->currentPage = 0;
	this->readDone = false;
	
	this->treeWidget->clear();
	
	this->mainView->reset();
	this->textEdit->newFile(fileName.toLocal8Bit(), this->pagesNumber);
	
	this->mainView->setDisabled(false);
	
	//update reading actions
	if ( langCombo->count() > 0 ) {	
	    readAct->setEnabled(true);
	    this->treeWidget->actionSelectAll->setEnabled(false);
	    this->treeWidget->actionSelectAllD->setEnabled(false);
	    this->readAct->setEnabled(true);
	}
	
	//update image actions
	this->fitToWindowAct->setEnabled(true);
	this->normalSizeAct->setEnabled(true);
	this->zoomInAct->setEnabled(true);
	this->zoomOutAct->setEnabled(true);
	
	//restore cursor
	QApplication::restoreOverrideCursor();
	
	this->spinBox->setRange(1,this->pagesNumber);
	this->spinBox->setEnabled(true);	
	if ( this->spinBox->value() != 1 )
	    this->spinBox->setValue(1);
	else
	    this->mainView->pageChanged(this->fileName, 1);
    }
    
    void MyQMainWindow::pageChanged(int page)
    {
	if ( ( page == this->currentPage ) && ( this->pagesNumber > 1 ) )
	    return;
	
	spinBox->setEnabled(false);	// Deshabilita señales de cambio de pagina	   
	this->hidePageItems(this->currentPage);	
	this->mainView->pageChanged(this->fileName, page);

	if ( this->readDone ) {
	    this->textEdit->pageChanged(page);
	}
	
	this->showPageItems(page);
	this->currentPage = page;
	spinBox->setEnabled(true);
    }
    
    void MyQMainWindow::read()
    {
	int index;
	QByteArray fileName;
	QByteArray language;
	QGraphicsRectItem *pageRect;
	QTreeWidgetItem *pageItem;
    
	//Select language
	index = langCombo->currentIndex();
	language = langCombo->itemData(index).toByteArray();
    
	//Create progress dialog
	this->progress = new QProgressDialog(this);
	this->progress->setWindowModality(Qt::WindowModal);
	this->progress->setLabelText(tr("Reading %1").arg(fileName.constData()));
	if ( !this->enhanceIsEnabled )
	    this->progress->setRange(0, this->pagesNumber*2);
	else
	    this->progress->setRange(0, this->pagesNumber*1024);
	this->progress->setValue(0);
    
    
	//block mechanism to save page position in mainView
	this->mainView->saveAndLock();
    
	//remove previous qgraphicsrectitems
	this->mainView->removeRectItemList();
	
	this->treeWidget->clear();
	this->treeWidget->insertPagesInfo(this->tamanyos);
    
	this->pagesRect.clear();
	this->pagesRect.resize(this->pagesNumber);
	for ( int i = 0 ; i < this->pagesNumber ; i++ ) {    /* read one beyond to catch error */
		fileName = QByteArray(getTempFileName(this->fileName, i));
		pageItem = treeWidget->insertPage(this->fileName.constData(), i+1);
		pageRect = this->mainView->insertPageRectItem(QRect(0, 0, 1, 1));
		pageItem->setData(1,Qt::UserRole,reinterpret_cast< int>(pageRect));
		pageRect->setData(0,i+1);		// Set pageNumber
		pagesRect.insert(i+1, pageRect);
		thread.read(fileName, i + 1, pageItem);
	}
	thread.start(this->enhanceIsEnabled, language, this->textEdit->logicalDpiX());
    }
    
    void MyQMainWindow::threadIsFinished()
    {
	if ( this->progress ) {
	    this->progress->close();
	    delete this->progress;
	    this->progress = 0;
	}
	
	this->printAct->setEnabled(true);
	this->treeWidget->setEnabled(true);
	this->showAllPagesItems->setEnabled(true);
	this->hideAllPagesItems->setEnabled(true);
	
	// This block must be before call restoreAndUnlock becase if you show
	// a Dock then anyelse dock change its scrollbars to adjust on the window
	this->treeWidgetDock->show();
	this->textEditDock->show();	
	
	this->mainView->restoreAndUnlock();
	
	this->readDone = true;
    
	foreach ( QGraphicsRectItem *item, this->wordsRect )
	    item->setVisible(false);
	this->wordsRect.clear();
    
	if ( this->pagesNumber == 1 )
	    this->pageChanged(1);
	else
	    this->spinBox->setValue(1);
    }
    
    void MyQMainWindow::treeItemChanged(QTreeWidgetItem * item)
    {    
	int itemPageNumber = 0;
	
	/// QGraphicsRectItem del visor relacionado con item
	QGraphicsRectItem *rectItem;    
	QGraphicsItem *rectItemPage;
	rectItem = getRect(item);
	if ( rectItem ) {
	    switch( item->checkState(0) ) {
		case Qt::Unchecked:
		    rectItem->setVisible(false);
		    break;
		case Qt::Checked:
		    itemPageNumber = item->data(0,Qt::UserRole).toInt();
		    rectItem->setVisible(true);
		    if ( itemPageNumber != this->currentPage ) {
			rectItemPage = rectItem->parentItem();
			if ( rectItemPage ) {
			    rectItemPage->setVisible(true);
			    rectItemPage->setVisible(false);
			}
		    }
		    break;
		case Qt::PartiallyChecked:
		    break;
	    }
	    //update();
	}
    }
    
    void MyQMainWindow::treeItemClicked(QTreeWidgetItem * item, int)
    {
	if ( !this->readDone )
	    return;
	int type = item->data(2,Qt::UserRole).toInt();
	int page = item->data(0,Qt::UserRole).toInt();
	if ( ( this->pagesNumber > 1 ) && ( page != spinBox->value() ) )
	    spinBox->setValue(page);
	
	if ( type != PAGEv ) {
	    QGraphicsRectItem *rectItem = getRect(item);
	    if ( this->mainViewDock->isVisible() ) {
		QSize sizeRect = rectItem->rect().size().toSize();
		QSize sizeView = this->mainView->rect().size();
		if ( (sizeRect.width() > sizeView.width() )
		    || (sizeRect.height() > sizeView.width() ) )
		    this->mainView->fitInView(rectItem, Qt::KeepAspectRatio);
		else
		    this->mainView->centerOn(rectItem);
	    }
	    if ( timer.isActive() ) {
		this->timer.stop();
		clickedItem->setBrush(QBrush(Qt::transparent));
		clickedItem->setVisible(clickedItemIsVisible);
		clickedItem = 0;
	    }
	    step = 1;
	    this->clickedItem = rectItem;
	    clickedItemIsVisible = clickedItem->isVisible();
	    timer.start(0,this); //signal every 100ms
	    clickedItem->setVisible(true);
	    QColor color = clickedItem->pen().color();
	    color.setAlpha(150);
	    clickedItem->setBrush(QBrush(color));
	}
    }
    
    
    /***************************************************************/
    /*******************  PROTECTED FUNCTIONS  *********************/
    /***************************************************************/
    
    void MyQMainWindow::closeEvent(QCloseEvent *e)
    {
	bool success;
    
	this->removeTempFile();
	writeSettings();
	success = textEdit->close();
	if ( success ) {
	    e->accept();
	}
	else
	    e->ignore();
    }
    
    void MyQMainWindow::timerEvent(QTimerEvent *event)
    {
	if ( event->timerId() == timer.timerId() ) {
	    if ( clickedItem ) {
		QColor color;
		if ( step > 30 ) {
			step = 1; //step must never be 0
			this->timer.stop();
			clickedItem->setBrush(QBrush(Qt::transparent));
			clickedItem->setVisible(clickedItemIsVisible);
			clickedItem = 0;
		}
		else {
		    color = clickedItem->pen().color();
		    color.setAlpha(255 / step);
		    step = step + 10;
		    clickedItem->setBrush(QBrush(color));
		}
	    update();
	    }
	}
	else {
	    QWidget::timerEvent(event);
	}
    }
    
    /***************************************************************/
    /*******************  PRIVATED FUNCTIONS  *********************/
    /***************************************************************/
    
    void MyQMainWindow::createActions()
    {    
	aboutAct = new QAction(QIcon(":/images/Info.png"),tr("&About"), this);
	connect(aboutAct, SIGNAL(triggered()), this, SLOT(about()));
    
	aboutQtAct = new QAction(QIcon(":/images/Qt.png"),tr("About &Qt"), this);
	connect(aboutQtAct, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
	
	exitAct = new QAction(QIcon(":/images/Exit.png"), tr("E&xit"), this);
	exitAct->setShortcut(tr("Ctrl+Q"));
	connect(exitAct, SIGNAL(triggered()), this, SLOT(close()));
	
	openAct = new QAction(QIcon(":/images/Load.png"), tr("&Open..."), this);
	openAct->setShortcut(tr("Ctrl+O"));
	connect(openAct, SIGNAL(triggered()), this, SLOT(open()));
    
	printAct = new QAction(QIcon(":/images/Print.png"), tr("&Print..."), this);
	printAct->setShortcut(tr("Ctrl+P"));
	printAct->setEnabled(false);
	connect(printAct, SIGNAL(triggered()), this->textEdit, SLOT(filePrint()));
	
	/// Acciones de árbol
	showAllPagesItems = new QAction(QIcon(":/images/SelectAll.png")
						 ,tr("Select All Tree Items")
						 , this);
	showAllPagesItems->setEnabled(false);
	connect(showAllPagesItems, SIGNAL(triggered()), this
		, SLOT(showAllPageItems()));
	
	hideAllPagesItems = new QAction(QIcon(":/images/Clear.png")
						 ,tr("Deselect All Tree Items")
						 , this);
	hideAllPagesItems->setEnabled(false);
	connect(hideAllPagesItems, SIGNAL(triggered()), this
		, SLOT(hideAllPageItems()));
    
	/// Accion de hacer OCR
	readAct = new QAction(QIcon(":/images/Read.png"), tr("&Read..."), this);
	readAct->setShortcut(tr("Ctrl+R"));
	readAct->setEnabled(false);
	connect(readAct, SIGNAL(triggered()), this, SLOT(read()));
	
	/// Accion de aplicar mejoras a hacer OCR
	enhancementAct = new QAction(QIcon(":/images/LightOff.png"),tr("Enhancement"), this);
	enhancementAct->setCheckable(true);
	connect(enhancementAct, SIGNAL(triggered()), this, SLOT(enhance()));
	
	/* Acciones del visor */
	
	zoomInAct = new QAction(QIcon(":/images/Zoom-in.png"),tr("Zoom &In (25%)"), this);
	zoomInAct->setShortcut(tr("Ctrl++"));
	zoomInAct->setEnabled(false);
	connect(zoomInAct, SIGNAL(triggered()), mainView, SLOT(zoomIn()));
    
	zoomOutAct = new QAction(QIcon(":/images/Zoom-out.png"),tr("Zoom &Out (25%)"), this);
	zoomOutAct->setShortcut(tr("Ctrl+-"));
	zoomOutAct->setEnabled(false);
	connect(zoomOutAct, SIGNAL(triggered()), this->mainView, SLOT(zoomOut()));
    
	normalSizeAct = new QAction(QIcon(":/images/Zoom-original.png"),tr("&Normal Size"), this);
	normalSizeAct->setShortcut(tr("Ctrl+S"));
	normalSizeAct->setEnabled(false);
	connect(normalSizeAct, SIGNAL(triggered()), this->mainView, SLOT(normalSize()));
    
	fitToWindowAct = new QAction(QIcon(":/images/Zoom-best-fit.png"),tr("&Fit to Window"), this);
	fitToWindowAct->setEnabled(false);
	fitToWindowAct->setShortcut(tr("Ctrl+F"));
	connect(fitToWindowAct, SIGNAL(triggered()), this->mainView, SLOT(fitToWindow()));
    }
    
    void MyQMainWindow::createDockWidgets()
    {
	treeWidgetDock = new QDockWidget(tr("Word's Tree"), this);
	treeWidgetDock->setAllowedAreas(Qt::AllDockWidgetAreas);
	treeWidgetDock->setWidget(this->treeWidget);
	addDockWidget(Qt::BottomDockWidgetArea, treeWidgetDock);
	/// Añade en un menu la posibildad de ocultar este Dock
	viewMenu->addAction(treeWidgetDock->toggleViewAction());
	treeWidgetDock->hide();
    
	mainViewDock = new QDockWidget(tr("Image View"), this);
	mainViewDock->setAllowedAreas(Qt::AllDockWidgetAreas);
	mainViewDock->setWidget(this->mainView);
	addDockWidget(Qt::LeftDockWidgetArea, mainViewDock);
	viewMenu->addAction(mainViewDock->toggleViewAction());
	mainViewDock->hide();
	mainViewDock->setWindowIcon(QIcon(":/images/Picture.png"));
    
	textEditDock= new QDockWidget(tr("Text Editor"), this);
	textEditDock->setAllowedAreas(Qt::AllDockWidgetAreas);
	textEditDock->setWidget(this->textEdit);
	addDockWidget(Qt::RightDockWidgetArea, textEditDock);
	viewMenu->addAction(textEditDock->toggleViewAction());
	textEditDock->hide();
	textEditDock->setWindowIcon(QIcon(":/images/Text.png"));
    }
    
    void MyQMainWindow::createMenus()
    {
	fileMenu = new QMenu(tr("&File"), this);
	fileMenu->addAction(openAct);
	fileMenu->addAction(textEdit->actionSaveAs);
	fileMenu->addAction(printAct);
	fileMenu->addSeparator();
	fileMenu->addAction(exitAct);
    
	viewMenu = new QMenu(tr("&View"), this);
	viewMenu->addAction(zoomInAct);
	viewMenu->addAction(zoomOutAct);
	viewMenu->addAction(normalSizeAct);
	viewMenu->addSeparator();
	viewMenu->addAction(fitToWindowAct);
    
	helpMenu = new QMenu(tr("&Help"), this);
	helpMenu->addAction(aboutAct);
	helpMenu->addAction(aboutQtAct);
    
	menuBar()->addMenu(fileMenu);
	menuBar()->addMenu(this->textEdit->menuEdit);
	menuBar()->addMenu(this->textEdit->menuFormat);
	menuBar()->addMenu(viewMenu);
	menuBar()->addMenu(helpMenu);
    }
    
    void MyQMainWindow::createToolbars()
    {
	QString buttonStyle("QToolButton {background-color: "
	     "qradialgradient(spread:pad, cx:0.5, cy:0.5, radius:0.9, fx:0.5, fy:0.5,"
	     "stop:0 rgba(255, 255, 255, 255), stop:0.3 rgba(255, 255, 255, 255), "
	     "stop:0.5 rgba(255, 255, 255, 255), stop:0.6 rgba(255, 255, 255, 30), "
	     "stop:1 rgba(255, 255, 255, 0))} QToolButton:checked {background-color: black;}"
	     "QToolButton:pressed {background-color: white;border: none;}");
	
	basicToolBar = addToolBar(tr("Basic"));
	basicToolBar->setStyleSheet(buttonStyle);
	basicToolBar->addAction(openAct);
	basicToolBar->addAction(textEdit->actionSaveAs);
	basicToolBar->addAction(printAct);
	basicToolBar->addSeparator();
	langCombo = new QComboBox;
	langCombo->setMinimumContentsLength(10);
	this->updateLangs();
	basicToolBar->addWidget(langCombo);
	basicToolBar->addAction(readAct);
	basicToolBar->addAction(showAllPagesItems);
	basicToolBar->addAction(hideAllPagesItems);
	enhancementToolBar = addToolBar(tr("Enhancement"));
	enhancementToolBar->addAction(enhancementAct);
	enhancementToolBar->setStyleSheet("QToolButton {background-color: "
	     "qradialgradient(spread:pad, cx:0.5, cy:0.5, radius:0.9, fx:0.5, fy:0.5,"
	     "stop:0 rgba(255, 255, 255, 255), stop:0.3 rgba(255, 255, 255, 255), "
	     "stop:0.5 rgba(255, 255, 255, 255), stop:0.6 rgba(255, 255, 255, 30), "
	     "stop:1 rgba(255, 255, 255, 0));} "
	     "QToolButton:checked { background-color: "
	     "qradialgradient(spread:pad, cx:0.5, cy:0.5, radius:0.9, fx:0.5, fy:0.45,"
	     "stop:0 yellow, stop:0.48 lightyellow, "
	     "stop:0.5 rgba(255, 255, 255, 255), stop:0.6 rgba(255, 255, 255, 30), "
	     "stop:1 rgba(255, 255, 255, 0)); border: none;width: 31px; height: 29px;}"
	     "QToolButton:pressed {border-image:none;background-color: white;}"
	     "QToolBar {background-color: transparent;}");
	enhancementToolBar->setToolButtonStyle(Qt::ToolButtonIconOnly);
	zoomToolBar = addToolBar(tr("Zoom"));
	zoomToolBar->setStyleSheet(buttonStyle);
    
	spinBox = new QSpinBox(zoomToolBar);
	spinBox->setPrefix(tr("Page "));
	spinBox->setEnabled(false);
	connect(spinBox, SIGNAL(valueChanged(int)), this, SLOT(pageChanged(int)));
	zoomToolBar->addWidget(spinBox);
	zoomToolBar->addSeparator();
	zoomToolBar->addAction(zoomInAct);
	zoomToolBar->addAction(zoomOutAct);
	zoomToolBar->addAction(fitToWindowAct);
	zoomToolBar->addAction(normalSizeAct);
	zoomToolBar->setToolButtonStyle(Qt::ToolButtonIconOnly);
    
	textEdit->barEdit->setStyleSheet(buttonStyle);
	this->addToolBar(textEdit->barEdit);
	textEdit->barFormat->setStyleSheet(buttonStyle);
    
	infoToolBar = addToolBar(tr("Info"));
	infoToolBar->setStyleSheet(buttonStyle);
	infoToolBar->addAction(aboutAct);
	infoToolBar->addAction(aboutQtAct);
	infoToolBar->addAction(exitAct);
    
	this->addToolBarBreak();
    
	this->addToolBar(textEdit->barFormat);
	textEdit->barText->setStyleSheet(buttonStyle);
	this->addToolBar(textEdit->barText);
    }
    
    QGraphicsRectItem *MyQMainWindow::getRect(QTreeWidgetItem* treeItem)
    {
	QVariant variant = treeItem->data(1, Qt::UserRole);
	return (reinterpret_cast< QGraphicsRectItem * >(variant.toInt()));
    }
    
    void MyQMainWindow::hidePageItems(int page)
    {
	QTreeWidgetItem *item;
	QGraphicsRectItem *rect;
	item = treeWidget->getPage(page);
	if ( item ) {
	    rect = this->getRect(item);
	    if ( rect ) {
		rect->setVisible(true);
		rect->setVisible(false);
	    }
	}
    }
    
    int MyQMainWindow::readImage(const QString &fileName)
    {
	FILE *fp;
	QString filename;
	spinBox->setEnabled(false);
	this->fileName = fileName.toUtf8();
	this->pagesNumber = 0;
    
	fp = fopen(fileName.toLocal8Bit().data(), "r");
	if ( fp == NULL )
	    return -1;
	if ( fileFormatIsTiff(fp) )
	    tiffGetCount(fp, &this->pagesNumber);
	else
	    this->pagesNumber = 1;
	fclose(fp);
	tamanyos.clear();
    
	QList<QString> list;
	for ( int i = 0 ; i < this->pagesNumber ; ++i )
	    list.insert(i,QString("%1,%2").arg(fileName).arg(i));
	// Create a progress dialog.
	QProgressDialog dialog;
	dialog.setLabelText(QString("Progressing using %1 thread(s)...").arg(QThread::idealThreadCount()));
    
	// Create a QFutureWatcher and conncect signals and slots.
	QFutureWatcher<QString> futureWatcher;
	QObject::connect(&futureWatcher, SIGNAL(finished()), &dialog, SLOT(reset()));
	QObject::connect(&dialog, SIGNAL(canceled()), &futureWatcher, SLOT(cancel()));
	QObject::connect(&futureWatcher, SIGNAL(progressRangeChanged(int, int)), &dialog, SLOT(setRange(int, int)));
	QObject::connect(&futureWatcher, SIGNAL(progressValueChanged(int)), &dialog, SLOT(setValue(int)));
    
	// Start the computation.
	futureWatcher.setFuture(QtConcurrent::mapped(list, myPixRead));
    
	// Display the dialog and start the event loop.
	dialog.exec();
    
	futureWatcher.waitForFinished();
	QString resultado;
	for ( int i = 0 ; i < this->pagesNumber ; ++i ) {
	    resultado = futureWatcher.resultAt(i);
	    tamanyos.insert(i, resultado);
	}
    
	return this->pagesNumber;
    }
    
    
    void MyQMainWindow::readSettings()
    {
	QSettings settings("QTesseract", "Tesseract Viewer");
	QPoint pos = settings.value("pos", QPoint(200, 200)).toPoint();
	QSize size = settings.value("size", QSize(500, 400)).toSize();
	int indexLangCombo = settings.value("lang", 1).toInt();
	this->lastOpenDirectory =  settings.value("path", QDir::homePath()).toString();
	move(pos);
	resize(size);
	langCombo->setCurrentIndex(indexLangCombo);	 
	if ( ! QDir(this->lastOpenDirectory).exists() )
	    this->lastOpenDirectory = QDir::homePath();
    }
    
    bool MyQMainWindow::removeTempFile()
    {
	bool successdelete = true;
	if ( this->pagesNumber ) //assert at least open once
	  for( int i=0 ; i<this->pagesNumber ; ++i ) {
	      QByteArray tempName = getTempFileName(this->fileName, i);
	      bool booltemp = QFile::remove(QString(tempName));
	      if ( successdelete )
		  successdelete = booltemp;
	  }
	return successdelete;
    }
    
    void MyQMainWindow::hideAllPageItems()
    {	
	this->treeWidget->selectAll(false);
    }
    
    
    void MyQMainWindow::showAllPageItems()
    {
	this->treeWidget->selectAll(true);
    }
    
    void MyQMainWindow::showPageItems(int page)
    {
	QTreeWidgetItem *item;
	QGraphicsRectItem *rect;
	item = treeWidget->getPage(page);
	if ( item ) {
	    rect = this->getRect(item);
	    if ( rect )
		rect->setVisible(true);
	}
    }
    
    void MyQMainWindow::updateLangs()
    {
	QString aux;
	QStringList languages_ext;
	int index;
    
	hashlangtessdata.clear();
	hashlangtessdata["deu"] = tr("German");
	hashlangtessdata["eng"] = tr("English");
	hashlangtessdata["fra"] = tr("French");
	hashlangtessdata["ita"] = tr("Italian");
	hashlangtessdata["nld"] = tr("Dutch");
	hashlangtessdata["por"] = tr("Portuguese");
	hashlangtessdata["spa"] = tr("Spanish");
	hashlangtessdata["vie"] = tr("Vietnamese");
	QDir dir = QDir("/usr/local/share/tessdata","*.traineddata");
	QStringList languages_dir;
	languages_dir = dir.entryList();
	languages_dir.replaceInStrings(".traineddata", "");
	QStringListIterator javaStyleIterator(languages_dir);
	langCombo->clear();
	while ( javaStyleIterator.hasNext() ) {
	    aux = javaStyleIterator.next();
	    if ( hashlangtessdata.contains(aux) ) {
		   char* auxlang = strdup(aux.toLocal8Bit().data());
		   //toLocal8Bit para poder pasar de variable de QT a estandar char*
		   mytesseractlangs << auxlang;
		   langCombo->addItem(hashlangtessdata[auxlang],aux);
	       }
	 }
	if ( (index = langCombo->findText(tr("English"))) != -1 )
	    langCombo->setCurrentIndex(index);
	else
	    if ( langCombo->count() > 0 ) {
		langCombo->setCurrentIndex(1);
	    }
	    else {
		enhancementAct->setIcon(QIcon(":/images/LightRed.png"));
		enhancementAct->setEnabled(false);
		QMessageBox::critical(this, tr("Critical Error")
				      , tr("Language's files not found."));
	    }
    }
    
    void MyQMainWindow::writeSettings()
    {
	 QSettings settings("QTesseract", "Tesseract Viewer");
	 settings.setValue("pos", pos());
	 settings.setValue("size", size());
	 settings.setValue("lang",langCombo->currentIndex());
	 settings.setValue("path", this->lastOpenDirectory);
    }
}