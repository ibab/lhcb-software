// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Det/DetDesc/DetDesc/IXmlParserSvc.h,v 1.2 2001-05-18 16:48:42 sponce Exp $

#ifndef DETDESC_IXMLPARSERSVC_H
#define DETDESC_IXMLPARSERSVC_H

// Include files
#include "GaudiKernel/IInterface.h"
#include <parsers/DOMParser.hpp>
#include <dom/DOM_Document.hpp>

/// Declaration of the interface ID (interface id, major version, minor version)
static const InterfaceID IID_IXmlParserSvc(4846, 1 , 0);


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

};

#endif // DETDESC_IXMLPARSERSVC_H
