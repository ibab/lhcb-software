#ifndef DETDESCCNV_XMLGENERICCNV_H
#define DETDESCCNV_XMLGENERICCNV_H

// Include files
#include "GaudiKernel/Converter.h"
#include "GaudiKernel/IConversionSvc.h"
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/SmartIF.h"

#include "XmlTools/IXmlSvc.h"

#include <string>

#include <xercesc/dom/DOMElement.hpp>
#include <xercesc/dom/DOMText.hpp>


// Forward and external declarations
class ISvcLocator;
class IXmlSvc;
class GenericAddress;
template <class TYPE> class CnvFactory;


/** @class XmlGenericCnv XmlGenericCnv.h DetDescCnv/XmlGenericCnv.h
 *
 *  Base class for the XML converters. It encapsulates the generic functionality
 *  required by any converter of this type. The actual converters are children
 *  of this one
 *
 *  @author Sebastien Ponce
 *  @author Radovan Chytracek
 *  @author Pere Mato
 */
class XmlGenericCnv : public Converter {

 public:

  /**
   * Initializes the converter
   *  @return status depending on the completion of the call
   */
  virtual StatusCode initialize();

  /**
   * Finalizes the converter
   *  @return status depending on the completion of the call
   */
  virtual StatusCode finalize();

  /**
   * Creates the transient representation of an object.
   * @param addr the address of the object representation
   * @param refpObject the object created
   * @return status depending on the completion of the call
   */
  virtual StatusCode createObj (IOpaqueAddress* addr,
                                DataObject*& refpObject);
  /**
   * Updates the transient object from the other representation.
   * @param pAddress the address of the object representation
   * @param pObject the object updated
   *  @return status depending on the completion of the call
   */
  virtual StatusCode updateObj (IOpaqueAddress* pAddress,
                                DataObject* pObject);

  /**
   * Converts the transient object to the requested representation.
   * @param refpAddress the address of the object representation
   * @param pObject the object to convert
   *  @return status depending on the completion of the call
   */
  virtual StatusCode createRep (DataObject* pObject,
                                IOpaqueAddress*& refpAddress);

  /**
   * Updates the converted representation of a transient object.
   * @param pAddress the address of the object representation
   * @param pObject the object whose representation has to be updated
   *  @return status depending on the completion of the call
   */
  virtual StatusCode updateRep (IOpaqueAddress* pAddress,
                                DataObject* pObject);

  /**
   * Accessor to the IXmlSvc interface of the XmlCnvSvc service
   * @return the IXmlSvc interface of this object
   *  @return status depending on the completion of the call
   */
  IXmlSvc* xmlSvc() {
    if (!m_xmlSvc.isValid()) {
      m_xmlSvc = conversionSvc();
    }
    return m_xmlSvc;
  }

  /**
   * Accessor to the StorageType value
   * @return the storage type for this object
   */
  static long storageType() {
    return XML_StorageType;
  }

  /**
   * Accessor to the StorageType value
   * @return the storage type for this object
   */
  virtual long repSvcType() const  {
    return XML_StorageType;
  }


protected:

  /**
   * Constructor for this converter
   * @param svc a ISvcLocator interface to find services
   * @param clid the type of object the converter is able to convert
   */
  XmlGenericCnv (ISvcLocator* svc,const CLID& clid);

  /**
   * Default destructor
   */
  ~XmlGenericCnv();

  /** This creates the transient representation of an object from the
   *  DOMElement representing it, then fills it and process it.
   *  This implementation actually only calls the i_* methods to do the job.
   *  Most of the converters don't need to override it but only to
   *  override/implement some of the i_* methods.
   *  @param element the DOMElement to be used to builds the object
   *  @param refpObject the object to be built
   *  @param address the address for this object
   *  @return status depending on the completion of the call
   */
  virtual StatusCode internalCreateObj (xercesc::DOMElement* element,
                                        DataObject*& refpObject,
                                        IOpaqueAddress* address);

  /** This creates the transient representation of an object from the
   *  DOMElement representing it. This actually does the "new" operation
   *  and deals with the attributes of the node. This should not deal with
   *  children of the node.
   *  @param element the DOMElement to be used to builds the object
   *  @param refpObject the object to be built
   *  @return status depending on the completion of the call
   */
  virtual StatusCode i_createObj (xercesc::DOMElement* element,
                                  DataObject*& refpObject);

  /** This fills the current object for its child element childElement.
   *  This will be called for each element child of the current object
   *  @param childElement the child processed here
   *  @param pObject the object to be filled
   *  @param address the address for this object
   *  @return status depending on the completion of the call
   */
  virtual StatusCode i_fillObj (xercesc::DOMElement* childElement,
                                DataObject* pObject,
                                IOpaqueAddress* address);

  /** This fills the current object for its child text node childText.
   *  This will be called for each text child of the current object
   *  @param childText the child processed here
   *  @param pObject the object to be filled
   *  @param address the address for this object
   *  @return status depending on the completion of the call
   */
  virtual StatusCode i_fillObj (xercesc::DOMText* childText,
                                DataObject* pObject,
                                IOpaqueAddress* address);

  /** This is called after the current object was filled. This is were
   *  some computation based on the object content could be done
   *  @param pObject the object to be processed
   *  @param address the address for this object
   *  @return status depending on the completion of the call
   */
  virtual StatusCode i_processObj (DataObject* pObject,
                                   IOpaqueAddress* address);

  /**
   * This parses a href attribute string and creates an address out of it
   * Currently, two types of references are understood : regular URLs and
   * references starting with "conddb:/". The former create addresses with
   * storage type XML_StorageType, the later addresses with storage type
   * CONDDB_StorageType.
   * @param href the string contained in the href attribute
   * @param clid the clid of the address to create
   * @param parent the address of the dataObject that will contain the
   * new object.
   * @return the new Address
   */
  IOpaqueAddress* createAddressForHref (std::string href,
                                        CLID clid,
                                        IOpaqueAddress* parent) const;

  /**
   * This creates an XmlAddress using the location, entryName and clid
   * @param location the location string for this address
   * @param entryName the entryName string for this address
   * @param clid the clidof the address to create
   * @return the new Address
   */
  IOpaqueAddress* createXmlAddress (std::string location,
                                    std::string entryName,
                                    CLID clid) const;

  /**
   * This creates a ConddbAddress using the location, entryName, channelID and clid
   * @param path the path string for this address
   * @param entryName the entryName string for this address
   * @param channelId the channelID for this address
   * @param clid the clidof the address to create
   * @return the new Address
   */
  IOpaqueAddress* createCondDBAddress (std::string path,
                                       std::string entryName,
                                       unsigned long channelId,
                                       CLID clid) const;

  /**
   * builds a standard string from a XMLCh*
   * @param domString the XMLCh*
   * @return a standard string with the same characters
   */
  static const std::string dom2Std (const XMLCh* domString);

  /// the IXmlSvc interface of this object
  SmartIF<IXmlSvc> m_xmlSvc;

  /// The message stream
  MsgStream* m_msg;
  
  /// Methods to print as in GaudiAlgorithms
  MsgStream& verbose() const { return *m_msg << MSG::VERBOSE; }
  
  MsgStream& debug()   const { return *m_msg << MSG::DEBUG; }

  MsgStream& info()    const { return *m_msg << MSG::INFO; }
  
  MsgStream& warning() const { return *m_msg << MSG::WARNING; }

  MsgStream& error()   const { return *m_msg << MSG::ERROR; }  
 
  MsgStream& fatal()   const { return *m_msg << MSG::FATAL; }  
 
  inline bool msgLevel( const MSG::Level level ) const { return m_msg->level() <= level; }
  
private:

  // Constant strings for element and parameter names
  const XMLCh* DDDBString;
  const XMLCh* materialsString;
  const XMLCh* versionString;
  const XMLCh* DTD_VersionString;
  const XMLCh* macroString;
  const XMLCh* nameString;
  const XMLCh* valueString;
  const XMLCh* parameterString;
  const XMLCh* detelemString;
  const XMLCh* conditionString;
  const XMLCh* classIDString;
  const XMLCh* serialNumberString;
  
  /// Flag that says if the storage type CONDDB_StorageType is accessible.
  bool m_have_CONDDB_StorageType;

};

#endif // DETDESCCNV_XMLGENERICCNV_H
