///	$Header: /afs/cern.ch/project/cvs/reps/lhcb/Det/DetDesc/src/XmlCatalogCnv.h,v 1.1 2001-01-29 13:59:50 ibelyaev Exp $
#ifndef DETDESC_XMLCNVSVC_XMLCATALOGCNV_H
#define DETDESC_XMLCNVSVC_XMLCATALOGCNV_H

/// Include files
#include "GaudiKernel/ClassID.h"

#include "DetDesc/XmlGenericCnv.h"
#include "DetDesc/ISax8BitDocHandler.h"

/// Forward and external declarations
class RegistryEntry;
template <class TYPE> class CnvFactory;


class XmlCatalogCnv : public XmlGenericCnv, public ISax8BitDocHandler
{
  /// Friend: HbookPersSvc needed for instantiation
  friend class CnvFactory<XmlCatalogCnv>;
  
public:
  
  /// -----------------------------------------------------------------------
  ///  Implementations of the Converter interface
  /// -----------------------------------------------------------------------
  /// Create the transient representation of an object.
  virtual StatusCode createObj(IOpaqueAddress* pAddress, DataObject*& refpObject);
  
  /// Update the transient object from the other representation.
  virtual StatusCode updateObj(IOpaqueAddress* pAddress, DataObject* refpObject);
  
  /// Convert the transient object to the requested representation.
  virtual StatusCode createRep(DataObject* pObject, IOpaqueAddress*& refpAddress);
  
  /// Update the converted representation of a transient object.
  virtual StatusCode updateRep(IOpaqueAddress* pAddress, DataObject* pObject);
  
  static const unsigned char& storageType() { return XML_StorageType; }
  
  static const CLID& classID();
  
  /// -----------------------------------------------------------------------
  ///  Implementations of the SAX 8 bit DocumentHandler interface
  /// -----------------------------------------------------------------------
  virtual void startDocument( const char* info );

  virtual void endDocument( const char* info );
  
  virtual void characters( const char* const chars, const unsigned int length );
  
  virtual void ignorableWhitespace( const char* const chars, const unsigned int length );
  
  virtual void processingInstruction( const char* const target, const char* const data );
  
  virtual void startElement( const char* const name, XmlCnvAttributeList& attributes);
  
  virtual void endElement( const char* const name );
  
  // Implementation of the obligatory method to provide a hint to generic XML
  // converter about the XML tag we want to be notfied about
  virtual const char* tag() const;
  
protected:
  
  /// Standard Constructor
  XmlCatalogCnv(ISvcLocator* svcs);
  
  /// Standard Destructor
  virtual ~XmlCatalogCnv() {}
  
  void checkConverterExistence(const CLID& clsID);
private:
  
  /// Remember the current level of nesting for catalogs and their children elements
  RegistryEntry* m_nestedDir;
  bool           m_ignore;
  
};

inline void XmlCatalogCnv::startDocument(
                                          const char* //info
                                        )
{
}

inline void XmlCatalogCnv::endDocument(
                                        const char* //info
                                      )
{
}
  
inline void XmlCatalogCnv::characters(
                                       const char* const  //chars
                                      ,const unsigned int //length
                                     )
{
}
  
inline void XmlCatalogCnv::ignorableWhitespace(
                                                const char* const  //chars
                                               ,const unsigned int //length
                                              )
{
}
  
inline void XmlCatalogCnv::processingInstruction(
                                                  const char* const //target
                                                 ,const char* const //data
                                                )
{
}


#endif // DETDESC_XMLCNVSVC_XMLCATALOGCNV_H

