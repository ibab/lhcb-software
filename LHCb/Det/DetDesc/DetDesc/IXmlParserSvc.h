// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Det/DetDesc/DetDesc/IXmlParserSvc.h,v 1.5 2001-11-20 15:22:20 sponce Exp $

#ifndef DETDESC_IXMLPARSERSVC_H
#define DETDESC_IXMLPARSERSVC_H

// Include files
#include "GaudiKernel/IInterface.h"
#include <parsers/DOMParser.hpp>
#include <dom/DOM_Document.hpp>

/// Declaration of the interface ID (interface id, major version, minor version)
static const InterfaceID IID_IXmlParserSvc(131, 1 , 0);


/** @class IXmlParserSvc IXmlParserSvc.h DetDesc/IXmlParserSvc.h
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
  virtual DOM_Document parse (const char* fileName) = 0;

  /**
   * This method parses XML from a string and produces the corresponding DOM
   * document.
   * @param source the string to parse
   * @return the document issued from the parsing
   */
  virtual DOM_Document parseString (std::string source) = 0;

  /**
   * This clears the cache of previously parsed xml files.
   */
  virtual void clearCache() = 0;

};

#endif // DETDESC_IXMLPARSERSVC_H
