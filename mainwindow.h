#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QProcess>
#include <QFile>
#include <QTextEdit>

#include "options.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();



public slots:

public:

private slots:
     void on_startButton_clicked();
     void readyReadStandardOutput();
     void processStarted();
     void encodingFinished();
     void endRecordingAndClose();
     void on_fileOpenButton_clicked();
     void on_playOutputButton_clicked();

     void on_actionClose_triggered();

     void on_closeButton_clicked();

     void on_optionButton_clicked();

private:
    Ui::MainWindow *ui;

    Options * options;

    QProcess *mTranscodingProcess;
    QProcess *mInputPlayProcess;
    QString mOutputString;

    QString outputFile;

    bool recording;
};

#endif // MAINWINDOW_H
