#include "handwritingcanvas.h"

#include "mainwindow.h"

#include <cmath>

#include <QColor>
#include <QPainter>
#include <QMouseEvent>

HandwritingCanvas::HandwritingCanvas(QWidget *parent) :
    QLabel(parent)
{
    m_points.clear();
    m_pressed = false;
    m_pensize = 1;
    m_handler = NULL;
}

void HandwritingCanvas::setEventHandler(HandwritingEventHandler * handler)
{
    m_handler = handler;
}

void HandwritingCanvas::clear()
{
    m_points.clear();
    m_pressed = false;

    repaint();
}

void HandwritingCanvas::setPenSize(int size)
{
    m_pensize = size;
}

std::vector< std::vector<QPoint> > * HandwritingCanvas::getPoints()
{
    return &m_points;
}

int HandwritingCanvas::getPoints(std::vector< std::vector<QPoint> > * points)
{
    if(points == NULL) return 0;

    points->clear();

    int lcount = m_points.size();
    for(int i=0; i<lcount; i++)
    {
        std::vector<QPoint> line;
        int pcount = m_points.at(i).size();
        for(int j=0; j<pcount; j++)
        {
            int x = m_points.at(i).at(j).x();
            int y = m_points.at(i).at(j).y();
            line.push_back(QPoint(x, y));
            //qDebug("[%d, %d] %d, %d", i, j, x, y);
        }
        points->push_back(line);
    }

    return points->size();
}

void HandwritingCanvas::assignPoints(std::vector< std::vector<QPoint> >* points)
{
    if(points == NULL) return;

    m_points.clear();

    int lcount = points->size();
    for(int i=0; i<lcount; i++)
    {
        std::vector<QPoint> line;
        int pcount = points->at(i).size();
        for(int j=0; j<pcount; j++)
        {
            line.push_back(QPoint(points->at(i).at(j).x(), points->at(i).at(j).y()));
        }
        m_points.push_back(line);
    }
}

void HandwritingCanvas::resizePoints(double rate)
{
    int lcount = m_points.size();
    for(int i=0; i<lcount; i++)
    {
        int pcount = m_points.at(i).size();
        for(int j=0; j<pcount; j++)
        {
            double x = m_points.at(i).at(j).x();
            double y = m_points.at(i).at(j).y();

            //qDebug("Point: %f, %f", x, y);

            x = x * rate;
            y = y * rate;
            m_points.at(i).at(j).setX(round(x));
            m_points.at(i).at(j).setY(round(y));

            //qDebug("Point: %d, %d",
            //       m_points.at(i).at(j).x(), m_points.at(i).at(j).y());
        }
    }
}

void HandwritingCanvas::updateCanvas()
{
    repaint();
}

void HandwritingCanvas::fillLine(int idx)
{
    int lcount = m_points.size();
    if(idx < 0 || idx >= lcount) return;

    int pcount = m_points.at(idx).size();
    if(pcount <= 1) return;
    int deltax,deltay,numpixels,d,dinc1,dinc2,x,xinc1,xinc2,y,yinc1,yinc2,x1,y1,x2,y2;
    std::vector<QPoint> line;
    for(int i = 0; i < pcount - 1; i++)
    {
        x1 = m_points.at(idx).at(i).x();
        y1 = m_points.at(idx).at(i).y();
        x2 = m_points.at(idx).at(i+1).x();
        y2 = m_points.at(idx).at(i+1).y();

        //x0 = std::max(std::min(x1,x2), 0);
        //y0 = std::max(std::min(y1,y2), 0);

        deltax = abs(x2 - x1);
        deltay = abs(y2 - y1);

        // Initialize all vars based on which is the independent variable
        if (deltax >= deltay)
        {

            // x is independent variable
            numpixels = deltax + 1;
            d = (2 * deltay) - deltax;
            dinc1 = deltay << 1;
            dinc2 = (deltay - deltax) << 1;
            xinc1 = 1;
            xinc2 = 1;
            yinc1 = 0;
            yinc2 = 1;
        }
        else
        {
            // y is independent variable
            numpixels = deltay + 1;
            d = (2 * deltax) - deltay;
            dinc1 = deltax << 1;
            dinc2 = (deltax - deltay) << 1;
            xinc1 = 0;
            xinc2 = 1;
            yinc1 = 1;
            yinc2 = 1;
        }

        // Make sure x and y move in the right directions
        if (x1 > x2)
        {
            xinc1 = 0 - xinc1;
            xinc2 = 0 - xinc2;
        }
        if (y1 > y2)
        {
            yinc1 = 0 - yinc1;
            yinc2 = 0 - yinc2;
        }

        // Start drawing at
        x = x1;
        y = y1;

        for (int j = 1; j < numpixels; j++)
        {
            line.push_back(QPoint(x, y));
            if (d < 0)
            {
                d += dinc1;
                x += xinc1;
                y += yinc1;
            }
            else
            {
                d += dinc2;
                x += xinc2;
                y += yinc2;
            }
        }

    }

    line.push_back(QPoint(m_points.at(idx).at(pcount - 1).x(),
                          m_points.at(idx).at(pcount - 1).y()));

    m_points.at(idx).clear();
    pcount = line.size();
    for(int i=0; i<pcount; i++)
    {
        m_points.at(idx).push_back(line[i]);
    }
}

void HandwritingCanvas::fillLines()
{
    int lcount = m_points.size();
    for(int i=0; i <lcount; i++) fillLine(i);

    //std::sort(m_points.begin(), m_points.end(), cmp_line);
}

int HandwritingCanvas::getLineCount()
{
    return m_points.size();
}

void HandwritingCanvas::paintEvent(QPaintEvent *)
{
    QPainter paint(this);
    QPen pen;

    paint.fillRect(QRect(0, 0, this->width(), this->height()),
                   QColor(255, 255, 255));

    //paint.setPen(QColor(0, 0, 0));

    pen.setColor(QColor(0, 0, 0));
    pen.setWidth(m_pensize);

    paint.setPen(pen);

    int lcount = m_points.size();
    for(int i=0; i<lcount; i++)
    {
        int pcount = m_points.at(i).size();
        for(int j=1; j<pcount; j++)
        {
            paint.drawLine(m_points.at(i).at(j-1), m_points.at(i).at(j));
        }
    }
}

void HandwritingCanvas::mousePressEvent(QMouseEvent * event)
{
    m_pressed = true;

    std::vector<QPoint> line;
    line.push_back(event->pos());
    m_points.push_back(line);

    repaint();
}

void HandwritingCanvas::mouseReleaseEvent(QMouseEvent *)
{
    m_pressed = false;

    repaint();

    if(m_handler)
    {
        m_handler->OnFinishHandwriting(&m_points);
    }
}

void HandwritingCanvas::mouseMoveEvent(QMouseEvent * event)
{
    if(m_pressed && m_points.size() > 0)
    {
        m_points[m_points.size() - 1].push_back(event->pos());
        repaint();
    }
}
