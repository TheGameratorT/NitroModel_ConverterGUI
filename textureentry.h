#ifndef TEXTUREENTRY_H
#define TEXTUREENTRY_H

#include <QWidget>
#include <QFileInfo>

namespace Ui {
class TextureEntry;
}

class TextureEntry : public QWidget
{
    Q_OBJECT

public:
    explicit TextureEntry(QWidget *parent = nullptr);
    ~TextureEntry();

    void SetID(int ID);
    QString GetName();
    QString GetPath();
    QFileInfo GetFileInfo();
    QString GetNewFileNameWithExtension();

private slots:
    void on_search_pb_clicked();

    void on_name_le_textChanged();

private:
    Ui::TextureEntry *ui;
};

#endif // TEXTUREENTRY_H
