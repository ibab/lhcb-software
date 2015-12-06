// $Id: XmlParserSvc.h,v 1.12 2009-05-04 15:03:48 ocallot Exp $
#ifndef DETDESCCNV_XMLPARSERSVC_H
#define DETDESCCNV_XMLPARSERSVC_H

// Include files
#include <map>

#include <xercesc/sax/ErrorHandler.hpp>
#include <xercesc/sax/SAXParseException.hpp>

#include <GaudiKernel/Service.h> 
#include "GaudiKernel/MsgStream.h"

#include "XmlTools/IXmlParserSvc.h"

// Forward and external declarations
class IXmlEntityResolver;
class IDetDataSvc;
class IToolSvc;
class IAlgTool;

template <class TYPE> class SvcFactory;


/** @class XmlParserSvc XmlParserSvc.h DetDescCnv/XmlParserSvc.h
 *
 * A parsing service for Xml files. Besides pure parsing, it also sperforms
 * some caching.
 *
 * @author Sebastien Ponce
 * @author Marco Clemencic
 */
class XmlParserSvc : public Service,
                     virtual public IXmlParserSvc,
                     virtual public xercesc::ErrorHandler {
  
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
  virtual StatusCode queryInterface ( const InterfaceID& riid,
                                      void** ppvInterface );

  /// Initialize the service
  virtual StatusCode initialize();

  /// Finalize the service
  virtual StatusCode finalize();

  ///////////////////////////////////////////////////
  // implementation of the IXmlParserSvc interface //
  ///////////////////////////////////////////////////

  /**
   * This method parses an xml file and produces the corresponding DOM
   * document. The actual document is kept in a cache and locked. The user must
   * call IXmlParserSvc::releaseDoc() when he does not need anymore the document.
   * @param fileName the name of the file to parse
   * @return the document issued from the parsing
   */
  virtual IOVDOMDocument* parse (const char* fileName);

  /**
   * This method parses XML from a string and produces the corresponding DOM
   * document. Like for the DOMDocument generated from a file, this one has to be
   * released with IXmlParserSvc::releaseDoc() too.
   * @param source the string to parse
   * @return the document issued from the parsing
   */
  virtual IOVDOMDocument* parseString (std::string source);

  /**
   * This clears the cache of previously parsed xml files.
   */
  virtual void clearCache();

  /// Method to remove the lock from a document in the cache or to delete the document
  /// generated from a string.
  virtual void releaseDoc(IOVDOMDocument* doc);

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
  virtual void warning( const xercesc::SAXParseException& exception );

  /**
   * Receives notification of a recoverable error. 
   * This corresponds to the definition of "error" in section 1.2 of the W3C
   * XML 1.0 Recommendation. For example, a validating parser would use
   * this callback to report the violation of a validity constraint. The default
   * behaviour is to display the error via the Message service.
   * @param exception the error information encapsulated in a SAX parse
   * exception.
   */
  virtual void error( const xercesc::SAXParseException& exception );

  /**
   * Receives notification of a non-recoverable error. 
   * This corresponds to the definition of "fatal error" in section 1.2 of the
   * W3C XML 1.0 Recommendation. For example, a parser would use this callback
   * to report the violation of a well-formedness constraint. The default
   * behaviour is to display the error via the Message service.
   * @param exception the error information encapsulated in a SAX parse
   * exception. 
   */
  virtual void fatalError( const xercesc::SAXParseException& exception );

  /**
   * Resets the Error handler object on its reuse. 
   * This method helps in reseting the Error handler object implementational
   * defaults each time the Error handler is begun.
   * The default implementation does nothing
   */
  virtual void resetErrors ();


private:
  
  /**
   * Caches the new document, parsed from the given file
   * Since this adds an item into the cache, this may remove another
   * item if the cache was full
   * @param fileName the name of the file that was just parsed
   * @param document the document that is the result of the parsing
   */
  IOVDOMDocument* cacheItem (std::string fileName, std::unique_ptr<IOVDOMDocument> document);

  /**
   * this only increases the age of the cache.
   * It also checks that the age don't go back to 0. If it is the case,
   * it puts back every item birthDate to 0 also
   */
  void increaseCacheAge ();

  /// Return the pointer to the detector data service (loading it if not yet done).
  IDetDataSvc *detDataSvc();

  /// Utilities to print...
  MsgStream& info()    { return *m_msg << MSG::INFO; }
  
  MsgStream& debug()   { return *m_msg << MSG::DEBUG; }
  
  MsgStream& warning() { return *m_msg << MSG::WARNING; }

  MsgStream& error()   { return *m_msg << MSG::ERROR; }  
  
  inline bool msgLevel( const MSG::Level level ) const { return m_msg->level() <= level; }

private:

  /// the actual DOM parser
  std::unique_ptr<xercesc::XercesDOMParser> m_parser;

  /**
   * this is a parameter that defines the cache behavior.
   * the default behavior is that an item is released if there is no more space
   * and if it has the smallest birthDate+cacheAgressivity*utility score.
   * Thus, a 0 value allows to have a FIFO cache behavior, while a bigger
   * value tends to keep only reused items.
   */
  unsigned int m_cacheBehavior;

  /**
   * a structure containing a cached document, its birthDate and its utility.
   * The birthDate is the age of the cache when this item arrived.
   * The utility is the number of times this item was retrieved since it is
   * in the cache.
   * The rule is that an item is released if there is no more space,
   * if it has the smallest birthDate+cacheBehavior*utility score and is not locked.
   */
  struct cachedItem final {
    std::unique_ptr<IOVDOMDocument> document;
    unsigned int birthDate, utility;
    int lock;
  };

  /**
   * This is the type of the cache : a map of cachedItems,
   * indexed by a fileName, given as a standard string
   */
  typedef std::map<std::string, cachedItem> cacheType;

  /// a map for caching DOMDocuments
  cacheType m_cache;

  /**
   * The age of the cache : this is the number of operations done
   * on it, provided that an operation is either retrieving an element
   * or puting a new one (eventually removing another one but this is
   * not taken into account
   */
  unsigned int m_cacheAge = 0;
  
  /// The maximum number of cached documents
  unsigned int m_maxDocNbInCache;

  /// Name of the service which will provide the xercesc::EntityResolver pointer (option "EntityResolver").
  std::string m_resolverName;

  /// Pointer to the IXmlEntityResolver tool interface (for bookkeeping).
  IAlgTool *m_resolverTool = nullptr;
  
  /// Pointer to the IXmlEntityResolver.
  SmartIF<IXmlEntityResolver> m_resolver;

  /// Name of the service which will provide the event time (option "DetectorDataSvc", default = "DetectorDataSvc").
  std::string m_detDataSvcName;
  
  /// Pointer to the detector data service
  SmartIF<IDetDataSvc> m_detDataSvc;
  
  /// Pointer to the ToolSvc.
  SmartIF<IToolSvc> m_toolSvc;

  /// Pointer to a message stream
  std::unique_ptr<MsgStream> m_msg;

  /// Flag to decide if we measure time...
  bool m_measureTime;
  bool m_printTime;
  double m_sumCpu = 0;
  double m_sumClock = 0;
};

#endif    // DETDESCCNV_XMLPARSERSVC_H
