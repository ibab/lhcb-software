// $Id: DaDiTools.cpp,v 1.2 2001-10-09 17:11:05 mato Exp $


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

char* DaDiTools::chooseAccess(const DOMString& s)
{
  std::string str = s.transcode();
  if (str == "PUBLIC") {return "public";}
  else if (str == "PROTECTED") {return "protected";}
  else if (str == "PRIVATE") {return "private";}
  else {return "";}
}


void DaDiTools::warning(const SAXParseException&)
{
  //
  // Ignore all warnings.
  //
}

void DaDiTools::error(const SAXParseException& toCatch)
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

void DaDiTools::fatalError(const SAXParseException& toCatch)
{
  std::cerr << "Fatal Error at file \"" << DOMString(toCatch.getSystemId()).transcode()
   << "\", line " << toCatch.getLineNumber()
   << ", column " << toCatch.getColumnNumber()
     << "\n   Message: " << DOMString(toCatch.getMessage()).transcode() << std::endl;
  throw SAXParseException(toCatch);
}

void DaDiTools::resetErrors()
{
  // No-op in this case
}




// Standard creator
//DadiTools::DadiTools( ) {
//}
