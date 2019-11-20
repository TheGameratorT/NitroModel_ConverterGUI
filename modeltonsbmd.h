#ifndef MODELTONSBMD_H
#define MODELTONSBMD_H

#include <QMainWindow>
#include <QProcess>

namespace Ui {
class ModelToNSBMD;
}

class ModelToNSBMD : public QMainWindow
{
    Q_OBJECT

public:
    explicit ModelToNSBMD(QWidget *parent = nullptr);
    ~ModelToNSBMD();

private slots:
    void on_searchPath1_pb_clicked();

    void on_seachPath2_pb_clicked();

    void on_convert_pb_clicked();

    void on_path3_le_textChanged();

private:
    Ui::ModelToNSBMD *ui;

    void printToConsole(const QString &text);
    void printAppOutputToConsole(QProcess* process);
};

#endif // MODELTONSBMD_H
