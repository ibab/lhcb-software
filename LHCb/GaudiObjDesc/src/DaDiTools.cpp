// $Id: DaDiTools.cpp,v 1.9 2002-02-22 16:52:22 mato Exp $


// Include files
#include <iostream>
#include <fstream>
#include <string>
#include <dom/DOMString.hpp>

#include "DaDiTools.h"

//-----------------------------------------------------------------------------
// Implementation file for class : DadiTools
//
// 13/06/2001 : 
//-----------------------------------------------------------------------------

// Global streaming operator for DOMString is defined in DOMPrint.cpp
//extern std::ostream& operator<<(std::ostream& target, const DOMString& s);


std::vector<std::string> DaDiTools::additionalImports;

//-----------------------------------------------------------------------------
std::string DaDiTools::chooseAccess(const std::string& str)
//-----------------------------------------------------------------------------
{
  if (str == "PUBLIC") {return "public";}
  else if (str == "PROTECTED") {return "protected";}
  else if (str == "PRIVATE") {return "private";}
  else {return "";}
}

//-----------------------------------------------------------------------------
bool DaDiTools::isSimple(std::string value)
//-----------------------------------------------------------------------------
{
  if (isPointer(value) || isFundamental(value))
  { 
    return true; 
  }
  else
  {
    return false;
  }
}

//-----------------------------------------------------------------------------
bool DaDiTools::isFundamental(std::string value)
//-----------------------------------------------------------------------------
{
  if ((value == "bool")  || (value == "short")    ||
      (value == "long")  || (value == "int")      || 
      (value == "float") || (value == "double")   || 
      (value == "char")  || (value == "unsigned") ||
      (value == "signed"))
  {
    return true;
  }
  else
  {
    return false;
  }
}

//-----------------------------------------------------------------------------
bool DaDiTools::isPointer(std::string value)
//-----------------------------------------------------------------------------
{
  int i = value.find_last_of(" ");
  value = value.substr(i+1, value.size()-i);
  if (value.substr(value.size()-1, value.size()).compare("*") == 0)
  {
    return true;
  }
  return false;
}

//-----------------------------------------------------------------------------
bool DaDiTools::isRef(std::string value)
//-----------------------------------------------------------------------------
{
  int i = value.find_last_of(" ");
  value = value.substr(i+1, value.size()-i);
  if (value.substr(value.size()-1, value.size()).compare("&") == 0)
  {
    return true;
  }
  return false;
}

//-----------------------------------------------------------------------------
bool DaDiTools::isArray(std::string value)
//-----------------------------------------------------------------------------
{
  if (value.substr(value.size()-1, value.size()).compare("]") == 0)
  {
    return true;
  }
  else
  {
    return false;
  }
}

//-----------------------------------------------------------------------------
bool DaDiTools::isStdContainer(std::string value)
//-----------------------------------------------------------------------------
{
  int pos;
  pos = value.find("<");
  value = value.substr(0,pos);
  pos = value.find("::");
  if (pos < value.size())
  {
    value = value.substr(pos+2, value.size());
  }
  if ((value == "vector") || (value == "list")  ||
      (value == "deque")  || (value == "queue") ||
      (value == "stack")  || (value == "map")   ||
      (value == "set")    || (value == "bitset"))
  {
    return true;
  }
  else
  {
    return false;
  }
}

//-----------------------------------------------------------------------------
std::string DaDiTools::containerType(std::string value)
//-----------------------------------------------------------------------------
{
  int pos1, pos2;
  pos1 = value.find("<");
  pos2 = value.find_last_of(">");
  pos1++;
  value = value.substr(pos1, pos2-pos1);
  while (value.substr(value.size()-1, 1).compare(" ") == 0)
  {
    value = value.substr(0, value.size()-1);
  }
  return value;
}

//-----------------------------------------------------------------------------
void DaDiTools::warning(const SAXParseException&)
//-----------------------------------------------------------------------------
{
  //
  // Ignore all warnings.
  //
}

//-----------------------------------------------------------------------------
void DaDiTools::error(const SAXParseException& toCatch)
//-----------------------------------------------------------------------------
{
  std::cerr << "Error at file \"" << DOMString(toCatch.getSystemId()).transcode()
   << "\", line " << toCatch.getLineNumber()
   << ", column " << toCatch.getColumnNumber()
     << "\n   Message: " << DOMString(toCatch.getMessage()).transcode() << std::endl;
  throw SAXParseException(toCatch);  // Copy the 'toCatch' object before 
                     // throwing - otherwise we would be 
                     // throwing a reference to a local object
                     // that gets destroyed before the catch.
}

//-----------------------------------------------------------------------------
void DaDiTools::fatalError(const SAXParseException& toCatch)
//-----------------------------------------------------------------------------
{
  std::cerr << "Fatal Error at file \"" << DOMString(toCatch.getSystemId()).transcode()
   << "\", line " << toCatch.getLineNumber()
   << ", column " << toCatch.getColumnNumber()
     << "\n   Message: " << DOMString(toCatch.getMessage()).transcode() << std::endl;
  throw SAXParseException(toCatch);
}

//-----------------------------------------------------------------------------
std::string DaDiTools::firstUp(const std::string& s)
//-----------------------------------------------------------------------------
{ 
  std::string r;
  r = toupper(s[0]);
  r += s.substr(1,std::string::npos);
  return r;
}

//-----------------------------------------------------------------------------
void DaDiTools::resetErrors()
//-----------------------------------------------------------------------------
{
  // No-op in this case
}

//-----------------------------------------------------------------------------
void DaDiTools::pushAddImport(std::string value)
//-----------------------------------------------------------------------------
{
  additionalImports.push_back(value);
}

//-----------------------------------------------------------------------------
const std::vector<std::string>& DaDiTools::addImport()
//-----------------------------------------------------------------------------
{
  return additionalImports;
}