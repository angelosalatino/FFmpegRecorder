#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>
#include <QString>
#include <QProcess>
#include <QScrollBar>
#include <QMessageBox>
#include <QFileInfo>
#include <QFileDialog>


MainWindow::MainWindow(QWidget *parent) :
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle("FFmpeg Recorder");

    // Play button for output - initially disabled
    ui->playOutputButton->setEnabled(false);

    options = new Options();

    mTranscodingProcess = new QProcess(this);
    mInputPlayProcess = new QProcess(this);

    this->recording = false; //initialize the flag that considers the recording phase
    this->outputFile.clear();

    connect(mTranscodingProcess, SIGNAL(started()), this, SLOT(processStarted()));
    connect(mTranscodingProcess,SIGNAL(readyReadStandardOutput()),this,SLOT(readyReadStandardOutput()));
    connect(mTranscodingProcess, SIGNAL(finished(int)), this, SLOT(encodingFinished()));

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::processStarted()
{
    qDebug() << "processStarted()";
}

void MainWindow::processEnded()
{
    qDebug() << "processEnded()";
}

void MainWindow::stop()
{
    if(this->recording)
    {
        mTranscodingProcess->write("q");
        mTranscodingProcess->waitForFinished(-1);
        this->stopUI();
    }
}

void MainWindow::stopUI()
{
    this->ui->startButton->setText("Record");
    recording = false;
}

void MainWindow::on_startButton_clicked()
{
    if(this->recording)
    {
        this->stop();
    } else
    {

        QString program = "ffmpeg";

        QStringList arguments;

        QString output = ui->fromLineEdit->text();
        if(output.isEmpty()) {
            qDebug() << "No output";
            QMessageBox::information(this,
                                     tr("ffmpeg"),tr("Output file not specified"));
            return;
        }


        if(options->isThereAudioInterface() && options->isThereVideoInterface())
        {
            QString fileName = output+"/output.avi";
            this->outputFile = fileName;
            arguments << "-f" << "alsa" << "-ac" << options->getNumChannels() << "-i" << options->getAudioInterface() << "-f" << "video4linux2" << "-framerate" << "30" << "-s" << options->getVideoSize() << "-i" << options->getVideoInterface() << "-acodec" << options->getAudioCodec() << "-vcodec" << options->getVideoCodec() << "-preset" << "ultrafast" << "-crf" << "0" << "-threads" << "0" << fileName;
        }
        else if(options->isThereAudioInterface() && !options->isThereVideoInterface())
        {
            QString fileName = output+"/output.wav";
            this->outputFile = fileName;
            arguments << "-f" << "alsa" << "-ac" << options->getNumChannels() << "-i" << options->getAudioInterface() << "-acodec" << options->getAudioCodec() << fileName;
        }
        else if(!options->isThereAudioInterface() && options->isThereVideoInterface())
        {
            QString fileName = output+"/output.avi";
            this->outputFile = fileName;
            arguments << "-f" << "video4linux2" << "-framerate" << "30" << "-s" << options->getVideoSize() << "-i" << options->getVideoInterface() << "-vcodec" << options->getVideoCodec() << "-preset" << "ultrafast" << "-crf" << "0" << "-threads" << "0" << fileName;

        }else
        {
            QMessageBox msgBox;
            msgBox.setText("Check the options. The operation cannot be peformed.");
            msgBox.exec();
            return;
        }
        //        qDebug() << arguments;
        if (QFile::exists(this->outputFile)) {
            if (QMessageBox::question(this, tr("ffmpeg"),
                                      tr("There already exists a file called %1 in "
                                         "the current directory. Overwrite?").arg(this->outputFile),
                                      QMessageBox::Yes|QMessageBox::No, QMessageBox::No)
                    == QMessageBox::No)
            {
                return;
            }
            else{
                QFile::remove(this->outputFile);
                /*qDebug()<<"file dovrebbe essere rimosso";*/
                while(QFile::exists(this->outputFile)) {
                    qDebug() << "output file still there";
                }
            }
        }

        mTranscodingProcess->setProcessChannelMode(QProcess::MergedChannels);
        mTranscodingProcess->start(program, arguments);
        this->ui->startButton->setText("Stop");
        recording= true;
    }
}

void MainWindow::readyReadStandardOutput()
{
    mOutputString.append(mTranscodingProcess->readAllStandardOutput());
    ui->textEdit->setText(mOutputString);

    // put the slider at the bottom
    ui->textEdit->verticalScrollBar()
            ->setSliderPosition(
                ui->textEdit->verticalScrollBar()->maximum());
}

void MainWindow::encodingFinished()
{
    // Set the encoding status by checking output file's existence
    QString fileName = this->outputFile;

    if (QFile::exists(fileName)) {
        ui->transcodingStatusLabel
                ->setText("Recording Status: Successful!");
        ui->playOutputButton->setEnabled(true);
    }
    else {
        ui->transcodingStatusLabel
                ->setText("Recording Status: Failed!");
        this->stopUI();

    }
    this->processEnded();
}

void MainWindow::endRecordingAndClose()
{
    this->stop();
    this->close();
}

void MainWindow::on_fileOpenButton_clicked()
{
    QString fileName =
            QFileDialog::getExistingDirectory(this, tr("Choose Or Create Directory"),
                                              "/home",
                                              QFileDialog::DontResolveSymlinks | QFileDialog::ReadOnly);

    if (!fileName.isEmpty()) {
        ui->fromLineEdit->setText(fileName);
    }
}

void MainWindow::on_playOutputButton_clicked()
{
    QString program = "ffplay";
    QStringList arguments;
    QString output = this->outputFile;
    arguments << output;
    mInputPlayProcess->start(program, arguments);
}

void MainWindow::on_actionClose_triggered()
{
    this->endRecordingAndClose();
}

void MainWindow::on_closeButton_clicked()
{
    this->endRecordingAndClose();
}

void MainWindow::on_optionButton_clicked()
{
    this->options->show();
}
