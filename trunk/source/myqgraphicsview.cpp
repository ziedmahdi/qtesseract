/**********************************************************************
 * File:        myqgraphicsview.cpp 
 * Description: The MyQGraphicsView class provides a image viewer for
 ** displaying the result of Tesseract.
 * Author:      Roberto Benitez Monje
 * Created:     Mon Jan 04 2010
 **********************************************************************/
#include <QApplication>
#include <QGLWidget>
#include <QMouseEvent>
#include <QScrollBar>
#include <QtDebug>

#include "include/myqgraphicsview.h"
#include "include/tesstext.h"

namespace qtessnamespace
{
    
    MyQGraphicsView::MyQGraphicsView(QWidget *parent):QGraphicsView(parent)
    {
	this->setScene(new QGraphicsScene());
	this->setBackgroundBrush(Qt::lightGray);
	this->setDisabled(true);
    #ifdef QT_OPENGL_SUPPORT
	if ( QGLFormat::hasOpenGL() ) {
	    this->setViewport(new QGLWidget); //OpenGL On
	}
    #endif
	this->setMinimumSize(200, 200);
	this->setResizeAnchor(QGraphicsView::AnchorViewCenter);
	this->setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
	this->setViewportUpdateMode(QGraphicsView::FullViewportUpdate);//SmartViewportUpdate);
	this->currentPage = 0;
	this->pixmap = QPixmap();
	this->notReading = true;
	this->zoomarea = 0;
	QPixmapCache::setCacheLimit(32768); // 32 MB of image cache
    }
    
    
    QGraphicsRectItem* MyQGraphicsView::insertPageRectItem(const QRect &rect)
    {
	QGraphicsRectItem* pageRect = new QGraphicsRectItem(rect, 0, this->scene());
	pageRect->setFlag(QGraphicsItem::ItemDoesntPropagateOpacityToChildren, true);
	pageRect->setPen(Qt::NoPen);
	pageRect->setZValue( -1 );
	pageRect->setData(2,PAGEv);
	return pageRect;
    }
    
    void MyQGraphicsView::insertPagesInfo(const QList<QString> &v)
    {
	/// v contiene información de las imagenes originales
	int i;
	qreal width;
	qreal height;
	int page;
	qreal xres;
	qreal yres;
	sizeHash.clear();
	QStringList list;
	for (i=0;i<v.count();++i) {
	    list = v.at(i).split(',');
	    if (list.count() == 5) {
		page = list.at(0).toInt(0,10);
		width = list.at(1).toInt(0,10);
		height = list.at(2).toInt(0,10);
		xres = list.at(3).toInt(0,10);
		yres = list.at(4).toInt(0,10);
		
		// Se calcula el tamaño de la imagen que se visualiza,
		// no la original, que puede tener otra resolucion
		width *= 300 / xres;
		height *= 300 / yres;
		sizeHash.insert(page,QSizeF(width, height));
	    }
	}
    }
    
    /***************************************************************/
    /**************************  SLOTS  ****************************/
    /***************************************************************/
    
    void MyQGraphicsView::fitToWindow()
    {
	this->fitInView(this->sceneRect(),Qt::KeepAspectRatio);
    }
    
    void MyQGraphicsView::normalSize()
    {
	qreal scalex = 1/this->matrix().m11();
	qreal scaley = 1/this->matrix().m22();
	this->scale(scalex, scaley);
    }
    
    void MyQGraphicsView::removeRectItemList()
    {
	this->setEnabled(false);
	//QGraphicsScene * sceneBCKP = this->scene();
	if ( this->zoomarea ) {
	    this->scene()->removeItem(zoomarea);
	    QHashIterator< int, ZoomArea * > i(zoomAreaHash);
	    while ( i.hasNext() ) {
		i.next();
		i.value()->~ZoomArea();
	    }
	    zoomAreaHash.clear();
	    this->scene()->clear();
	}
	this->scene()->clear();
	zoomarea = new ZoomArea();
	this->scene()->addItem(zoomarea);
	zoomarea->hide();
	this->setEnabled(true);
    }
    
    void MyQGraphicsView::reset()
    {
	this->pixmap = QPixmap();
	QPixmapCache::clear();
	this->currentPage = 0;
	this->notReading = true;
	this->removeRectItemList();
	this->matrixHash.clear();
    }
    
    void MyQGraphicsView::restoreAndUnlock()
    {
	this->setEnabled(false);
	if ( zoomarea ) {
	    qreal scalex;
	    qreal scaley;
	    if ( matrixHash.contains(currentPage) ) {
		QMatrix m = matrixHash.value(currentPage);
		scalex = m.m11() / this->matrix().m11();
		scaley = m.m22() / this->matrix().m22();
		this->scale(scalex, scaley);
		this->horizontalScrollBar()->setValue(m.dx());
		this->verticalScrollBar()->setValue(m.dy());
	    } else {
		scalex = 1 / this->matrix().m11();
		scaley = 1 / this->matrix().m22();
		this->scale(scalex, scaley);
		this->horizontalScrollBar()->setValue(0);
		this->verticalScrollBar()->setValue(0);
		matrixHash.insert(currentPage, QMatrix(scalex, 0, 0
						       , scaley, 0, 0));
	    }
	}
	this->notReading = true;
	this->setEnabled(true);
    }
    
    void MyQGraphicsView::saveAndLock()
    {
	this->notReading = false;
	this->setEnabled(false);
	if ( zoomarea ) {
	    if ( matrixHash.contains(currentPage) ) {
		matrixHash.remove(currentPage);
	    }
	    matrixHash.insert(currentPage,this->viewMatrix());
	}
	this->setEnabled(true);
    }
    
    void MyQGraphicsView::pageChanged(const QByteArray &fileName, int page)
    {
	this->setEnabled(false);
	if ( !this->notReading ) {
	    this->pixmap = loadImage(fileName, page);
	    QSizeF sizeF = sizeHash.at(page - 1);
	    this->setSceneRect(QRectF(QPointF(0,0), sizeF));
	} else if ( zoomarea ) {
	    if ( matrixHash.contains(currentPage) ) {
		matrixHash.remove(currentPage);
	    }
	    matrixHash.insert(currentPage, this->viewMatrix());
	    if ( !matrixHash.contains(page) ) {
		this->normalSize();
	    }
	    this->pixmap = loadImage(fileName, page);
	    
	    QSizeF sizeF = sizeHash.at(page - 1);
	    this->setSceneRect(QRectF(QPointF(0,0), sizeF));
	    
	    this->scene()->removeItem(zoomarea);
	    if ( zoomAreaHash.contains(page) )
		zoomarea = zoomAreaHash.value(page);
	    else {
		zoomarea = new ZoomArea();
		zoomarea->hide();
		zoomAreaHash.insert(page, zoomarea);
	    }
	    this->scene()->addItem(zoomarea);
    
	    if ( matrixHash.contains(page) ) {
		QMatrix m = matrixHash.value(page);
		this->scale( m.m11() / this->matrix().m11(), m.m22() / this->matrix().m22() );
		this->horizontalScrollBar()->setValue(m.dx());
		this->verticalScrollBar()->setValue(m.dy());
	    } else
		matrixHash.insert(page, this->viewMatrix());
	}
	currentPage = page;
	this->fileName = fileName;
	this->setEnabled(true);
    }
    
    void MyQGraphicsView::zoomIn()
    {
	this->scale(1.25, 1.25);
    }
    
    void MyQGraphicsView::zoomOut()
    {
	this->scale(0.75, 0.75);
    }
    
    /***************************************************************/
    /*******************  PROTECTED FUNCTIONS  *********************/
    /***************************************************************/
    
    void MyQGraphicsView::drawBackground (QPainter *painter, const QRectF &rect)
    {
	QGraphicsView::drawBackground(painter, rect);
	if ( !this->pixmap.isNull() ) {
	    qreal scalex = this->matrix().m11();
	    qreal scaley = this->matrix().m22();
	    QByteArray tempName = QByteArray( getTempFileName(this->fileName
							      ,this->currentPage
							      - 1) );
	    int widthScale = this->sceneRect().width() * scalex;
	    int heightScale = this->sceneRect().height() * scaley;
	    QApplication::setOverrideCursor(Qt::WaitCursor);
	    if ( heightScale > this->pixmap.height() ||
		 widthScale > this->pixmap.width() ) {
		QPixmapCache::remove(tempName);
		this->pixmap = this->loadImage(this->fileName
					       ,this->currentPage);
		this->pixmap = this->pixmap.scaled(QSize(widthScale
							 , heightScale));
		QPixmapCache::remove(tempName);
		QPixmapCache::insert(tempName,this->pixmap);
	    }
	    else if ( heightScale < this->pixmap.height() ||
		 widthScale < this->pixmap.width() ) {
		QPixmap pm;
		if (QPixmapCache::find(tempName,pm)) {
		    this->pixmap = pm.scaled(QSize(widthScale, heightScale));
		}
		else {
		    this->pixmap = this->loadImage(this->fileName
						   ,this->currentPage);
		    this->pixmap = this->pixmap.scaled(QSize(widthScale
							     , heightScale));
		}
		QPixmapCache::remove(tempName);
		QPixmapCache::insert(tempName,this->pixmap);
	    }
	    
	    // Se establece el cursor anterior
	    QApplication::restoreOverrideCursor();
	    
	    //Se halla el rectangulo que se ve en la vista
	    QRectF source(this->sceneRect());
	    QPoint invertedPoint = painter->matrix().inverted().map(QPoint(0,0));
	    if ( this->horizontalScrollBar()->isVisible() ) {
		source.setX(invertedPoint.x());
		source.setWidth(this->horizontalScrollBar()->pageStep() / this->matrix().m11() );
	    }
	    if ( this->verticalScrollBar()->isVisible() ) {
		source.setY(invertedPoint.y());
		source.setHeight( this->verticalScrollBar()->pageStep() / this->matrix().m22() );
	    }
	    painter->scale(1 / scalex, 1 / scaley);
	    painter->setPen(Qt::NoPen);
	    QRectF source2(source.x() * scalex
			   , source.y() * scaley
			   , source.width() * scalex
			   , source.height() * scaley);
	    painter->drawPixmap(source2,this->pixmap,source2);
	}
    }
    /*
    void MyQGraphicsView::drawBackground (QPainter *painter, const QRectF &rect)
    {
	QGraphicsView::drawBackground(painter, rect);
	if ( !this->pixmap.isNull() ) {
	    QRectF source(this->sceneRect());
	    QPoint invertedPoint = painter->matrix().inverted().map(QPoint(0,0));
	    if ( this->horizontalScrollBar()->isVisible() ) {
		source.setX(invertedPoint.x());
		source.setWidth(this->horizontalScrollBar()->pageStep() / this->matrix().m11() );
	    }
	    if ( this->verticalScrollBar()->isVisible() ) {
		source.setY(invertedPoint.y());
		source.setHeight( this->verticalScrollBar()->pageStep() / this->matrix().m22() );
	    }
	    painter->setClipRect(source);
	    painter->setPen(Qt::NoPen);
	    painter->drawPixmap(source,this->pixmap,source);
	}
    }
    */
    void MyQGraphicsView::mouseDoubleClickEvent(QMouseEvent *event)
    {
	if ( event->buttons().testFlag(Qt::LeftButton) && zoomarea->isVisible() ) {
	    if ( zoomarea->isSelected() ) {
		// Se hace el zoom, se oculta el ZoomArea y el cursor se restablece.
		fitInView (zoomarea->getSceneArea(), Qt::KeepAspectRatio);
		zoomarea->appendActualView();
		zoomarea->hide();
		this->unsetCursor();
		return;
	    }
	}
	QGraphicsView::mouseDoubleClickEvent(event);
    }
    
    void MyQGraphicsView::mousePressEvent(QMouseEvent *event)
    {
	if ( event->buttons().testFlag(Qt::LeftButton) ) {
	    // Si se pulsa ademas la tecla Shift del teclado se inicia
	    // zoomarea, se desactiva arrastre del escenario y se modifica
	    // el dibujo del cursor
	    if ( event->modifiers().testFlag(Qt::ShiftModifier) ) {
		QPointF mouseDownPos = mapToScene(event->pos()).toPoint();
		zoomarea->initCreation(mouseDownPos);
		setDragMode(QGraphicsView::NoDrag);
		setCursor(Qt::CrossCursor);
		return;
	    } else
		if ( zoomarea->isVisible() ) {
		// Se clica con el raton para cambiar el tamaño de zoomarea. Se ajusta
		// el cursor y que zoomarea no se pueda mover (solo cambiar tamaño)
		    QPoint pos = mapToScene(event->pos()).toPoint();
		    zoomarea->initResize(pos);
		    if ( zoomarea->isResizing() ) {
			setDragMode(QGraphicsView::NoDrag);
		    }
		    
		}
	} else
	    if ( event->buttons().testFlag(Qt::RightButton) ) {
		// Si zoom area es visible solo se oculta, si no, volver a un zoom anterior
		if ( zoomarea->isVisible() )
		    zoomarea->hide();
		else
		    fitInView(zoomarea->getPreviousView(), Qt::KeepAspectRatio);
		return;
	    }
	QGraphicsView::mousePressEvent(event);
    }
    
    void MyQGraphicsView::mouseMoveEvent(QMouseEvent *event)
    {
	// Se convierte la posicion del evento que esta sobre la aplicacion entera, a
	// respecto el escenario.
	QPointF lastMouseViewPos = mapToScene(event->pos());
	if ( zoomarea->isVisible() ) {
	    if ( zoomarea->isMaking() || zoomarea->isResizing() ) {
		    zoomarea->resize(lastMouseViewPos);
	    } else {
		if ( zoomarea->areaContains(lastMouseViewPos) ) {
		    setDragMode(QGraphicsView::NoDrag);
		    if ( zoomarea->insideAreaContains(lastMouseViewPos) ) {
			zoomarea->setFlag(QGraphicsRectItem::ItemIsMovable, true);
			if ( !event->buttons().testFlag(Qt::LeftButton) )
			    zoomarea->setCursor(Qt::OpenHandCursor);
			QGraphicsView::mouseMoveEvent(event);
		    } else {
			zoomarea->setFlag(QGraphicsRectItem::ItemIsMovable, false);
			zoomarea->updateZoomCursor(lastMouseViewPos);
			setCursor(zoomarea->cursor());
		    }
		    return;
		} else {
		    this->unsetCursor();
		    setDragMode(QGraphicsView::ScrollHandDrag);
		}
	    }
	}
	QGraphicsView::mouseMoveEvent(event);
    }
    
    void MyQGraphicsView::mouseReleaseEvent(QMouseEvent *event)
    {
	this->setDragMode(QGraphicsView::ScrollHandDrag);
	zoomarea->done();
	QGraphicsView::mouseReleaseEvent(event);
    }
    
    /***************************************************************/
    /*******************  PRIVATED FUNCTIONS  *********************/
    /***************************************************************/
    
    QPixmap MyQGraphicsView::loadImage(const QByteArray &fileName, int page)
    {
	QPixmap pm;
	int n = page - 1;
	QByteArray tempName = QByteArray(getTempFileName(fileName,n));
	if ( !QPixmapCache::find(QString(tempName),pm) ) {
	    pm = QPixmap(tempName);
	    QPixmapCache::insert(QString(tempName), pm);
	}
	this->fileName = fileName;
	return pm;
    }
    
    
    QPixmap MyQGraphicsView::loadImage(const QByteArray &fileName, int page, QSize size)
    {
	QPixmap pm;
	int n = page - 1;
	QByteArray tempName = QByteArray(getTempFileName(fileName,n));
	if ( !QPixmapCache::find(QString(tempName),pm) ) {
	    pm = QPixmap(tempName).scaled(size);
	    QPixmapCache::insert(QString(tempName), pm);
	}
	this->fileName = fileName;
	return pm;
    }
    
    void MyQGraphicsView::removeImage(const QByteArray &fileName, int page)
    {
	QPixmap pm;
	int n = page - 1;
	QByteArray tempName = QByteArray(getTempFileName(fileName,n));
	if ( QPixmapCache::find(QString(tempName),pm) ) {
	    QPixmapCache::remove(tempName);
	}
    }
    
    QMatrix MyQGraphicsView::viewMatrix()
    {
	float rectWidth = 0;
	float rectHeight = 0;
	float rectPosX = 0;
	float rectPosY = 0;
	if ( this->horizontalScrollBar()->isVisible() ) {
	    rectWidth = this->horizontalScrollBar()->pageStep();
	    rectPosX = this->horizontalScrollBar()->value();
	}
	if ( this->verticalScrollBar()->isVisible() ) {
	    rectHeight = this->verticalScrollBar()->pageStep();
	    rectPosY = this->verticalScrollBar()->value();
	}
	return QMatrix(this->matrix().m11(), rectWidth,rectHeight
		       , this->matrix().m22(), rectPosX,rectPosY);
    }
}