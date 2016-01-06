// $Id: IXmlParserSvc.h,v 1.6 2007-02-05 18:51:19 marcocle Exp $

#ifndef DETDESCCNV_IXMLPARSERSVC_H
#define DETDESCCNV_IXMLPARSERSVC_H

// Include files
#include "GaudiKernel/IInterface.h"
#include <xercesc/parsers/XercesDOMParser.hpp>
#include "XmlTools/IOVDOMDocument.h"



/** @class IXmlParserSvc IXmlParserSvc.h DetDescCnv/IXmlParserSvc.h
 *
 *  this interface defines a DOM XmlParser service
 *
 *  @author Sebastien Ponce
 *  @author Marco Clemencic
 */

class IXmlParserSvc : public extend_interfaces<IInterface> {

 public:
   DeclareInterfaceID( IXmlParserSvc, 2 , 0);

  /**
   * This method parses an xml file and produces the corresponding DOM
   * document. The actual document is kept in a cache and locked. The user must
   * call IXmlParserSvc::releaseDoc() when he does not need anymore the document.
   * @param fileName the name of the file to parse
   * @return the document issued from the parsing
   */
  virtual IOVDOMDocument* parse (const char* fileName) = 0;

  /**
   * This method parses XML from a string and produces the corresponding DOM
   * document. Like for the DOMDocument generated from a file, this one has to be
   * released with IXmlParserSvc::releaseDoc() too.
   * @param source the string to parse
   * @return the document issued from the parsing
   */
  virtual IOVDOMDocument* parseString (std::string source) = 0;

  /**
   * This clears the cache of previously parsed xml files.
   */
  virtual void clearCache() = 0;

  /// Method to remove the lock from a document in the cache or to delete the document
  /// generated from a string.
  virtual void releaseDoc(IOVDOMDocument* doc) = 0;

};

#endif // DETDESCCNV_IXMLPARSERSVC_H
