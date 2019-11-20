#ifndef TEXTUREDNSBMDTONSBTX_H
#define TEXTUREDNSBMDTONSBTX_H

#include <QMainWindow>

namespace Ui {
class TexturedNSBMDToNSBTX;
}

class TexturedNSBMDToNSBTX : public QMainWindow
{
    Q_OBJECT

public:
    explicit TexturedNSBMDToNSBTX(QWidget *parent = nullptr);
    ~TexturedNSBMDToNSBTX();

private slots:
    void on_searchPath1_pb_clicked();

    void on_seachPath2_pb_clicked();

    void on_convert_pb_clicked();

private:
    Ui::TexturedNSBMDToNSBTX *ui;

    void printToConsole(const QString &text);
};

#endif // TEXTUREDNSBMDTONSBTX_H
