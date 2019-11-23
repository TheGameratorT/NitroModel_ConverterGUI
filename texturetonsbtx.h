#ifndef TEXTURETONSBTX_H
#define TEXTURETONSBTX_H

#include <QMainWindow>
#include <QProcess>
#include <QFile>

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
    void on_addTexture_pb_clicked();

    void on_removeTexture_pb_clicked();

    void on_selectNsbtxPath_pb_clicked();

    void on_convert_pb_clicked();

private:
    Ui::TextureToNSBTX *ui;

    void printToConsole(const QString &text);
    void printAppOutputToConsole(QProcess* process);

    void WriteOBJFile(QFile* file);
    void WriteMTLFile(QFile* file);
};

#endif // TEXTURETONSBTX_H
