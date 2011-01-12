// $Id: XmlTabulatedPropertyCnv.h,v 1.4 2005-03-24 16:21:18 cattanem Exp $

#ifndef     DETDESCCNV_XMLTABULATEDPROPERTYCNV_H
#define     DETDESCCNV_XMLTABULATEDPROPERTYCNV_H 1 

// Include files
#include "DetDescCnv/XmlGenericCnv.h"
#include "DetDesc/TabulatedProperty.h"

// Forward declarations
template <class TYPE> class CnvFactory;


/** @class XmlTabulatedPropertyCnv
 *   
 * Xml converter for TabulatedProperties
 *
 * @author Sebastien Ponce
 * @author  Vanya Belyaev
 */
class XmlTabulatedPropertyCnv : public XmlGenericCnv {
  
  /// Friend needed for instantiation
  friend class CnvFactory<XmlTabulatedPropertyCnv>;

 public:

  /**
   * accessor to the type of elements that this converter converts
   * @return the classID for this type
   */
  static const CLID& classID() { return TabulatedProperty::classID(); }


 protected:

  /**
   * Constructor for this converter
   * @param svcs a ISvcLocator interface to find services
   */
  XmlTabulatedPropertyCnv (ISvcLocator* svcs);

  /**
   * Default destructor
   */
  virtual ~XmlTabulatedPropertyCnv();

  /** Creates the transient representation of an object from a DOMElement.
   * Overrides the default method in XmlGenericCnv
   * @param element the DOMElement to be used to builds the object
   * @param refpObject the object to be built
   * @return status depending on the completion of the call
   */
  virtual StatusCode i_createObj (xercesc::DOMElement* element,
                                  DataObject*& refpObject);

  /** This fills the current object for its child childElement.
   * Overrides the default method in XmlGenericCnv
   * @param childElement the DOMElement that addr represents and from which we
   * will get the informations to creates new addresses
   * @param refpObject the object to be built
   * @param address the address for this object
   * @return status depending on the completion of the call
   */
  virtual StatusCode i_fillObj (xercesc::DOMElement* childElement,
                                DataObject* refpObject,
                                IOpaqueAddress* address);

  /** This fills the current object for its child text node childText.
   * Overrides the default method in XmlGenericCnv
   * @param childText the child processed here
   * @param refpObject the object to be filled
   * @param address the address for this object
   * @return status depending on the completion of the call
   */
  virtual StatusCode i_fillObj (xercesc::DOMText* childText,
                                DataObject* refpObject,
                                IOpaqueAddress* address);

#ifdef __INTEL_COMPILER         // Disable ICC warning
  #pragma warning(disable:1125) // virtual function is hidde, override intended?
  #pragma warning(push)
#endif
  /** This processes the current object.
   * Overrides the default method in XmlGenericCnv
   * @param refpObject the object to be processed
   * @return status depending on the completion of the call
   */
  virtual StatusCode i_processObj (DataObject* refpObject);
#ifdef __INTEL_COMPILER // Re-enable ICC warning
  #pragma warning(pop)
#endif
  

 private:

  /// user defined unit for the x axe
  double m_xunit;

  /// user defined unit for the y axe
  double m_yunit;

private:

  // Constant strings for element and parameter names
  const XMLCh* nameString;
  const XMLCh* typeString;
  const XMLCh* xaxisString;
  const XMLCh* yaxisString;
  const XMLCh* xunitString;
  const XMLCh* yunitString;
  const XMLCh* entryString;
  const XMLCh* xString;
  const XMLCh* yString;

};


#endif   // DETDESCCNV_XMLTABULATEDPROPERTYCNV_H

