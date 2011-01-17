// $Id: XmlDummyDECnv.h,v 1.1 2005-04-22 15:17:54 marcocle Exp $
#ifndef XMLDUMMYDECNV_H 
#define XMLDUMMYDECNV_H 1

// Include files
#include "DetDescCnv/XmlUserDetElemCnv.h"
#include "DummyDetectorElement.h"

/** @class XmlDummyDECnv XmlDummyDECnv.h
 *  
 *
 *  @author Marco CLEMENCIC
 *  @date   2005-04-11
 */
class XmlDummyDECnv: public XmlUserDetElemCnv<DummyDetectorElement>{
public: 
  /// Standard constructor
  XmlDummyDECnv(ISvcLocator* svc); 

  virtual ~XmlDummyDECnv(); ///< Destructor

#ifdef __INTEL_COMPILER         // Disable ICC warning
  #pragma warning(disable:1125) // virtual function is hidden, override intended?
  #pragma warning(push)
#endif
  virtual StatusCode i_fillSpecificObj (xercesc::DOMElement* childElement,
                                        DummyDetectorElement* dataObj,
                                        IOpaqueAddress* address);
#ifdef __INTEL_COMPILER // Re-enable ICC warning
  #pragma warning(pop)
#endif

protected:

private:

};
#endif // XMLDUMMYDECNV_H
