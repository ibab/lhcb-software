/// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Det/DetDesc/DetDesc/XmlUserDeCnv.h,v 1.1.1.1 2000-11-23 13:44:44 ranjard Exp $
#ifndef DDEXAMPLE_XMLUSERDECNV_H
#define DDEXAMPLE_XMLUSERDECNV_H 1

#include "GaudiKernel/Converter.h"
#include "GaudiKernel/DetectorElement.h"

#include "DetDesc/IUserSax8BitDocHandler.h"

/// Forward and external declarations
class     ISvcLocator;
class     XmlCnvAttributeList;
class     XmlDetectorElementCnv;
class     IUserSax8BitDocHandler;
class     IXmlSvc;

template <class TYPE> class CnvFactory;

template <class DeType>
class XmlUserDeCnv : public Converter, virtual public IUserSax8BitDocHandler
{
  typedef XmlUserDeCnv<DeType> MyType;

  // Friend needed for instantiation
  friend class CnvFactory<MyType>;

public:
  ///  Constructors
  XmlUserDeCnv( ISvcLocator* svc, std::string& typeName );
  XmlUserDeCnv( ISvcLocator* svc, char* typeName );
  XmlUserDeCnv( ISvcLocator* svc, const char* typeName );

  ///  Destructor
  virtual ~XmlUserDeCnv();
  
  static const unsigned char& storageType();
  static const CLID& classID();

  // -----------------------------------------------------------------------
  /// Implementations of the Converter interface
  // -----------------------------------------------------------------------
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
  /// Implementation of the User SAX DocumentHandler interface
  // -----------------------------------------------------------------------
  
  /// Start of the XML document callback
  virtual void uStartDocument()
  {
  }

  /// End of the XML document callback
  virtual void uEndDocument()
  {
  }
  
  /// Parsed character data callback
  virtual void uCharacters( const char* const chars, const unsigned int length );
  
  /// White space characters callback
  virtual void uIgnorableWhitespace( const char* const chars, const unsigned int length );
  
  /// Start of the XML element callback
  virtual void uStartElement( const char* const name, XmlCnvAttributeList& attributes );
  
  /// End of the XML element callback
  virtual void uEndElement( const char* const name );

  /// Processing instruction callback
  virtual void uProcessingInstruction( const char* const target, const char* const data );

  /// Access to the IXmlSvc interface of the XmlCnvSvc service
  IXmlSvc* xmlSvc();

protected:

  /// Our "quazi base class" generic detector element converter
  IConverter*       m_deCnv;

  /// We have to keep the given DataObject pointer to be visible inside callbacks
  DeType*           m_dataObj;

  /// String holding the class name Id
  std::string       m_msId;

  /// Pointer to IXmlSvc interface
  IXmlSvc*          m_xmlSvc;

};

#include "XmlUserDeCnv.icpp"


#endif // DDEXAMPLE_XMLUSERDECNV_H
