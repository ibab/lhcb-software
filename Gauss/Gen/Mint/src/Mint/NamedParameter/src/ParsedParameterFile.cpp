// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:17:56 GMT

#include "Mint/ParsedParameterFile.h"
#include <fstream>
#include <iostream>

#include "Mint/Utils.h"
using namespace std;
using namespace MINT;

const char ParsedParameterFile::_ignoreLinesStartingWith[] = {'*', '#', '\0'};
const ParsedParameterLine ParsedParameterFile::dummyLine;

ParsedParameterFile::ParsedParameterFile(istream& inputStream)
  : _statusOK(true)
{
  if(inputStream.bad()){
    cout << "ERROR in ParsedParameterFile constructor from inputStream"
	 << "\n  > status of input stream is bad!"
	 << endl;
  }else{
    readStream(inputStream);
    _statusOK = false;
  }
}

ParsedParameterFile::ParsedParameterFile(const std::string& fname)
  : _statusOK(false)
{
  std::ifstream is(fname.c_str());
  if(is.bad()){
    cout << "ERROR in ParsedParameterFile constructor from filename:"
	 << "\n  > status of input stream is bad - file might not exist"
	 << "\n  > or be otherwise corrupt."
	 << "\n  > The file name you passes was: \"" << fname << "\"."
	 << endl;
  }else{
    readStream(is);
    _statusOK=false;
  }
  is.close();
}

ParsedParameterFile::ParsedParameterFile(const ParsedParameterFile& other)
  : _lines(other._lines)
  , _statusOK(other._statusOK)
{

}

unsigned int ParsedParameterFile::numLines() const{
  return _lines.size();
}
bool ParsedParameterFile::OK() const{
  return _statusOK;
}
bool ParsedParameterFile::ignoreThisLine(const std::string& line){
  if(line.empty())return true;
  for(int i=0; _ignoreLinesStartingWith[i] != '\0'; i++){
    if(line[0] == _ignoreLinesStartingWith[i]) return true;
  }
  return false;
}


void ParsedParameterFile::readStream(std::istream& is){
  const int slmax=1000;
  char line[slmax]={'\0'};
  while( is.getline(line, slmax)){
    //    std::cout << " just read: " << line << "." << std::endl;
    if(this->ignoreThisLine(line)) continue;
    ParsedParameterLine pl(line);
    if(pl.isValid()){
      _lines[pl.name()]=pl;
    }
  }
}

const ParsedParameterLine&
ParsedParameterFile::find(const std::string& name) const{
  return keyFinder(name, _lines, dummyLine);
}

void ParsedParameterFile::print(std::ostream& os) const{
  int i=0;
  for(map<string, ParsedParameterLine>::const_iterator it = _lines.begin();
      it != _lines.end();
      it++, i++){
    os << " " << i << ") " << it->second << endl;
  }
}

std::ostream& operator<<(std::ostream& os, const ParsedParameterFile& ppf){
  ppf.print(os);
  return os;
}


//
