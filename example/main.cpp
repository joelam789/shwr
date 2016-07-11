
#include <cstring> // memset
#include <iostream>
#include <iconv.h> // display utf8

#include "shwr.h"

void print_utf8(const std::string & str);

int main(int argc, char **argv)
{
    shwr::Recognizer recognizer;
    recognizer.init("simple-handwriting-zh_CN.model");

    shwr::Character character;

    character.add(0, 353, 233);
    character.add(0, 698, 163);
    character.add(1, 223, 462);
    character.add(1, 808, 379);
    character.add(2, 483, 227);
    character.add(2, 450, 423);
    character.add(2, 393, 608);
    character.add(2, 260, 710);
    character.add(2, 133, 763);
    character.add(3, 475, 484);
    character.add(3, 668, 682);
    character.add(3, 805, 765);
    character.add(3, 933, 771);

    std::vector<std::string> words;

    recognizer.recognize( &character, &words, 10, 0 );

    for (size_t i=0; i<words.size(); i++) print_utf8(words[i]);

    return 0;
}

void print_utf8(const std::string & str)
{
    iconv_t cd = iconv_open("","UTF-8");

    size_t inlen = str.length();
    char *inbuf = (char*)str.c_str();

    size_t outlen = inlen * 2;
    char *outbuf = new char[outlen];

    char *pin = inbuf;
    char *pout = outbuf;

    memset(outbuf, 0, outlen);
    iconv(cd, &pin, &inlen, &pout, &outlen);

    iconv_close(cd);

    std::cout << outbuf << std::endl;

    delete [] outbuf;

}

