// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Det/DetDesc/DetDesc/IXmlParserSvc.h,v 1.1 2001-05-14 15:13:36 sponce Exp $

#ifndef DETDESC_IXMLPARSERSVC_H
#define DETDESC_IXMLPARSERSVC_H

// Include files
#include <parsers/DOMParser.hpp>
#include <dom/DOM_Document.hpp>


/** @class IXmlParserSvc IXmlParserSvc.h DetDesc/IXmlParserSvc.h
 *
 *  this interface defines a DOM XmlParser service
 *
 *  @author Sebastien Ponce
 */

class IXmlParserSvc {

 public:

  /**
   * This method parses an xml file and produces the corresponding DOM
   * document.
   * @param fileName the name of the file to parse
   * @return the document issued from the parsing
   */
  virtual DOM_Document parse (const char* fileName) = 0;

};

#endif // DETDESC_IXMLPARSERSVC_H
