// $Id: XmlIsotopeCnv.h,v 1.1.1.1 2003-04-23 13:59:46 sponce Exp $
#ifndef DETDESCCNV_XMLCNVSVC_XMLISOTOPECNV_H
#define DETDESCCNV_XMLCNVSVC_XMLISOTOPECNV_H

// generic experiment headers
#include "DetDescCnv/XmlGenericCnv.h"
#include "DetDesc/Isotope.h"

// Forward declarations
template <class TYPE> class CnvFactory;


/** @class XmlIsotopeCnv
 *
 * XML converter for Isotopes.
 *
 * @author Sebastien Ponce
 * @author Radovan Chytracek
 * @author Pere Mato
 */
class XmlIsotopeCnv : public XmlGenericCnv {
  
  /// Friend needed for instantiation
  friend class CnvFactory<XmlIsotopeCnv>;
  
public:
  
  /**
   * accessor to the type of elements that this converter converts
   * @return the classID for this type
   */
  static const CLID& classID() { return CLID_Isotope; }  

 
protected:
  
  /**
   * Constructor for this converter
   * @param svcs a ISvcLocator interface to find services
   */
  XmlIsotopeCnv (ISvcLocator* svcs);
  
  /**
   * Default destructor
   */
  virtual ~XmlIsotopeCnv() {}
  
  /** Creates the transient representation of an object from a DOM_Element.
   * Overrides the default method in XmlGenericCnv
   * @param element the DOM_Element to be used to builds the object
   * @param refpObject the object to be built
   * @return status depending on the completion of the call
   */
  virtual StatusCode i_createObj 
  ( DOM_Element  element    ,
    DataObject*& refpObject );
  
  /** Fills the current object for its child element childElement.
   * Overrides the default method in XmlGenericCnv
   * @param element the child processed here
   * @param refpObject the object to be filled
   * @return status depending on the completion of the call
   */
    virtual StatusCode i_fillObj 
  ( DOM_Element     childElement ,
    DataObject*     refpObject   ,
    IOpaqueAddress* address      );
  
};

// ============================================================================
// End 
// ============================================================================
#endif // DETDESCCNV_XMLCNVSVC_XMLISOTOPECNV_H
// ============================================================================

