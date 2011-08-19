// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:17:56 GMT
#ifndef PARSED_PARAMETER_FILE
#define PARSED_PARAMETER_FILE

#include <map>
#include <string>
#include <istream>
#include <iostream>

#include "Mint/ParsedParameterLine.h"

namespace MINT{

class ParsedParameterFile{
 protected:
  static const ParsedParameterLine dummyLine;// will be invalid;
  static const char _ignoreLinesStartingWith[];

  std::map<std::string, ParsedParameterLine> _lines;
  bool _statusOK;


  static bool ignoreThisLine(const std::string& line);
  void readStream(std::istream& is);
 public:
  ParsedParameterFile(std::istream& inputStream = std::cin);
  ParsedParameterFile(const std::string& fname);
  ParsedParameterFile(const ParsedParameterFile& other);

  unsigned int numLines() const; // number of lines;
  bool OK()const;

  const ParsedParameterLine& find(const std::string& name) const;

  void print(std::ostream& os=std::cout) const;
};
}// namespace MINT;
std::ostream& operator<<(std::ostream& os, const MINT::ParsedParameterFile& ppf);

#endif
//
