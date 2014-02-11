#ifndef OPTIONS_H
#define OPTIONS_H

#include <QWidget>

namespace Ui {
class Options;
}

class Options : public QWidget
{
    Q_OBJECT
    
public:
    explicit Options(QWidget *parent = 0);
    ~Options();

    void checkDevices();
    QString executeCommand(QString comand, QStringList arguments);
    void setUP(QStringList videoInterfaces, QStringList audioInterfaces);
    void setParameters();
    
    QString getVideoInterface() const;
    void setVideoInterface(const QString &value);

    QString getAudioInterface() const;
    void setAudioInterface(const QString &value);

    QString getVideoCodec() const;
    void setVideoCodec(const QString &value);

    QString getAudioCodec() const;
    void setAudioCodec(const QString &value);

    QString getNumChannels() const;
    void setNumChannels(const QString &value);

    QString getVideoSize() const;
    void setVideoSize(const QString &value);

    bool isThereAudioInterface();
    bool isThereVideoInterface();

private slots:
    void on_saveButton_clicked();

    void on_testVideo_clicked();

    void on_testAudio_clicked();

private:
    Ui::Options *ui;

    //private variables
    QString videoInterface;
    QString audioInterface;
    QString videoCodec;
    QString audioCodec;
    QString numChannels;
    QString videoSize;
};

#endif // OPTIONS_H
