/// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Det/DetDesc/DetDesc/XmlGenericCnv.h,v 1.2 2001-01-22 09:55:38 ibelyaev Exp $
#ifndef DETDESC_XMLCNVSVC_XMLGENERICCNV_H
#define DETDESC_XMLCNVSVC_XMLGENERICCNV_H

/// Include files
#include <sax/HandlerBase.hpp>

#include "GaudiKernel/Converter.h"

#include "DetDesc/IXmlCnv.h"

/// Forward and external declarations
class ISvcLocator;
class GenericAddress;

class SAXParser;

class ISax8BitDocHandler;
class XmlCnvAttrList;
class XmlAddress;


extern unsigned const char    XML_StorageType;

template <class TYPE> class CnvFactory;

class XmlGenericCnv : public Converter, virtual public IXmlCnv, public HandlerBase
{
public:
  
  /// -----------------------------------------------------------------------
  ///  Implementations of the Converter interface
  /// -----------------------------------------------------------------------
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

  /// 
  ///  virtual unsigned char repSvsType() const { return XML_StorageType; } 

  static const unsigned char& storageType();
 
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
  void notationDecl
    (
    const   XMLCh* const    name
    , const XMLCh* const    publicId
    , const XMLCh* const    systemId
    );
  
  void unparsedEntityDecl
    (
    const   XMLCh* const    name
    , const XMLCh* const    publicId
    , const XMLCh* const    systemId
    , const XMLCh* const    notationName
    );
  
  // -----------------------------------------------------------------------
  //  Implementation of the IXmlCnv interface
  // -----------------------------------------------------------------------
  /// We need for each run a new SAX so...
  virtual StatusCode initParser();
  virtual StatusCode finiParser();

  /// Calling this method uses inside try{...}catch(...){...} block
  virtual StatusCode parse( const char* fileName );

protected:
  
  /// -----------------------------------------------------------------------
  ///  Constructors
  /// -----------------------------------------------------------------------
  XmlGenericCnv( ISvcLocator* svc,const CLID& clid );
  
  ~XmlGenericCnv()    {
  }

  /// The receipt for us that we need to store for the object
  /// we want to look for in Xml data
  GenericAddress*       m_objRcpt;
  DataObject*           m_dataObj;

  /// Level of nesting
  unsigned int          m_level;

  /// Set to true if the given data object has been located in the data file
  bool                  m_doFound;
  
  /// Dirty Xml worker and SAX event provider
  SAXParser*            m_xmlParser;

  /// Indicator whether the XML tag data should be sent to ASCII client or not
  bool                  m_send;
 
  /// Name of the tag which is start and the end of controlled tag sequence
  std::string           m_tag;

};

#endif // DETDESC_XMLCNVSVC_XMLGENERICCNV_H









