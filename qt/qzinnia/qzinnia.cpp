#include "qzinnia.h"

#include <QDebug>

#include "zinnia.h"

#define MAX_RETURN_WORD_COUNT 10

QZinnia::QZinnia()
{
    m_recognizer = NULL;
}

QZinnia::~QZinnia()
{
    if(m_recognizer) delete m_recognizer;
    m_recognizer= NULL;
}

bool QZinnia::init(const std::string& modelfile)
{
    if(m_recognizer) return true;

    m_recognizer = zinnia::Recognizer::create();

    zinnia::Recognizer * recognizer = (zinnia::Recognizer *) m_recognizer;

    if (!recognizer->open(modelfile.c_str()))
    {
        qDebug("QZinnia - Fail to load model file: %s", modelfile.c_str());
        qDebug("QZinnia - Error message: %s", recognizer->what());
        if(recognizer) delete recognizer;
        m_recognizer= NULL;
    }

    return m_recognizer != NULL;
}

bool QZinnia::ready()
{
    return m_recognizer != NULL;
}

int QZinnia::words(int w, int h, std::vector< std::vector<QPoint> > * points, std::vector<std::string>* rsl)
{
    if(!m_recognizer) return -99;

    if(!points || !rsl) return -1;

    if(w <= 0 || h <= 0) return -1;

    int lcount = points->size();
    if(lcount <= 0) return -1;

    int ret = 0;

    zinnia::Recognizer * recognizer = (zinnia::Recognizer *) m_recognizer;

    zinnia::Character *character = zinnia::Character::create();
    character->clear();
    character->set_width(w);
    character->set_height(h);

    for(int i=0; i<lcount; i++)
    {
        int pcount = points->at(i).size();
        for(int j = 0; j<pcount; j++)
        {
            character->add(i, points->at(i).at(j).x(), points->at(i).at(j).y());
        }
    }

    zinnia::Result *result = recognizer->classify(*character, MAX_RETURN_WORD_COUNT);
    if (result)
    {
        rsl->clear();
        for (size_t i = 0; i < result->size(); ++i)
        {
            std::string word = result->value(i);
            rsl->push_back(word);
        }
        ret = rsl->size();
    }
    else
    {
        qDebug("QZinnia - Error: %s", recognizer->what());
        ret = -2;
    }

    if(result) delete result;
    if(character) delete character;

    return ret;

}
