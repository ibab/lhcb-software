// $Id: XmlIsotopeCnv.h,v 1.3 2002-01-18 18:23:10 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $ 
// ============================================================================
#ifndef DETDESC_XMLCNVSVC_XMLISOTOPECNV_H
#define DETDESC_XMLCNVSVC_XMLISOTOPECNV_H

// generic experiment headers
#include "DetDesc/XmlGenericCnv.h"
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
#endif // DETDESC_XMLCNVSVC_XMLISOTOPECNV_H
// ============================================================================

