#ifndef     DETDESC_XMLTABULATEDPROPERTYCNV_H
#define     DETDESC_XMLTABULATEDPROPERTYCNV_H 1 
/// DetDesc 
#include "DetDesc/XmlGenericCnv.h"
#include "DetDesc/ISax8BitDocHandler.h"
#include "DetDesc/TabulatedProperty.h"
/// Forward declarations
template <class TYPE> class CnvFactory;
///

/** @class XmlTabulatedPropertyCnv XmlTabulatedPropertyCnv.h DetDesc/XmlTabulatedPropertyCnv.h
    
    XmlConverter for conversion of TabulatedProperty object form XML-file.
    It converts "tabproperty" tag.
    @author  Vanya Belyaev
    @date    26/02/2001
*/


class   XmlTabulatedPropertyCnv : public XmlGenericCnv      , 
                                  public ISax8BitDocHandler
{  
  /// Friend needed for instantiation
  friend class CnvFactory<XmlTabulatedPropertyCnv>;
  ///
public:
  ///
  /// Create the transient representation of an object.
  virtual StatusCode createObj(IOpaqueAddress* pAddress, DataObject*& refpObject);
  ///   
  static const unsigned char& storageType() { return XML_StorageType              ; }
  static const CLID&          classID    () { return TabulatedProperty::classID() ; }
  ///
  virtual void startDocument         ( const char*        /* info   */ ){};
  virtual void endDocument           ( const char*        /* info   */ ){};
  virtual void ignorableWhitespace   ( const char* const  /* chars  */ , 
                                       const unsigned int /* length */ );
  virtual void processingInstruction ( const char* const  /* target */ , 
                                       const char* const  /* data   */ ){};
  virtual void characters            ( const char* const  /* chars  */ ,
                                       const unsigned int /* length */ );
  virtual void startElement          ( const char* const     name      , 
                                       XmlCnvAttributeList& attributes );
  virtual void endElement            ( const char* const  /* name   */ );
  ///  
  /// Implementation of the obligatory method to provide a hint to generic XML
  /// converter about the XML tag we want to be notfied about
  virtual const char* tag() const  { return "tabproperty" ; }
  ///
protected:
  ///
  /// Constructor
  XmlTabulatedPropertyCnv( ISvcLocator* svcs );
  ///
  /// Standard Destructor
  virtual ~XmlTabulatedPropertyCnv();
  ///
private:
  ///
  XmlTabulatedPropertyCnv()                                           ; /// no default constructor 
  XmlTabulatedPropertyCnv           ( const XmlTabulatedPropertyCnv& ); /// no copy    constructor 
  XmlTabulatedPropertyCnv& operator=( const XmlTabulatedPropertyCnv& ); /// no assignment 
  ///
private:
  ///
  bool              m_tagRead;
  /// 
  std::string       m_pcdata ;
  ///
  double            m_xunit  ;
  double            m_yunit  ;
  ///
};


#endif   // DETDESC_XMLTABULATEDPROPERTYCNV_H

