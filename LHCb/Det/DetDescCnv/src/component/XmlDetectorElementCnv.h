///  $ID: $

#ifndef DETDESCCNV_XMLCNVSVC_XMLDECNV_H
#define DETDESCCNV_XMLCNVSVC_XMLDECNV_H

/// Include files
#include "DetDescCnv/XmlBaseDetElemCnv.h"


/** @class XmlDetectorElementCnv
 *
 * XML converter for DetectorElements.
 *
 * @author Sebastien Ponce
 * @author Radovan Chytracek
 * @author Pere Mato
 */
class XmlDetectorElementCnv : public XmlBaseDetElemCnv {

  /// Friend needed for instantiation
  friend class CnvFactory<XmlDetectorElementCnv>;
  
protected:
  
  /**
   * Constructor for this converter
   * @param svcs a ISvcLocator interface to find services
   */
  XmlDetectorElementCnv(ISvcLocator* svcs);

  /**
   * Default destructor
   */
  virtual ~XmlDetectorElementCnv() {}

  /** This fills the current object for specific child.
   * Implements the abstract method in XmlBaseDetElemCnv
   * @param childElement the specific child processed here
   * @param refpObject the object to be filled
   * @param address the address for this object
   * @return status depending on the completion of the call
   */
  virtual StatusCode i_fillSpecificObj (xercesc::DOMElement* childElement,
                                        DetectorElement* refpObject,
                                        IOpaqueAddress* address);

};

#endif // DETDESCCNV_XMLDECNV_H

