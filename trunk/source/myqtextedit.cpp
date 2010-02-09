/**********************************************************************
 * File:        myqgraphicsview.cpp 
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
#include <QAction>
#include <QApplication>
#include <QAbstractPrintDialog>
#include <QClipboard>
#include <QCloseEvent>
#include <QColorDialog>
#include <QDialog>
#include <QFileDialog>
#include <QMenu>
#include <QMessageBox>
#include <QPainter>
#include <QPrinter>
#include <QPrintDialog>
#include <QTextBlock>
#include <QTextDocumentWriter>
#include <QTextList>
#include "include/myqtextedit.h"

namespace qtessnamespace
{    
    MyQTextEdit::MyQTextEdit(QWidget *parent):QTextEdit(parent)
    {
	this->setFontPointSize(10.0);
    
	this->setupFileActions();
	this->setupEditActions();
	this->setupTextActions();
    
	connect(this, SIGNAL(currentCharFormatChanged(const QTextCharFormat &))
		, this, SLOT(currentCharFormatChanged(const QTextCharFormat &)));
	connect(this, SIGNAL(cursorPositionChanged())
		, this, SLOT(cursorPositionChanged()));
    
	this->setFocus();
	this->setCurrentFileName(QString());
    
	this->fontChanged(this->font());
	this->colorChanged(this->textColor());
	this->alignmentChanged(this->alignment());
    
	this->connectDocument();
    
	this->setWindowModified(this->document()->isModified());
	this->actionSaveAs->setEnabled(this->document()->isModified());
	
	this->actionUndo->setEnabled(this->document()->isUndoAvailable());
	this->actionRedo->setEnabled(this->document()->isRedoAvailable());
	connect(actionUndo, SIGNAL(triggered()), this, SLOT(undo()));
	connect(actionRedo, SIGNAL(triggered()), this, SLOT(redo()));
	
	connect(actionSelectAll, SIGNAL(triggered()), this, SLOT(selectAll()));
    
	this->actionCut->setEnabled(false);
	this->actionCopy->setEnabled(false);
	this->actionDelete->setEnabled(false);
	connect(actionCut, SIGNAL(triggered()), this, SLOT(cut()));
	connect(actionCopy, SIGNAL(triggered()), this, SLOT(copy()));
	connect(actionPaste, SIGNAL(triggered()), this, SLOT(paste()));
	connect(actionDelete, SIGNAL(triggered()), this, SLOT(deleteSelectedText()));
    
	connect(this, SIGNAL(copyAvailable(bool)), actionCut, SLOT(setEnabled(bool)));
	connect(this, SIGNAL(copyAvailable(bool)), actionCopy, SLOT(setEnabled(bool)));
	connect(this, SIGNAL(copyAvailable(bool)), actionDelete, SLOT(setEnabled(bool)));
    
	connect(QApplication::clipboard(), SIGNAL(dataChanged()), this
		, SLOT(clipboardDataChanged()));
    }
    
    void MyQTextEdit::newFile(QByteArray fileName, int pagesNumber)
    {
	QTextDocument *aux = new QTextDocument(this);
	// If the current document is a child of the text editor, then it is deleted.
	this->setDocument(aux);
	this->setCurrentFileName(fileName);
	
	QHashIterator< int,QTextDocument* > i(documentHash);
	while ( i.hasNext() ) {
	    i.next();
	    i.value()->~QTextDocument();
	}
	documentHash.clear();
	
	this->pagesNumber = pagesNumber; 
	this->actionSaveAs->setEnabled(false);
    }
    
    void MyQTextEdit::alignmentChanged(Qt::Alignment a)
    {
	if ( a & Qt::AlignLeft ) {
	    actionAlignLeft->setChecked(true);
	} else if ( a & Qt::AlignHCenter ) {
	    actionAlignCenter->setChecked(true);
	} else if ( a & Qt::AlignRight ) {
	    actionAlignRight->setChecked(true);
	} else if ( a & Qt::AlignJustify ) {
	    actionAlignJustify->setChecked(true);
	}
    }
    
    void MyQTextEdit::clipboardDataChanged()
    {
	actionPaste->setEnabled( !QApplication::clipboard()->text().isEmpty() );
    }
    
    void MyQTextEdit::closeEvent(QCloseEvent *e)
    {
	if ( maybeSave() )
	    e->accept();
	else
	    e->ignore();
    }
    
    void MyQTextEdit::colorChanged(const QColor &c)
    {
	QPixmap pix(16, 16);
	pix.fill(c);
	actionTextColor->setIcon(pix);
    }
    
    void MyQTextEdit::connectDocument()
    {
	connect(this->document(), SIGNAL(modificationChanged(bool))
		, this, SLOT(setWindowModified(bool)));
	connect(this->document(), SIGNAL(undoAvailable(bool))
		, actionUndo, SLOT(setEnabled(bool)));
	connect(this->document(), SIGNAL(redoAvailable(bool))
		, actionRedo, SLOT(setEnabled(bool)));   
    }
    
    void MyQTextEdit::currentCharFormatChanged(const QTextCharFormat &format)
    {
	fontChanged(format.font());
	colorChanged(format.foreground().color());
    }
    
    void MyQTextEdit::cursorPositionChanged()
    {
	alignmentChanged(this->alignment());
    }
    
    void MyQTextEdit::deleteSelectedText()
    {
       this->textCursor().removeSelectedText();
    }
    
    void MyQTextEdit::filePrint()
    {
    #ifndef QT_NO_PRINTER
	QPrinter printer(QPrinter::HighResolution);
	printer.setFromTo(1,this->pagesNumber);
	printer.setCollateCopies(false);
	QPrintDialog *dlg = new QPrintDialog(&printer, this);
	dlg->setOption(QAbstractPrintDialog::PrintCollateCopies, false);
	if ( this->textCursor().hasSelection() )
	    dlg->addEnabledOption(QAbstractPrintDialog::PrintSelection);
	dlg->setWindowTitle(tr("Print Document"));
	if ( dlg->exec() == QDialog::Accepted )
	    this->multiPagePrint(&printer);
	delete dlg;
    #endif
    }
    
    bool MyQTextEdit::fileSave()
    {
	if ( fileName.isEmpty() )
	    return fileSaveAs();
	QString buffer;
	QTextDocumentWriter writer(fileName);
	bool success;
	for (int i = 1 ; i <= this->pagesNumber ; i++ ) {
	    if ( i != 1 )
		buffer.append(QChar::ParagraphSeparator);
	    buffer.append(documentHash.value(i)->toHtml("UTF-8"));
	}
	QTextDocument doc;
	doc.setHtml(buffer);
	success = writer.write(&doc);
	return success;
    }
    
    bool MyQTextEdit::fileSaveAs()
    {
	QByteArray extensions("ODF files (*.odt);;HTML-Files (*.htm *.html);;"
			      "Plain Text (*.txt);;All Files (*)");
	QString fn;
	fn = QFileDialog::getSaveFileName(this, tr("Save as...")
					  , QDir::homePath()
					  , tr(extensions.constData()));
	if ( fn.isEmpty() )
	    return false;
	if (! (fn.endsWith(".odt", Qt::CaseInsensitive) 
	    || fn.endsWith(".htm", Qt::CaseInsensitive)
	    || fn.endsWith(".html", Qt::CaseInsensitive)
	    || fn.endsWith(".txt", Qt::CaseInsensitive)) )
	    fn += ".odt"; // default
	setCurrentFileName(fn);
	return fileSave();
    }
    
    void MyQTextEdit::fontChanged(const QFont &f)
    {
	comboFont->setCurrentIndex(comboFont->findText(QFontInfo(f).family()));
	comboSize->setCurrentIndex(
		comboSize->findText( QString::number(f.pointSize()) ) );
	actionTextBold->setChecked(f.bold());
	actionTextItalic->setChecked(f.italic());
	actionTextUnderline->setChecked(f.underline());
    }
    
    void MyQTextEdit::insertDocument(int pageNumber, QTextDocument *doc)
    {
	QTextDocument *temp;
	if(documentHash.contains(pageNumber)) {
	    temp = documentHash.value(pageNumber);
	    if ( temp != this->document() )	// Evita un cuelgue
		temp->~QTextDocument();
	    documentHash.remove(pageNumber);
	}
	documentHash.insert(pageNumber,doc);
    }
    
    bool MyQTextEdit::maybeSave()
    {
	if (!this->document()->isModified())
	    return true;
	if (fileName.startsWith(QLatin1String(":/")))
	    return true;
	QMessageBox::StandardButton ret;
	ret = QMessageBox::warning(this, tr("QTesseract"),
				   tr("The document has been modified.\n"
				      "Do you want to save your changes?"),
				   QMessageBox::Save | QMessageBox::Discard
				   | QMessageBox::Cancel);
	if (ret == QMessageBox::Save)
	    return fileSaveAs();
	else if (ret == QMessageBox::Cancel)
	    return false;
	return true;
    }
    
    void MyQTextEdit::mergeFormatOnWordOrSelection(const QTextCharFormat &format)
    {
	QTextCursor cursor = this->textCursor();
	if (!cursor.hasSelection())
	    cursor.select(QTextCursor::WordUnderCursor);
	cursor.mergeCharFormat(format);
	this->mergeCurrentCharFormat(format);
    
    }
    
    void MyQTextEdit::multiPagePrint(QPrinter *printer)
    {
	QPainter p(printer);
	QTextDocument *doc = 0;
	qreal dpiScaleX;
	qreal dpiScaleY;    
	bool abort = false;
	int firstPage = 0;
	int index = 0;
	int lastPage = 0;
	int numPages = 0;
	
	/// Check that there is a valid device to print to.
	if (!p.isActive())
	    return;
	  
	dpiScaleX = qreal(printer->logicalDpiX()) / this->logicalDpiX();
	dpiScaleY = qreal(printer->logicalDpiY()) / this->logicalDpiY();
	
	/// scale to dpi
	p.scale(dpiScaleX, dpiScaleY);
	     
	firstPage = printer->fromPage();
	if (firstPage >= this->pagesNumber)
	    return;
	if (firstPage == 0)
	    firstPage = 1;
    
	lastPage = printer->toPage();
	if (lastPage == 0 || lastPage >= this->pagesNumber)
	    lastPage = this->pagesNumber;
	numPages = lastPage - firstPage + 1;
	for (int k = 0; k < printer->numCopies(); k++) {
	    for (int j = 0; j < numPages; ++j) {
		if ( !abort ) {
		    if ( j != 0 || k != 0 )
			printer->newPage();
		    if (printer->pageOrder() == QPrinter::FirstPageFirst) {
			index = firstPage + j;
		    } else {
			index = lastPage - j;
		    }
		    if ( (doc = this->documentHash.value(index)) )
			doc->drawContents(&p);
		}
		if ( printer->printerState() == QPrinter::Aborted
		     || printer->printerState() == QPrinter::Error)
		    abort = true;
	    }
	}
    }
    
    void MyQTextEdit::pageChanged(int page)
    {
	if (documentHash.contains(page)) {
	    QTextDocument *doc = documentHash.value(page);
	    this->setDocument(doc);
	    this->connectDocument();
    
	    setWindowModified(this->document()->isModified());
	    actionUndo->setEnabled(this->document()->isUndoAvailable());
	    actionRedo->setEnabled(this->document()->isRedoAvailable());
	}
    }
    
    void MyQTextEdit::setCurrentFileName(const QString &fileName)
    {
	this->fileName = fileName;
	this->document()->setModified(false);
	QString shownName;
	if (fileName.isEmpty())
	    shownName = "untitled.txt";
	else
	    shownName = QFileInfo(fileName).fileName();
    
	setWindowTitle(tr("%1[*] - %2").arg(shownName).arg(tr("Rich Text")));
	setWindowModified(false);
    }
    
    void MyQTextEdit::setupFileActions()
    {
	QAction *a;
	a = actionSaveAs = new QAction(QIcon(":/images/Save.png"), tr("Save &As..."), this);
	connect(a, SIGNAL(triggered()), this, SLOT(fileSaveAs()));
    }
    
    void MyQTextEdit::setupEditActions()
    {
	QString clipboard;
    
	barEdit = new QToolBar();
	barEdit->setWindowTitle(tr("Edit Actions"));
    
	menuEdit = new QMenu(tr("&Edit"), this);
    
	QAction *a;
	a = actionUndo = new QAction(QIcon(":/images/Undo.png"), tr("&Undo"), this);
	a->setShortcut(QKeySequence::Undo);
	barEdit->addAction(a);
	menuEdit->addAction(a);
	a = actionRedo = new QAction(QIcon(":/images/Redo.png"), tr("&Redo"), this);
	a->setShortcut(QKeySequence::Redo);
	barEdit->addAction(a);
	menuEdit->addAction(a);
	menuEdit->addSeparator();
	barEdit->addSeparator();
	a = actionCut = new QAction(QIcon(":/images/Cut.png"), tr("Cu&t"), this);
	a->setShortcut(QKeySequence::Cut);
	barEdit->addAction(a);
	menuEdit->addAction(a);
	a = actionCopy = new QAction(QIcon(":/images/Copy.png"), tr("&Copy"), this);
	a->setShortcut(QKeySequence::Copy);
	barEdit->addAction(a);
	menuEdit->addAction(a);
	a = actionPaste = new QAction(QIcon(":/images/Paste.png"), tr("&Paste"), this);
	a->setShortcut(QKeySequence::Paste);
	barEdit->addAction(a);
	menuEdit->addAction(a);
	clipboard = QApplication::clipboard()->text();
	actionPaste->setEnabled(!clipboard.isEmpty());
	a = actionDelete = new QAction(QIcon(":/images/Delete.png"), tr("&Delete"), this);
	a->setShortcut(QKeySequence::Delete);
	menuEdit->addAction(a);
	menuEdit->addSeparator();
	a = actionSelectAll = new QAction(QIcon(":/images/SelectAll.png"), tr("Select &All"), this);
	a->setShortcut(QKeySequence::SelectAll);
	menuEdit->addAction(a);
    }
    
    void MyQTextEdit::setupTextActions()
    {
	QAction *a;
	barFormat = new QToolBar();
	barFormat->setWindowTitle(tr("Format Actions"));
	menuFormat = new QMenu(tr("F&ormat"), this);
    
	a = actionTextBold = new QAction(QIcon(":/images/Bold.png"), tr("&Bold"), this);
	a->setShortcut(Qt::CTRL + Qt::Key_B);
	QFont bold;
	bold.setBold(true);
	a->setFont(bold);
	connect(a, SIGNAL(triggered()), this, SLOT(textBold()));
	barFormat->addAction(a);
	menuFormat->addAction(a);
	a->setCheckable(true);
    
	a = actionTextItalic = new QAction(QIcon(":/images/Italic.png"), tr("&Italic"), this);
	a->setShortcut(Qt::CTRL + Qt::Key_I);
	QFont italic;
	italic.setItalic(true);
	a->setFont(italic);
	connect(a, SIGNAL(triggered()), this, SLOT(textItalic()));
	barFormat->addAction(a);
	menuFormat->addAction(a);
	a->setCheckable(true);
    
	a = actionTextUnderline = new QAction(QIcon(":/images/Underline.png"), tr("&Underline"), this);
	a->setShortcut(Qt::CTRL + Qt::Key_U);
	QFont underline;
	underline.setUnderline(true);
	a->setFont(underline);
	connect(a, SIGNAL(triggered()), this, SLOT(textUnderline()));
	barFormat->addAction(a);
    
	menuFormat->addAction(a);
	a->setCheckable(true);
    
	QActionGroup *grp = new QActionGroup(this);
	connect(grp, SIGNAL(triggered(QAction *)), this, SLOT(textAlign(QAction *)));
    
	// Make sure the alignLeft  is always left of the alignRight
	if (QApplication::isLeftToRight()) {
	    actionAlignLeft = new QAction(QIcon(":/images/Justify-Left.png"), tr("&Left"), grp);
	    actionAlignCenter = new QAction(QIcon(":/images/Justify-Center.png"), tr("C&enter"), grp);
	    actionAlignRight = new QAction(QIcon(":/images/Justify-Right.png"), tr("&Right"), grp);
	} else {
	    actionAlignRight = new QAction(QIcon(":/images/Justify-Right.png"), tr("&Right"), grp);
	    actionAlignCenter = new QAction(QIcon(":/images/Justify-Center.png"), tr("C&enter"), grp);
	    actionAlignLeft = new QAction(QIcon(":/images/Justify-Left.png"), tr("&Left"), grp);
	}
	actionAlignJustify = new QAction(QIcon(":/images/Justify-Fill.png"), tr("&Justify"), grp);
    
	actionAlignLeft->setShortcut(Qt::CTRL + Qt::Key_L);
	actionAlignLeft->setCheckable(true);
	actionAlignCenter->setShortcut(Qt::CTRL + Qt::Key_E);
	actionAlignCenter->setCheckable(true);
	actionAlignRight->setShortcut(Qt::CTRL + Qt::Key_R);
	actionAlignRight->setCheckable(true);
	actionAlignJustify->setShortcut(Qt::CTRL + Qt::Key_J);
	actionAlignJustify->setCheckable(true);
    
	barFormat->addActions(grp->actions());
	menuFormat->addActions(grp->actions());
    
	QPixmap pix(16, 16);
	pix.fill(Qt::black);
	a = actionTextColor = new QAction(pix, tr("&Color..."), this);
	connect(a, SIGNAL(triggered()), this, SLOT(textColorSlot()));
	barFormat->addAction(a);
	menuFormat->addAction(a);
    
	barText = new QToolBar();
	barText->setAllowedAreas(Qt::TopToolBarArea | Qt::BottomToolBarArea);
	barText->setWindowTitle(tr("Format Actions"));
    
	comboStyle = new QComboBox(barText);
	barText->addWidget(comboStyle);
	comboStyle->addItem(tr("Standard"));
	comboStyle->addItem(tr("Bullet List (Disc)"));
	comboStyle->addItem(tr("Bullet List (Circle)"));
	comboStyle->addItem(tr("Bullet List (Square)"));
	comboStyle->addItem(tr("Ordered List (Decimal)"));
	comboStyle->addItem(tr("Ordered List (Alpha lower)"));
	comboStyle->addItem(tr("Ordered List (Alpha upper)"));
	connect(comboStyle, SIGNAL(activated(int)),
		this, SLOT(textStyle(int)));
    
	comboFont = new QFontComboBox(barText);
	barText->addWidget(comboFont);
	connect(comboFont, SIGNAL(activated(const QString &)),
		this, SLOT(textFamily(const QString &)));
    
	comboSize = new QComboBox(barText);
	comboSize->setObjectName("comboSize");
	barText->addWidget(comboSize);
	comboSize->setEditable(true);
    
	QFontDatabase db;
	foreach(int size, db.standardSizes())
	    comboSize->addItem(QString::number(size));
    
	connect(comboSize, SIGNAL(activated(const QString &)),
		this, SLOT(textSize(const QString &)));
	comboSize->setCurrentIndex(comboSize->findText(QString::number(QApplication::font()
								       .pointSize())));
    }
    
    void MyQTextEdit::textAlign(QAction *a)
    {
	if (a == actionAlignLeft)
	    this->setAlignment(Qt::AlignLeft | Qt::AlignAbsolute);
	else if (a == actionAlignCenter)
	    this->setAlignment(Qt::AlignHCenter);
	else if (a == actionAlignRight)
	    this->setAlignment(Qt::AlignRight | Qt::AlignAbsolute);
	else if (a == actionAlignJustify)
	    this->setAlignment(Qt::AlignJustify);
    }
    
    void MyQTextEdit::textBold()
    {
	QTextCharFormat fmt;
	fmt.setFontWeight(actionTextBold->isChecked() ? QFont::Bold : QFont::Normal);
	mergeFormatOnWordOrSelection(fmt);
    }
    
    void MyQTextEdit::textColorSlot()
    {
	QColor col = QColorDialog::getColor(this->textColor(), this);
	if (!col.isValid())
	    return;
	QTextCharFormat fmt;
	fmt.setForeground(col);
	mergeFormatOnWordOrSelection(fmt);
	colorChanged(col);
    }
    
    void MyQTextEdit::textFamily(const QString &f)
    {
	QTextCharFormat fmt;
	fmt.setFontFamily(f);
	mergeFormatOnWordOrSelection(fmt);
    }
    
    void MyQTextEdit::textItalic()
    {
	QTextCharFormat fmt;
	fmt.setFontItalic(actionTextItalic->isChecked());
	mergeFormatOnWordOrSelection(fmt);
    }
    
    void MyQTextEdit::textSize(const QString &p)
    {
	qreal pointSize = p.toFloat();
	if (p.toFloat() > 0) {
	    QTextCharFormat fmt;
	    fmt.setFontPointSize(pointSize);
	    mergeFormatOnWordOrSelection(fmt);
	}
    }
    
    void MyQTextEdit::textStyle(int styleIndex)
    {
	QTextCursor cursor = this->textCursor();
    
	if (styleIndex != 0) {
	    QTextListFormat::Style style = QTextListFormat::ListDisc;
    
	    switch (styleIndex) {
		default:
		case 1:
		    style = QTextListFormat::ListDisc;
		    break;
		case 2:
		    style = QTextListFormat::ListCircle;
		    break;
		case 3:
		    style = QTextListFormat::ListSquare;
		    break;
		case 4:
		    style = QTextListFormat::ListDecimal;
		    break;
		case 5:
		    style = QTextListFormat::ListLowerAlpha;
		    break;
		case 6:
		    style = QTextListFormat::ListUpperAlpha;
		    break;
	    }
    
	    cursor.beginEditBlock();
    
	    QTextBlockFormat blockFmt = cursor.blockFormat();
    
	    QTextListFormat listFmt;
    
	    if (cursor.currentList()) {
		listFmt = cursor.currentList()->format();
	    } else {
		listFmt.setIndent(blockFmt.indent() + 1);
		blockFmt.setIndent(0);
		cursor.setBlockFormat(blockFmt);
	    }
    
	    listFmt.setStyle(style);
    
	    cursor.createList(listFmt);
    
	    cursor.endEditBlock();
	} else {
	    QTextBlockFormat bfmt;
	    bfmt.setObjectIndex(-1);
	    cursor.mergeBlockFormat(bfmt);
	}
    }
    
    void MyQTextEdit::textUnderline()
    {
	QTextCharFormat fmt;
	fmt.setFontUnderline(actionTextUnderline->isChecked());
	mergeFormatOnWordOrSelection(fmt);
    }
    
    
    void MyQTextEdit::updateWindow(bool visible)
    {
	if (!visible)
	{
	    this->barEdit->hide();
	    this->barFormat->hide();
	    this->barText->hide();
	    this->menuEdit->setEnabled(false);
	    this->menuFormat->setEnabled(false);
	}
	else
	{
	    this->barEdit->show();
	    this->barFormat->show();
	    this->barText->show();
	    this->menuEdit->setEnabled(true);
	    this->menuFormat->setEnabled(true); 
	    this->actionSaveAs->setEnabled(true);
	}
    }

}