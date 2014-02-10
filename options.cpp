#include "options.h"
#include "ui_options.h"
#include <QProcess>
#include <QDebug>

Options::Options(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Options)
{
    ui->setupUi(this);
    this->setWindowTitle("Options");
    this->checkDevices();
}

Options::~Options()
{
    delete ui;
}

void Options::checkDevices()
{
    QString program = "arecord";
    QStringList arguments;
    arguments << "-l";

    QString string = executeCommand(program, arguments);

    //Parsing
    QRegExp rx("(card)(\\s)(\\d)(:)");
    QRegExp rx1("(device)(\\s)(\\d)(:)");

    QStringList audioDevices;
    int pos = 0;

    while ((pos = rx.indexIn(string, pos)) != -1) {
        audioDevices <<"hw:" + rx.cap(3) + ",";
        pos += rx.matchedLength();
    }


    pos = 0;
    int i = 0;

    while ((pos = rx1.indexIn(string, pos)) != -1) {
        QString temp = audioDevices.at(i);
        temp.append(rx1.cap(3));
        audioDevices.removeAt(i);
        audioDevices.insert(i,temp);
        pos += rx1.matchedLength();
        i++;
    }
    //qDebug()<<audioDevices;

    arguments.clear();
    program.clear();

    program = "ls";
    arguments << "/dev";
    string=executeCommand(program,arguments);
    QRegExp rx2("(video)(\\d)");
    pos = 0;
    QStringList videoDevices;
    while ((pos = rx2.indexIn(string, pos)) != -1) {
        videoDevices << "/dev/"+rx2.cap(1) + rx2.cap(2);
        pos += rx2.matchedLength();

    }
   // qDebug()<<videoDevices;
    this->setUP(videoDevices,audioDevices);
}

QString Options::executeCommand(QString comand, QStringList arguments)
{
    QString string;
    QProcess *myProcess = new QProcess();
    myProcess->start(comand, arguments);
    myProcess->waitForFinished(-1);
    QByteArray result = myProcess->readAllStandardOutput();
    myProcess->~QProcess();
    return string.append(result);
}

void Options::setUP(QStringList videoInterfaces, QStringList audioInterfaces)
{
    //video Comboboxes

    //audio Comboboxes

    //set default parameters

    this->setVideoInterface("/dev/video0");
    this->setVideoSize("1024x768");
    this->setVideoCodec("libx264");

    this->setAudioInterface("hw:0,0");
    this->setNumChannels("2");
    this->setAudioCodec("pcm_s16le");
}

void Options::on_saveButton_clicked()
{
    this->close();
}










////////////////////////////////////////////////////////////////////////////////////
/// SETTERS AND GETTERS
///
///

QString Options::getVideoSize() const
{
    return videoSize;
}

void Options::setVideoSize(const QString &value)
{
    videoSize = value;
}

QString Options::getNumChannels() const
{
    return numChannels;
}

void Options::setNumChannels(const QString &value)
{
    numChannels = value;
}

QString Options::getAudioCodec() const
{
    return audioCodec;
}

void Options::setAudioCodec(const QString &value)
{
    audioCodec = value;
}

QString Options::getVideoCodec() const
{
    return videoCodec;
}

void Options::setVideoCodec(const QString &value)
{
    videoCodec = value;
}

QString Options::getAudioInterface() const
{
    return audioInterface;
}

void Options::setAudioInterface(const QString &value)
{
    audioInterface = value;
}

QString Options::getVideoInterface() const
{
    return videoInterface;
}

void Options::setVideoInterface(const QString &value)
{
    videoInterface = value;
}


