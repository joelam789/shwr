# shwr

1.What is SHWR?

SHWR (Simple Hand-Writing Recognizer) is a very simple but effective online hand recognition system based on DTW (Dynamic Time Warping) algorithm. It is written in pure C++ with no third party dependencies (except STL).

![test program with qt](https://a.fsdn.com/con/app/proj/shwr/screenshots/shwr_qt_sample_01.png)

<br />
2.How to use it?

Here is an example (very like zinnia):

``` c++

#include <iostream>
#include "shwr.h"

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

    for (size_t i=0; i<words.size(); i++) std::cout << words[i] << std::endl;

    return 0;
}

```

<br />
3.License

Free and open-source, BSD License.


