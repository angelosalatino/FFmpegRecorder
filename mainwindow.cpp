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

    //    this->checkDevices();
}

MainWindow::~MainWindow()
{
    delete ui;
}

//void MainWindow::checkDevices()
//{
//    QString program = "arecord";
//    QStringList arguments;
//    arguments << "-l";

//    QString string = eseguiComando(program, arguments);

//    //Parsing
//    QRegExp rx("(card)(\\s)(\\d)(:)");
//    QRegExp rx1("(device)(\\s)(\\d)(:)");

//    QStringList audioDevices;
//    int pos = 0;

//    while ((pos = rx.indexIn(string, pos)) != -1) {
//        audioDevices <<"hw:" + rx.cap(3) + ",";
//        pos += rx.matchedLength();
//    }


//    pos = 0;
//    int i = 0;

//    while ((pos = rx1.indexIn(string, pos)) != -1) {
//        QString temp = audioDevices.at(i);
//        temp.append(rx1.cap(3));
//        audioDevices.removeAt(i);
//        audioDevices.insert(i,temp);
//        pos += rx1.matchedLength();
//        i++;
//    }
//    qDebug()<<audioDevices;

//    arguments.clear();
//    program.clear();

//    program = "ls";
//    arguments << "/dev";
//    string=eseguiComando(program,arguments);
//    QRegExp rx2("(video)(\\d)");
//    pos = 0;
//    QStringList videoDevices;
//    while ((pos = rx2.indexIn(string, pos)) != -1) {
//        videoDevices << "/dev/"+rx2.cap(1) + rx2.cap(2);
//        pos += rx2.matchedLength();

//    }
//    qDebug()<<videoDevices;

//}

//QString MainWindow::eseguiComando(QString comand, QStringList arguments)
//{
//    QString string;
//    QProcess *myProcess = new QProcess();
//    myProcess->start(comand, arguments);
//    myProcess->waitForFinished(-1);
//    QByteArray result = myProcess->readAllStandardOutput();
//    myProcess->~QProcess();
//    return string.append(result);
//}

void MainWindow::processStarted()
{
    qDebug() << "processStarted()";
}


void MainWindow::on_startButton_clicked()
{
    if(this->recording)
    {
        QString save("q");
        QByteArray ex;
        ex.append(save);
        mTranscodingProcess->write(ex);
        this->ui->startButton->setText("Record");
        recording = false;
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

        QString fileName = output+"/output.avi";
        this->outputFile = fileName;
//        qDebug() << "output file check " << fileName;
//        qDebug() << "QFile::exists(fileName) = " << QFile::exists(fileName);
        if (QFile::exists(fileName)) {
            if (QMessageBox::question(this, tr("ffmpeg"),
                                      tr("There already exists a file called %1 in "
                                         "the current directory. Overwrite?").arg(fileName),
                                      QMessageBox::Yes|QMessageBox::No, QMessageBox::No)
                    == QMessageBox::No)
            {
                return;
            }
            else{
                QFile::remove(fileName);
                /*qDebug()<<"file dovrebbe essere rimosso";*/
                while(QFile::exists(fileName)) {
                    qDebug() << "output file still there";
                }
            }
        }

        arguments << "-f" << "alsa" << "-ac" << options->getNumChannels() << "-i" << options->getAudioInterface() << "-f" << "video4linux2" << "-framerate" << "30" << "-s" << options->getVideoSize() << "-i" << options->getVideoInterface() << "-acodec" << options->getAudioCodec() << "-vcodec" << options->getVideoCodec() << "-preset" << "ultrafast" << "-crf" << "0" << "-threads" << "0" << fileName;

        qDebug() << arguments;

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
    }
}

void MainWindow::endRecordingAndClose()
{
    if(this->recording)
    {

        QString save("q");
        QByteArray ex;
        ex.append(save);
        mTranscodingProcess->write(ex);
        mTranscodingProcess->waitForFinished(-1);
    }
    qDebug()<<"processEnded()";
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
