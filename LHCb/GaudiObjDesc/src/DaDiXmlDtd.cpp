// $Id: DaDiXmlDtd.cpp,v 1.2 2003-04-30 12:04:20 mato Exp $

#include <iostream>
#include <fstream>
#include <string>

//
// Usage()
//
void usage(std::string argV0)
{
  std::cout << std::endl << std::endl;
  std::cout << "Usage: " << argV0 << " [-h] [-v]"
            << std::endl
            << "Produce a .dtd-file in the local directory which "
            << "links to gdd.dtd" << std::endl
            << std::endl
            << "   -h            display this help and exit" << std::endl
            << "   -v            display version information and exit"
            << std::endl
            << std::endl << std::endl;
  exit(0);
}

//
// Versioninfo
//
void version(std::string argV0)
{
  std::cout << std::endl << std::endl
            << argV0 << " version 1.0" << std::endl
            << "written by Stefan Roiser" << std::endl << std::endl;
  exit(0);
}


int main(int argC, char* argV[])
{

#ifdef WIN32
  std::string sep = "\\";
#elif defined(__linux)
  std::string sep = "/";
#endif

  std::string path2gdd;
  char* fileName = "link2gdd.dtd";
  std::cout << "Writing link2gdd.dtd ";
  std::ofstream dtdOut(fileName);

  std::string argV0 = std::string(argV[0]);
  argV0.erase(0,argV0.find_last_of("\\")+1);
  argV0.erase(0,argV0.find_last_of("/")+1);

  if(argC>=2)
  {
    if (strcmp(argV[1], "-v") == 0)
    {
      version(argV0);
    }
    else
    {
      usage(argV0);
    }
  }


  if (getenv("GAUDIOBJDESCROOT") == NULL)
  {
    std::cerr << "Please set Environmentvariable 'GAUDIOBJDESCROOT'"
              << std::endl;
    exit(1);
  }
  else
  {
    path2gdd = std::string(getenv("GAUDIOBJDESCROOT"))
      + sep + "xml_files" + sep + "gdd.dtd";
  }

  dtdOut << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << std::endl
         << "<!ENTITY % link2gdd SYSTEM \"" << path2gdd << "\">" << std::endl
         << "%link2gdd;" << std::endl;

  dtdOut.close();
  delete [] fileName;

  std::cout << " - OK" << std::endl;

  return 0;
}
