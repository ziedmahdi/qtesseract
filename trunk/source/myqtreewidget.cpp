/**********************************************************************
 * File:        myqtreewidget.cpp 
 * Description: The MyQTreeWidget class provides a tree view to view
 * Tesseract result.
 * Author:      Roberto Benitez Monje
 * Created:     Mon Jan 04 2010
 **********************************************************************/
#include <QtGui>
#include <QPair> //to QGradientStops
#include "include/myqtreewidget.h"

namespace qtessnamespace {
	
    MyQTreeWidget::MyQTreeWidget(QWidget *parent):QTreeWidget(parent)
    {
	this->setAutoFillBackground(false);
	//this->setSelectionMode(QAbstractItemView::ExtendedSelection); //multiple selection on tree
	this->setStyleSheet(
	 "QTreeView::item:selected:active{background: qlineargradient("
	 "x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #6ea1f1, stop: 1 #567dbc);height: 10px;}"
	 "QTreeView::item:selected:!active {background: qlineargradient("
	 "x1: 0, y1: 0, x2: 0, y2: 1, stop: 0 #6b9be8, stop: 1 #577fbf); }"
	 "QTreeView::branch {background: palette(base);}"
    
	 "QTreeView::branch:has-siblings:!adjoins-item {border-image: "
	 "url(:/images/vline.png)  0; }"
    
	 "QTreeView::branch:has-siblings:adjoins-item {border-image: "
	 "url(:/images/branch-more.png) 0;}"
    
	 "QTreeView::branch:!has-children:!has-siblings:adjoins-item { border-image: "
	 "url(:/images/branch-end.png) 0;}"
	 "QTreeView::branch:has-children:!has-siblings:closed,"
	 "QTreeView::branch:closed:has-children:has-siblings {border-image: none;"
	 "image: url(:/images/branch-closed.png);}"
	 "QTreeView::branch:open:has-children:!has-siblings,"
	 "QTreeView::branch:open:has-children:has-siblings  {border-image: none;"
		 "image: url(:/images/branch-open.png);}");
	this->setSelectionBehavior(QAbstractItemView::SelectRows);
    
	this->setContextMenuPolicy(Qt::ActionsContextMenu); //actions over a menu
	this->createActions();
	this->createBrushes();
	this->createHeader();
    
	//this->setBackgroundRole(QPalette::Base);
	//this->setAlternatingRowColors(true);
	this->setMouseTracking(true); //mouse interaction without pressed button
	//this->setRootIsDecorated(false);
	this->itemsNumber;
    }
    
    void MyQTreeWidget::createActions()
    {
	QAction *a;
    
	a = this->actionSelectAll = new QAction(tr("Select All"), this);
	connect(a, SIGNAL(triggered()), this, SLOT(selectAll()));
    
	a = this->actionAllLetters = new QAction(tr("Select all Letters"), this);
	a->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_L));
	this->addAction(a);
	connect(a, SIGNAL(triggered()), this, SLOT(selectAllLettersSinceItem()));
    
	a = this->actionAllWords = new QAction(tr("Select all Words"), this);
	a->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_W));
	this->addAction(a);
	connect(a, SIGNAL(triggered()), this, SLOT(selectAllWordsSinceItem()));
    
	a = this->actionAllRows = new QAction(tr("Select all Rows"), this);
	a->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_R));
	this->addAction(a);
	connect(a, SIGNAL(triggered()), this, SLOT(selectAllRowsSinceItem()));
    
	a = this->actionAllBlocks = new QAction(tr("Select all Blocks"), this);
	a->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_B));
	this->addAction(a);
	connect(a, SIGNAL(triggered()), this, SLOT(selectAllBlocksSinceItem()));
    
	a = this->actionAllPage = new QAction(tr("Select all Page"), this);
	a->setShortcut(QKeySequence::SelectAll);
	this->addAction(a);
	connect(a, SIGNAL(triggered()), this, SLOT(selectAllSincePageItem()));
    
    
	a = this->actionSelectAllD = new QAction(tr("Deselect All"), this);
	connect(a, SIGNAL(triggered()), this, SLOT(deselectAll()));
    
	a = this->actionAllLettersD = new QAction(tr("Deselect all Letters"), this);
	a->setShortcut(QKeySequence(Qt::CTRL + Qt::ALT + Qt::Key_L));
	this->addAction(a);
	connect(a, SIGNAL(triggered()), this, SLOT(deselectAllLettersSinceItem()));
    
	a = this->actionAllWordsD = new QAction(tr("Deselect all Words"), this);
	a->setShortcut(QKeySequence(Qt::CTRL + Qt::ALT + Qt::Key_W));
	this->addAction(a);
	connect(a, SIGNAL(triggered()), this, SLOT(deselectAllWordsSinceItem()));
    
	a = this->actionAllRowsD = new QAction(tr("Deselect all Rows"), this);
	a->setShortcut(QKeySequence(Qt::CTRL + Qt::ALT + Qt::Key_R));
	this->addAction(a);
	connect(a, SIGNAL(triggered()), this, SLOT(deselectAllRowsSinceItem()));
    
	a = this->actionAllBlocksD = new QAction(tr("Deselect all Blocks"), this);
	a->setShortcut(QKeySequence(Qt::CTRL + Qt::ALT + Qt::Key_B));
	this->addAction(a);
	connect(a, SIGNAL(triggered()), this, SLOT(deselectAllBlocksSinceItem()));
    
	a = this->actionAllPageD = new QAction(tr("Deselect all Page"), this);
	a->setShortcut(QKeySequence(Qt::CTRL + Qt::ALT + Qt::Key_A));
	this->addAction(a);
	connect(a, SIGNAL(triggered()), this, SLOT(deselectAllSincePageItem()));
    }
    
    void MyQTreeWidget::createBrushes()
    {
	QGradientStops stops;
	this->page0Brush = QBrush(QColor(238,162,173),Qt::SolidPattern);
    
	QLinearGradient *gradient = new QLinearGradient(0,0,100,100);
	stops << QGradientStop(0, QColor(238,162,173));
	stops << QGradientStop(1, QColor(255,182,193));
	gradient->setStops(stops);
	this->page1Brush = QBrush(*gradient);
    
	gradient = new QLinearGradient(0,0,100,100);
	stops.clear();
	stops << QGradientStop(0, QColor(255,182,193));
	stops << QGradientStop(1, QColor(255,240,245));
	gradient->setStops(stops);
	this->page2Brush = QBrush(*gradient);
    }
    
    void MyQTreeWidget::createHeader() {
	QStringList headers;
	headers << tr("Type") << tr("Info") << tr("Bounding box");
	this->setHeaderLabels(headers);
	
	QHeaderView *header = this->header();
	//header->setStretchLastSection(false);
	header->setResizeMode(0, QHeaderView::ResizeToContents);
	header->setResizeMode(1, QHeaderView::ResizeToContents);
	header->setResizeMode(2, QHeaderView::ResizeToContents);
	header->setDefaultAlignment(Qt::AlignCenter);
    }
    
    bool MyQTreeWidget::itemIsPage(QTreeWidgetItem *item)
    {
	int type = item->data(2,Qt::UserRole).toInt();
	return (type == PAGEv );
    }
    
    bool MyQTreeWidget::itemIsWord(QTreeWidgetItem *item)
    {
	int type = item->data(2,Qt::UserRole).toInt();
	return (type == WORDv );
    }
    
    bool MyQTreeWidget::itemIsRow(QTreeWidgetItem *item)
    {
	int type = item->data(2,Qt::UserRole).toInt();
	return (type == ROWv );
    }
    
    bool MyQTreeWidget::itemIsBlock(QTreeWidgetItem *item)
    {
	int type = item->data(2,Qt::UserRole).toInt();
	return (type == BLOCKv );
    }
    
    bool MyQTreeWidget::itemIsLetter(QTreeWidgetItem *item)
    {
	int type = item->data(2,Qt::UserRole).toInt();
	return (type == LETTERv );
    }
    
    QTreeWidgetItem * MyQTreeWidget::insertPage(const char*fileName, int page)
    {
	QStringList list;
	QTreeWidgetItem *item = new QTreeWidgetItem(this);
	item->setExpanded(true);
	item->setBackground(0,this->page0Brush);
	item->setBackground(1,this->page1Brush);
	item->setBackground(2,this->page2Brush);
	item->setText(0, tr("page %1").arg(page));
	item->setText(1,QString(fileName));
	item->setData(0,Qt::UserRole,page);
	item->setData(2,Qt::UserRole,PAGEv);
	return item;
    }
    
    void MyQTreeWidget::insertPagesInfo(const QList<QString> &v){
	int i;
	QStringList list;
	QTreeWidgetItem *pageItem;
	for (i=0;i<v.count();++i) {
	    list = v.at(i).split(',');
	    if ( list.count() == 5 ) {
		pageItem = this->getPage(list.at(0).toInt(0,10));
		if ( pageItem ) {
		    pageItem->setText(2, QString("size(%1,%2) res(%1,%2)").arg(list.at(1))
			  .arg(list.at(2)).arg(list.at(3)).arg(list.at(4)));
		}
	    }
    
	}
	this->itemsNumber = 0;
    }
    int MyQTreeWidget::getCount(QTreeWidgetItem *item)
    {
	bool error;
	int count;
	QString head = item->text(0);
	head = head.right(head.length() - head.indexOf(" "));
	count = head.toInt(&error); //base 10 by default
	if ( !error )
	    qDebug() << "MyQTreeWidget,getCount: error al pasar qstring a int";
	return count;
    }
    
    
    QTreeWidgetItem *MyQTreeWidget::getPage(int number)
    {
	int i;
	QTreeWidgetItem *root,*out;
    
	root = this->invisibleRootItem();
	out = 0;
	i = 0;
	while (i<root->childCount() && !out) {
	    if ( this->getCount(root->child(i) ) == number ) {
		out = root->child(i);
	    }
	    i++;
	}
	return out;
    }
    QTreeWidgetItem * MyQTreeWidget::getPage(QTreeWidgetItem *item)
    {
	    QTreeWidgetItem *p = item;
	    QTreeWidgetItem *out = 0;
	    while(p && !out) {
		if ( itemIsPage(p) )
		    out = p;
		p = p->parent();
	    }
	    return out;
    }
    
    QList<QTreeWidgetItem *> MyQTreeWidget::selectItemsByLevel(QList<QTreeWidgetItem *> list, int level)
    {
	    if ( list.isEmpty() || level <= 0 )
		return list;
	    QList<QTreeWidgetItem *> items;
	    foreach(QTreeWidgetItem *itm, list)
		for(int i=0; i<itm->childCount(); ++i)
			items << itm->child(i);
	    list.clear();
	    return this->selectItemsByLevel(items,level-1);
    }
    
    void MyQTreeWidget::selectAllSincePageItem(bool check)
    {
	QList<QTreeWidgetItem *> items,items2;
	QTreeWidgetItem *page,*itm;
	items = this->selectedItems();
	if ( !items.isEmpty() )
	    foreach(QTreeWidgetItem *item, items) {
		page = this->getPage(item);
		for(int i=0; i<page->childCount(); ++i)
		    items2 << page->child(i);
		while(!items2.isEmpty()) {
		    itm = items2.takeFirst();
		    if ( check )
			itm->setCheckState(0,Qt::Checked);
		    else
			itm->setCheckState(0,Qt::Unchecked);
		    for(int i=0; i<itm->childCount(); ++i)
			items2 << itm->child(i);
		}
	    }
	items.clear();
	items2.clear();
	return;
    }
    
    void MyQTreeWidget::selectAllItemsSinceItemByType(QTreeWidgetItem *item,int type,bool check)
    {
	    QList<QTreeWidgetItem *> list; /// lista con los items a marcar
	    QList<QTreeWidgetItem *> aux;  /// lista temporal para hacer recursión
	    int level = -type;
	    if( !item )
		return;
	    aux.append(item);
	    if ( this->itemIsPage(item) )
		list = this->selectItemsByLevel(aux,level + 4);
	    else if ( this->itemIsBlock(item) ) {
		if ( type == PAGEv )
		    list << item->parent();
		else
		    list = this->selectItemsByLevel(aux,level + 3);
	    }
	    else if ( this->itemIsRow(item) ) {
		if ( type == BLOCKv )
		    list << item->parent();
		else if ( type == PAGEv )
		    list << item->parent()->parent();
		else
		    list = this->selectItemsByLevel(aux,level + 2);
	    }
	    else if ( this->itemIsWord(item) ) {
		if ( type == ROWv )
		    list << item->parent();		    
		else if ( type == BLOCKv )
		    list << item->parent()->parent();
		else if ( type == PAGEv )
		    list << item->parent()->parent()->parent();
		else
		    list = this->selectItemsByLevel(aux,level + 1);
	    }
	    else {
		if ( type == WORDv )
		    list << item->parent();	
		else if ( type == ROWv )
		    list << item->parent()->parent();		    
		else if ( type == BLOCKv )
		    list << item->parent()->parent()->parent();
		else if ( type == PAGEv )
		    list << item->parent()->parent()->parent()->parent();
		else
		    list = this->selectItemsByLevel(aux,level);
	    }
	    foreach(QTreeWidgetItem *itm, list)
		if ( check )
		    itm->setCheckState(0,Qt::Checked);
		else
		    itm->setCheckState(0,Qt::Unchecked);
	    return;
    }
    
    
    void MyQTreeWidget::selectAll(bool check)
    {
	int count = 0;
	bool canceled = false;
	this->setEnabled(false);
	QTreeWidgetItem *p = this->invisibleRootItem();
	QTreeWidgetItem *itm,*son;
	QList<QTreeWidgetItem *> items;
	QString actionString;
	if ( check )
	    actionString = tr("Selecting items...");
	else
	    actionString = tr("Deselecting items...");
	QProgressDialog progress(actionString, tr("Abort action"), 0
				 , this->itemsNumber, this);
	progress.setWindowModality(Qt::WindowModal);
	//Add pages
	for( int i=0 ; i<p->childCount() ; ++i ) {
	    items << p->child(i);
	}
	while( !items.isEmpty() && !canceled ) {
	    itm = items.takeFirst();
	    for( int i=0 ; i<itm->childCount() ; ++i ) {
		progress.setValue(count++);
		son = itm->child(i);
		if ( check )
		    son->setCheckState(0,Qt::Checked);
		else
		    son->setCheckState(0,Qt::Unchecked);
		if ( son->data(2,Qt::UserRole).toInt() != LETTERv )
		    items << son;
		if ( progress.wasCanceled() ) {
		    canceled = true;
		    break;
		}
	    }
	}
	this->setEnabled(true);
	return;
    }
    
    
    /**************************************************
     **************************************************
			    SLOTS
     **************************************************
     **************************************************/
    
    
    void MyQTreeWidget::deselectAll()
    {
	this->selectAll(false);
    }
    
    void MyQTreeWidget::deselectAllLettersSinceItem()
    {
	QList<QTreeWidgetItem *> items;
	items = this->selectedItems();
	if ( !items.isEmpty() )
	    foreach(QTreeWidgetItem *item, items)
		this->selectAllItemsSinceItemByType(item,LETTERv,false);
    }
    
    void MyQTreeWidget::deselectAllWordsSinceItem()
    {
	QList<QTreeWidgetItem *> items;
	items = this->selectedItems();
	if ( !items.isEmpty() )
	    foreach(QTreeWidgetItem *item, items)
		this->selectAllItemsSinceItemByType(item,WORDv,false);
    }
    
    void MyQTreeWidget::deselectAllRowsSinceItem()
    {
	QList<QTreeWidgetItem *> items;
	items = this->selectedItems();
	if ( !items.isEmpty() )
	    foreach(QTreeWidgetItem *item, items)
		this->selectAllItemsSinceItemByType(item,ROWv,false);
    }
    
    void MyQTreeWidget::deselectAllBlocksSinceItem()
    {
	QList<QTreeWidgetItem *> items;
	items = this->selectedItems();
	if ( !items.isEmpty() )
	    foreach(QTreeWidgetItem *item, items)
		this->selectAllItemsSinceItemByType(item,BLOCKv,false);
    }
    
    void MyQTreeWidget::deselectAllSincePageItem() {
	this->selectAllSincePageItem(false);
    }
    
    void MyQTreeWidget::selectAll()
    {
	this->selectAll(true);
    }
    
    
    void MyQTreeWidget::selectAllLettersSinceItem()
    {
	QList<QTreeWidgetItem *> items;
	items = this->selectedItems();
	if ( !items.isEmpty() )
	    foreach(QTreeWidgetItem *item, items)
		this->selectAllItemsSinceItemByType(item,LETTERv,true);
    }
    
    void MyQTreeWidget::selectAllWordsSinceItem()
    {
	QList<QTreeWidgetItem *> items;
	items = this->selectedItems();
	if ( !items.isEmpty() )
	    foreach(QTreeWidgetItem *item, items)
		this->selectAllItemsSinceItemByType(item,WORDv,true);
    }
    
    void MyQTreeWidget::selectAllRowsSinceItem()
    {
	QList<QTreeWidgetItem *> items;
	items = this->selectedItems();
	if ( !items.isEmpty() )
	    foreach(QTreeWidgetItem *item, items)
		this->selectAllItemsSinceItemByType(item,ROWv,true);
    }
    
    void MyQTreeWidget::selectAllBlocksSinceItem()
    {
	QList<QTreeWidgetItem *> items;
	items = this->selectedItems();
	if ( !items.isEmpty() )
	    foreach(QTreeWidgetItem *item, items)
		this->selectAllItemsSinceItemByType(item,BLOCKv,true);
    }
    
    void MyQTreeWidget::selectAllSincePageItem() {
	this->selectAllSincePageItem(true);
    }

}