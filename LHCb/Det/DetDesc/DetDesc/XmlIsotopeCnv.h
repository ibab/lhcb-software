#ifndef DETDESC_XMLCNVSVC_XMLISOTOPECNV_H
#define DETDESC_XMLCNVSVC_XMLISOTOPECNV_H

// generic experiment headers
#include "DetDesc/XmlGenericCnv.h"

#include "GaudiKernel/Converter.h"

#include "DetDesc/ISax8BitDocHandler.h"

// Forward declarations
class     ISvcLocator;
template <class TYPE> class CnvFactory;

class Isotope;

extern const    CLID&   CLID_Isotope;

class   XmlIsotopeCnv : public XmlGenericCnv, public ISax8BitDocHandler
{
  
  // Friend needed for instantiation
  friend class CnvFactory<XmlIsotopeCnv>;
  
public:
  
  // Create the transient representation of an object.
  virtual StatusCode createObj(IOpaqueAddress* pAddress, DataObject*& refpObject);
  
  // Update the transient object from the other representation.
  virtual StatusCode updateObj(IOpaqueAddress* pAddress, DataObject* refpObject);
  
  // Convert the transient object to the requested representation.
  virtual StatusCode createRep(DataObject* pObject, IOpaqueAddress*& refpAddress);
  
  // Update the converted representation of a transient object.
  virtual StatusCode updateRep(IOpaqueAddress* pAddress, DataObject* pObject);
  
  static const unsigned char& storageType() { return XML_StorageType; }
  
  static const CLID& classID() { return CLID_Isotope; }
  
  // -----------------------------------------------------------------------
  //  Implementations of the SAX 8 bit DocumentHandler interface
  // -----------------------------------------------------------------------
  virtual void startDocument( 
                              const char* //info
                            )
  {
  }
  
  virtual void endDocument(
                            const char* //info
                          )
  {
  }
  
  virtual void ignorableWhitespace(
                                    const char* const chars
                                   ,const unsigned int length
                                  );
  
  virtual void processingInstruction(
                                      const char* const //target
                                     ,const char* const //data
                                    )
  {
  }
  
  virtual void characters(
                           const char* const chars
                          ,const unsigned int length
                         );
  
  virtual void startElement(
                             const char* const name
                            ,XmlCnvAttributeList& attributes
                           );
  
  virtual void endElement(
                           const char* const name
                         );
  
  // Implementation of the obligatory method to provide a hint to generic XML
  // converter about the XML tag we want to be notfied about
  virtual const char* tag() const;
  
protected:
  
  // Standard Constructor
  XmlIsotopeCnv( ISvcLocator* svcs );
  
  // Standard Destructor
  virtual ~XmlIsotopeCnv()    {
  }
  
private:
  
  bool              m_tagRead;
};


#endif // DETDESC_XMLCNVSVC_XMLISOTOPECNV_H

