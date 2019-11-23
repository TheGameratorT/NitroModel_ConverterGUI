#include "texturednsbmdtonsbtx.h"
#include "ui_texturednsbmdtonsbtx.h"

#include <QFileDialog>

TexturedNSBMDToNSBTX::TexturedNSBMDToNSBTX(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::TexturedNSBMDToNSBTX)
{
    ui->setupUi(this);
}

TexturedNSBMDToNSBTX::~TexturedNSBMDToNSBTX()
{
    delete ui;
}

void TexturedNSBMDToNSBTX::on_searchPath1_pb_clicked()
{
    static QString lastDirSelected = QDir::homePath();

    QString fileName = QFileDialog::getOpenFileName(this, tr("Open NSBMD File"), lastDirSelected, tr("NSBMD File (*.nsbmd)"));
    if(fileName == "")
        return;
    lastDirSelected = fileName;
    ui->path1_le->setText(fileName);
}

void TexturedNSBMDToNSBTX::on_seachPath2_pb_clicked()
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

void TexturedNSBMDToNSBTX::printToConsole(const QString &text)
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

void TexturedNSBMDToNSBTX::on_convert_pb_clicked()
{
    ui->console_tb->clear();

    QString sourcePath = ui->path1_le->text();
    QString destinationPath = ui->path2_le->text();

    bool AtLeast1PathWasNotSpecified = false;
    if(sourcePath == "")
    {
        AtLeast1PathWasNotSpecified = true;
        printToConsole("Error: No source NSBMD file was specified!");
    }
    if(destinationPath == "")
    {
        AtLeast1PathWasNotSpecified = true;
        printToConsole("Error: No destination NSBTX path was specified!");
    }
    if(AtLeast1PathWasNotSpecified)
        return;

    QFile sourceFile(sourcePath);
    if (!sourceFile.open(QIODevice::ReadOnly))
    {
        printToConsole("Error: Could not access file for reading, check if it's being used by another program or if the program has permission to do so on that directory.");
        return;
    }
    QByteArray nsbmdData = sourceFile.readAll();
    sourceFile.close();

    printToConsole("Checking for NSBMD file magic (BMD0) match...");
    if(!(nsbmdData[0] == 'B' &&
         nsbmdData[1] == 'M' &&
         nsbmdData[2] == 'D' &&
         nsbmdData[3] == '0'))
    {
        printToConsole("Error: Invalid NSBMD file specified.\nError details: File magic didn't match.");
        return;
    }
    printToConsole("Match found, proceeding with conversion!");

    printToConsole("Searching for model (MDL0) block offset...");
    //Find block offsets
    int MLD0_blockOffset = 0;
    for (int i = 0; i < nsbmdData.size(); i++)
    {
        if(nsbmdData[i+0] == 'M' &&
           nsbmdData[i+1] == 'D' &&
           nsbmdData[i+2] == 'L' &&
           nsbmdData[i+3] == '0')
        {
            MLD0_blockOffset = i;
            break;
        }
    }

    if(MLD0_blockOffset == 0)
        printToConsole("Error: No model data to remove could be found in the NSBMD.\nError details: MDL0 block doesn't exist!");
    else
        printToConsole("Found model (MDL0) block!");

    printToConsole("Searching for texture (TEX0) block offset...");
    int TEX0_blockOffset = 0;
    for (int i = 0; i < nsbmdData.size(); i++)
    {
        if(nsbmdData[i+0] == 'T' &&
           nsbmdData[i+1] == 'E' &&
           nsbmdData[i+2] == 'X' &&
           nsbmdData[i+3] == '0')
        {
            TEX0_blockOffset = i;
            break;
        }
    }

    if(TEX0_blockOffset == 0)
        printToConsole("Error: No texture data to keep could be found in the NSBMD.\nError details: TEX0 block doesn't exist!");
    else
        printToConsole("Found texture (TEX0) block!");

    //Remove MDL0 block
    printToConsole("Removing model block (MDL0)...");
    int MLD0_blockLen = TEX0_blockOffset - MLD0_blockOffset;
    nsbmdData.remove(MLD0_blockOffset, MLD0_blockLen);

    //Update name in header
    printToConsole("Updating header file magic...");
    nsbmdData.replace(0, 4, "BTX0");

    //Update file size in header (remove MDL0 block length from it)
    printToConsole("Updating header file size...");
    quint32* nsbmdDataAsUint = reinterpret_cast<quint32*>(nsbmdData.data());
    nsbmdDataAsUint[2] -= static_cast<quint32>(MLD0_blockLen);

    //Update section count
    printToConsole("Updating header section count...");
    quint16* nsbmdDataAsUshort = reinterpret_cast<quint16*>(nsbmdData.data());
    nsbmdDataAsUshort[7] -= 1;

    //Save NSBTX file
    printToConsole("Saving...");
    QFile destinationFile(destinationPath);
    if (!destinationFile.open(QIODevice::WriteOnly))
    {
        printToConsole("Error: Could not access file for writing, check if it's being used by another program or if the program has permission to do so on that directory.");
        return;
    }
    destinationFile.write(nsbmdData);
    destinationFile.close();

    if(QFile::exists(destinationPath))
        printToConsole("Success: NSBTX converted and saved successfully!");
}
