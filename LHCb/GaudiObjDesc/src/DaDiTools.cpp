// $Id: DaDiTools.cpp,v 1.5 2002-01-28 18:49:18 mato Exp $


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
char* DaDiTools::chooseAccess(const DOMString& s)
//-----------------------------------------------------------------------------
{
  std::string str = s.transcode();
  if (str == "PUBLIC") {return "public";}
  else if (str == "PROTECTED") {return "protected";}
  else if (str == "PRIVATE") {return "private";}
  else {return "";}
}

bool DaDiTools::isSimple(std::string value)
{
  int i = value.find_last_of(" ");
  value = value.substr(i+1, value.size()-i);
  if ((value == "bool")   || (value == "short")   ||
      (value == "long")   || (value == "int")     || 
      (value == "float")  || (value == "double")  || 
      (value == "char")   || (value == "unsigned") ||
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