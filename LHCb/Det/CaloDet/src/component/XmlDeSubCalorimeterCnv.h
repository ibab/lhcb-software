// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Det/CaloDet/src/component/XmlDeSubCalorimeterCnv.h,v 1.2 2001-06-14 16:09:45 sponce Exp $

#ifndef            __CALO_XML_XMLDESUBCALORIMETERCNV_H__
#define            __CALO_XML_XMLDESUBCALORIMETERCNV_H__ 1 

// Include files
#include "DetDesc/XmlUserDetElemCnv.h"
#include "CaloDet/DeSubCalorimeter.h"

/** @class XmlCalorimeterCnv
 *
 * XML converter for SubCalorimeter
 */
class XmlDeSubCalorimeterCnv: public XmlUserDetElemCnv<DeSubCalorimeter> {
  
public:
  
  /**
   * Constructor for this converter
   * @param svc a ISvcLocator interface to find services
   */
  XmlDeSubCalorimeterCnv (ISvcLocator* svc);
  
  /**
   * Default destructor
   */
  virtual ~XmlDeSubCalorimeterCnv(){};
  

protected:

  /** This fills the current object for specific child.
   * Overrides the default implementation in XmlUserDetElemCnv.
   * @param childElement the specific child processed here
   * @param refpObject the object to be filled
   * @return status depending on the completion of the call
   */
  virtual StatusCode i_fillSpecificObj (DOM_Element childElement,
                                        DeSubCalorimeter* dataObj);

};


#endif //          __CALO_XML_XMLDESUBCALORIMETERCNV_H__
