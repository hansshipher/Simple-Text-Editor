#ifndef TEXTEDITOR_H
#define TEXTEDITOR_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui {
class TextEditor;
}
QT_END_NAMESPACE

class TextEditor : public QMainWindow
{
    Q_OBJECT

public:
    TextEditor(const QString &fileName = QString(), QWidget *parent = 0);
    ~TextEditor();

private slots:
    void on_actionNew_triggered();

    void on_actionClose_triggered();

    void on_actionExit_triggered();

    void documentModified();

    void on_actionSelect_Font_triggered();

    void about();

    void on_actionCut_triggered();

    void on_actionCopy_triggered();

    void on_actionPaste_triggered();

    void on_actionUndo_triggered();

    void on_actionRedo_triggered();

    void on_actionOpen_triggered();

    bool saveFile();

    bool saveFileAs();

private:
    Ui::TextEditor *ui;
    QString m_fileName;
    void loadFile(const QString &fileName);
    void setFileName(const QString &);

protected:
    void closeEvent(QCloseEvent *e);
};
#endif // TEXTEDITOR_H
