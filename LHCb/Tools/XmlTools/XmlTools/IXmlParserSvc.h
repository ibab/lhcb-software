// $Id: IXmlParserSvc.h,v 1.3 2003-04-25 08:53:56 sponce Exp $

#ifndef DETDESCCNV_IXMLPARSERSVC_H
#define DETDESCCNV_IXMLPARSERSVC_H

// Include files
#include "GaudiKernel/IInterface.h"
#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/dom/DOMDocument.hpp>

/// Declaration of the interface ID (interface id, major version, minor version)
static const InterfaceID IID_IXmlParserSvc(131, 1 , 0);


/** @class IXmlParserSvc IXmlParserSvc.h DetDescCnv/IXmlParserSvc.h
 *
 *  this interface defines a DOM XmlParser service
 *
 *  @author Sebastien Ponce
 */

class IXmlParserSvc : virtual public IInterface {

 public:

  /// Retrieve interface ID
  static const InterfaceID& interfaceID() { return IID_IXmlParserSvc; }

  /**
   * This method parses an xml file and produces the corresponding DOM
   * document.
   * @param fileName the name of the file to parse
   * @return the document issued from the parsing
   */
  virtual xercesc::DOMDocument* parse (const char* fileName) = 0;

  /**
   * This method parses XML from a string and produces the corresponding DOM
   * document.
   * @param source the string to parse
   * @return the document issued from the parsing
   */
  virtual xercesc::DOMDocument* parseString (std::string source) = 0;

  /**
   * This clears the cache of previously parsed xml files.
   */
  virtual void clearCache() = 0;

};

#endif // DETDESCCNV_IXMLPARSERSVC_H
