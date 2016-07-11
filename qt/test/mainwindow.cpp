#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QTime>

#include "shwr.h"
#include "qzinnia.h"

static QZinnia g_zinnia;
static shwr::Recognizer g_shwr;


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->spinBox->setEnabled(false);

    m_canvas = new HandwritingCanvas(this);
    m_canvas->setPenSize(5);
    m_canvas->setGeometry(20, 80, 300, 300);

    m_canvas->setEventHandler(this);

    g_zinnia.init("handwriting-zh_CN.model");
    g_shwr.init("simple-handwriting-zh_CN.model");


}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::OnFinishHandwriting(std::vector< std::vector<QPoint> > * points)
{
    if(ui->radioButton->isChecked())
    {
        QTime now = QTime::currentTime();
        now.start();

        if(points && m_canvas && g_zinnia.ready())
        {
            std::vector<std::string> wordlist;

            g_zinnia.words(m_canvas->geometry().width(),
                           m_canvas->geometry().height(),
                           points, &wordlist);

            ui->listWidget->clear();

            int count = wordlist.size();
            if(count > 10) count = 10;

            for(int i=0; i<count; i++)
            {
                ui->listWidget->addItem(QString::number(i) + ": " + QString::fromStdString(wordlist[i]));
            }
        }

        ui->label->setText(QString::number(now.elapsed()) + " ms");
    }
    else if(ui->radioButton_2->isChecked())
    {

        QTime now = QTime::currentTime();
        now.start();

        if(points && m_canvas && g_shwr.ready())
        {
            std::vector<std::string> wordlist;

            shwr::Character cha;

            int lcount = points->size();
            for(int i=0; i<lcount; i++)
            {
                int pcount = points->at(i).size();
                for(int j=0; j<pcount; j++)
                {
                    cha.add(i, points->at(i).at(j).x(), points->at(i).at(j).y());
                }
            }

            g_shwr.recognize( &cha, &wordlist, 10, ui->spinBox->value() );

            ui->listWidget->clear();

            int count = wordlist.size();
            if(count > 10) count = 10;

            for(int i=0; i<count; i++)
            {
                ui->listWidget->addItem(QString::number(i) + ": " + QString::fromStdString(wordlist[i]));
            }

        }

        ui->label->setText(QString::number(now.elapsed()) + " ms");
    }

}

void MainWindow::on_pushButton_clicked()
{
    if(m_canvas) m_canvas->clear();
    ui->listWidget->clear();
}

void MainWindow::on_radioButton_clicked()
{
    ui->spinBox->setEnabled(false);
}

void MainWindow::on_radioButton_2_clicked()
{
    ui->spinBox->setEnabled(true);
}
