#include <QGraphicsScene>
#include <QGraphicsView>
#include <QScrollBar>
#include "include/zoomarea.h"

namespace qtessnamespace
{
    
    ZoomArea::ZoomArea():QGraphicsRectItem()
    {
	this->setPos(0,0);
	this->setBrush(QColor(255,0,0,30));
	this->setFlags(QGraphicsItem::ItemIsFocusable | QGraphicsItem::ItemIsMovable |
		QGraphicsItem::ItemIsSelectable );
	QPen pen = QPen( Qt::darkRed, 2, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin );
	this->setOpacity(0.9);    //Arregla un problema de QtGraphicsScene 4.5.2
	this->setPen(pen);
	this->areaBorder = 2.0;
	this->currentZoom = 0;
	this->halfPenWidthF = 1.0;
	this->making = false;
	this->resizeBorder = 10.0;
	this->resizing = false;
	this->zoomStack.clear();
	this->zoomStack.append(QRectF(1,1,1,1));
	this->setAcceptHoverEvents(true);
    }
    
    ZoomArea::~ZoomArea()
    {
	this->zoomStack.clear();
    }
    
    void ZoomArea::appendActualView()
    {
	zoomStack.resize(this->currentZoom + 1);
	zoomStack.append(this->getSceneArea());
	this->currentZoom++;
    }
    
    void ZoomArea::normalized()
    {
	QRectF rectF = this->getSceneArea().normalized();
	this->setArea(rectF);
    }
    void ZoomArea::setArea(QRectF rectF)
    {
	this->setRect(0,0,rectF.width(),rectF.height());
	this->setPos(rectF.x(),rectF.y());
    }
    
    void ZoomArea::setOriginalView()
    {
	QScrollBar *horizontal = this->scene()->views().at(0)->horizontalScrollBar();
	QScrollBar *vertical = this->scene()->views().at(0)->verticalScrollBar();
	qreal left = horizontal->value();
	qreal top = vertical->value();
	qreal width = horizontal->pageStep();
	qreal height = vertical->pageStep();
	QRectF rectF;
	rectF.setLeft(left);
	rectF.setTop(top);
	rectF.setWidth(width);
	rectF.setHeight(height);
	this->zoomStack[0] = rectF;
    }
    
    QRectF ZoomArea::getPreviousView()
    {
	if (this->currentZoom > 0) {
	    this->currentZoom--;
	    return this->zoomStack[this->currentZoom];
	}
	else
	    return QRectF();
    }
    
    QRectF ZoomArea::getSceneArea()
    {
	if ( this->currentZoom == 0 )	    
	    // Si la lista de zooms es vacia, se inserta la vista actual. Asi se puede
	    // volver a esta vista una vez que se haya hecho zoom.
	    this->setOriginalView();
	return this->sceneBoundingRect().adjusted(this->halfPenWidthF
						  , this->halfPenWidthF
						  , -this->halfPenWidthF
						  , -this->halfPenWidthF);
    }
    
    void ZoomArea::done()
    {
	QRectF rectF = this->getSceneArea().normalized();
	QRectF viewRect = this->getViewRectF();
	rectF.setLeft(qMax(0.0, rectF.left()));
	rectF.setTop(qMax( 0.0, rectF.top() ) );
	rectF.setRight(qMin(rectF.right(), viewRect.right()));
	rectF.setBottom(qMin(rectF.bottom(), viewRect.bottom()));
	this->setArea(rectF);
	this->resizing = false;
	this->making = false;
	this->setFlag(QGraphicsRectItem::ItemIsMovable, true);
    }
    
    void ZoomArea::initCreation(QPointF pointF)
    {
	this->resizeType = 6;		    // 6-> cuenta tanto pos.x como pos.y
	this->making = true;
	float scalex = this->scene()->views().at(0)->matrix().m11();
	float scaley = this->scene()->views().at(0)->matrix().m22();
	this->updateZoomBorders(qMax(scalex,scaley));
	this->setArea(QRectF(pointF,QSize()));
	this->setFlag(QGraphicsRectItem::ItemIsMovable,false);
	this->show();
    }
    
    void ZoomArea::initResize(QPointF pointF)
    {
	this->resizeType = this->getZoomResizeHandleType(pointF);
	if ( this->resizeType ) {
	    this->resizing = true;
	    this->setFlag(QGraphicsRectItem::ItemIsMovable,false);
	}
    }
    
    void ZoomArea::resize(QPointF pointF)
    {	
	QRectF rectF = this->getSceneArea();;
	switch( this->resizeType ) {
	    case 1:
		rectF.setTop(pointF.y());
		break;
	    case 2:
		rectF.setRight(pointF.x());
		break;
	    case 3:
		rectF.setTopRight(pointF);
		break;
	    case 4:
		rectF.setBottom(pointF.y());
		break;
	    case 6:
		rectF.setBottomRight(pointF);
		break;
	    case 8:
		rectF.setLeft(pointF.x());
		break;
	    case 9:
		rectF.setTopLeft(pointF);
		break;
	    case 12:
		rectF.setBottomLeft(pointF);
		break;
	    default:
		break;
	}
	this->setArea(rectF);
	return;
    }
    
    bool ZoomArea::areaContains(QPointF pointF)
    {
	float auxBorder = this->resizeBorder;
	QRectF auxRect = this->getSceneArea();
	QRectF rectF = QRectF(auxRect.x() - auxBorder/2,auxRect.y() - auxBorder/2,
			  auxRect.width() + auxBorder,auxRect.height() + auxBorder);
	return rectF.contains(pointF);
    }
    
    bool ZoomArea::insideAreaContains(QPointF pointF)
    {
	float auxBorder = this->resizeBorder;
	QRectF auxRect = this->getSceneArea();
	QRectF rectF = QRectF(auxRect.x() + auxBorder/2,auxRect.y() + auxBorder/2,
	    auxRect.width() - auxBorder,auxRect.height() - auxBorder);
	return (rectF.contains(pointF));
    }
    int ZoomArea::getZoomResizeHandleType(QPointF pointF)
    {
	int total = 0;
	if ( this->areaContains(pointF) && !(this->insideAreaContains(pointF)) ) {
	    float auxBorder = this->resizeBorder;
	    QRectF auxRect = this->getSceneArea();//sceneBoundingRect();
	    bool topDifference = abs(qRound(auxRect.top() - pointF.y())) < auxBorder;
	    bool rightDifference = abs(qRound(auxRect.right() - pointF.x())) < auxBorder;
	    bool bottomDifference = abs(qRound(auxRect.bottom() - pointF.y())) < auxBorder;
	    bool leftDifference = abs(qRound(auxRect.left() - pointF.x())) < auxBorder;
	    if ( topDifference )
		total += 1;
	    if ( rightDifference )
		total += 2;
	    if ( bottomDifference )
		total += 4;
	    if ( leftDifference )
		total += 8;
	    switch ( total ) {
		case 5:
		case 7:
		case 10:
		case 11:
		case 13:
		case 14:
		case 15:
		    total = 0;
		    break;
		default:
		    break;
	    }
	}
	return total;
    }
    
    void ZoomArea::updateZoomCursor(QPointF pointF)
    {
	int typeCursor = this->getZoomResizeHandleType(pointF);
	switch( typeCursor ) {
	    case 0: //dentro de zoomarea o fuera
		unsetCursor();
		break;
	    case 1:
		setCursor(Qt::SizeVerCursor);
		break;
	    case 2:
		setCursor(Qt::SizeHorCursor);
		break;
	    case 3:
		setCursor(Qt::SizeBDiagCursor);
		break;
	    case 4:
		setCursor(Qt::SizeVerCursor);
		break;
	    case 6:
		setCursor(Qt::SizeFDiagCursor);
		break;
	    case 8:
		setCursor(Qt::SizeHorCursor);
		break;
	    case 9:
		setCursor(Qt::SizeFDiagCursor);
		break;
	    case 12:
		setCursor(Qt::SizeBDiagCursor);
		break;
	}
    }
    
    void ZoomArea::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
    {
	QGraphicsRectItem::mouseMoveEvent(event);
	if ( this->flags().testFlag(QGraphicsRectItem::ItemIsMovable) ) {
	    setCursor(Qt::ClosedHandCursor);
	    QRectF tester = this->getSceneArea();
	    if ( !this->isValid(tester) ) {
		QPointF pointF = tester.topLeft();
		QRectF viewRectF = this->getViewRectF();
		if ( tester.left() < viewRectF.left() )
		    pointF.setX(viewRectF.left());
		if ( tester.right() > viewRectF.right() )
		    pointF.setX(viewRectF.right() - tester.width() );
		if ( tester.top() < viewRectF.top())
		    pointF.setY(viewRectF.top());
		if ( tester.bottom() > viewRectF.bottom() )
		    pointF.setY(viewRectF.bottom() - tester.height());
		this->setPos(pointF);
	    }
	}
    }
    
    void ZoomArea::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
    {
	unsetCursor();
	QGraphicsRectItem::mouseReleaseEvent(event);
    }
    
    /***************************************************************/
    /*******************  PRIVATED FUNCTIONS  *********************/
    /***************************************************************/
    
    QRectF ZoomArea::getViewRectF()
    {
	QScrollBar *horizontal = this->scene()->views().at(0)->horizontalScrollBar();
	QScrollBar *vertical = this->scene()->views().at(0)->verticalScrollBar();
	
	QRectF viewRect = this->scene()->views().at(0)->sceneRect();  
	if ( horizontal->isVisible() ) {
	    viewRect.setX( horizontal->value()
			       / this->scene()->views().at(0)->matrix().m11() );
	    viewRect.setWidth( horizontal->pageStep()
			       / this->scene()->views().at(0)->matrix().m11() );
	}
	if ( vertical->isVisible() ) {
	    viewRect.setY( vertical->value()
				/ this->scene()->views().at(0)->matrix().m22() );
	    viewRect.setHeight( vertical->pageStep()
				/ this->scene()->views().at(0)->matrix().m22() );
	}
	return viewRect;
    }
    
    bool ZoomArea::isValid(QRectF rectF)
    {
	QRectF viewRect = this->getViewRectF();    
	if ( ( rectF.x() < viewRect.x() ) 
	    || ( rectF.right() > viewRect.right() ) )
	    return false;
	if ( ( rectF.y() < viewRect.y() ) 
	    || ( rectF.bottom() > viewRect.bottom() ) )
	    return false;
	return true;
    }
    
    void ZoomArea::updateZoomBorders(float factor)
    {
	this->areaBorder = 2 / factor ;
	this->resizeBorder = 10 / factor;
	QPen pen = this->pen();
	pen.setWidthF(this->areaBorder);
	this->setPen(pen);
	this->halfPenWidthF = this->areaBorder / 2;
    }
}