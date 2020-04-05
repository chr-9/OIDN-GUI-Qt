#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "imagebuffer.h"

#include "sequenceParser/Sequence.hpp"
#include "sequenceParser/filesystem.hpp"

#include <OpenImageDenoise/oidn.hpp>

using namespace std;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //QSettings settings("oidn.ini", QSettings::IniFormat);
    //settings.setIniCodec(QTextCodec::codecForName("UTF-8"));

    ui->pushButton_runDenoiser->setEnabled(false);
    ui->pushButton_stop->setVisible(false);
    ui->pushButton_stop->setEnabled(false);
}

MainWindow::~MainWindow()
{
    delete ui;
}


QString path_input = "";
QString path_output = "";
Imf::Compression compression = Imf::PXR24_COMPRESSION;

QString layername_albedo = "albedo";
QString layername_normal = "N";
bool useaovs = true;
bool hdr = true;

bool isSequence = false;
int minFrame = 0;
int maxFrame = 0;
int totalFrame = 0;

bool denoiserRunning = false;
bool stopProcessing = false;
bool seqRunning = false;

sequenceParser::Sequence seqInput;
std::string parentDir;


///////////////////////////////
/// OIDN Functions
///

Imf::FrameBuffer frameBufferEXR(const ImageBuffer& image)
{
    Imf::FrameBuffer frameBuffer;
    int bytePixelStride = image.getChannels()*sizeof(float);
    int byteRowStride = image.getWidth()*bytePixelStride;
    frameBuffer.insert("R", Imf::Slice(Imf::FLOAT, (char*)&image[0], bytePixelStride, byteRowStride, 1, 1, 0.0, false, false));
    frameBuffer.insert("G", Imf::Slice(Imf::FLOAT, (char*)&image[1], bytePixelStride, byteRowStride));
    frameBuffer.insert("B", Imf::Slice(Imf::FLOAT, (char*)&image[2], bytePixelStride, byteRowStride));
    return frameBuffer;
}

Imf::FrameBuffer frameBufferAOV(const ImageBuffer& image, const string& aovname)
{
    Imf::FrameBuffer frameBuffer;
    int bytePixelStride = image.getChannels()*sizeof(float);
    int byteRowStride = image.getWidth()*bytePixelStride;
    frameBuffer.insert(aovname+".R", Imf::Slice(Imf::FLOAT, (char*)&image[0], bytePixelStride, byteRowStride, 1, 1, 0.0, false, false));
    frameBuffer.insert(aovname+".G", Imf::Slice(Imf::FLOAT, (char*)&image[1], bytePixelStride, byteRowStride));
    frameBuffer.insert(aovname+".B", Imf::Slice(Imf::FLOAT, (char*)&image[2], bytePixelStride, byteRowStride));
    return frameBuffer;
}

ImageBuffer loadEXR(const string& filename)
{
    Imf::InputFile inputFile(filename.c_str());
    if (!inputFile.header().channels().findChannel("R") ||
            !inputFile.header().channels().findChannel("G") ||
            !inputFile.header().channels().findChannel("B"))
        throw invalid_argument("image must have 3 channels");
    Imath::Box2i dataWindow = inputFile.header().dataWindow();
    ImageBuffer image(dataWindow.max.x-dataWindow.min.x+1, dataWindow.max.y-dataWindow.min.y+1, 3);
    inputFile.setFrameBuffer(frameBufferEXR(image));
    inputFile.readPixels(dataWindow.min.y, dataWindow.max.y);
    return image;
}

ImageBuffer loadAOV(const string& filename, const string& aovname)
{
    Imf::InputFile inputFile(filename.c_str());
    if (!inputFile.header().channels().findChannel(aovname + ".R") ||
            !inputFile.header().channels().findChannel(aovname + ".G") ||
            !inputFile.header().channels().findChannel(aovname + ".B"))
        throw invalid_argument("image must have 3 channels");
    Imath::Box2i dataWindow = inputFile.header().dataWindow();
    ImageBuffer image(dataWindow.max.x-dataWindow.min.x+1, dataWindow.max.y-dataWindow.min.y+1, 3);
    inputFile.setFrameBuffer(frameBufferAOV(image, aovname));
    inputFile.readPixels(dataWindow.min.y, dataWindow.max.y);
    return image;
}

void saveEXR(const string& filename, const ImageBuffer& image)
{
    if (image.getChannels() != 3)
        throw invalid_argument("image must have 3 channels");

    Imf::Header header(image.getWidth(), image.getHeight(), 1, Imath::V2f(0, 0), image.getWidth(), Imf::INCREASING_Y, compression);
    header.channels().insert("R", Imf::Channel(Imf::FLOAT));
    header.channels().insert("G", Imf::Channel(Imf::FLOAT));
    header.channels().insert("B", Imf::Channel(Imf::FLOAT));
    Imf::OutputFile outputFile(filename.c_str(), header);
    outputFile.setFrameBuffer(frameBufferEXR(image));
    outputFile.writePixels(image.getHeight());
}

void errorCallback(void* userPtr, oidn::Error error, const char* message)
{
    throw runtime_error(message);
}

bool progressCallback(void* userPtr, double n)
{
  int progress = int(n * 100.);
  return true;
}

void denoise(string filename, string outputFilename, int maxmem)
{
    ImageBuffer color, albedo, normal;
    ImageBuffer ref;

    /*
    Imf::InputFile inputFile(filename.c_str());
    Imf::ChannelList channels = inputFile.header().channels();
    set<string> layerNames;
    channels.layers(layerNames);
    for(string layerName : layerNames){
        cout << "Layer: " << layerName << endl;
    }*/

    color = loadEXR(filename);
    if (color.getChannels() != 3)
        throw runtime_error("invalid color image");

    if (useaovs)
    {
        albedo = loadAOV(filename, layername_albedo.toStdString());
        if (albedo.getChannels() != 3 || albedo.getSize() != color.getSize())
            throw runtime_error("invalid albedo image");

        normal = loadAOV(filename, layername_normal.toStdString());
        if (normal.getChannels() != 3 || normal.getSize() != color.getSize())
            throw runtime_error("invalid normal image");
    }

    int width  = color.getWidth();
    int height = color.getHeight();

    ImageBuffer output(width, height, 3);

    cout << "Initializing" << endl;
    oidn::DeviceRef device = oidn::newDevice();
    const char* errorMessage;
    if (device.getError(errorMessage) != oidn::Error::None)
        throw runtime_error(errorMessage);
    device.setErrorFunction(errorCallback);
    device.set("setAffinity", true);
    device.commit();

    // Set RT(Raytracing) Filter
    oidn::FilterRef filter = device.newFilter("RT");

    filter.setImage("color", color.getData(), oidn::Format::Float3, width, height);
    if (albedo)
        filter.setImage("albedo", albedo.getData(), oidn::Format::Float3, width, height);
    if (normal)
        filter.setImage("normal", normal.getData(), oidn::Format::Float3, width, height);
    filter.setImage("output", output.getData(), oidn::Format::Float3, width, height);
    filter.set("hdr", hdr);
    filter.set("srgb", false);
    filter.set("maxMemoryMB", maxmem);
    filter.commit();

    cout << "Execute denoiser" << endl;
    filter.execute();

    cout << "Saving output" << endl;
    saveEXR(outputFilename, output);
}

///////////////////////////////
/// UI Functions
///

void MainWindow::on_pushButton_addSingleFile_clicked()
{
    QString selFilter = tr("OpenEXR(*.exr)");
    QString fileName = QFileDialog::getOpenFileName(
                this,
                tr("Open"),
                "",
                tr("OpenEXR(*.exr)"),
                &selFilter,
                QFileDialog::DontUseCustomDirectoryIcons
                );

    if (fileName.isEmpty()) {
        return;
    }

    QFileInfo fi(fileName);
    minFrame = 0;
    maxFrame = 0;
    totalFrame = 1;

    isSequence = false;

    path_input = fileName;
    path_output = path_input; path_output.replace(".exr", ".denoised.exr");

    ui->label_inputPath->setText("Input: <b>"+path_input);
    ui->lineEdit_outputPath->setText(path_output);
    ui->label_animStart->setText("Animation Start: -");
    ui->label_animEnd->setText("End: -");
    ui->label_animTotal->setText("Total: 1");

    ui->pushButton_runDenoiser->setEnabled(true);
}

void MainWindow::on_pushButton_addSequenceFiles_clicked()
{
    QString selFilter = tr("OpenEXR(*.exr)");
    QString fileName = QFileDialog::getOpenFileName(
                this,
                tr("Open"),
                "",
                tr("OpenEXR(*.exr)"),
                &selFilter,
                QFileDialog::DontUseCustomDirectoryIcons
                );

    if (fileName.isEmpty()) {
        return;
    }

    QFileInfo fi(fileName);
    QDir d = fi.absoluteDir();
    parentDir = d.absolutePath().toStdString() + "/";

    sequenceParser::Item item;

    vector<string> filters = { "*.exr" };
    vector<sequenceParser::Item> items = sequenceParser::browse(parentDir, sequenceParser::eDetectionDefault, filters);
    if( !items.empty() ) {
        item = items.front();
    }else{
        return;
    }

    seqInput = item.getSequence();

    minFrame = seqInput.getFirstTime();
    maxFrame = seqInput.getLastTime();
    totalFrame = (maxFrame-minFrame)+1;

    isSequence = true;

    path_input = QString::fromStdString(item.getAbsoluteFilepath());
    path_output = path_input; path_output.replace(".exr", ".denoised.exr");

    ui->label_inputPath->setText("Input: <b>"+path_input);
    ui->lineEdit_outputPath->setText(path_output);
    ui->label_animStart->setText("Animation Start: "+QString::number(minFrame));
    ui->label_animEnd->setText("End: " + QString::number(maxFrame));
    ui->label_animTotal->setText("Total: " + QString::number(totalFrame));

    ui->pushButton_runDenoiser->setEnabled(true);
}

void MainWindow::on_pushButton_runDenoiser_clicked()
{
    path_output = ui->lineEdit_outputPath->text();

    if (path_input == "")
    {
        QMessageBox::critical(this, tr("Error"), tr("Input file path is empty"));
        return;
    }
    if (path_output == "")
    {
        QMessageBox::critical(this, tr("Error"), tr("Output file path is empty"));
        return;
    }

    ui->progressBar_total->setValue(0);

    if (isSequence)
    {
        int processedFrame=0;
        QDir().mkdir(QString::fromStdString(parentDir+"denoised"));

        ui->pushButton_stop->setVisible(true);
        ui->pushButton_stop->setEnabled(true);
        ui->pushButton_runDenoiser->setEnabled(false);
        ui->pushButton_addSingleFile->setEnabled(false);
        ui->pushButton_addSequenceFiles->setEnabled(false);
        ui->pushButton_setArnoldAOV->setEnabled(false);
        ui->pushButton_setRedshiftAOV->setEnabled(false);

        for(string filename : seqInput.getFiles()){
            if(stopProcessing){
                ui->label_stats->setText("");
                ui->progressBar_total->setValue(0);
                break;
            }
            ui->label_stats->setText("Processing " + QString::fromStdString(filename));

            path_output = QString::fromStdString(parentDir+"denoised/"+filename); path_output.replace(".exr", ".denoised.exr");
            denoise(parentDir+filename, path_output.toStdString(), ui->spinBox_maxmem->value());

            processedFrame++;
            ui->progressBar_total->setValue((processedFrame/float(totalFrame))*100);
            qApp->processEvents();
        }

        if(stopProcessing){
            stopProcessing = false;
        }

        ui->pushButton_stop->setVisible(false);
        ui->pushButton_stop->setEnabled(false);
        ui->pushButton_runDenoiser->setEnabled(true);
        ui->pushButton_addSingleFile->setEnabled(true);
        ui->pushButton_addSequenceFiles->setEnabled(true);
        ui->pushButton_setArnoldAOV->setEnabled(true);
        ui->pushButton_setRedshiftAOV->setEnabled(true);
    }
    else
    {
        ui->pushButton_runDenoiser->setEnabled(false);
        ui->pushButton_addSingleFile->setEnabled(false);
        ui->pushButton_addSequenceFiles->setEnabled(false);
        ui->pushButton_setArnoldAOV->setEnabled(false);
        ui->pushButton_setRedshiftAOV->setEnabled(false);

        denoise(path_input.toStdString(), path_output.toStdString(), ui->spinBox_maxmem->value());
        ui->progressBar_total->setValue(100);

        ui->pushButton_runDenoiser->setEnabled(true);
        ui->pushButton_addSingleFile->setEnabled(true);
        ui->pushButton_addSequenceFiles->setEnabled(true);
        ui->pushButton_setArnoldAOV->setEnabled(true);
        ui->pushButton_setRedshiftAOV->setEnabled(true);
    }

}


///////////////////////////////
/// Other Functions
///

void MainWindow::on_pushButton_setArnoldAOV_clicked()
{
    layername_albedo = "albedo";
    layername_normal = "N";

    ui->lineEdit_layernameAlbedo->setText(layername_albedo);
    ui->lineEdit_layernameNormal->setText(layername_normal);
}

void MainWindow::on_pushButton_setRedshiftAOV_clicked()
{
    layername_albedo = "diffusefilter";
    layername_normal = "n";

    ui->lineEdit_layernameAlbedo->setText(layername_albedo);
    ui->lineEdit_layernameNormal->setText(layername_normal);
}

void MainWindow::on_checkBox_useaovs_stateChanged(int arg1)
{
    useaovs = ui->checkBox_useaovs->checkState();
}

void MainWindow::on_checkBox_hdr_stateChanged(int arg1)
{
    hdr = ui->checkBox_hdr->checkState();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    stopProcessing = true;
}

void MainWindow::on_pushButton_stop_clicked()
{
    stopProcessing = true;
    ui->pushButton_stop->setVisible(false);
    ui->pushButton_stop->setEnabled(false);
}

void MainWindow::on_comboBox_compress_currentIndexChanged(int index)
{
    switch (index) {
    case 0:
        compression = Imf::PXR24_COMPRESSION;
        break;
    case 1:
        compression = Imf::ZIP_COMPRESSION;
        break;
    case 2:
        compression = Imf::ZIPS_COMPRESSION;
        break;
    case 3:
        compression = Imf::PIZ_COMPRESSION;
        break;
    case 4:
        compression = Imf::RLE_COMPRESSION;
        break;
    default:
        break;
    }
}
