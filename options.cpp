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
    this->ui->videoBox->insertItem(-1,"Not use Video");
    this->ui->videoBox->addItems(videoInterfaces);
    this->ui->videoBox->setCurrentIndex(1);

    this->ui->sizeBox->addItems(QStringList()<<"1024x768");
    this->ui->sizeBox->setCurrentIndex(0);

    this->ui->vCodecBox->addItems(QStringList()<<"libx264");
    this->ui->vCodecBox->setCurrentIndex(0);

    //audio Comboboxes
    this->ui->audioBox->insertItem(-1,"Not use Audio");
    this->ui->audioBox->addItems(audioInterfaces);
    this->ui->audioBox->setCurrentIndex(1);

    this->ui->channelsBox->addItems(QStringList()<<"1"<<"2");
    this->ui->channelsBox->setCurrentIndex(0);

    this->ui->aCodecBox->addItems(QStringList()<<"pcm_s16le");
    this->ui->aCodecBox->setCurrentIndex(0);


    //set default parameters
    this->setParameters();
}

void Options::setParameters()
{

    this->setVideoInterface(this->ui->videoBox->currentText());
    this->setVideoSize(this->ui->sizeBox->currentText());
    this->setVideoCodec(this->ui->vCodecBox->currentText());

    this->setAudioInterface(this->ui->audioBox->currentText());
    this->setNumChannels(this->ui->channelsBox->currentText());
    this->setAudioCodec(this->ui->aCodecBox->currentText());
}

void Options::on_saveButton_clicked()
{
    this->setParameters();
    this->close();
}

bool Options::isThereAudioInterface()
{
    if(this->ui->audioBox->currentIndex() > 0) return true;
    else return false;
}

bool Options::isThereVideoInterface()
{
    if(this->ui->videoBox->currentIndex() > 0) return true;
    else return false;
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



void Options::on_testVideo_clicked()
{
    this->executeCommand("ffplay",QStringList()<< "-f" << "video4linux2" << this->ui->videoBox->currentText() << "-x" << "320" << "-y" << "240");
}

void Options::on_testAudio_clicked()
{

}
