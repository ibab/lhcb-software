// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Det/DetDesc/DetDesc/XmlGenericCnv.h,v 1.4 2001-05-14 15:13:37 sponce Exp $

#ifndef DETDESC_XMLGENERICCNV_H
#define DETDESC_XMLGENERICCNV_H

// Include files
#include <string>

#include <dom/DOM_Element.hpp>
#include <dom/DOM_Text.hpp>

#include "GaudiKernel/Converter.h"

// Forward and external declarations
class ISvcLocator;
class IXmlSvc;
class GenericAddress;
class XmlCnvAttrList;
template <class TYPE> class CnvFactory;


/** @class XmlGenericCnv XmlGenericCnv.h DetDesc/XmlGenericCnv.h
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
   * @param pAddress the address of the object representation
   * @param refpObject the object created
   *  @return status depending on the completion of the call
   */
  virtual StatusCode createObj (IOpaqueAddress* pAddress,
                                DataObject*& refpObject);
  
  /**
   * Updates the transient object from the other representation.
   * @param pAddress the address of the object representation
   * @param refpObject the object updated
   *  @return status depending on the completion of the call
   */
  virtual StatusCode updateObj (IOpaqueAddress* pAddress,
                                DataObject* refpObject);
  
  /**
   * Converts the transient object to the requested representation.
   * @param pAddress the address of the object representation
   * @param refpObject the object to convert
   *  @return status depending on the completion of the call
   */
  virtual StatusCode createRep (DataObject* pObject,
                                IOpaqueAddress*& refpAddress);
  
  /**
   * Updates the converted representation of a transient object.
   * @param pAddress the address of the object representation
   * @param refpObject the object whose representation has to be updated
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
    return m_xmlSvc;
  }

  /**
   * Accessor to the StorageType value
   * @return the storage type for this object
   */
  static const unsigned char& storageType() {
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
  ~XmlGenericCnv() { }
  
  /** This creates the transient representation of an object from the
   *  DOM_Element representing it. This actually does the "new" operation
   *  and deals with the attributes of the node. This should not deal with
   *  children of the node.
   *  @param element the DOM_Element to be used to builds the object
   *  @param refpObject the object to be built
   *  @param SUCCESS or FAILURE depending on the completion of the
   *  object creation
   *  @return status depending on the completion of the call
   */
  virtual StatusCode i_createObj (DOM_Element element,
                                  DataObject*& refpObject) = 0;

  /** This fills the current object for its child element childElement.
   *  This will be called for each element child of the current object
   *  @param element the child processed here
   *  @param refpObject the object to be filled
   *  @return status depending on the completion of the call
   */
  virtual StatusCode i_fillObj (DOM_Element childElement,
                                DataObject* refpObject);

  /** This fills the current object for its child text node childText.
   *  This will be called for each text child of the current object
   *  @param childText the child processed here
   *  @param refpObject the object to be filled
   *  @return status depending on the completion of the call
   */
  virtual StatusCode i_fillObj (DOM_Text childText,
                                DataObject* refpObject);

  /** This is called after the current object was filled. This is were
   *  some computation based on the object content could be done
   *  @param refpObject the object to be processed
   *  @return status depending on the completion of the call
   */
  virtual StatusCode i_processObj (DataObject* refpObject);

  /**
   * builds a standard string from a DOMString
   * @param domString the DOMString
   * @return a standard string with the same characters
   */
  static const std::string dom2Std (DOMString domString);

  /// The receipt for us that we need to store for the object
  GenericAddress*  m_objRcpt;

  /// the IXmlSvc interface of this object
  IXmlSvc* m_xmlSvc;

};

#endif // DETDESC_XMLGENERICCNV_H
