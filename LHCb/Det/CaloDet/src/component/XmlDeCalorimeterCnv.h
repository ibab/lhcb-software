// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Det/CaloDet/src/component/XmlDeCalorimeterCnv.h,v 1.2 2001-06-14 16:09:45 sponce Exp $

#ifndef            __CALO_XML_XMLDECALORIMETERCNV_H__
#define            __CALO_XML_XMLDECALORIMETERCNV_H__ 1 

// Include files
#include "DetDesc/XmlUserDetElemCnv.h"
#include "CaloDet/DeCalorimeter.h"

/** @class XmlCalorimeterCnv
 *
 * XML converter for Calorimeter
 */
class XmlDeCalorimeterCnv: public XmlUserDetElemCnv<DeCalorimeter> {
  
public:
  
  /**
   * Constructor for this converter
   * @param svc a ISvcLocator interface to find services
   */
  XmlDeCalorimeterCnv (ISvcLocator* svc);
  
  /**
   * Default destructor
   */
  virtual ~XmlDeCalorimeterCnv(){};
  

protected:

  /** This fills the current object for specific child.
   * Overrides the default implementation in XmlUserDetElemCnv.
   * @param childElement the specific child processed here
   * @param refpObject the object to be filled
   * @return status depending on the completion of the call
   */
  virtual StatusCode i_fillSpecificObj (DOM_Element childElement,
                                        DeCalorimeter* dataObj);
};


#endif //          __CALO_XML_XMLDECALORIMETERCNV_H__
