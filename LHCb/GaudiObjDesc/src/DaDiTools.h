// $Id: DaDiTools.h,v 1.12 2003-11-27 10:22:55 mato Exp $
#ifndef DADITOOLS_H
#define DADITOOLS_H 1

// Some pragmas to avoid warnings in VisualC
#ifdef _WIN32
// Disable warning C4786: identifier was truncated to '255' characters in the 
// debug information
#pragma warning ( disable : 4786 )
// Disable warning C4291: no matching operator delete found; memory will not 
// be freed if initialization throws an exception
#pragma warning ( disable : 4291 )
// Disable warning C4250: inheritance via dominance
#pragma warning ( disable : 4250 )
#endif

// Include files
//#include "GaudiKernel/Kernel.h"

#include <iostream>
#include <string>
#include <vector>

#include "xercesc/sax/SAXParseException.hpp"
#include "xercesc/sax/ErrorHandler.hpp"

#include "xercesc/util/XMLString.hpp"
#include "xercesc/util/XercesDefs.hpp"


/** @class DaDiTools DaDiTools.h
 *
 *
 *  @author Stefan Roiser
 *  @date   13/06/2001
 */


class DaDiTools : public xercesc::ErrorHandler
{

public:
  DaDiTools() {}
  ~DaDiTools() {}

  void warning(const xercesc::SAXParseException& toCatch);
  void error(const xercesc::SAXParseException& toCatch);
  void fatalError(const xercesc::SAXParseException& toCatch);
  void resetErrors();

  static void remSpaces(std::string& word);
  static std::vector<std::string> findWords(std::string,
                                            std::string delim);
  static std::string chooseAccess(const std::string& str);
  static bool isSimple(const std::string& value);
  static bool isFundamental(std::string value);
  static bool isPointer(std::string value);
  static bool isRef(std::string value);
  static bool isArray(std::string value);
  static bool isStdContainer(std::string value);
  static bool isEmpty(const std::string& value);
  static std::string containerType(std::string value);
  static std::string firstUp(const std::string& s);
  static std::string retGetName(const std::string& s);

  static void pushAddImport(const std::string& value);
  static const std::vector<std::string>& addImport();

private:
  static std::vector<std::string> additionalImports;
  static std::string name;

};





#endif // DADITOOLS_H
