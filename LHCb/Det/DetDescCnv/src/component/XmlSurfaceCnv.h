//  $Id: XmlSurfaceCnv.h,v 1.4 2005-03-24 16:21:18 cattanem Exp $

#ifndef     DETDESCCNV_XMLSurfaceCNV_H
#define     DETDESCCNV_XMLSurfaceCNV_H 1 


// Include files
#include "DetDescCnv/XmlGenericCnv.h"
#include "DetDesc/Surface.h"

// Forward declarations
template <class TYPE> class CnvFactory;


/** @class XmlSurfaceCnv
 *   
 *  XML converter for Surfaces
 *
 *  @author  Sebastien Ponce
 *  @author  Vanya Belyaev
 */
class XmlSurfaceCnv : public XmlGenericCnv {
  
  /// Friend needed for instantiation
  friend class CnvFactory<XmlSurfaceCnv>;

 public:

  /**
   * accessor to the type of elements that this converter converts
   * @return the classID for this type
   */
  static const CLID& classID () { return Surface::classID(); }


 protected:

  /**
   * Constructor for this converter
   * @param svcs a ISvcLocator interface to find services
   */
  XmlSurfaceCnv (ISvcLocator* svcs);

  /**
   * Default destructor
   */
  virtual ~XmlSurfaceCnv();

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
   * @param refpObject the object to be filled
   * @param address the address for this object
   * @return status depending on the completion of the call
   */
  virtual StatusCode i_fillObj (xercesc::DOMElement* childElement,
                                DataObject* refpObject,
                                IOpaqueAddress* address);


private:

  // Constant strings for element and parameter names
  const XMLCh* nameString;
  const XMLCh* modelString;
  const XMLCh* finishString;
  const XMLCh* typeString;
  const XMLCh* valueString;
  const XMLCh* volfirstString;
  const XMLCh* volsecondString;
  const XMLCh* tabpropsString;
  const XMLCh* addressString;

};


#endif   // DETDESCCNV_XMLSurfaceCNV_H

