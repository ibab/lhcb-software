// $Id: XmlElementCnv.h,v 1.4 2008-05-20 08:19:25 smenzeme Exp $ 
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
  virtual ~XmlElementCnv();

  /** Creates the transient representation of an object from a DOMElement.
   *  Overrides the default method in XmlGenericCnv
   *  @param element the DOMElement to be used to builds the object
   *  @param refpObject the object to be built
   *  @return status depending on the completion of the call
   */
  virtual StatusCode i_createObj 
  ( xercesc::DOMElement*  element    ,
    DataObject*& refpObject );
  
  using XmlGenericCnv::i_fillObj;
  /** Fills the current object for its child element childElement.
   *  Overrides the default method in XmlGenericCnv
   *  @param childElement the child processed here
   *  @param refpObject the object to be filled
   *  @param address the address for this object
   *  @return status depending on the completion of the call
   */
  virtual StatusCode i_fillObj 
  ( xercesc::DOMElement* childElement ,
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
  
private:

  // Constant strings for element and parameter names
  const XMLCh* nameString;
  const XMLCh* temperatureString;
  const XMLCh* pressureString;
  const XMLCh* stateString;
  const XMLCh* densityString;
  const XMLCh* radlenString;
  const XMLCh* lambdaString;
  const XMLCh* symbolString;
  const XMLCh* AString;
  const XMLCh* ZeffString;
  const XMLCh* IString;
  const XMLCh* hrefString;
  const XMLCh* fractionmassString;
  const XMLCh* tabpropsString;
  const XMLCh* addressString;
  const XMLCh* isotoperefString;
  const XMLCh* atomString;

};

// ============================================================================
// End 
// ============================================================================
#endif // DETDESCCNV_XMLCNVSVC_XMLELEMENTCNV_H
// ============================================================================

