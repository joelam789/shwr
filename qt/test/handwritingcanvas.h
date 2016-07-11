#ifndef HANDWRITING_CANVAS_H
#define HANDWRITING_CANVAS_H

#include <QLabel>

#include <QPoint>

#include <vector>
#include <string>

class HandwritingEventHandler
{
public:
    virtual void OnFinishHandwriting(std::vector< std::vector<QPoint> > * points) = 0;
};

class HandwritingCanvas : public QLabel
{
    Q_OBJECT
public:
    explicit HandwritingCanvas(QWidget *parent = 0);

    void setEventHandler(HandwritingEventHandler * handler);

    void clear();

    void setPenSize(int size);
    std::vector< std::vector<QPoint> > * getPoints();
    int getPoints(std::vector< std::vector<QPoint> > * points);
    void assignPoints(std::vector< std::vector<QPoint> > * points);
    void resizePoints(double rate);
    void updateCanvas();

    void fillLine(int idx);
    void fillLines();

    int getLineCount();


protected:

    void paintEvent(QPaintEvent *);
    void mousePressEvent(QMouseEvent *);
    void mouseReleaseEvent(QMouseEvent *);
    void mouseMoveEvent(QMouseEvent *);

private:

    std::vector< std::vector<QPoint> > m_points;

    bool m_pressed;

    int m_pensize;

    HandwritingEventHandler * m_handler;

signals:

public slots:

};

#endif // HANDWRITING_CANVAS_H
