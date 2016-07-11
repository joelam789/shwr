#include "shwr.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <algorithm>

#define SHWR_MAX_DIST         99999999

#define SHWR_DF_WIDTH         1000
#define SHWR_DF_HEIGHT        1000
#define SHWR_DF_DIST          50
#define SHWR_DF_ST_DY_COUNT   5

#define SHWR_ABS(a)           ((a) >= 0 ? (a) : 0 - (a))
#define SHWR_MAX2(a, b)       ((a) > (b) ? (a) : (b))
#define SHWR_MIN2(a, b)       ((a) > (b) ? (b) : (a))
#define SHWR_MIN3(a, b, c)    SHWR_MIN2(SHWR_MIN2(a, b), c)

using namespace shwr;

static bool cmp_word(const Word& a, const Word& b)
{
    return a.dist < b.dist;
}

static bool cmp_feature(const Feature& a, const Feature& b)
{
    if(a.strokecount == b.strokecount) return a.word < b.word;
    else return a.strokecount < b.strokecount;
}

static int dtw(const std::vector<int>& s, const std::vector<int>& t)
{
    int n = s.size() >> 1;
    int m = t.size() >> 1;

    int DTW[n+1][m+1];
    int i, j, cost;

    if(n > m)
    {
        for (i = 1; i <= m; i++)
        {
            DTW[0][i] = SHWR_MAX_DIST;
            DTW[i][0] = SHWR_MAX_DIST;
        }
        for(i=m+1; i <= n; i++) DTW[i][0] = SHWR_MAX_DIST;
    }
    else if(n < m)
    {
        for (i = 1; i <= n; i++)
        {
            DTW[0][i] = SHWR_MAX_DIST;
            DTW[i][0] = SHWR_MAX_DIST;
        }
        for(i=n+1; i <= m; i++) DTW[0][i] = SHWR_MAX_DIST;
    }
    else
    {
        for (i = 1; i <= m; i++)
        {
            DTW[0][i] = SHWR_MAX_DIST;
            DTW[i][0] = SHWR_MAX_DIST;
        }
    }

    DTW[0][0] = 0;
    int x, y;

    for (i = 1; i <= n; i++)
        for (j = 1; j <= m; j++)
        {
            x = (i-1) << 1;
            y = (j-1) << 1;
            cost = SHWR_ABS(s[x] - t[y]) + SHWR_ABS(s[x+1] - t[y+1]);
            DTW[i][j] = cost + SHWR_MIN3(DTW[i-1][j], DTW[i][j-1], DTW[i-1][j-1]);
        }

    return DTW[n][m];
}

Stroke::Stroke()
{
}

int Stroke::add(int x, int y)
{
    m_points.push_back(Point(x, y));
    return m_points.size();
}

void Stroke::resize(int aw, int bw, int ah, int bh)
{
    int pcount = m_points.size();
    for(int i=0; i<pcount; i++)
    {
        m_points[i].x = m_points[i].x * aw / bw;
        m_points[i].y = m_points[i].y * ah / bh;
    }
}

void Stroke::move(int dx, int dy)
{
    int pcount = m_points.size();
    for(int i=0; i<pcount; i++)
    {
        m_points[i].x = m_points[i].x + dx;
        m_points[i].y = m_points[i].y + dy;
    }
}

void Stroke::fill()
{
    int pcount = m_points.size();
    if(pcount <= 1) return;

    int deltax,deltay,numpixels,d,dinc1,dinc2,x,xinc1,xinc2,y,yinc1,yinc2,x1,y1,x2,y2;

    std::vector<Point> line;
    for(int i = 0; i < pcount - 1; i++)
    {
        x1 = m_points[i].x;
        y1 = m_points[i].y;
        x2 = m_points[i+1].x;
        y2 = m_points[i+1].y;

        deltax = SHWR_ABS(x2 - x1);
        deltay = SHWR_ABS(y2 - y1);

        // Initialize all vars based on which is the independent variable
        if (deltax >= deltay) // x is independent variable
        {
            numpixels = deltax + 1;
            d = (2 * deltay) - deltax;
            dinc1 = deltay << 1;
            dinc2 = (deltay - deltax) << 1;
            xinc1 = 1;
            xinc2 = 1;
            yinc1 = 0;
            yinc2 = 1;
        }
        else // y is independent variable
        {
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

        // Start drawing ...
        x = x1;
        y = y1;
        for (int j = 1; j < numpixels; j++)
        {
            line.push_back(Point(x, y));
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
    line.push_back(Point(m_points[pcount-1].x, m_points[pcount-1].y));
    m_points = line;
}

std::vector<Point> * Stroke::points()
{
    return &m_points;
}

void Stroke::thin(int pointdist)
{
    int dist = pointdist * pointdist;

    int x1 = 0;
    int y1 = 0;
    int x2 = 0;
    int y2 = 0;

    int k = 0;

    std::vector<Point> line;
    int pcount = m_points.size();
    for(int i=0; i<pcount; i++)
    {
        x2 = m_points[i].x;
        y2 = m_points[i].y;

        if(k > 0 && (x2-x1) * (x2-x1) + (y2-y1) * (y2-y1) <= dist) continue;

        line.push_back(Point(x2, y2));

        x1 = x2;
        y1 = y2;

        k++;
    }
    m_points = line;
}

/* ------------------ Character ----------------- */
Character::Character()
{
    m_lastidx = -1;
    m_mindist = SHWR_DF_DIST;
    m_strokecount = 0;
    m_fullpoints = false;
}

int Character::strokecount()
{
    return m_strokecount;
}

void Character::pointdist(int dist)
{
    m_mindist = dist;
}

int Character::add(int idx, int x, int y)
{
    if(idx < 0) return 0;
    if(idx != m_lastidx)
    {
        Stroke newone;
        newone.add(x, y);
        m_strokes.push_back(newone);
        m_strokecount = m_strokecount + 1;
        m_lastidx = idx;
    }
    else
    {
        if(m_strokes.size() > 0) m_strokes.back().add(x, y);
    }
    return m_strokecount;
}

void Character::fullpoints(bool full)
{
    m_fullpoints = full;
}

void Character::fillstrokes()
{
    if(m_fullpoints) return;
    int count = m_strokes.size();
    for(int i=0; i<count; i++)
    {
        m_strokes[i].fill();
    }
    m_fullpoints = true;
}

void Character::fill()
{
    int x1 = 0;
    int y1 = 0;
    int x2 = 0;
    int y2 = 0;

    int count, i, j;
    int dist = m_mindist * m_mindist * 2;

    j = 0;
    std::vector<Stroke>::iterator it = m_strokes.begin();
    count = m_strokes.size();
    for(i=0; i<count-1; i++)
    {
        it++;

        x1 = m_strokes[j].points()->back().x;
        y1 = m_strokes[j].points()->back().y;
        x2 = m_strokes[j+1].points()->front().x;
        y2 = m_strokes[j+1].points()->front().y;

        if((x2-x1) * (x2-x1) + (y2-y1) * (y2-y1) > dist)
        {
            Stroke newone;
            newone.add(x1, y1);
            newone.add(x2, y2);
            if(m_fullpoints) newone.fill();
            it = m_strokes.insert(it, newone);
            it++;
            j++;
        }
        j++;
    }
}

void Character::thin()
{
    int count = m_strokes.size();
    for(int i=0; i<count; i++)
    {
        m_strokes[i].thin(m_mindist);
    }
}

void Character::resize(int aw, int bw, int ah, int bh)
{
    int count = m_strokes.size();
    for(int i=0; i<count; i++)
    {
        m_strokes[i].resize(aw, bw, ah, bh);
    }
}

void Character::move(int dx, int dy)
{
    int count = m_strokes.size();
    for(int i=0; i<count; i++)
    {
        m_strokes[i].move(dx, dy);
    }
}

void Character::rect(int* x, int* y, int* w, int* h)
{
    int xmin = SHWR_MAX_DIST; // 4294967296 # 2^32
    int ymin = SHWR_MAX_DIST;
    int xmax = 0;
    int ymax = 0;
    int ptx = 0;
    int pty = 0;

    int count = m_strokes.size();
    for(int i=0; i<count; i++)
    {
        int pcount = m_strokes[i].points()->size();
        if(pcount <= 0) continue;
        for(int j=0; j<pcount; j++)
        {
            ptx = m_strokes[i].points()->at(j).x;
            pty = m_strokes[i].points()->at(j).y;
            xmin = SHWR_MIN2(xmin, ptx);
            ymin = SHWR_MIN2(ymin, pty);
            xmax = SHWR_MAX2(xmax, ptx);
            ymax = SHWR_MAX2(ymax, pty);
        }
    }
    *x = xmin;
    *y = ymin;
    *w = xmax-xmin+1;
    *h = ymax-ymin+1;
}

void Character::print()
{
    int ptx = 0;
    int pty = 0;
    int count = m_strokes.size();
    for(int i=0; i<count; i++)
    {
        int pcount = m_strokes[i].points()->size();
        if(pcount <= 0) continue;
        for(int j=0; j<pcount; j++)
        {
            ptx = m_strokes[i].points()->at(j).x;
            pty = m_strokes[i].points()->at(j).y;
            printf("[%d, %d] = (%d, %d)\n", i, j, ptx, pty);
        }
    }
    fflush(stdout);
}

int Character::feature(std::vector<int> * featurelist)
{
    if(!featurelist) return -1;
    featurelist->clear();
    int ptx = 0;
    int pty = 0;
    int count = m_strokes.size();
    for(int i=0; i<count; i++)
    {
        int pcount = m_strokes[i].points()->size();
        if(pcount <= 0) continue;
        for(int j=0; j<pcount; j++)
        {
            ptx = m_strokes[i].points()->at(j).x;
            pty = m_strokes[i].points()->at(j).y;
            featurelist->push_back(ptx);
            featurelist->push_back(pty);
        }
    }
    return featurelist->size();
}

/* ------------------ Recognizer ----------------- */

Recognizer::Recognizer()
{
    m_ver = 1;
    m_type = 1;
    m_width = SHWR_DF_WIDTH;
    m_height = SHWR_DF_HEIGHT;
    m_pointdist = SHWR_DF_DIST;
    m_lang = "zh-CN"; // default language
}

int Recognizer::ver()
{
    return m_ver;
}

int Recognizer::type()
{
    return m_type;
}

int Recognizer::width()
{
    return m_width;
}

int Recognizer::height()
{
    return m_height;
}

int Recognizer::pointdist()
{
    return m_pointdist;
}

std::string Recognizer::lang()
{
    return m_lang;
}

int Recognizer::modelsize()
{
    return m_features.size();
}

bool Recognizer::ready()
{
    return m_features.size() > 0;
}

int Recognizer::init(const std::string& modelfile)
{
    int headersize = sizeof(ModelHeader);
    int subsize = sizeof(FeatureHeader);
    int shortsize = sizeof(short);

    std::ifstream inputfile;
    inputfile.open(modelfile.c_str(), std::ios::binary);
    if(!inputfile.is_open())
    {
        printf("Fail to open model file: %s\n", modelfile.c_str());
        return -1;
    }
    inputfile.seekg(0, std::ios::end);
    int filesize = inputfile.tellg();
    if(filesize < headersize)
    {
        inputfile.close();
        printf("Invalid model file: %s\n", modelfile.c_str());
        return -1;
    }
    inputfile.seekg(0, std::ios::beg);
    try
    {
        ModelHeader header;
        inputfile.read((char*)&header, headersize);

        bool ok = true;
        if(header.sign[0] != 'S'
                || header.sign[1] != 'H'
                || header.sign[2] != 'W'
                || header.sign[3] != 'R') ok = false;

        if(ok)
        {
            if(header.width < 10
                    || header.height < 10
                    || header.pointdist < 1
                    || header.count < 0) ok = false;
        }

        if(ok)
        {
            if(filesize - (header.count) * subsize < 0) ok = false;
        }

        if(ok)
        {
            char lang[8];
            memset(lang, 0, 8);
            memcpy(lang, &(header.lang[0]), 6);

            m_lang = lang;
            m_ver = header.ver;
            m_type = header.type;
            m_pointdist = header.pointdist;
            m_width = header.width;
            m_height = header.height;

            int count = header.count;
            m_features.clear();
            for(int i=0; i<count; i++)
            {
                if(filesize - subsize < 0)
                {
                    printf("Invalid model file: Sub header is not valid.\n");
                    ok = false;
                    break;
                }

                bool subok = true;
                FeatureHeader subheader;
                inputfile.read((char*)&subheader, subsize);

                if(subheader.count <= 0
                        || subheader.strokecount <= 0
                        || subheader.word[0] == '0')
                {
                    printf("Invalid model file: Sub header is not valid.\n");
                    subok = false;
                    ok = false;
                    break;
                }

                if(subok)
                {
                    filesize = filesize - subsize;
                    if(filesize - (subheader.count) * shortsize < 0)
                    {
                        printf("Invalid model file: Feature data is not valid.\n");
                        subok = false;
                        ok = false;
                        break;
                    }

                    char newword[8];
                    memset(newword, 0, 8);
                    memcpy(newword, &(subheader.word[0]), 4);

                    Feature feature;
                    feature.strokecount = subheader.strokecount;
                    feature.word = newword;

                    for(int j = 0; j < subheader.count; j++)
                    {
                        short sv;
                        inputfile.read((char*)&sv, shortsize);
                        int fv = sv;
                        feature.feature.push_back(fv);
                    }

                    m_features.push_back(feature);
                    filesize = filesize - (subheader.count) * shortsize;
                }

                if(filesize <= 0)
                {
                    if(filesize < 0) ok = false;
                    break;
                }
            }
            if(!ok) m_features.clear();
        }
    }
    catch(...)
    {
        printf("Found error when parse model file: %s\n", modelfile.c_str());
    }

    inputfile.close();

    if(m_features.size() > 0)
    {
        std::sort(m_features.begin(), m_features.end(), cmp_feature);
    }
    return m_features.size();
}

int Recognizer::feature(const std::string& word, std::vector< std::vector<int> > * featurelists)
{
    if(!featurelists) return -1;
    if(word.length() <= 0 || !ready()) return 0;
    featurelists->clear();
    int count = m_features.size();
    for(int i=0; i<count; i++)
    {
        if(m_features[i].word == word)
        {
            featurelists->push_back(m_features[i].feature);
        }
    }
    return featurelists->size();
}

int Recognizer::recognize(Character* cha, std::vector<std::string> * words, int maxcount, int dynstroke)
{
    if(!cha || !words) return -1;
    if(!ready()) return 0;

    int strokecount = cha->strokecount();
    if(strokecount <= 0) return 0;

    cha->pointdist(m_pointdist);

    int x, y, width, height;
    std::vector<int> featurelist;

    cha->rect(&x, &y, &width, &height);

    int aw = 7*m_width;
    int bw = 10*width;
    int ah = 7*m_height;
    int bh = 10*height;

    if(m_width / width > 10)
    {
        aw = 1;
        bw = 1;
    }
    if(m_height / height > 10)
    {
        ah = 1;
        bh = 1;
    }

    cha->resize(aw, bw, ah, bh);
    cha->rect(&x, &y, &width, &height);

    int dx = (m_width - width) / 2 - x;
    int dy = (m_height - height) / 2 - y;

    cha->move(dx, dy);
    cha->thin();
    cha->feature(&featurelist);

    std::vector<Word> wordlist;

    int count = m_features.size();
    int dist = 0;
    int countdiff = 0;
    int lastdist = 0;
    std::string lastword = "";

    if(dynstroke < 0)
    {
        dynstroke = 0 - dynstroke;
        for(int i=0; i<count; i++)
        {
            countdiff = m_features[i].strokecount - strokecount;

            if(countdiff > dynstroke) break;
            else if(countdiff < 0) countdiff = 0 - countdiff;

            if(countdiff > dynstroke) continue;

            dist = dtw(featurelist, m_features[i].feature);

            Word record;
            record.word = m_features[i].word;
            record.dist = dist;
            if(record.word == lastword)
            {
                if(record.dist < lastdist)
                {
                    wordlist.back().dist = record.dist;
                }
            }
            else
            {
                wordlist.push_back(record);
            }
            lastword = record.word;
            lastdist = record.dist;
        }
    }
    else
    {
        for(int i=0; i<count; i++)
        {
            countdiff = m_features[i].strokecount - strokecount;

            if(countdiff < 0) continue;
            if(countdiff > dynstroke) break;

            dist = dtw(featurelist, m_features[i].feature);

            Word record;
            record.word = m_features[i].word;
            record.dist = dist;
            if(record.word == lastword)
            {
                if(record.dist < lastdist)
                {
                    wordlist.back().dist = record.dist;
                }
            }
            else
            {
                wordlist.push_back(record);
            }
            lastword = record.word;
            lastdist = record.dist;
        }
    }

    std::sort(wordlist.begin(), wordlist.end(), cmp_word);

    int wordcount = wordlist.size();
    wordcount = SHWR_MIN2(wordcount, maxcount);

    words->clear();
    for(int i=0; i<wordcount; i++)
    {
        words->push_back(wordlist[i].word);
    }
    return words->size();
}

int Recognizer::recognize(Character* cha, std::vector<Word> * words, int maxcount, int dynstroke)
{
    if(!cha || !words) return -1;
    if(!ready()) return 0;

    int strokecount = cha->strokecount();
    if(strokecount <= 0) return 0;

    cha->pointdist(m_pointdist);

    int x, y, width, height;
    std::vector<int> featurelist;

    cha->rect(&x, &y, &width, &height);

    int aw = 7*m_width;
    int bw = 10*width;
    int ah = 7*m_height;
    int bh = 10*height;

    if(m_width / width > 10)
    {
        aw = 1;
        bw = 1;
    }
    if(m_height / height > 10)
    {
        ah = 1;
        bh = 1;
    }

    cha->resize(aw, bw, ah, bh);
    cha->rect(&x, &y, &width, &height);

    int dx = (m_width - width) / 2 - x;
    int dy = (m_height - height) / 2 - y;

    cha->move(dx, dy);
    cha->thin();
    cha->feature(&featurelist);
    words->clear();

    int count = m_features.size();
    int dist = 0;
    int countdiff = 0;
    int lastdist = 0;
    std::string lastword = "";

    if(dynstroke < 0)
    {
        dynstroke = 0 - dynstroke;
        for(int i=0; i<count; i++)
        {
            countdiff = m_features[i].strokecount - strokecount;

            if(countdiff > dynstroke) break;
            else if(countdiff < 0) countdiff = 0 - countdiff;

            if(countdiff > dynstroke) continue;

            dist = dtw(featurelist, m_features[i].feature);

            Word record;
            record.word = m_features[i].word;
            record.dist = dist;
            if(record.word == lastword)
            {
                if(record.dist < lastdist)
                {
                    words->back().dist = record.dist;
                }
            }
            else
            {
                words->push_back(record);
            }
            lastword = record.word;
            lastdist = record.dist;
        }
    }
    else
    {
        for(int i=0; i<count; i++)
        {
            countdiff = m_features[i].strokecount - strokecount;

            if(countdiff < 0) continue;
            if(countdiff > dynstroke) break;

            dist = dtw(featurelist, m_features[i].feature);

            Word record;
            record.word = m_features[i].word;
            record.dist = dist;
            if(record.word == lastword)
            {
                if(record.dist < lastdist)
                {
                    words->back().dist = record.dist;
                }
            }
            else
            {
                words->push_back(record);
            }
            lastword = record.word;
            lastdist = record.dist;
        }
    }
    std::sort(words->begin(), words->end(), cmp_word);
    int wordcount = words->size();
    if(wordcount != maxcount)
    {
        wordcount = SHWR_MIN2(wordcount, maxcount);
        words->resize(wordcount);
    }
    return words->size();
}
