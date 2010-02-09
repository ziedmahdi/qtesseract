/**********************************************************************
 * File:        myqgraphicsview.h 
 * Description: The MyQMainWindow class provides a widget that is used 
 * to edit and display both plain and rich text. It supports multipage
 * both to print and save a file. It uses a simplified version of the
 * demonstration in Qt.
 * Author:      Roberto Benitez Monje
 * Created:     Mon Jan 04 2010
 **********************************************************************/
/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the demonstration applications of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial Usage
** Licensees holding valid Qt Commercial licenses may use this file in
** accordance with the Qt Commercial License Agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Nokia.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain
** additional rights. These rights are described in the Nokia Qt LGPL
** Exception version 1.0, included in the file LGPL_EXCEPTION.txt in this
** package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
** If you are unsure which license is appropriate for your use, please
** contact the sales department at http://www.qtsoftware.com/contact.
** $QT_END_LICENSE$
**
****************************************************************************/
#ifndef MYQTEXTEDIT_H
#define MYQTEXTEDIT_H

#include <QTextEdit>
#include <QFontComboBox>
#include <QToolBar>

namespace qtessnamespace
{
    /**
      La clase del editor de texto
      * @author Roberto Benitez Monje
    */
    class MyQTextEdit : public QTextEdit
    {
	Q_OBJECT
    public:
	/**
	  * Constructor de MyQTextEdit
	  * @param parent El QWidget padre
	  */
	MyQTextEdit(QWidget *parent = 0);
    
	/**
	  * Borra los documentos almacenados y establece el nombre de archivo a fileName
	  * @param fileName Nombre de archivo
	  */
	void newFile(QByteArray fileName, int pagesNumber);
	
	QAction *actionSaveAs;
	
	QMenu *menuEdit;
	QMenu *menuFormat;
	
	QToolBar *barEdit;
	QToolBar *barFormat;
	QToolBar *barText;
    public slots:
    
	/**
	  * El thread que realiza OCR se comunica con esta clase mediante la señal
	  * writedDocument(int , QTextDocument) para pasarle los documentos.
	  * @param pageNumber Numero de pagina procesado
	  * @param doc Documento procesado
	  */
	void insertDocument(int pageNumber, QTextDocument *doc);
	void filePrint();
	
	/**
	  * Cuando se cambia de pagina en la interfaz se llama a esta funcion. Se coge
	  * el documento correspodiende a la pagina y se conecta con el editor de
	  * texto.
	  * @param page Pagina a la que se va a cambiar
	  */
	void pageChanged(int page);
	
	/**
	  * Esta funcion sirve para ocultar y mostrar los menus y barras de la
	  * interfaz
	  * @param visible Con true, se ven los menus y barras. Con false, se ocultan
	  */
	void updateWindow(bool visible);
    
    private:
	
	/**
	  * Esta funcion sirve para ocultar y mostrar los menus y barras de la
	  * interfaz
	  * @param visible Con true, se ven los menus y barras. Con false, se ocultan
	  */
	void connectDocument();
	
	/**
	  * Esta funcion sirve para imprimir más de una página (QTextDocument). Esta
	  * echa a partir de QTextDocument::print. Soporta varias copias. Orden
	  * inverso.
	  * @param printer Impresora configurada para empezar a escribir en ella.
	  */
	void multiPagePrint(QPrinter *printer);
	
	void setupFileActions();
	void setupEditActions();
	void setupTextActions();
	
	bool maybeSave();
	
	/**
	  * Establece el nombre del documento actual. Pone el titulo a la ventana y
	  * isModified a falso.
	  * @param fileName Nombre del documento actual.
	  */
	void setCurrentFileName(const QString &fileName);
    protected:
	
	/**
	  * Funcion que se ejecuta cuando se cierra el programa. Esta
	  * implementado que se guarde la posicion y tamaño de la ventana
	  * @param e Evento de cierre que contiene informacion.
	  */
	virtual void closeEvent(QCloseEvent *e);
    private slots:
	bool fileSave();
	bool fileSaveAs();
    
	void textBold();
	void textUnderline();
	void textItalic();
	void textFamily(const QString &f);
	void textSize(const QString &p);
	
	/** Aplica el estilo de lineado */
	void textStyle(int styleIndex);
	
	void textColorSlot();
	void textAlign(QAction *a);
    
	void currentCharFormatChanged(const QTextCharFormat &format);
	void cursorPositionChanged();
	
	/**
	  * Actualiza el disparador de Pegar
	  */
	void clipboardDataChanged();
    
	void deleteSelectedText();
    private:
	void alignmentChanged(Qt::Alignment a);
	void colorChanged(const QColor &c);
	void fontChanged(const QFont &f);
	void mergeFormatOnWordOrSelection(const QTextCharFormat &format);
    
	int pagesNumber;
	QAction *actionTextBold;
	QAction *actionTextUnderline;
	QAction *actionTextItalic;
	QAction *actionTextColor;
	QAction *actionAlignLeft;
	QAction *actionAlignCenter;
	QAction *actionAlignRight;
	QAction *actionAlignJustify;
	QAction *actionUndo;
	QAction *actionRedo;
	QAction *actionCut;
	QAction *actionCopy;
	QAction *actionPaste;
	QAction *actionDelete;
	QAction *actionSelectAll;
    
	/// Estilo de listados.
	QComboBox *comboStyle;
	QFontComboBox *comboFont;
	QComboBox *comboSize;
    
	/// Hash donde se guardan los documentos de cada página.
	QHash<int,QTextDocument*> documentHash;
    
	QString fileName;
    };
}
#endif // MYQTEXTEDIT_H
