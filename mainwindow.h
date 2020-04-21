#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include <QMessageBox>
#include <QTextCodec>
#include <QSettings>
#include <QFile>
#include <QDir>
#include <QThread>

#include <OpenEXR/ImfFrameBuffer.h>
#include <OpenEXR/ImfChannelList.h>
#include <OpenEXR/ImfInputFile.h>
#include <OpenEXR/ImfOutputFile.h>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_pushButton_addSingleFile_clicked();

    void on_pushButton_addSequenceFiles_clicked();

    void on_pushButton_runDenoiser_clicked();

    void on_pushButton_stop_clicked();

    void on_pushButton_setArnoldAOV_clicked();

    void on_pushButton_setRedshiftAOV_clicked();

    void on_comboBox_compress_currentIndexChanged(int index);

    void on_pushButton_setChan_RGB_clicked();

    void on_pushButton_setChan_redgreenblue_clicked();

private:
    Ui::MainWindow *ui;

protected:
    void closeEvent(QCloseEvent *);
};
#endif // MAINWINDOW_H
