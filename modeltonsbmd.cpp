#include "modeltonsbmd.h"
#include "ui_modeltonsbmd.h"

#include <QFileDialog>

static QString exeDir;

ModelToNSBMD::ModelToNSBMD(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::ModelToNSBMD)
{
    ui->setupUi(this);
    exeDir = QApplication::applicationDirPath();
}

ModelToNSBMD::~ModelToNSBMD()
{
    delete ui;
}

void ModelToNSBMD::on_searchPath1_pb_clicked()
{
    static QString lastDirSelected = QDir::homePath();

    QString fileName = QFileDialog::getOpenFileName(this, tr("Open Model File"), lastDirSelected, ""/*tr("Model Files (*.png *.jpg *.bmp)")*/);
    if(fileName == "")
        return;
    lastDirSelected = fileName;
    ui->path1_le->setText(fileName);
}

void ModelToNSBMD::on_seachPath2_pb_clicked()
{
    static QString lastDirSelected = QDir::homePath();
    if(lastDirSelected == QDir::homePath() && ui->path1_le->text() != "")
        lastDirSelected = ui->path1_le->text().split('.')[0];

    QString fileName = QFileDialog::getSaveFileName(this, tr("Save NSBMD File"), lastDirSelected, tr("NSBMD File (*.nsbmd)"));
    if(fileName == "")
        return;
    lastDirSelected = fileName;
    ui->path2_le->setText(fileName);
}

//CONVERSION STUFF AHEAD

void ModelToNSBMD::printToConsole(const QString &text)
{
    ui->console_tb->setTextColor(QColor(0, 255, 0));
    QString textToLower = text.toLower();

    if(textToLower.contains("error"))
        ui->console_tb->setTextColor(QColor("red"));
    else if(textToLower.contains("warning"))
        ui->console_tb->setTextColor(QColor("yellow"));

    ui->console_tb->append(text);
}

void ModelToNSBMD::printAppOutputToConsole(QProcess* process)
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

void ModelToNSBMD::on_convert_pb_clicked()
{
    ui->console_tb->clear();

    QString sourcePath = ui->path1_le->text();
    QString destinationPath = ui->path2_le->text();

    bool AtLeast1PathWasNotSpecified = false;
    if(sourcePath == "")
    {
        AtLeast1PathWasNotSpecified = true;
        printToConsole("Error: No source model file was specified!");
    }
    if(destinationPath == "")
    {
        AtLeast1PathWasNotSpecified = true;
        printToConsole("Error: No destination NSBMD path was specified!");
    }
    if(AtLeast1PathWasNotSpecified)
        return;

    bool IsSourceFileIMD = false;
    if(sourcePath.endsWith(".imd", Qt::CaseInsensitive))
        IsSourceFileIMD = true;

    QDir tempDir(exeDir + "/temp");
    if(!IsSourceFileIMD)
    {
        printToConsole("Cleaning temp folder...");

        tempDir.removeRecursively();
        while(tempDir.exists()) {} //Wait for temp folder to be deleted
        QDir().mkdir(tempDir.path());

        printToConsole("Starting conversion to IMD...");

        QProcess* ass2imd = new QProcess();
        ass2imd->setProgram(exeDir + "/bin/ass2imd/AssToImd");
        ass2imd->setArguments({sourcePath, "-o", tempDir.path() + "/temp.imd"});
        ass2imd->start();
        if(ass2imd->state() == QProcess::NotRunning)
        {
            printToConsole("Error: AssToImd failed to start or could not be found!");
            return;
        }
        connect(ass2imd, &QProcess::readyRead, [=](){ printAppOutputToConsole(ass2imd); });
        ass2imd->waitForFinished(-1);

        if(QFile::exists(tempDir.path() + "/temp.imd"))
            printToConsole("Success: IMD generated successfully!");
        else
        {
            printToConsole("Error: IMD failed to generate!");
            return;
        }
    }

    QString sourcePath2 = tempDir.path() + "/temp.imd";
    if(IsSourceFileIMD)
        sourcePath2 = sourcePath;

    printToConsole("Starting conversion to NSBMD...");

    QProcess* imd2nsbmd = new QProcess();
    imd2nsbmd->setProgram(exeDir + "/bin/imd2nsbmd/imd2nsbmd");
    imd2nsbmd->setArguments({sourcePath2, "-o", destinationPath});
    imd2nsbmd->start();
    if(imd2nsbmd->state() == QProcess::NotRunning)
    {
        printToConsole("Error: imd2nsbmd failed to start or could not be found!");
        return;
    }
    connect(imd2nsbmd, &QProcess::readyRead, [=](){ printAppOutputToConsole(imd2nsbmd); });
    imd2nsbmd->waitForFinished(-1);

    if(QFile::exists(destinationPath))
    {
        QFile file(destinationPath);
        if (!file.open(QIODevice::ReadWrite)) return;
        QByteArray fileBytes = file.readAll();

        QString name = ui->path3_le->text();
        if(name == "")
            name = "unnamed";
        //Mess crap because QByteArray replace is stupid and a fixed replace size can't be set
        QByteArray nameBytes = name.toUtf8();
        nameBytes.resize(16);
        for(int i = name.length(); i < 16; i++)
            nameBytes[i] = 0;
        //Mess end lol

        //Set NSBMD name
        fileBytes.replace(0x38, 16, nameBytes);
        file.resize(0);
        file.write(fileBytes);
        file.close();

        printToConsole("Success: NSBMD generated successfully!");
    }
    else
    {
        printToConsole("Error: NSBMD failed to generate!");
        return;
    }
}

void ModelToNSBMD::on_path3_le_textChanged()
{
    int cursorPos = ui->path3_le->cursorPosition();
    QString newText;

    for(QChar character : ui->path3_le->text())
    {
        if(!((character >= 'A' && character <= 'Z') ||
             (character >= 'a' && character <= 'z') ||
             (character >= '0' && character <= '9')))
        {
            character = '_';
        }
        newText.append(character);
    }

    ui->path3_le->setText(newText);
    ui->path3_le->setCursorPosition(cursorPos);
}
