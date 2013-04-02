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
  virtual ~XmlIsotopeCnv();
  
  /** Creates the transient representation of an object from a DOMElement.
   * Overrides the default method in XmlGenericCnv
   * @param element the DOMElement to be used to builds the object
   * @param refpObject the object to be built
   * @return status depending on the completion of the call
   */
  virtual StatusCode i_createObj (xercesc::DOMElement* element,
                                  DataObject*& refpObject);
  
  using XmlGenericCnv::i_fillObj;
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
  const XMLCh* temperatureString;
  const XMLCh* pressureString;
  const XMLCh* stateString;
  const XMLCh* AString;
  const XMLCh* ZString;
  const XMLCh* IString;
  const XMLCh* densityString;
  const XMLCh* radlenString;
  const XMLCh* lambdaString;
  const XMLCh* tabpropsString;
  const XMLCh* addressString;

};

// ============================================================================
// End 
// ============================================================================
#endif // DETDESCCNV_XMLCNVSVC_XMLISOTOPECNV_H
// ============================================================================

