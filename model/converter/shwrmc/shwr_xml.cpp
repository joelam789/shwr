#include "shwr.h"
#include "shwr_xml.h"
#include "tinyxml2.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>

#include <fstream>
#include <algorithm>

using namespace tinyxml2;

int shwr::convert_xml_to_model(const std::string& xmlfile)
{
    std::ifstream inputfile;
    inputfile.open(xmlfile.c_str(), std::ios::binary);

    if(!inputfile.is_open())
    {
        printf("Fail to open XML file: %s\n", xmlfile.c_str());
        return -1;
    }

    inputfile.seekg(0, std::ios::end);
    int filesize = inputfile.tellg();
    inputfile.seekg(0, std::ios::beg);

    char* xmldata = (char*)malloc(filesize);

    if(xmldata == NULL)
    {
        printf("Fail to get memory to read XML file: %s, %d\n", xmlfile.c_str(), filesize);
        return -1;
    }

    // read data as a block:
    inputfile.read(xmldata, filesize);

    inputfile.close();

    int df_pointdist = 50;
    int df_width = 1000;
    int df_height = 1000;

    std::vector<Feature> features;

    try
    {
        XMLDocument doc;
        doc.Parse( xmldata );

        int wordcount = 0;

        XMLElement* root = doc.FirstChildElement("dictionary");
        if(root)
        {
            XMLElement* xmlcharacter = root->FirstChildElement("character");
            while(xmlcharacter)
            {
                XMLElement* utf = xmlcharacter->FirstChildElement();
                if(!utf)
                {
                    xmlcharacter = xmlcharacter->NextSiblingElement();
                    continue;
                }

                wordcount++;

                std::string unicode = utf->GetText();
                printf("Char[%d]: %s\n", wordcount, unicode.c_str());

                //QString ch = QString::fromUtf16((const ushort *) unicode, 1);
                //qDebug("Char: %s (%s)", ch.toStdString().c_str(), unicode);


                XMLElement* xmlstrokes = utf->NextSiblingElement();

                if(!xmlstrokes)
                {
                    xmlcharacter = xmlcharacter->NextSiblingElement();
                    continue;
                }
                else
                {
                    Character cha;

                    int strokeidx = 0;

                    XMLElement* xmlstroke = xmlstrokes->FirstChildElement();
                    while(xmlstroke)
                    {
                        strokeidx++;

                        XMLElement* xmlpoint = xmlstroke->FirstChildElement();
                        while(xmlpoint)
                        {
                            std::string xtext = xmlpoint->Attribute("x");
                            std::string ytext = xmlpoint->Attribute("y");
                            int xv = atoi(xtext.c_str());
                            int yv = atoi(ytext.c_str());

                            cha.add(strokeidx, xv, yv);

                            xmlpoint = xmlpoint->NextSiblingElement();
                        }

                        xmlstroke = xmlstroke->NextSiblingElement();
                    }

                    //std::sort(lines.begin(), lines.end(), cmp_stroke);

                    int strokecount = cha.strokecount();

                    cha.fillstrokes();

                    cha.pointdist(df_pointdist);

                    //cha.fill();

                    int x, y, width, height;
                    Feature feature;

                    cha.rect(&x, &y, &width, &height);

                    int aw = 7*df_width;
                    int bw = 10*width;
                    int ah = 7*df_height;
                    int bh = 10*height;

                    if(df_width / width > 10)
                    {
                        aw = 1;
                        bw = 1;
                    }

                    if(df_height / height > 10)
                    {
                        ah = 1;
                        bh = 1;
                    }

                    cha.resize(aw, bw, ah, bh);

                    cha.rect(&x, &y, &width, &height);

                    int dx = (df_width - width) / 2 - x;
                    int dy = (df_height - height) / 2 - y;

                    cha.move(dx, dy);

                    cha.thin();

                    cha.feature(&(feature.feature));

                    feature.strokecount = strokecount;
                    feature.word = unicode;

                    features.push_back(feature);

                }


                xmlcharacter = xmlcharacter->NextSiblingElement();
            }
        }
    }
    catch(...)
    {
        features.clear();
        printf("Error found when parse the XML file: %s\n", xmlfile.c_str());
    }

    if(xmldata) free(xmldata);

    if(features.size() <= 0)
    {
        printf("Fail to read valid data from the XML file: %s\n", xmlfile.c_str());
        return features.size();
    }

    std::string modelfile = xmlfile;

    size_t dotpos = xmlfile.find_last_of('.');
    modelfile = xmlfile.substr(0, dotpos) + ".model";

    std::ofstream outputfile;
    outputfile.open(modelfile.c_str(), std::ios::out | std::ios::trunc | std::ios::binary);

    if(!outputfile.is_open())
    {
        printf("Fail to recreate a new model file: %s\n", modelfile.c_str());
        return -1;
    }

    printf("\nReady to build new model file: %s\n", modelfile.c_str());

    try
    {
        int headersize = sizeof(ModelHeader);
        int subsize = sizeof(FeatureHeader);
        int shortsize = sizeof(short);

        ModelHeader header;
        memset(&header, 0, headersize);

        header.sign[0] = 'S'; header.sign[1] = 'H'; header.sign[2] = 'W'; header.sign[3] = 'R';
        header.lang[0] = 'z'; header.lang[1] = 'h'; header.lang[2] = '-'; header.lang[3] = 'C'; header.lang[4] = 'N';

        header.ver = 1;
        header.type = 1;

        header.width = df_width;
        header.height = df_height;
        header.pointdist = df_pointdist;

        header.count = features.size();

        outputfile.write((char*)&header, headersize);

        for(int i=0; i<header.count; i++)
        {
            FeatureHeader subheader;
            memset(&subheader, 0, subsize);

            int wordlen = features[i].word.length();
            if(wordlen > 4) wordlen = 4;

            memcpy(&(subheader.word[0]), features[i].word.c_str(), wordlen);
            subheader.strokecount = features[i].strokecount;
            subheader.count = features[i].feature.size();

            printf("Processing %d / %d \n", i+1, header.count);

            outputfile.write((char*)&subheader, subsize);

            for(int j=0; j<subheader.count; j++)
            {
                short val = features[i].feature[j];
                outputfile.write((char*)&val, shortsize);
            }
        }

        printf("Done!\n");

    }
    catch(...)
    {
        features.clear();
        printf("Error found when write data into the model file: %s\n", modelfile.c_str());
    }

    try { outputfile.close(); } catch(...) {}


    return features.size();
}

