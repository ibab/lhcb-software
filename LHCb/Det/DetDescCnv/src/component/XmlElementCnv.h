// $Id: XmlElementCnv.h,v 1.1.1.1 2003-04-23 13:59:46 sponce Exp $ 
#ifndef DETDESCCNV_XMLCNVSVC_XMLELEMENTCNV_H
#define DETDESCCNV_XMLCNVSVC_XMLELEMENTCNV_H

// Include files
#include "DetDescCnv/XmlGenericCnv.h"
#include "DetDesc/Element.h"

// Forward declarations
class Isotope;
class ISvcLocator;
template <class TYPE> class CnvFactory;


/** @class XmlElementCnv
 *
 * XML converter for Elements
 *
 * @author Sebastien Ponce
 * @author Radovan Chytracek
 * @author Pere Mato
 */
class XmlElementCnv : public XmlGenericCnv {
  
  /// Friend needed for instantiation
  friend class CnvFactory<XmlElementCnv>;
  
public:
  
  /**
   * accessor to the type of elements that this converter converts
   * @return the classID for this type
   */  
  static const CLID& classID () { return CLID_Element; }  
  

protected:
  
  /**
   * Constructor for this converter
   * @param svcs a ISvcLocator interface to find services
   */
  XmlElementCnv (ISvcLocator* svcs);
  
  /**
   * Default destructor
   */
  virtual ~XmlElementCnv() {}

  /** Creates the transient representation of an object from a DOM_Element.
   *  Overrides the default method in XmlGenericCnv
   *  @param element the DOM_Element to be used to builds the object
   *  @param refpObject the object to be built
   *  @return status depending on the completion of the call
   */
  virtual StatusCode i_createObj 
  ( DOM_Element  element    ,
    DataObject*& refpObject );
  
  /** Fills the current object for its child element childElement.
   *  Overrides the default method in XmlGenericCnv
   *  @param element the child processed here
   *  @param refpObject the object to be filled
   *  @param address the address for this object
   *  @return status depending on the completion of the call
   */
  virtual StatusCode i_fillObj 
  ( DOM_Element childElement ,
    DataObject* refpObject   ,
    IOpaqueAddress* address  );
  
  /** This processes the current object.
   *  Overrides the default method in XmlGenericCnv
   *  @param refpObject the object to be processed
   *  @param address the address for this object
   *  @return status depending on the completion of the call
   */
  virtual StatusCode i_processObj 
  ( DataObject*     refpObject ,
    IOpaqueAddress* address    );
  
};

// ============================================================================
// End 
// ============================================================================
#endif // DETDESCCNV_XMLCNVSVC_XMLELEMENTCNV_H
// ============================================================================

