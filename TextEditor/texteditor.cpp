#include "texteditor.h"
#include <QTextEdit>
#include <QMessageBox>
#include <QCloseEvent>
#include <QFontDialog>
#include <QSettings>
#include <QFile>
#include <QFileInfo>
#include <QFileDialog>
#include <QDir>
#include "./ui_texteditor.h"

TextEditor::TextEditor(const QString &fileName, QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::TextEditor), m_fileName(fileName)
{
    ui->setupUi(this);
    setCentralWidget(ui->textEdit);

    ui->actionCut->setEnabled(ui->textEdit->textCursor().hasSelection());
    ui->actionCopy->setEnabled(ui->textEdit->textCursor().hasSelection());
    ui->actionUndo->setEnabled(ui->textEdit->document()->isUndoAvailable());
    ui->actionRedo->setEnabled(ui->textEdit->document()->isRedoAvailable());

    connect(ui->textEdit, &QTextEdit::textChanged, this, &TextEditor::documentModified);
    setWindowTitle("File[*]");

    QSettings settings;
    QFont savedFont = settings.value("viewFont", ui->textEdit->font()).value<QFont>();
    ui->textEdit->setFont(savedFont);

    connect(ui->actionAboutQt, &QAction::triggered, qApp, &QApplication::aboutQt);
    connect(ui->actionAbout, &QAction::triggered,this, &TextEditor::about);

    loadFile(fileName);

    connect(ui->actionSave, &QAction::triggered, this, &TextEditor::saveFile);
    connect(ui->actionSave_as, &QAction::triggered, this, &TextEditor::saveFileAs);
}

TextEditor::~TextEditor()
{
    delete ui;
}

void TextEditor::on_actionNew_triggered()
{
    TextEditor *newWnd = new TextEditor();
    QRect geom = this->geometry();
    newWnd->setGeometry(geom.x() + 30, geom.y() + 30, geom.width(), geom.height());
    newWnd->show();
}


void TextEditor::on_actionClose_triggered()
{
    this->close();
}


void TextEditor::on_actionExit_triggered()
{
    QApplication::quit();
}

void TextEditor::documentModified()
{
    setWindowModified(true);
}

void TextEditor::closeEvent(QCloseEvent *e)
{
    if (isWindowModified()) {
        auto reply = QMessageBox::warning(
            this,
            "Document modified",
            "The document has been modified.\nDo you want to save your changes?",
            QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel
            );

        if (reply == QMessageBox::Yes) {
            if (saveFile()) {
                e->accept();
            } else {
                e->ignore();
            }
        }
        else if (reply == QMessageBox::No) {
            e->accept();
        }
        else {
            e->ignore();
        }
    }
    else {
        e->accept();
    }
}

void TextEditor::on_actionSelect_Font_triggered()
{
    bool ok;
    QFont initFont = ui->textEdit->font();
    QFont font = QFontDialog::getFont(&ok,initFont,this);
    if(ok){
        ui->textEdit->setFont(font);
        QSettings settings;
        settings.setValue("viewFont", font);
    }
}

void TextEditor::about()
{
    QMessageBox::about(this, "About Programm",
                       "TextEditor v0.0.1 pre-pre-pre-pre-pre-pre-... ok it's will never be released\n @ 2025 \thans_shipher");
}


void TextEditor::on_actionCut_triggered()
{
    ui->textEdit->cut();
}


void TextEditor::on_actionCopy_triggered()
{
    ui->textEdit->copy();
}


void TextEditor::on_actionPaste_triggered()
{
    ui->textEdit->paste();
}


void TextEditor::on_actionUndo_triggered()
{
    ui->textEdit->undo();
}


void TextEditor::on_actionRedo_triggered()
{
    ui->textEdit->redo();
}

void TextEditor::loadFile(const QString &fileName)
{
    if(fileName.isEmpty()) {
        setFileName(QString());
        return;
    }

    QFile file(fileName);
    if(file.open(QIODevice::ReadOnly | QIODevice::Text)){
        QTextStream in(&file);
        ui->textEdit->setPlainText(in.readAll());
        file.close();
        setFileName(fileName);
        setWindowModified(false);
    }
    else{
        QMessageBox::warning(this, "Open File", "Cannot open file: " + fileName);
        setFileName(QString());
    }
}

void TextEditor::setFileName(const QString &fileName)
{
    m_fileName = fileName;

    QString title = QString("%1[*] - %2")
                        .arg(m_fileName.isNull() ? "untitled" : QFileInfo(m_fileName).fileName())
                        .arg(QApplication::applicationName());

    setWindowTitle(title);
}

void TextEditor::on_actionOpen_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(
        this,
        "Open document",
        QDir::currentPath(),
        "Text documents (*.txt)"
        );

    if(fileName.isNull()){
        return;
    }
    if(m_fileName.isNull() && !isWindowModified()){
        loadFile(fileName);
    }
    else {
        TextEditor *newWnd = new TextEditor(fileName);
        QRect geom = this->geometry();
        newWnd->setGeometry(geom.x() + 30, geom.y() + 30, geom.width(), geom.height());
        newWnd->show();
    }
}

bool TextEditor::saveFileAs()
{
    QString fileName = QFileDialog::getSaveFileName(
        this,
        "Save document",
        m_fileName.isNull() ? QDir::currentPath() : m_fileName,
        "Text documents (*.txt)"
        );

    if (fileName.isNull())
        return false;

    setFileName(fileName);
    return saveFile();
}

bool TextEditor::saveFile()
{
    if (m_fileName.isNull())
        return saveFileAs();

    QFile file(m_fileName);

    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::warning(this,
                             "Save Error",
                             "Cannot save file: " + m_fileName);
        setFileName(QString());
        return false;
    }

    QTextStream out(&file);
    out << ui->textEdit->toPlainText();
    file.close();
    setWindowModified(false);

    return true;
}
