///  $Id: XmlBaseConditionCnv.h,v 1.5 2005-08-31 15:53:49 marcocle Exp $

#ifndef DETDESCCNV_XMLCONDITIONCNV_H
#define DETDESCCNV_XMLCONDITIONCNV_H

/// Include files
#include "DetDescCnv/XmlGenericCnv.h"
#include "DetDesc/Condition.h"

#include "GaudiKernel/Converter.h"

/// Forward and external declarations
class     ISvcLocator;
template <class TYPE> class CnvFactory;


/** @class XmlBaseConditionCnv
 *
 * XML converter for Conditions
 *
 * @author Sebastien Ponce
 */
class XmlBaseConditionCnv : public XmlGenericCnv {

public:
  
  /**
   * Initializes the converter - Overrides the default method in XmlGenericCnv
   * @return status depending on the completion of the call
   */
  virtual StatusCode initialize();
  
  /**
   * accessor to the type of elements that this converter converts
   * @return the classID for this type
   */
  static const CLID& classID();
  
protected:
  
  /**
   * Constructor for this converter
   * @param svcs a ISvcLocator interface to find services
   */
  XmlBaseConditionCnv(ISvcLocator* svcs);

  /**
   * Constructor for this converter
   * @param svc   a ISvcLocator interface to find services
   * @param clsID the type of object the converter is able to convert
   */
  XmlBaseConditionCnv(ISvcLocator* svc, const CLID& clsID );

  /**
   * Default destructor
   */
  virtual ~XmlBaseConditionCnv();

  /**
   * Resolves the references of the created transient object.
   */
  virtual StatusCode fillObjRefs(IOpaqueAddress* pAddress, 
                                 DataObject* pObject);

  /**
   * Resolves the references of the just updated transient object.
   */
  virtual StatusCode updateObjRefs(IOpaqueAddress* pAddress,
                                   DataObject* pObject);

  /** Creates the transient representation of an object from a DOMElement.
   * Overrides the default method in XmlGenericCnv
   * @param element the DOMElement to be used to builds the object
   * @param refpObject the object to be built
   * @return status depending on the completion of the call
   */
  virtual StatusCode i_createObj (xercesc::DOMElement* element,
                                  DataObject*& refpObject);

  /** Fills the current object for its child element childElement.
   * Overrides the default method in XmlGenericCnv
   * @param childElement the child processed here
   * @param refpObject   the object to be filled
   * @param address the address for this object
   * @return status depending on the completion of the call
   */
  virtual StatusCode i_fillObj (xercesc::DOMElement* childElement,
                                DataObject* refpObject,
                                IOpaqueAddress* address);

  /** This fills the current object for specific child.
   * Specific children are children of children \<specific\>
   * of the \<condition\> tag. This is typically used for specializing
   * consditions and define new properties.
   * This will be called for each specific child of the current object
   * @param childElement the specific child processed here
   * @param refpObject the object to be filled
   * @param address the address for this object
   * @return status depending on the completion of the call
   */
  virtual StatusCode i_fillSpecificObj (xercesc::DOMElement* childElement,
                                        Condition* refpObject,
                                        IOpaqueAddress* address) = 0;

private:
  
  /// Whether to use the generic converter in case a specific one does not exist
  bool m_doGenericCnv;
  
  // Constant strings for element names
  const XMLCh* specificString;
  const XMLCh* paramString;
  const XMLCh* paramVectorString;

  // Constant Strings for parameter names
  const XMLCh* typeString;
  const XMLCh* nameString;
  const XMLCh* commentString;
  
};

#endif // DETDESCCNV_XMLCONDITIONCNV_H

