///  $ID: $

#ifndef DETDESCCNV_XMLCNVSVC_XMLCONDITIONCNV_H
#define DETDESCCNV_XMLCNVSVC_XMLCONDITIONCNV_H

/// Include files
#include "DetDescCnv/XmlBaseConditionCnv.h"


/** @class XmlConditionCnv
 *
 * XML converter for Conditions.
 *
 * @author Sebastien Ponce
 */
class XmlConditionCnv : public XmlBaseConditionCnv {

  /// Friend needed for instantiation
  friend class CnvFactory<XmlConditionCnv>;
  
protected:
  
  /**
   * Constructor for this converter
   * @param svcs a ISvcLocator interface to find services
   */
  XmlConditionCnv(ISvcLocator* svcs);

  /**
   * Default destructor
   */
  virtual ~XmlConditionCnv() {}

  /** This fills the current object for specific child.
   * Implements the abstract method in XmlConditionCnv
   * @param childElement the specific child processed here
   * @param refpObject the object to be filled
   * @param address the address for this object
   * @return status depending on the completion of the call
   */
  virtual StatusCode i_fillSpecificObj (xercesc::DOMElement* childElement,
                                        Condition* refpObject,
                                        IOpaqueAddress* address);

};

#endif // DETDESCCNV_XMLCONDITIONCNV_H

