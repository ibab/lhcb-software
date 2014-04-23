// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:17:56 GMT

#include "Mint/NamedParameterBase.h"
#include "Mint/ParsedParameterFileList.h"
#include "Mint/ParsedParameterFile.h"

using namespace std;
using namespace MINT;

//ParsedParameterFile* NamedParameterBase::_ppF=0;

bool NamedParameterBase::IveSaidItOnce=0;

std::string NamedParameterBase::_defaultFile = ""; // if "", read default-stream
std::istream* NamedParameterBase::_defaultStreamPtr = &std::cin;


bool NamedParameterBase::setDefaultInputFile(const std::string& fname){
  _defaultFile = fname;
  return true;
}

bool NamedParameterBase::setDefaultInputStream(std::istream& str){
  _defaultFile="";
  _defaultStreamPtr = &str;
  return true;
}



bool NamedParameterBase::reloadFile(const std::string& id){
  bool success = true;
  success |= (bool) getFlist()->reloadFile(id);
  success |= read(id);
  success |= setFromParsedFile();
  return success;
}

NamedParameterBase::NamedParameterBase(const std::string& name_in
				       , const char* fname
				       , NamedParameterBase::VERBOSITY vb
				       )
  : INamedParameter()
  , _name(name_in)
  , _gotInitialised(false)
  , _quiet(vb==NamedParameterBase::QUIET)
{
  _name = ParsedParameterLine::removeLeadingBlanks(
	   ParsedParameterLine::removeTrailingBlanks(
	    _name));
  if(0 == fname){
    if((! _quiet) && 0 == IveSaidItOnce){
      IveSaidItOnce=true;
      cout << "INFO in constructor of NamedParameterBase:"
	   << "\n  > For parameter named: \"" << _name << "\""
	   << "\n  > You did not provide a filename."
	   << "\n  > I'll read from stdin. This is completely OK,"
	   << "\n  > but if there is no input from stdin ending in EOF,"
	   << "\n  > the program will stall after this message."
	   << "\n  > If you find yourself in this situation, press Ctr-d."
	   << "\n  > In future, run the program as ./program < inputFile.txt"
	   << "\n  > even if inputFile.txt is just an empty dummy file."
	   << "\n  > This message is only printed once."
	   << endl;
    }
    NamedParameterBase::read();
  }else{
    NamedParameterBase::read(fname);
  }
}
NamedParameterBase::NamedParameterBase(const NamedParameterBase& other)
  : INamedParameter()
  , _name(other._name)
  , _ppF(other._ppF)
  , _gotInitialised(other._gotInitialised)
  , _quiet(other._quiet)
{}

NamedParameterBase::~NamedParameterBase(){
  if( (! _gotInitialised) && (! _quiet)){
    cout << "WARNING in destructor of NamedParameterBase:"
	 << "\n  > Hello. My name is \"" << _name << "\"."
	 << "\n  > I'm being destroyed, and"
	 << "\n  > I never even got initialised." 
	 << "\n  > Did you do that deliberatly?"
	 << endl;
  }
}

ParsedParameterFileList* NamedParameterBase::getFlist(){
  return ParsedParameterFileList::getMe();
}

bool NamedParameterBase::setPPF(ParsedParameterFile* ppF_in){
  //  if(0 != _ppF) delete _ppF;
  // above now managed by ParsedParameterFileList
  _ppF = ppF_in;
  return true;
}

bool NamedParameterBase::read(){
  if(! ("" == _defaultFile)) return read(_defaultFile);
  else return read(*_defaultStreamPtr);
}
bool NamedParameterBase::read(std::istream& is){
  _ppF = getFlist()->getParsedFile(is);
  if(0 == _ppF){
    cout << "SERIOUS ERROR IN NamedParameterBase::read(std::istream& is)"
	 << "\n  > call to ParsedParameterFileList::getParsedFile"
	 << "\n  > return 0 pointer. This is bad. Let's see how long"
	 << "\n  > I'll last..." << endl;
    return false;
  }
  return true;
}

bool NamedParameterBase::read(const std::string& fname){
  _ppF = getFlist()->getParsedFile(fname);
  if(0 == _ppF){
    cout << "SERIOUS ERROR IN NamedParameterBase::read(const std::string&)"
	 << "\n  > with string value = " << fname << "."
	 << "\n  > Call to ParsedParameterFileList::getParsedFile"
	 << "\n  > return 0 pointer. This is bad. Let's see how long"
	 << "\n  > I'll last..." << endl;
    return false;
  }
  return true;
}

bool NamedParameterBase::setFromParsedFile(){
  bool success=false;
  if(0 == _ppF){
    if((! _gotInitialised) && (! _quiet)){
      cout << "WARNING in  NamedParameterBase::setFromParsedFile():"
	   << "\n No ParsedParameterFile attached!." << endl;
    }
    return false;
  }
  const ParsedParameterLine& ppL =_ppF->find(name());
  if(! ppL.isValid()){
    if(! _quiet){
      cout << "WARNING in  NamedParameterBase::setFromParsedFile():"
	   << "\n  > Can't find variable with name \"" << name() << "\""
	   << "\n  > in attached ParsedParameterFile." << endl;
    }
  }else{
    success = setFromParsedLine(ppL);
  }
  if(success) _gotInitialised = true;
  return success;
}

bool NamedParameterBase::setFromInitString(const std::string& str){
  ParsedParameterLine ppl(str);
  return this->setFromParsedLine(ppl);
}
bool NamedParameterBase::setFromInitStringNoName(const std::string& str_in){
  std::string str = name() + "\t" + str_in;
  ParsedParameterLine ppl(str);
  return this->setFromParsedLine(ppl);
}

std::stringstream& operator>>(std::stringstream& is
			      , MINT::NamedParameterBase& npb){
  std::string str;
  is >> str;
  npb.setFromInitStringNoName(str);
  return is;
}

//
