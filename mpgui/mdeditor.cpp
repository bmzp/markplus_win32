#include "mdeditor.h"

MdEditor::MdEditor(QWidget *parent)
	: TextEditBase(parent)
{

}

MdEditor::~MdEditor()
{

}

void MdEditor::readSettings()
{
	QSettings settings;
	settings.beginGroup("MdEditor");
	move( settings.value("pos", pos()).toPoint() );
	resize( settings.value("size", size()).toSize());
	if( settings.value("maximzed", false).toBool() )
		setWindowState( windowState() | Qt::WindowMaximized );
	settings.endGroup();
}

void MdEditor::writeSettings()
{
	QSettings settings;
	settings.beginGroup("mdEditor");
	settings.setValue( "maximized", isMaximized() );
	if( !isMaximized() )
	{
		settings.setValue( "pos", pos() );
		settings.setValue( "size", size() );
	}
	settings.endGroup();
}

void MdEditor::hideEvent(QHideEvent *event)
{
	writeSettings();
	event->accept();
}

TextEditMetaBorder::TextEditMetaBorder( TextEditBase * editor )
	: QWidget(editor)
	, _editor(editor)
{
}

TextEditMetaBorder::~TextEditMetaBorder()
{
}

QSize TextEditMetaBorder::sizeHint() const
{
	return QSize( _editor->metaBorderWidth(), 0 );
}

void TextEditMetaBorder::paintEvent(QPaintEvent *e)
{
	_editor->metaBorderPaintEvent( e );
}

TextEditBase::TextEditBase(QWidget *parent)
    : QPlainTextEdit(parent)
    , _metaBorder( new TextEditMetaBorder(this) )
{
    connect( this, SIGNAL( blockCountChanged(int) ), this, SLOT( updateMetaBorderWidth(int) ) );
    connect( this, SIGNAL( updateRequest(QRect,int) ), this, SLOT( updateMetaBorder(QRect,int) ) );
    connect( this, SIGNAL( cursorPositionChanged() ), this, SLOT( highlightCurrentLine() ) );

    updateMetaBorderWidth(0);
    highlightCurrentLine();
}

TextEditBase::~TextEditBase()
{
}

void TextEditBase::resizeEvent(QResizeEvent *e)
{
    QPlainTextEdit::resizeEvent(e);

    QRect cr = contentsRect();
    _metaBorder->setGeometry( QRect(cr.left(), cr.top(), metaBorderWidth(), cr.height()) );
}

/**
Highlights the current edited text line.
*/
void TextEditBase::highlightCurrentLine()
{
    QList<QTextEdit::ExtraSelection> extraSelections;

    if ( !isReadOnly() )
    {
        QTextEdit::ExtraSelection selection;

        QColor lineColor = QColor(240,240,255);
        selection.format.setBackground(lineColor);
        selection.format.setProperty( QTextFormat::FullWidthSelection, true );

        selection.cursor = textCursor();
        selection.cursor.clearSelection();
        extraSelections.append(selection);
    }

    setExtraSelections( extraSelections );
}

/**
Called when the meta border widget is painted.
*/
void TextEditBase::metaBorderPaintEvent(QPaintEvent *e)
{
    QPainter painter( _metaBorder );

    QTextBlock block = firstVisibleBlock();
    int blockNumber = block.blockNumber();
    int top = (int) blockBoundingGeometry(block).translated(contentOffset()).top();
    int bottom = top + (int) blockBoundingRect(block).height();

    while (block.isValid() && top <= e->rect().bottom())
    {
        if (block.isVisible() && bottom >= e->rect().top())
        {
            QString number = QString::number(blockNumber + 1);
            painter.drawText(0, top, _metaBorder->width(), fontMetrics().height(), Qt::AlignRight, number);
        }

        block = block.next();
        top = bottom;
        bottom = top + (int) blockBoundingRect(block).height();
        ++blockNumber;
    }
}

/**
Calculates the width of the meta border.
*/
int TextEditBase::metaBorderWidth() const
{
    int digits = 2;
    int max = qMax(1, blockCount());
    while (max >= 10) {
        max /= 10;
        ++digits;
    }

    int space = fontMetrics().width(QLatin1Char('9')) * digits;

    return space;
}

void TextEditBase::updateMetaBorderWidth(int newBlockCount)
{
    Q_UNUSED( newBlockCount );

    setViewportMargins( metaBorderWidth() + 3, 0, 0, 0);
}

void TextEditBase::updateMetaBorder(const QRect & r, int dy)
{
    if (dy)
        _metaBorder->scroll(0, dy);
    else
        _metaBorder->update(0, r.y(), _metaBorder->width(), r.height());

    if ( r.contains(viewport()->rect()) )
        updateMetaBorderWidth(0);
}
