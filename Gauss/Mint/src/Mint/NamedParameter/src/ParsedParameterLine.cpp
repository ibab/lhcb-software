// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:17:56 GMT
#include "Mint/ParsedParameterLine.h"
#include <cctype>
#include <iostream>

#include <algorithm>

using namespace MINT;

const std::string ParsedParameterLine::_commentStart="//";
const std::string ParsedParameterLine::_testString="    \"para Name\"  78    70 80 90   100 101    10    11     12\\\\ comment       ";

const std::string ParsedParameterLine::_testName="para Name";

// note: valide parameter names start with a letter (isalpha())
using namespace std;

bool ParsedParameterLine::isBlank(char c){
  return c==' ' || c=='\t' || c=='\n';
}
std::string ParsedParameterLine::removeLeadingBlanks(const string& s){
  if(s.empty()) return s;
  std::string::const_iterator firstNonBlank=s.begin();
  for(firstNonBlank=s.begin();
      firstNonBlank != s.end() && (isBlank(*firstNonBlank));
      firstNonBlank++) ;
  //  cout << "firstNonBlank: " << *firstNonBlank << "." << endl;

  string new_s(firstNonBlank, s.end());
  return new_s;
}
std::string ParsedParameterLine::removeTrailingBlanks(const string& s){
  if(s.empty()) return s;
  std::string::const_iterator lastNonBlank = s.end();
  for(string::const_iterator it=s.begin();
      it != s.end(); it++){
    if(! isBlank(*it)) lastNonBlank = it;
  }
  if(lastNonBlank != s.end()) lastNonBlank++;

  string new_s(s.begin(), lastNonBlank);
  return new_s;
}
std::string ParsedParameterLine::removeLeadingAndTrailingBlanks(const string& s){
  return removeTrailingBlanks(removeLeadingBlanks(s));
}

string ParsedParameterLine::removeComment(const string& s_in){
  if(s_in.empty()) return s_in;
  string s = s_in;
  int commentStart = s.find(_commentStart);
  //cout << " commentStart " << commentStart << endl;
  if(commentStart >= 0 && commentStart < (int) s.size()){
    s.resize(commentStart);
  }
  return s;
}

string ParsedParameterLine::removeCommentAndLeadingBlanks(const string& s_in){
  return removeComment(removeLeadingBlanks(s_in));
}

bool ParsedParameterLine::isValid() const{
  return (! _parsedStrings.empty()) && isalpha(name()[0]);
}

int ParsedParameterLine::makeParsedStrings(const std::string& line
					 , std::vector<std::string>& fillThisList){
  
  string s = removeLeadingBlanks(
              removeTrailingBlanks(
	       removeComment(line
			     )));

  if(s.empty()) return 0;

  s.push_back(' '); // makes sure we get last element
  s = " " + s;      // makes things easier when we start with quotes.
  string::const_iterator prev=s.begin();
  bool prevBlank=true;
  bool insideQuotes=false;
  // quotes are treated like blanks
  // but blanks are ignored if we 
  // are inside quotes... makes sense?

  bool ignore=false; 
  // NOT ANYMORE: we ignore things inside sqare brackets []
  //   to be compatible with Mikhail's ASCII file
  //   format.
  
  for(string::const_iterator it=s.begin();
      it != s.end(); it++){
    //if((! insideQuotes) && *it == '[') ignore=true;
    if( ((! insideQuotes) && isBlank(*it)) || *it == '"' || ignore){
      if(! prevBlank){
	string tmp_s(prev, it);
	fillThisList.push_back(tmp_s);
      }
      prevBlank=true;
    }else{
      if(prevBlank) prev=it;
      prevBlank=false;
    }
    //if((!insideQuotes) && *it == ']') ignore=false;
    if(*it == '"') insideQuotes = ! insideQuotes;
  }
  if(insideQuotes){
    cout << "WARNING in ParsedParameterLine::makeParsedStrings \n"
	 << " unbalanced quotes in string:\n"
	 << line << endl;
  }
  return fillThisList.size();
}

bool ParsedParameterLine::makeParsedStrings(const std::string& line){
  makeParsedStrings(line, _parsedStrings);
  return ! _parsedStrings.empty();
}

void ParsedParameterLine::print(ostream& os) const{
  for(unsigned int i=0; i<_parsedStrings.size(); i++){
    os << _parsedStrings[i];
    if(i != _parsedStrings.size()-1) os << "; ";
  }
}

void ParsedParameterLine::stringParsingTest(std::ostream& os) const{
  os << "Testing the String Parsing in ParsedParameterLine:" << endl;
  os << "  test string is (w/o the '>' and '<'):\n"
     << "  >" << _testString << "<" << endl;
  string noBlanks = removeLeadingBlanks(_testString);
  os << "  ... removing leading blanks gives:\n"
     << "  >" << noBlanks << "<" << endl;
  os << "  ... removing trailing  blanks gives:\n"
     << "  >" << removeTrailingBlanks(_testString) << "<" << endl;
  string noComment = removeComment(_testString);
  os << "  ... string without comment:\n" 
     << "  >" << noComment << "<"
     << endl;
  os << "  ... string without comment run through removeComment again:\n" 
     << "  >" << removeComment(noComment) << "<"
     << endl;
  os << " ... removeCommentAndLeadingBlanks\n"
     << "  >" << removeCommentAndLeadingBlanks(_testString) << "<"
     << endl;

  vector<string> valueStringList;
  makeParsedStrings(_testString, valueStringList);
  os << "  made value string list:\n";
  for(unsigned int i=0; i< valueStringList.size(); i++){
    os << "  " << i << ") >" << valueStringList[i] << "< " << endl;
  }
  os << "test done." << endl;

  return;
}


std::ostream& operator<<(std::ostream& os, const ParsedParameterLine& ppl){
  ppl.print(os);
  return os;
}

//
