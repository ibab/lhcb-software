/// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Det/DetDesc/DetDesc/XmlGenericCnv.h,v 1.3 2001-01-25 12:12:29 mato Exp $
#ifndef DETDESC_XMLGENERICCNV_H
#define DETDESC_XMLGENERICCNV_H

/// Include files
#include <sax/HandlerBase.hpp>

#include "GaudiKernel/Converter.h"
#include "DetDesc/IXmlCnv.h"

/// Forward and external declarations
class ISvcLocator;
class ISax8BitDocHandler;
class IXmlSvc;
class GenericAddress;
class SAXParser;
class XmlCnvAttrList;
class XmlAddress;
template <class TYPE> class CnvFactory;

/** @class XmlGenericCnv XmlGenericCnv.h DetDesc/XmlGenericCnv.h

    Base class for the XML converters. It encapsulates the generic functionality
    required by any converter of this type. Implements the SAX interface and the
    IXmlSvc.

    @author Radovan Chytracek
    @author Pere Mato
*/
class XmlGenericCnv : public Converter, virtual public IXmlCnv, public HandlerBase {
public:
  /// Initialize the converter
  virtual StatusCode initialize();
  
  /// Initialize the converter
  virtual StatusCode finalize();
  
  /// Create the transient representation of an object.
  virtual StatusCode createObj(IOpaqueAddress* pAddress, DataObject*& refpObject);
  
  /// Update the transient object from the other representation.
  virtual StatusCode updateObj(IOpaqueAddress* pAddress, DataObject* refpObject);
  
  /// Convert the transient object to the requested representation.
  virtual StatusCode createRep(DataObject* pObject, IOpaqueAddress*& refpAddress);
  
  /// Update the converted representation of a transient object.
  virtual StatusCode updateRep(IOpaqueAddress* pAddress, DataObject* pObject);

 
  // -----------------------------------------------------------------------
  //  Implementations of the SAX DocumentHandler interface
  // -----------------------------------------------------------------------
  void startDocument();
  void endDocument();  
  void characters( const XMLCh* const chars, const unsigned int length );  
  void ignorableWhitespace( const XMLCh* const chars, const unsigned int length );  
  void processingInstruction( const XMLCh* const target, const XMLCh* const data );  
  void startElement( const XMLCh* const name, AttributeList& attributes );  
  void endElement( const XMLCh* const name );

  // -----------------------------------------------------------------------
  //  Implementations of the SAX ErrorHandler interface
  // -----------------------------------------------------------------------
  void warning( const SAXParseException& exception );
  void error( const SAXParseException& exception );
  void fatalError( const SAXParseException& exception );
  
  // -----------------------------------------------------------------------
  //  Implementation of the SAX DTDHandler interface
  // -----------------------------------------------------------------------
  void notationDecl ( const XMLCh* const name, 
                      const XMLCh* const publicId, 
                      const XMLCh* const systemId );
  
  void unparsedEntityDecl ( const XMLCh* const name, 
                            const XMLCh* const publicId, 
                            const XMLCh* const systemId, 
                            const XMLCh* const notationName );
  
  // -----------------------------------------------------------------------
  //  Implementation of the IXmlCnv interface
  // -----------------------------------------------------------------------
  /// We need for each run a new SAX so...
  virtual StatusCode initParser();
  virtual StatusCode finiParser();

  /// Calling this method uses inside try{...}catch(...){...} block
  virtual StatusCode parse( const char* fileName );

  // -----------------------------------------------------------------------
  //  Some Accessors
  //------------------------------------------------------------------------
  /// Access to the IXmlSvc interface of the XmlCnvSvc service
  IXmlSvc* xmlSvc() { return m_xmlSvc; }
  /// Access to the StorageType value 
  static const unsigned char& storageType() { return XML_StorageType; }

protected:
  /// Constructor
  XmlGenericCnv( ISvcLocator* svc,const CLID& clid );
  /// Destructor
  ~XmlGenericCnv() { }

  GenericAddress*  m_objRcpt;   ///< The receipt for us that we need to store for the object
  DataObject*      m_dataObj;   ///< Target DataObject 
  unsigned int     m_level;     ///< Level of nesting
  bool             m_doFound;   ///< Set to true if the given data object has been located in the data file
  SAXParser*       m_xmlParser; ///< Dirty Xml worker and SAX event provider
  bool             m_send;      ///< Indicator whether the XML tag data should be sent to ASCII client or not
  std::string      m_tag;       ///< Name of the tag which is start and the end of controlled tag sequence
  IXmlSvc*         m_xmlSvc;    ///< IXmlSvc reference
};

#endif // DETDESC_XMLGENERICCNV_H









