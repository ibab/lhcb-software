// $Id: StringParse.h,v 1.1.1.1 2005-06-20 21:42:17 robbep Exp $

// --------------------------------------------------
// 
// File:  GeneratorModules/StringParse.h
// Description:
//    This code is used to parse a string.
//    It breaks it into components which are seperated by spaces  
//    in the original.
//    Three methods can then be called to 
//    Return the nth component as a string (piece method)
//    Return the nth component as an integer (intpiece method) 
// if conversion is possible, returns -1 if it is off the end and 0 
// if it cannot be converted
//    Return the nth component as a double (numpiece method)  if conversion 
// is possible, returns -1.1 if it is off the end and 0 if it cannot be 
// converted
// AuthorList:
//         Ian Hinchliffe April 2000

#ifndef GENERATORS_STRINGPARSE_H
#define GENERATORS_STRINGPARSE_H
#include <string>
#include <vector>
class StringParse {
public:
  StringParse( const std::string & input); //constructor
  StringParse(); //constructor
  ~StringParse(); //constructor
  std::string piece(const int & num);
  int intpiece(const int & num); 
  float numpiece(const int & num); 
private:
  std::string m_lstring;
  std::vector<std::string> m_lsubstring;
  int m_nword;
  std::string m_past_end;
};
#endif  //GENERATORS_STRINGPARSE_H
