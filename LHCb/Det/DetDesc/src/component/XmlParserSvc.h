// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Det/DetDesc/src/component/XmlParserSvc.h,v 1.1 2001-05-18 16:48:47 sponce Exp $
#ifndef DETDESC_XMLPARSERSVC_H
#define DETDESC_XMLPARSERSVC_H

// Include files
#include <sax/ErrorHandler.hpp>
#include <sax/SAXParseException.hpp>

#include <GaudiKernel/Service.h> 

#include "DetDesc/IXmlParserSvc.h"

// Forward and external declarations
template <class TYPE> class SvcFactory;


/** @class XmlParserSvc XmlParserSvc.h DetDesc/XmlParserSvc.h
 *
 * A parsing service for Xml files. Besides pure parsing, it also sperforms
 * some caching.
 *
 * @author Sebastien Ponce
 */
class XmlParserSvc : public Service,
                     virtual public IXmlParserSvc,
                     virtual public ErrorHandler {
  
  /// Friend needed
  friend class SvcFactory<XmlParserSvc>;
  
public:

  /**
   * Standard Constructor
   * @param name   String with service name
   * @param svc    Pointer to service locator interface
   */
  XmlParserSvc (const std::string& name, ISvcLocator* svc);

  /**
   * default destructor
   */
  virtual ~XmlParserSvc();

  /**
   * Queries interfaces of Interface.
   * @param riid ID of Interface to be retrieved
   * @param ppvInterface Pointer to Location for interface pointer
   * @return status depending on the completion of the call
   */
  virtual StatusCode queryInterface (const IID& riid, void** ppvInterface);

  ///////////////////////////////////////////////////
  // implementation of the IXmlParserSvc interface //
  ///////////////////////////////////////////////////

  /**
   * This method parses an xml file and produces the corresponding DOM
   * document.
   * @param fileName the name of the file to parse
   * @return the document issued from the parsing
   */
  virtual DOM_Document parse( const char* fileName );

  //////////////////////////////////////////////////////
  // implementation of the SAX ErrorHandler interface //
  //////////////////////////////////////////////////////

  /**
   * Receives notification of a warning. 
   * The parser will use this method to report conditions that are not errors
   * or fatal errors as defined by the XML 1.0 recommendation. The default
   * behaviour is to display the warning via the Message service.
   * @param exception the warning information encapsulated in a SAX parse
   * exception.
   */
  virtual void warning( const SAXParseException& exception );

  /**
   * Receives notification of a recoverable error. 
   * This corresponds to the definition of "error" in section 1.2 of the W3C
   * XML 1.0 Recommendation. For example, a validating parser would use
   * this callback to report the violation of a validity constraint. The default
   * behaviour is to display the error via the Message service.
   * @param exception the error information encapsulated in a SAX parse
   * exception.
   */
  virtual void error( const SAXParseException& exception );

  /**
   * Receives notification of a non-recoverable error. 
   * This corresponds to the definition of "fatal error" in section 1.2 of the
   * W3C XML 1.0 Recommendation. For example, a parser would use this callback
   * to report the violation of a well-formedness constraint. The default
   * behaviour is to display the error via the Message service.
   * @param exception the error information encapsulated in a SAX parse
   * exception. 
   */
  virtual void fatalError( const SAXParseException& exception );

  /**
   * Resets the Error handler object on its reuse. 
   * This method helps in reseting the Error handler object implementational
   * defaults each time the Error handler is begun.
   * The default implementation does nothing
   */
  virtual void resetErrors ();

  
private:

  /// the actual DOM parser
  DOMParser* m_parser;

};

#endif    // DETDESC_XMLPARSERSVC_H
