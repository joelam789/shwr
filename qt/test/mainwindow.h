#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "handwritingcanvas.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow, HandwritingEventHandler
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public:

    virtual void OnFinishHandwriting(std::vector< std::vector<QPoint> > * points);

private slots:
    void on_pushButton_clicked();

    void on_radioButton_clicked();

    void on_radioButton_2_clicked();

private:

    HandwritingCanvas * m_canvas;


private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
