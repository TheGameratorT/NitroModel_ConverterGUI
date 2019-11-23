#include "texturetonsbtx.h"
#include "ui_texturetonsbtx.h"

#include "textureentry.h"

#include <QTextStream>
#include <QFileDialog>

static QString exeDir;
static QVector<TextureEntry*> texEntries;

TextureToNSBTX::TextureToNSBTX(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::TextureToNSBTX)
{
    ui->setupUi(this);
    exeDir = QApplication::applicationDirPath();

    QVBoxLayout* scrollarea_layout = new QVBoxLayout();
    scrollarea_layout->setMargin(0);
    scrollarea_layout->setSpacing(0);

    TextureEntry* texEntry = new TextureEntry();
    texEntries.append(texEntry);

    scrollarea_layout->addWidget(texEntry);
    ui->textures_sa_widget->setLayout(scrollarea_layout);
}

void TextureToNSBTX::on_selectNsbtxPath_pb_clicked()
{
    static QString lastDirSelected = QDir::homePath();
    if(lastDirSelected == QDir::homePath() && ui->nsbtxPath_le->text() != "")
        lastDirSelected = ui->nsbtxPath_le->text().split('.')[0];

    QString fileName = QFileDialog::getSaveFileName(this, tr("Save NSBTX File"), lastDirSelected, tr("NSBTX File (*.nsbtx)"));
    if(fileName == "")
        return;
    lastDirSelected = fileName;
    ui->nsbtxPath_le->setText(fileName);
}

TextureToNSBTX::~TextureToNSBTX()
{
    delete ui;
}

void TextureToNSBTX::on_addTexture_pb_clicked()
{
    TextureEntry* texEntry = new TextureEntry();
    texEntries.append(texEntry);
    texEntry->SetID(texEntries.size());

    ui->textures_sa_widget->layout()->addWidget(texEntry);
}

void TextureToNSBTX::on_removeTexture_pb_clicked()
{
    TextureEntry* texEntry = texEntries.last();
    ui->textures_sa_widget->layout()->removeWidget(texEntry);

    delete texEntry;
    texEntry = nullptr;

    texEntries.removeLast();

    //If all got removed add 1 back
    if(texEntries.size() == 0)
    {
        on_addTexture_pb_clicked();
    }
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

    QApplication::processEvents(); //Force UI update
}

void TextureToNSBTX::printAppOutputToConsole(QProcess* process)
{
    QString line;
    do
    {
        line = process->readLine();
        if (line.toLower().contains("error:") || line.toLower().contains("warning:"))
        {
            printToConsole(line.trimmed());
        }
    }
    while (!line.isNull());
}

void TextureToNSBTX::WriteOBJFile(QFile* file)
{
    QTextStream stream(file);

    stream << R"(mtllib temp.mtl
o MaterialArray
v -1.000000 0.000000 1.000000
v 1.000000 0.000000 1.000000
v -1.000000 0.000000 -1.000000
v 1.000000 0.000000 -1.000000
vt 0.000000 0.000000
vt 1.000000 0.000000
vt 1.000000 1.000000
vt 0.000000 1.000000
vn 0.0000 1.0000 0.0000
)";

    for(int i = 0; i < texEntries.size(); i++)
    {
        stream << "usemtl Material" << i+1 << "\n";
        stream << "f 1/1/1 2/2/1 4/3/1 3/4/1" << "\n";
    }
}

void TextureToNSBTX::WriteMTLFile(QFile* file)
{
    QTextStream stream(file);

    QDir tempDir(exeDir + "/temp");
    for(int i = 0; i < texEntries.size(); i++)
    {
        QString tempTexPath = tempDir.path() + "/" + texEntries[i]->GetNewFileNameWithExtension();
        QFile::copy(texEntries[i]->GetPath(), tempTexPath);

        stream << "newmtl Material" << i+1 << "\n";
        stream << "map_Kd " << tempTexPath << "\n";
    }
}

void TextureToNSBTX::on_convert_pb_clicked()
{
    ui->console_tb->clear();

    QString destinationPath = ui->nsbtxPath_le->text();
    if(destinationPath == "")
    {
        printToConsole("Error: No destination NSBTX path was specified!");
        return;
    }

    // CHECK ALL PATHS -----------------------------------------------------------

    for(int i = 0; i < texEntries.size(); i++)
    {
        if(texEntries[i]->GetPath() == "")
        {
            printToConsole("Error: At least one of the texture paths specified was empty!");
            return;
        }
    }

    // TEMP CLEANUP --------------------------------------------------------------

    QDir tempDir(exeDir + "/temp");
    printToConsole("Cleaning temp folder...");
    tempDir.removeRecursively(); //Remove temp folder
    while(tempDir.exists()) {} //Wait for temp folder to be deleted
    QDir().mkdir(tempDir.path()); //Add temp folder back but empty

    // OBJ GENERATION ------------------------------------------------------------

    printToConsole("Generating temporary OBJ file...");
    QFile tempObjFile(tempDir.path() + "/temp.obj");
    if(!tempObjFile.open(QIODevice::WriteOnly | QIODevice::Text))
        return;
    WriteOBJFile(&tempObjFile);
    tempObjFile.close();

    // MTL GENERATION ------------------------------------------------------------

    printToConsole("Generating temporary MTL file...");
    QFile tempMtlFile(tempDir.path() + "/temp.mtl");
    if(!tempMtlFile.open(QIODevice::WriteOnly | QIODevice::Text))
        return;
    WriteMTLFile(&tempMtlFile);
    tempMtlFile.close();

    // IMD CONVERSION ------------------------------------------------------------

    printToConsole("Starting conversion to IMD...");

    QProcess* ass2imd = new QProcess();
    ass2imd->setProgram(exeDir + "/bin/ass2imd/AssToImd");
    ass2imd->setArguments({tempDir.path() + "/temp.obj", "-o", tempDir.path() + "/temp.imd"});
    ass2imd->start();
    if(ass2imd->state() == QProcess::NotRunning)
    {
        printToConsole("Error: AssToImd failed to start or could not be found!");
        return;
    }
    connect(ass2imd, &QProcess::readyRead, [=](){ printAppOutputToConsole(ass2imd); });
    ass2imd->waitForFinished(-1);
    delete ass2imd;
    ass2imd = nullptr;

    if(QFile::exists(tempDir.path() + "/temp.imd"))
        printToConsole("Success: IMD generated successfully!");
    else
    {
        printToConsole("Error: IMD failed to generate!");
        return;
    }

    // NSBTX CONVERSION ----------------------------------------------------------

    printToConsole("Starting conversion to NSBTX...");

    if(QFile::exists(destinationPath))
    {
        printToConsole("Deleting existing destination NSBTX...");
        QFile::remove(destinationPath);
    }
    while(QFile::exists(destinationPath)) {} //Wait for removal

    QProcess* imd2nsbmd = new QProcess();
    imd2nsbmd->setProgram(exeDir + "/bin/imd2bin/imd2bin");
    imd2nsbmd->setArguments({tempDir.path() + "/temp.imd", "-etex", "-o", destinationPath});
    imd2nsbmd->start();
    if(imd2nsbmd->state() == QProcess::NotRunning)
    {
        printToConsole("Error: imd2bin failed to start or could not be found!");
        return;
    }
    connect(imd2nsbmd, &QProcess::readyRead, [=](){ printAppOutputToConsole(imd2nsbmd); });
    imd2nsbmd->waitForFinished(-1);
    delete imd2nsbmd;
    imd2nsbmd = nullptr;

    if(QFile::exists(destinationPath))
    {
        printToConsole("Success: NSBTX generated successfully!");
    }
    else
    {
        printToConsole("Error: NSBTX failed to generate!");
        return;
    }
}
