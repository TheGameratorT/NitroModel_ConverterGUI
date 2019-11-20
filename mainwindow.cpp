#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "modeltonsbmd.h"
#include "texturetonsbtx.h"
#include "texturednsbmdtonsbtx.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ModelToNSBMD *ModelToNSBMD_Window = new ModelToNSBMD;
    ui->ModelToNSBMD_mdiArea->addSubWindow(ModelToNSBMD_Window, Qt::FramelessWindowHint | Qt::MSWindowsFixedSizeDialogHint);
    ModelToNSBMD_Window->showMaximized();

    /*TextureToNSBTX *TextureToNSBTX_Window = new TextureToNSBTX;
    ui->TextureToNSBTX_mdiArea->addSubWindow(TextureToNSBTX_Window, Qt::FramelessWindowHint | Qt::MSWindowsFixedSizeDialogHint);
    TextureToNSBTX_Window->showMaximized();*/

    TexturedNSBMDToNSBTX *TexturedNSBMDToNSBTX_Window = new TexturedNSBMDToNSBTX;
    ui->TexturedNSBMDToNSBTX_mdiArea->addSubWindow(TexturedNSBMDToNSBTX_Window, Qt::FramelessWindowHint | Qt::MSWindowsFixedSizeDialogHint);
    TexturedNSBMDToNSBTX_Window->showMaximized();
}

MainWindow::~MainWindow()
{
    delete ui;
}
