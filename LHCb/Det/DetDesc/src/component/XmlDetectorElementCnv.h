///	$Header: /afs/cern.ch/project/cvs/reps/lhcb/Det/DetDesc/src/component/XmlDetectorElementCnv.h,v 1.1 2001-02-05 12:45:53 ranjard Exp $
#ifndef DETDESC_XMLCNVSVC_XMLDECNV_H
#define DETDESC_XMLCNVSVC_XMLDECNV_H

/// Include files
#include "DetDesc/XmlGenericCnv.h"

#include "GaudiKernel/Converter.h"

#include "DetDesc/ISax8BitDocHandler.h"

/// Forward and external declarations
class     ISvcLocator;
class     XmlDetectorElementCnv;
class     IUserSax8BitDocHandler;
template <class TYPE> class CnvFactory;

class XmlDetectorElementCnv : public XmlGenericCnv,
virtual public ISax8BitDocHandler
{
  // Friend needed for instantiation
  friend class CnvFactory<XmlDetectorElementCnv>;
  
public:
  
  virtual StatusCode initialize();

  // -----------------------------------------------------------------------
  //  Implementations of the Converter interface
  // -----------------------------------------------------------------------

  // Create the transient representation of an object.
  virtual StatusCode createObj(IOpaqueAddress* pAddress, DataObject*& refpObject);
  
  // Update the transient object from the other representation.
  virtual StatusCode updateObj(IOpaqueAddress* pAddress, DataObject* refpObject);
  
  // Convert the transient object to the requested representation.
  virtual StatusCode createRep(DataObject* pObject, IOpaqueAddress*& refpAddress);
  
  // Update the converted representation of a transient object.
  virtual StatusCode updateRep(IOpaqueAddress* pAddress, DataObject* pObject);
  
  static const CLID& classID();
  
  // -----------------------------------------------------------------------
  //  Implementations of the SAX 8 bit DocumentHandler interface
  // -----------------------------------------------------------------------
  virtual void startDocument( const char* info );
  
  virtual void endDocument( const char* info );
  
  virtual void processingInstruction( const char* const target, const char* const data );
  
  virtual void startElement( const char* const name, XmlCnvAttributeList& attributes);
  
  virtual void endElement( const char* const name );
  
  virtual void characters( const char* const chars, const unsigned int length );
  
  virtual void ignorableWhitespace( const char* const chars, const unsigned int length );
  
  // Implementation of the obligatory method to provide a hint to generic XML
  // converter about the XML tag we want to be notfied about
  virtual const char* tag() const;
  
protected:
  
  // Standard Constructor
  XmlDetectorElementCnv(ISvcLocator* svcs);
  XmlDetectorElementCnv(ISvcLocator* svc, const CLID& clsID );

  // Standard Destructor
  virtual ~XmlDetectorElementCnv() {}
  
private:
  
  bool                    m_doGenericCnv;  // Whether to do or not the generic conversion
  bool                    m_customData;    // Flag if custom data available
  std::string             m_lvname;        // Name of the associated logical volume
  std::string             m_support;       // Name of the supporting detectot element
  std::string             m_rpath;         // Number replica path
  std::string             m_npath;         // Name replica path
};

#endif // DETDESC_XMLDECNV_H

