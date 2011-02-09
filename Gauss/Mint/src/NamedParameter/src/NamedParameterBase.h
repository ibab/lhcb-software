#ifndef NAMED_PARAMETER_BASE_HH
#define NAMED_PARAMETER_BASE_HH
// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:17:56 GMT

#include <string>
#include <istream>
#include <sstream>
#include <iostream>
#include <vector>

#include "INamedParameter.h"

// forward declaration
namespace MINT{
  class NamedParameterBase;
}

// ... makes this possible as friend of NamedParameterBase
std::stringstream& operator>>(std::stringstream& is
			      , MINT::NamedParameterBase& npb);

// back to main definitions/declarations:
namespace MINT{

class ParsedParameterFile;
class ParsedParameterFileList; //forward declaration

class NamedParameterBase : virtual public INamedParameter{
 protected:
  static bool IveSaidItOnce;
  std::string _name;
  //  static ParsedParameterFile* _ppF;
  ParsedParameterFile* _ppF;
  static ParsedParameterFileList* getFlist();

  virtual bool setFromParsedLine(const ParsedParameterLine& line) = 0;
  virtual bool setFromParsedFile();
  bool _gotInitialised;

  bool _quiet;

 public:
  enum VERBOSITY{VERBOSE=0, QUIET=1};

  bool setPPF(ParsedParameterFile* ppF_in = 0);
  bool read(std::istream& is = std::cin);
  bool read(const std::string& fname);

  bool reloadFile(const std::string& id);

  bool setFromInitString(const std::string& str);
  bool setFromInitStringNoName(const std::string& str_in);

  NamedParameterBase(const std::string& name_in=""
		     , const char* fname=0
		     , VERBOSITY = VERBOSE);
  NamedParameterBase(const NamedParameterBase& other);
  virtual ~NamedParameterBase();

  virtual const std::string& name() const{
    return _name;
  }

  void quiet(bool beSo=true){_quiet = beSo;}
  bool gotInitialised() const{return _gotInitialised;}

  friend std::stringstream& ::operator>>(std::stringstream& is
				       , MINT::NamedParameterBase& npb);
};

}//namespace MINT


#endif
//
