///  $Header: /afs/cern.ch/project/cvs/reps/lhcb/Det/DetDesc/src/component/XmlDetectorElementCnv.h,v 1.4 2001-05-17 13:26:52 sponce Exp $

#ifndef DETDESC_XMLCNVSVC_XMLDECNV_H
#define DETDESC_XMLCNVSVC_XMLDECNV_H

/// Include files
#include "DetDesc/XmlBaseDetElemCnv.h"


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
   * @return status depending on the completion of the call
   */
  virtual StatusCode i_fillSpecificObj (DOM_Element childElement,
                                        DetectorElement* refpObject);

};

#endif // DETDESC_XMLDECNV_H

