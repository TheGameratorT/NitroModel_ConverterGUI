#include "textureentry.h"
#include "ui_textureentry.h"

#include <QFileDialog>

TextureEntry::TextureEntry(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::TextureEntry)
{
    ui->setupUi(this);
}

TextureEntry::~TextureEntry()
{
    delete ui;
}

void TextureEntry::SetID(int ID) { ui->id_le->setText(QString::number(ID)); }
QString TextureEntry::GetName() { return ui->name_le->text(); }
QString TextureEntry::GetPath() { return ui->path_le->text(); }
QFileInfo TextureEntry::GetFileInfo()
{
    QFileInfo fileInfo(ui->path_le->text());
    return fileInfo;
}
QString TextureEntry::GetNewFileNameWithExtension()
{
    if(ui->name_le->text() == "")
        ui->name_le->setText(GetFileInfo().completeBaseName());
    return ui->name_le->text() + "." + GetFileInfo().suffix();
}

void TextureEntry::on_search_pb_clicked()
{
    static QString lastDirSelected = QDir::homePath();

    QString fileName = QFileDialog::getOpenFileName(this, tr("Open Texture File"), lastDirSelected, tr("Image Files (*.png *.tga);;"
                                                                                                       "PNG Files (*.png);;"
                                                                                                       "Nitro TGA Files (*.tga)"));
    if(fileName == "")
        return;
    lastDirSelected = fileName;
    ui->path_le->setText(fileName);
    ui->name_le->setText(GetFileInfo().completeBaseName());
}

void TextureEntry::on_name_le_textChanged()
{
    QLineEdit* sender = qobject_cast<QLineEdit*>(QObject::sender());

    int cursorPos = sender->cursorPosition();
    QString newText;

    for(QChar character : sender->text())
    {
        if(!((character >= 'A' && character <= 'Z') ||
             (character >= 'a' && character <= 'z') ||
             (character >= '0' && character <= '9')))
        {
            character = '_';
        }
        newText.append(character);
    }

    sender->setText(newText);
    sender->setCursorPosition(cursorPos);
}
