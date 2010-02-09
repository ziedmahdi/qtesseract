#ifndef ZOOMAREA_H
#define ZOOMAREA_H

#include <QGraphicsRectItem>

namespace qtessnamespace
{    

    /**
      * ZoomArea es un objeto de QGraphicsScene para seleccionar un
      * area donde aplicar zoom. Guarda una lista de los zooms aplicados
      * para deshacer los zooms.
      * @author Roberto Benitez Monje
    */
    class ZoomArea : public QGraphicsRectItem
    {
    public:
	/**
	  * Constructor de ZoomArea
	  */
	ZoomArea();
	
	/**
	  * Destructor de ZoomArea
	  */
	~ZoomArea();
	
	/**
	  * Devuelve si el punto pointF esta dentro del rectángulo de
	  * ZoomArea
	  * @param pointF Un punto en coordenadas del escenario
	  * @return True si esta dentro del ractángulo de ZoomArea, False
	  * si no lo está,
	  */
	bool areaContains(QPointF pointF);
	
	/**
	  * Devuelve si el punto pointF esta dentro del rectángulo de
	  * ZoomArea sin contar los bordes.
	  * @param pointF Un punto en coordenadas del escenario
	  * @return True si esta dentro del ractángulo de ZoomArea, False
	  * si no lo está,
	  */
	bool insideAreaContains(QPointF pointF);
	
	/**
	  * Devuelve el zoom realizado inmediatamente antes.
	  * @return El rectángulo de la imagen que se veia antes de aplicar
	  * el zoom
	  */
	QRectF getPreviousView();
	
	/**
	  * Añade a la lista de zooms, el rectángulo que tiene ZoomArea actual
	  */
	void appendActualView();
	
	/**
	  * Ajusta el rectangulo actual para que sea válido. Desactiva el
	  * resize y el creation.
	  */
	void done();
	
	/**
	  * Devuelve el rectángulo de ZoomArea quitando los bordes
	  * @return Rectángulo a devolver
	  */
	QRectF getSceneArea();
	
	/**
	  * Se hace visible ZoomArea. Actualiza los bordes del rectángulo.
	  * Se convierte en estático, sin posibilidad de moverse. Solo
	  * crecer.
	  * @param pointF Es el punto de origen de ZoomArea.
	  */
	void initCreation(QPointF pointF);
	
	/**
	  * Calcula si se hace resize, para ello el punto pointF debe estar en 
	  * el borde del rectángulo de ZoomArea. De ser cierto se convierte en 
	  * estático
	  * @param pointF Es el punto donde se hizo clic en ZoomArea
	  */
	void initResize(QPointF pointF);
	
	/**
	  * Modifica el tamaño de ZoomArea
	  * @param pointF Es el punto donde se encuentra el cursor
	  */
	void resize(QPointF pointF);
	
	/**
	  * Actualiza el cursor de ZoomArea segun el borde en el que
	  * este situado el cursor del ratón.
	  * @param pointF Es el punto donde se encuentra el cursor
	  */
	void updateZoomCursor(QPointF pointF);
    
	bool isMaking() {
	    return this->making;
	}
	
	bool isResizing() {
	    return this->resizing;
	}
    protected:	
	/**
	  * Cuando el raton se mueve por creacion o cambio de tamaño de zoomarea.
	  * Tambien cuando se mueve con zoomarea en el escenario en otro caso.
	  * @param event Evento que se recibe cuando se mueve el raton
	  */
	void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
	
	/**
	  * Vuelve al estado inicial del visor. Sirve tambien para terminar la creacion
	  * de zoomarea.
	  * @param event Evento que se recibe cuando se suelta un boton del raton.
	  */
	void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    private:
	
	/**
	  * Devuelve el rectangulo que se ve del escenario.
	  * @return Es el rectangulo.
	  */
	QRectF getViewRectF();
	
	/**
	  * Devuelve en que borde o esquina esta el punto pointF
	  * @return 0 si no esta en el borde. Si no, indica borde
	  * o esquina del rectángulo
	  */
	int getZoomResizeHandleType(QPointF pointF);
	
	/**
	  * Calcula si el rectángulo pasado como argumento está dentro
	  * de la vista
	  * @return True, si cierto. False, si falso.
	  */
	bool isValid(QRectF rectF);
	
	/** 
          * Normaliza el rectangulo de ZoomArea para que no tenga ancho
	  * ni alto negativo
	  */
	void normalized();
	
	/**
	  * Actualiza el rectángulo actual a rectF
	  * @param rectF Rectángulo que se convierte en el actual.
	  */
	void setArea(QRectF rectF);
	
	/**
	  * Guarda en la posicion inicial de la lista la vista actual
	  */
	void setOriginalView();
	
	/**
	  * Actualiza el borde del rectángulo
	  * @param factor Es el factor de escala mayor entre el vertical y 
	  * horizontal
	  */
	void updateZoomBorders(float factor);
	
	/// Grosor del borde del rectángulo
	float areaBorder;
	
	/// Tamaño de la lista de zooms, apunta al ultimo zoom guardado
	int currentZoom;
	
	/// Distancia donde al clicar con el ratón se hará resize
	float resizeBorder;
	
	/// Guarda en que posicion del rectángulo se hace resize
	int resizeType;
	
	/// Es la mitad de areaBorder
	float halfPenWidthF;
	
	/// Indica que se está haciendo resize
	bool resizing;
	
	/// Indica que se está construyendo el rectángulo
	bool making;
	
	/// Es el contenedor donde se guardan los zooms
	QVector<QRectF> zoomStack;
    };
    #endif // ZOOMAREA_H
    
	/*
	enum ZoomResizeHandle
	{
	    None = 0,
	    TopLeft = 9,
	    Top = 1,
	    TopRight = 3,
	    Right = 2,
	    BottomRight = 6,
	    Bottom = 4,
	    BottomLeft = 12,
	    Left = 8
	};
	*/
}