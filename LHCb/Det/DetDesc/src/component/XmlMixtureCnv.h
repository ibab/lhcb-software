#ifndef DETDESC_XMLMIXTURECNV_H
#define DETDESC_XMLMIXTURECNV_H

// generic experiment headers
#include "DetDesc/XmlGenericCnv.h"
#include "DetDesc/ISax8BitDocHandler.h"
#include "DetDesc/Mixture.h"
#include <string>

// Forward declarations
class     ISvcLocator;
template <class TYPE> class CnvFactory;


class   XmlMixtureCnv : public XmlGenericCnv       ,
                        public ISax8BitDocHandler
{
  
  // Friend needed for instantiation
  friend class CnvFactory<XmlMixtureCnv>;
  
 public:
  
  // Create the transient representation of an object.
  virtual StatusCode createObj(IOpaqueAddress* pAddress, DataObject*& refpObject);

  // Resolve the references of the created transient object.
  virtual StatusCode fillObjRefs(IOpaqueAddress* pAddress, DataObject* pObject);
  
  // Update the transient object from the other representation.
  virtual StatusCode updateObj(IOpaqueAddress* pAddress, DataObject* refpObject);
  
  // Convert the transient object to the requested representation.
  virtual StatusCode createRep(DataObject* pObject, IOpaqueAddress*& refpAddress);
  
  // Update the converted representation of a transient object.
  virtual StatusCode updateRep(IOpaqueAddress* pAddress, DataObject* pObject);
  
  static const unsigned char& storageType() { return XML_StorageType ; }
  static const CLID&          classID    () { return CLID_Mixture    ; }
  
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
  XmlMixtureCnv( ISvcLocator* svcs );
  
  // Standard Destructor
  virtual ~XmlMixtureCnv()
  {
  }
  enum MixMode { MM_undefined, MM_byFractionMass, MM_byNAtoms };
private:
  
  // Item object and its address we need for loading
  DataObject*            m_itemObj;
  double                 m_itemFraction;
  bool                   m_byFraction;
  MixMode                m_mixMode;

  std::string compactPath(std::string path);
};


#endif // DETDESC_XMLCNVSVC_XMLMIXTURECNV_H

