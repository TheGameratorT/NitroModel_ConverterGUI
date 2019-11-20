#include "texturetonsbtx.h"
#include "ui_texturetonsbtx.h"

#include <QFileDialog>

TextureToNSBTX::TextureToNSBTX(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::TextureToNSBTX)
{
    ui->setupUi(this);
}

TextureToNSBTX::~TextureToNSBTX()
{
    delete ui;
}

void TextureToNSBTX::on_searchPath1_pb_clicked()
{
    static QString lastDirSelected = QDir::homePath();

    QString fileName = QFileDialog::getOpenFileName(this, tr("Open Texture File"), lastDirSelected, ""/*tr("Model Files (*.png *.jpg *.bmp)")*/);
    if(fileName == "")
        return;
    lastDirSelected = fileName;
    ui->path1_le->setText(fileName);
}

void TextureToNSBTX::on_seachPath2_pb_clicked()
{
    static QString lastDirSelected = QDir::homePath();
    if(lastDirSelected == QDir::homePath() && ui->path1_le->text() != "")
        lastDirSelected = ui->path1_le->text().split('.')[0];

    QString fileName = QFileDialog::getSaveFileName(this, tr("Save NSBTX File"), lastDirSelected, tr("NSBTX File (*.nsbtx)"));
    if(fileName == "")
        return;
    lastDirSelected = fileName;
    ui->path2_le->setText(fileName);
}

//CONVERSION STUFF AHEAD

void TextureToNSBTX::printToConsole(const QString &text)
{
    ui->console_tb->setTextColor(QColor(0, 255, 0));
    QString textToLower = text.toLower();

    if(textToLower.contains("error"))
        ui->console_tb->setTextColor(QColor("red"));
    else if(textToLower.contains("warning"))
        ui->console_tb->setTextColor(QColor("yellow"));

    ui->console_tb->append(text);
}

void TextureToNSBTX::printAppOutputToConsole(QProcess* process)
{
    QString line;
    do
    {
        line = process->readLine();
        if (line.toLower().contains("error:") || line.toLower().contains("warning:")) {
            if(!(line.endsWith('.') or line.endsWith('!'))) {
                printToConsole(line.trimmed() + '!');
            }
            else {
                printToConsole(line.trimmed());
            }
        }
    }
    while (!line.isNull());
}

void TextureToNSBTX::on_convert_pb_clicked()
{
    ui->console_tb->clear();


}
