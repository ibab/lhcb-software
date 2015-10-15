#include "AlignmentMonitoring/ParseConstants.h"
// STL
#include <fstream>
#include <cstdio>
#include <iostream>

using namespace Alignment::AlignmentMonitoring;

// Constructor
ParseConstants::ParseConstants()
{
}

std::map<std::string, double>
ParseConstants::Parse(const char* filename)
{
  std::map<std::string, double> constants;
  std::string line;
  std::ifstream ifile(filename);
  if (ifile.is_open()) {
    while ( std::getline (ifile,line) ) {
      if (line.find("<!--") != std::string::npos ||
          line.size() == 0) continue;
      char name[30], classID[30];
      double Tx, Ty, Tz, Rx, Ry, Rz;
      //      std::cout << line << "\t" << line.size() << std::endl;
      //std::sscanf(line.c_str(),"<condition classID=\"6\" name=\"%[^\"]\"><paramVector name=\"dPosXYZ\" type=\"double\">%lf %lf %lf</paramVector><paramVector name=\"dRotXYZ\" type=\"double\">%lf %lf %lf</paramVector></condition>",name,&Tx,&Ty,&Tz,&Rx,&Ry,&Rz);
      //      std::cout << name << " " << Tx << " " << Ty << " " << Tz << " "  << Rx << " " << Ry<< " " << Rz << std::endl;
      
      std::sscanf( line.c_str(), "<condition classID=\"%[^\"]\" name=\"%[^\"]\">",classID,name); 
      std::sscanf( line.substr(line.find("<paramVector")).c_str(), "<paramVector name=\"dPosXYZ\" type=\"double\">%lf %lf %lf</paramVector><paramVector name=\"dRotXYZ\" type=\"double\">%lf %lf %lf</paramVector></condition>",&Tx,&Ty,&Tz,&Rx,&Ry,&Rz);
      constants[name+std::string(".Tx")]= Tx;
      constants[name+std::string(".Ty")]= Ty;
      constants[name+std::string(".Tz")]= Tz;
      constants[name+std::string(".Rx")]= Rx;
      constants[name+std::string(".Ry")]= Ry;
      constants[name+std::string(".Rz")]= Rz;
    }
    ifile.close();
  }
  return constants;
}
