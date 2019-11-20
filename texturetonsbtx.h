#ifndef TEXTURETONSBTX_H
#define TEXTURETONSBTX_H

#include <QMainWindow>
#include <QProcess>

namespace Ui {
class TextureToNSBTX;
}

class TextureToNSBTX : public QMainWindow
{
    Q_OBJECT

public:
    explicit TextureToNSBTX(QWidget *parent = nullptr);
    ~TextureToNSBTX();

private slots:
    void on_searchPath1_pb_clicked();

    void on_seachPath2_pb_clicked();

    void on_convert_pb_clicked();

private:
    Ui::TextureToNSBTX *ui;

    void printToConsole(const QString &text);
    void printAppOutputToConsole(QProcess* process);
};

#endif // TEXTURETONSBTX_H
