#ifndef PARSED_PARAMETER_LINE_HH
#define PARSED_PARAMETER_LINE_HH
// author: Jonas Rademacker (Jonas.Rademacker@bristol.ac.uk)
// status:  Mon 9 Feb 2009 19:17:56 GMT

#include <string>
#include <iostream>
#include <vector>
// note: valide parameter names start with a letter (isalpha())

namespace MINT{

class ParsedParameterLine{ 
  // mainly for string handling.
 protected:
  static const std::string _commentStart; 
  static const std::string _testString; 
  static const std::string _testName; 

  std::string _name;
  std::vector<std::string> _parsedStrings;


  static int makeParsedStrings(const std::string& line
				, std::vector<std::string>& fillThisList);

  virtual bool makeParsedStrings(const std::string& line);

 public:
  static bool isBlank(char c);
  static std::string removeLeadingBlanks(const std::string& s);
  static std::string removeTrailingBlanks(const std::string& s);
  static std::string removeLeadingAndTrailingBlanks(const std::string& s);
  static std::string removeComment(const std::string& line);
  static std::string removeCommentAndLeadingBlanks(const std::string& line);

  ParsedParameterLine(const std::string& line=""){
    makeParsedStrings(line);
    if(_parsedStrings.empty()){
      _parsedStrings.push_back("");
    }
  }
  ParsedParameterLine(const ParsedParameterLine& other)
    : _name(other._name)
    , _parsedStrings(other._parsedStrings)
    {}

  virtual ~ParsedParameterLine(){};

  bool isValid() const;

  virtual const std::string& name() const{
    return _parsedStrings[0];
  }
  const std::vector<std::string>& parsedStrings() const{
    return _parsedStrings;
  }

  virtual void print(std::ostream& os = std::cout) const;
  virtual void stringParsingTest(std::ostream& os=std::cout) const;
};
}//namespace MINT
std::ostream& operator<<(std::ostream& os, const MINT::ParsedParameterLine& ppl);

#endif
//
