// $Id: XmlCatalogCnv.h,v 1.5 2009-04-17 12:25:18 cattanem Exp $

#ifndef DETDESCCNV_XMLCATALOGCNV_H
#define DETDESCCNV_XMLCATALOGCNV_H

// Include files
#include "GaudiKernel/ClassID.h"

#include <xercesc/dom/DOMNode.hpp>

#include "DetDescCnv/XmlGenericCnv.h"

// Forward and external declarations
class RegistryEntry;
template <class TYPE> class CnvFactory;


/** @class XmlCatalogCnv
 *
 * XML converter for Catalogs
 *
 * @author Sebastien Ponce
 * @author Radovan Chytracek
 * @author Pere Mato
 */
class XmlCatalogCnv : public XmlGenericCnv {

  /// Friend needed for instantiation
  friend class CnvFactory<XmlCatalogCnv>;
  
public:
 
  /**
   * accessor to the type of elements that this converter converts
   * @return the classID for this type
   */
  static const CLID& classID();
  
  
protected:
  
  /**
   * Constructor for this converter
   * @param svcs a ISvcLocator interface to find services
   */
  XmlCatalogCnv(ISvcLocator* svcs);
  
  /**
   * Default destructor
   */
  virtual ~XmlCatalogCnv();

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

  /**
   * This method checks that a converter exists for a given clID.
   * If the converter exists, nothing is done. If not, an exception
   * is raised.
   * @param clsID the clID to check
   */
  void checkConverterExistence(const CLID& clsID);

private:
  /**
   * This gets the CLID of a given element depending on its tag name or
   * its classID attribute in case of detelems. In case the element is
   * not known as a possible child of the tag catalog, it returns -1
   * @param element the DOMElement of the node concerned
   * @return the CLID for this element
   */
  CLID getCLID (xercesc::DOMElement* element);

private:

  // Constant strings for element and parameter names
  const XMLCh* isotopeString;
  const XMLCh* materialString;
  const XMLCh* catalogString;
  const XMLCh* tabpropertyString;
  const XMLCh* surfaceString;
  const XMLCh* logvolString;
  const XMLCh* detelemString;
  const XMLCh* elementString;
  const XMLCh* conditionString;

  const XMLCh* isotoperefString;
  const XMLCh* materialrefString;
  const XMLCh* catalogrefString;
  const XMLCh* tabpropertyrefString;
  const XMLCh* surfacerefString;
  const XMLCh* logvolrefString;
  const XMLCh* detelemrefString;
  const XMLCh* elementrefString;
  const XMLCh* conditionrefString;

  const XMLCh* refString;
  const XMLCh* hrefString;
  const XMLCh* nameString;

};

#endif // DETDESCCNV_XMLCNVSVC_XMLCATALOGCNV_H

