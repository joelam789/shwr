#include "shwr_xml.h"

int main(int argc, char *argv[])
{
    if(argc <= 1)
    {
        //std::cout << "Please input the XML file path." << std::endl;
        shwr::convert_xml_to_model("simple-handwriting-zh_CN.xml");
    }
    else
    {
        shwr::convert_xml_to_model(argv[1]);
    }

    return 0;
}
