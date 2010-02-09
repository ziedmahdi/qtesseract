/**********************************************************************
 * File:        readerthread.cpp 
 * Description: The ReaderThread class provides a thread to process OCR
 * in a Image.
 * Author:      Roberto Benitez Monje
 * Created:     Mon Jan 04 2010
 **********************************************************************/
#include "include/readerthread.h"
#include <math.h>
#include <locale.h>
#include <QtDebug>


#include "tesseractclass.h" //to use unicharset

QList<Container *> list_container;

ReaderThread::ReaderThread(QObject *parent)
    : QThread(parent)
{
    dataDir = QByteArray("/usr/local/share");
    this->createBrushes();
    language = 0;
    blockCharsCount = 0;
}

void ReaderThread::read(const QByteArray &fileName,const int pageNumber,
			QTreeWidgetItem *treePage)
{
    Container *c = new Container(fileName,pageNumber,treePage);
    list_container.append(c);
}

int ReaderThread::process(Container *c)
{
    QByteArray fileName = c->fileName;
    int pageNumber = c->pageNumber;
    QTreeWidgetItem *treePage = c->treePage;
    PIX  *pix = pixRead(fileName);
    
    if (pix == NULL) {
	return -1;//
    }
    if (pix->w < kMinRectSize || pix->h < kMinRectSize){
	pixDestroy(&pix);
	return -1;//return -1;  // Nothing worth doing.
    }
    this->SetImage(pix);
    int height = pix->h;
    if ( !enhance ) {
	emit this->newState((pageNumber-1)*2,tr("Page %1: getting text...").arg(pageNumber));
	QString text = QString::fromUtf8(this->GetUTF8Text(),-1);
	pixDestroy(&pix);
	QTextDocument *doc = new QTextDocument();
	QTextCursor cursor(doc);//doc->textCursor());
	QTextCharFormat charFormat;
	charFormat.setFontPointSize(10);
	cursor.setCharFormat(charFormat);
	cursor.insertText(text);
	cursor.movePosition(QTextCursor::Start);
	doc->moveToThread(this->thread()); //this->thread, es el thread del main, donde vive this
	emit this->writedDocument(pageNumber,doc);
	emit this->newState((pageNumber-1)*2 + 1,tr("Page %1: creating tree...").arg(pageNumber));
	this->getTree(treePage,height,pageNumber);
	return 0;
    }
    emit this->newState((pageNumber-1)*1024,tr("processing page %1").arg(pageNumber));

    this->blockCharsCount = 0;
    QTextDocument *doc = new QTextDocument();
    float dpiCoeffX = this->dpiCoeffX / pix->xres;
    this->Recognize(0L);
    PAGE_RES *page_res = this->page_res_;
    if ( page_res != 0 )  {
	doc->setDocumentMargin(0);
	QString lastWord = QString();
	QString rowString; //getTree
	QTextCursor cursor(doc);//doc->textCursor());
	QTextCharFormat newCharFormat;
	int wordCount = 0;
	int rowCount = 0;
	int blockCount = 0;
	QTreeWidgetItem *lastWordItem = 0;
	QTreeWidgetItem *lastRowItem = 0;
	QTreeWidgetItem *lastBlockItem = 0;
	TBOX box;
	PAGE_RES_IT page_res_it(page_res);
	for ( page_res_it.restart_page() ; page_res_it.word () != NULL ;
	    page_res_it.forward() )
	{
	    BLOCK_RES *blockres = page_res_it.block(); //getTree
	    ROW_RES* rowres = page_res_it.row();
	    WERD_RES *wordres = page_res_it.word();

	    ROW *row = rowres->row;

	    lastWord = QString::fromUtf8(wordres->best_choice->unichar_string()
					 .string() ,-1);
	    //getTree begin
	    if (!(blockres == page_res_it.prev_block())) {
		lastBlockItem = this->insertBlock(treePage,++blockCount,blockres->block,
						  height,pageNumber);
		rowCount = 0;
		blockCharsCount += rowres->char_count;
		if (!lastWord.at(0).isLower() && page_res_it.prev_word())
		    cursor.insertText(QString(QChar::ParagraphSeparator));

		emit this->setValue((pageNumber-1)*1024 + 1024/(this->page_res_->char_count /
								this->blockCharsCount));
	    }
	    if (!(rowres == page_res_it.prev_row())) {
		if (lastRowItem)
		    lastRowItem->setText(1,rowString);
		lastRowItem = this->insertRow(lastBlockItem,++rowCount,row,height,
					      pageNumber);
		rowString.clear();
		wordCount = 0;
	    }
	    rowString.append(QString::fromUtf8(
		    wordres->best_choice->unichar_string().string(),-1));
	    rowString.append(" ");
	    lastWordItem = this->insertWord(lastRowItem,++wordCount,wordres,height,
					    pageNumber);
	    this->insertLetters(lastWordItem,wordres,height,pageNumber);
	    //End getTree

	    box = wordres->word->bounding_box();
	    box.set_top(height - box.top());
	    box.set_bottom(height - box.bottom());
	    newCharFormat = werdFormat(wordres,rowres,pix->yres);
	    if (newCharFormat.fontPointSize())
		cursor.mergeCharFormat(newCharFormat);
	    //doc->mergeCurrentCharFormat(newCharFormat);
	    QFontMetricsF metrics(newCharFormat.font());
	    if (werdIsBOL(wordres) && !lastWord.at(0).isLower() && page_res_it.prev_word())
		    cursor.insertText(QString("\n"));
	    cursor.insertText(getBlanks(cursor,wordres,dpiCoeffX)); //Insert blank spaces
	    cursor.insertText(lastWord);
	}
	cursor.movePosition(QTextCursor::Start);
	lastRowItem->setText(1,rowString);
	rowString.clear();
	doc->moveToThread(this->thread()); //this->thread == main's thread
	emit this->writedDocument(pageNumber,doc);
    }
    return 0;
}

void ReaderThread::run()
{
    setlocale (LC_ALL, "C"); // To fix problem with lib Tesseract upto 3.0
    Container *c;
    emit this->newState(0,tr("initializating tesseract."));
    
    bool done = this->SetVariable("save_best_choices","TRUE");
    if ( !done )
	this->enhance = false;
    
    int initialized = this->Init(this->dataDir.constData(), language.constData());
    if ( !initialized ) {
	this->SetPageSegMode(tesseract::PSM_AUTO);
    
	for (int i = 0; i < list_container.size(); ++i) {
	    c = list_container.at(i);
	    this->process(c);
	}
	this->End();
    }

    for (int i = 0; i < list_container.size(); ++i) {
	c = list_container.at(i);
	delete c;
    }
    list_container.clear();
    blockCharsCount = 0;
    this->quit();
}
void ReaderThread::start(bool enhance,QByteArray language, float dpiQTextEditX)
{
    this->dpiCoeffX = dpiQTextEditX;
    this->language = language;
    this->enhance = enhance;
    QThread::start();
}

void ReaderThread::createBrushes()
{
    QGradientStops stops;
    QGradient *gradient;
    this->block0Brush = QBrush(QColor(79,148,205),Qt::SolidPattern);

    gradient = new QLinearGradient(0,0,100,100);
    stops.clear();
    stops << QGradientStop(0, QColor(79,148,205));
    stops << QGradientStop(1, QColor(129,205,241));
    gradient->setStops(stops);
    this->block1Brush = QBrush(*gradient);

    gradient = new QLinearGradient(0,0,100,100);
    stops.clear();
    stops << QGradientStop(0, QColor(126,192,238));
    stops << QGradientStop(1, QColor(240,248,255));
    gradient->setStops(stops);
    this->block2Brush = QBrush(*gradient);

    this->row0Brush = QBrush(QColor(238,99,99),Qt::SolidPattern);

    gradient = new QLinearGradient(0,0,100,100);
    stops.clear();
    stops << QGradientStop(0, QColor(238,99,99));
    stops << QGradientStop(1, QColor(243,131,131));
    gradient->setStops(stops);
    this->row1Brush = QBrush(*gradient);

    gradient = new QLinearGradient(0,0,100,100);
    stops.clear();
    stops << QGradientStop(0, QColor(240,128,128));
    stops << QGradientStop(1, QColor(255,250,250,50));
    gradient->setStops(stops);
    this->row2Brush = QBrush(*gradient);

    this->word0Brush = QBrush(QColor(188,238,104),Qt::SolidPattern);

    gradient = new QLinearGradient(0,0,100,100);
    stops.clear();
    stops << QGradientStop(0, QColor(188,238,104));
    stops << QGradientStop(1, QColor(202,255,112));
    gradient->setStops(stops);
    this->word1Brush = QBrush(*gradient);

    gradient = new QLinearGradient(0,0,100,100);
    stops.clear();
    stops << QGradientStop(0, QColor(202,255,112));
    stops << QGradientStop(1, QColor(250,255,250));
    gradient->setStops(stops);
    this->word2Brush = QBrush(*gradient);

    this->letter0Brush = QBrush(QColor(192,192,192),Qt::SolidPattern);

    gradient = new QLinearGradient(0,0,100,100);
    stops.clear();
    stops << QGradientStop(0, QColor(192,192,192));
    stops << QGradientStop(1, QColor(220,220,220));
    gradient->setStops(stops);
    this->letter1Brush = QBrush(*gradient);

    gradient = new QLinearGradient(0,0,100,100);
    stops.clear();
    stops << QGradientStop(0, QColor(220,220,220));
    stops << QGradientStop(1, QColor(250,250,250));
    gradient->setStops(stops);
    this->letter2Brush = QBrush(*gradient);
}

int ReaderThread::getTree(QTreeWidgetItem *treePage,int height,int pageNumber) {
    if (this->tesseract_ != 0 && this->page_res_ != 0) {
	int wordCount = 0;
	int rowCount = 0;
	int blockCount = 0;
	QTreeWidgetItem *lastWordItem = 0;
	QTreeWidgetItem *lastRowItem = 0;
	QTreeWidgetItem *lastBlockItem = 0;
	QString rowString;
	PAGE_RES_IT page_res_it(this->page_res_);
	for (page_res_it.restart_page(); page_res_it.word () != NULL;
	    page_res_it.forward())
	{
	    BLOCK_RES *blockres = page_res_it.block();
	    ROW_RES* rowres = page_res_it.row();
	    WERD_RES *wordres = page_res_it.word();
	    BLOCK *block = blockres->block;
	    ROW *row = rowres->row;
	    if (!(blockres == page_res_it.prev_block())) {
		lastBlockItem = this->insertBlock(treePage,++blockCount,block,height,
						  pageNumber);
		rowCount = 0;
	    }
	    if (!(rowres == page_res_it.prev_row())) {
		if (lastRowItem)
		    lastRowItem->setText(1,rowString);
		lastRowItem = this->insertRow(lastBlockItem,++rowCount,row,height,
					      pageNumber);//rowString);
		rowString.clear();
		wordCount = 0;
	    }
	    QString palabra = QString::fromUtf8(wordres->best_choice->unichar_string().string(),-1);
	    rowString.append(palabra);
	    rowString.append(" ");
	    lastWordItem = this->insertWord(lastRowItem,++wordCount,wordres,height,
					    pageNumber);
	    this->insertLetters(lastWordItem,wordres,height,pageNumber);
	}
	lastRowItem->setText(1,rowString);
	return 0;
    }
    else {
	return 1; //invalid page_res
    }
}


QTreeWidgetItem *ReaderThread::insertBlock(QTreeWidgetItem *treePage,int blockCount,
					   BLOCK *block,int height,int pageNumber)
{
    QTreeWidgetItem *treeItem = new QTreeWidgetItem(treePage
						    , QTreeWidgetItem::UserType + BLOCKv);
    TBOX box = block->bounding_box();
    QString printedBlock = printBLOCK(block,height);
    treeItem->setFlags(treeItem->flags()|Qt::ItemIsUserCheckable);
    treeItem->setCheckState(0,Qt::Unchecked);
    treeItem->setBackground(0,block0Brush);
    treeItem->setBackground(1,block1Brush);
    treeItem->setBackground(2,block2Brush);
    treeItem->setToolTip(0,printedBlock);
    treeItem->setText(0, tr("block %1").arg(blockCount));
    treeItem->setText(2, printTBOX(box,height,false));
    treeItem->setData(2,Qt::UserRole,BLOCKv);
    emit this->newItem(treeItem,BLOCKv,pageNumber,box.left(),height-box.top(),
		       box.right()-box.left(),box.top()-box.bottom());
    return treeItem;
}

QTreeWidgetItem *ReaderThread::insertRow(QTreeWidgetItem *treeBlock,int rowCount,ROW *row
					 ,int height,int pageNumber) {


    TBOX box = row->bounding_box();
    QTreeWidgetItem *treeItem = new QTreeWidgetItem(treeBlock
						    , QTreeWidgetItem::UserType + ROWv);
    QString printedRow = printROW(row,height);

    treeItem->setFlags(treeItem->flags()|Qt::ItemIsUserCheckable);
    treeItem->setCheckState(0,Qt::Unchecked);
    treeItem->setToolTip(0,printedRow);
    treeItem->setBackground(0,row0Brush);
    treeItem->setBackground(1,row1Brush);
    treeItem->setBackground(2,row2Brush);
    treeItem->setText(0, tr("row %1").arg(rowCount));
    treeItem->setText(2, QString("(%1,%2)->(%3,%4)").arg(box.left()).arg(height - box.top())
		      .arg(box.right()).arg(height - box.bottom()));
    treeItem->setData(2,Qt::UserRole,ROWv);

    emit this->newItem(treeItem,ROWv,pageNumber,box.left(),height-box.top(),
		       box.right()-box.left(),box.top()-box.bottom());
    return treeItem;
}

QTreeWidgetItem *ReaderThread::insertWord(QTreeWidgetItem *treeRow, int wordCount,
					  WERD_RES *wordres, int height, int pageNumber) {

    TBOX box = wordres->word->bounding_box();
    QTreeWidgetItem *treeItem = new QTreeWidgetItem(treeRow
						    , QTreeWidgetItem::UserType + WORDv);
    QString printedWord = printWORD(wordres,height);

    treeItem->setFlags(treeItem->flags()|Qt::ItemIsUserCheckable);
    treeItem->setCheckState(0,Qt::Unchecked);
    treeItem->setBackground(0,word0Brush);
    treeItem->setBackground(1,word1Brush);
    treeItem->setBackground(2,word2Brush);
    treeItem->setToolTip(0,printedWord);
    treeItem->setText(0, tr("word %1").arg(wordCount));
    treeItem->setText(1, tr("%1: certainty %2%3")
		  .arg(QString::fromUtf8(wordres->best_choice->unichar_string().string(),-1))
		  .arg(rating_to_cost(wordres->best_choice->certainty())).arg("%"));
    treeItem->setText(2, printTBOX(box,height,false));
    treeItem->setData(2,Qt::UserRole,WORDv);

    emit this->newItem(treeItem,WORDv,pageNumber,box.left(),height-box.top(),
		       box.right()-box.left(),box.top()-box.bottom());
    return treeItem;
}

void ReaderThread::insertLetters(QTreeWidgetItem *treeWord,WERD_RES *word,int height,
				int pageNumber) {

    if (word->best_choice == NULL) {
	return;
    }
    if (word->best_choice->blob_choices() != NULL) {
	TBOX box;
	UNICHARSET unicharset;
	QTreeWidgetItem *item;
	QString letterRatings;
	QString toolTip;
	QString letterBoxString;
	const char* letter;
	int count = 1;
	BLOB_CHOICE_LIST_C_IT blob_choices_it;
	blob_choices_it.set_to_list(word->best_choice->blob_choices());
	C_BLOB_IT c_blob_it;                // cblob iterator
	c_blob_it.set_to_list(word->word->cblob_list());
	for (blob_choices_it.mark_cycle_pt(); !blob_choices_it.cycled_list();
	     blob_choices_it.forward())
	{
	    letterRatings.clear();
	    BLOB_CHOICE_IT blob_choice_it;
	    blob_choice_it.set_to_list(blob_choices_it.data());

	    for (blob_choice_it.mark_cycle_pt();!blob_choice_it.cycled_list();
		 blob_choice_it.forward()) {
		BLOB_CHOICE *blob_choice = blob_choice_it.data();
		//inT8 config = blob_choice->config(); // font of char,controll.cpp 1559
		if (blob_choice->unichar_id() != INVALID_UNICHAR_ID) {
		    letter = this->tesseract_->unicharset
			     .id_to_unichar(blob_choice->unichar_id());
		    toolTip.append(QString("%1: certainty %2, rating %3\n")
		    .arg(letter)
		    .arg(rating_to_cost(blob_choice->certainty()))
		    .arg(blob_choice->rating()));
		    letterRatings.append(QString("%1(%2%) ")
		    .arg(QString::fromUtf8(letter,-1))
		    .arg(rating_to_cost(blob_choice->certainty())));
		}
	    }
	    box = c_blob_it.data()->bounding_box();
	    letterBoxString = printTBOX(box,height,true);
	    toolTip.append(letterBoxString);
	    letterBoxString = printTBOX(box,height,false);

	    item = new QTreeWidgetItem(treeWord
						    , QTreeWidgetItem::UserType + LETTERv);
	    item->setFlags(item->flags()|Qt::ItemIsUserCheckable);
	    item->setCheckState(0,Qt::Unchecked);
	    item->setBackground(0,letter0Brush);
	    item->setBackground(1,letter1Brush);
	    item->setBackground(2,letter2Brush);
	    item->setText(0, tr("letter %1").arg(count));
	    item->setText(1,letterRatings);
	    item->setText(2, letterBoxString);
	    item->setToolTip(0,toolTip);
	    item->setData(2,Qt::UserRole,LETTERv);
	    toolTip.clear();
	    emit this->newItem(item,LETTERv,pageNumber,box.left(),height-box.top(),
					 box.right()-box.left(),box.top()-box.bottom());

	    if (!c_blob_it.cycled_list())
		c_blob_it.forward();
	    count++;
	}
    }
}

Container::Container(QByteArray fileName,int pageNumber,QTreeWidgetItem *treePage)
{
    this->fileName = fileName;
    this->pageNumber = pageNumber;
    this->treePage = treePage;
}
