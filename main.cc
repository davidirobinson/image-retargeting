//
// Author: David Robinson
// Date: 04/07/19
//

#include <string>
// #include <stdio.h>
#include <unistd.h>
#include <iostream>

void print_usage()
{
    std::cout << "USAGE: ./main -i <path-to-image>" << std::endl;
}

int main(int argc, char *argv[])
{
    int option = -1;
    std::string img_path;

    while ((option = getopt (argc, argv, "i:")) != -1)
    {
        switch (option)
        {
            case 'i':
                img_path = std::string(optarg);
                break;
            default:
                break;
        }
    }

    if (img_path.empty())
    {
        print_usage(); 
        return 1;
    }

    std::cout << img_path << std::endl;

    return 0;
}