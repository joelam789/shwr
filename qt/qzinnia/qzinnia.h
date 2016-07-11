#ifndef QZINNIA_H
#define QZINNIA_H

#include <string>
#include <vector>

#include <QPoint>

class QZinnia {
private:
    void * m_recognizer;

public:
    QZinnia();
    virtual ~QZinnia();

    bool init(const std::string& modelfile);
    bool ready();

    int words(int w, int h, std::vector< std::vector<QPoint> > * points, std::vector<std::string>* result);
};

#endif // QZINNIA_H
