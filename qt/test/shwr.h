#ifndef SHWR_H
#define SHWR_H

#include <vector>
#include <string>

namespace shwr
{

struct ModelHeader
{
    char sign[4];
    char lang[6];
    short ver;
    short type;
    short width;
    short height;
    short pointdist;
    int count;
};

struct FeatureHeader
{
    char word[4];
    int strokecount;
    int count;
};

struct Feature
{
    int strokecount;
    std::string word;
    std::vector<int> feature;
};

struct Point
{
    Point(int xv, int yv) : x(xv), y(yv) {}
    int x;
    int y;
};

struct Word
{
    std::string word;
    int dist;
};

class Stroke
{
public:
    Stroke();

    int add(int x, int y);

    void fill();
    void thin(int pointdist);

    void resize(int aw, int bw, int ah, int bh);
    void move(int dx, int dy);

    std::vector<Point> * points();

private:
    std::vector<Point> m_points;
};

class Character
{
public:
    Character();

    int add(int idx, int x, int y);

    void fullpoints(bool full);
    void fillstrokes();

    void fill();
    void thin();

    void resize(int aw, int bw, int ah, int bh);
    void move(int dx, int dy);

    void rect(int* x, int* y, int* w, int* h);

    int feature(std::vector<int> * featurelist);

    int strokecount();
    void pointdist(int dist);

    void print();

private:
    int m_lastidx;
    int m_mindist;
    int m_strokecount;
    bool m_fullpoints;
    std::vector<Stroke> m_strokes;
};

class Recognizer
{
public:
    Recognizer();

    int ver();
    int type();
    int width();
    int height();
    int pointdist();
    int modelsize();
    std::string lang();

    bool ready();

    int init(const std::string& modelfile);

    int feature(const std::string& word, std::vector< std::vector<int> > * featurelists);

    int recognize(Character* cha, std::vector<std::string> * words, int maxcount = 10, int dynstroke = 2);
    int recognize(Character* cha, std::vector<Word> * words, int maxcount = 10, int dynstroke = 2);

private:
    int m_ver;
    int m_type;
    int m_width;
    int m_height;
    int m_pointdist;
    std::string m_lang;
    std::vector<Feature> m_features;
};

}

#endif // SHWR_H
