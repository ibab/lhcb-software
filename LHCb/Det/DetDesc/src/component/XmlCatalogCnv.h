// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Det/DetDesc/src/component/XmlCatalogCnv.h,v 1.3 2001-05-14 15:13:42 sponce Exp $

#ifndef DETDESC_XMLCATALOGCNV_H
#define DETDESC_XMLCATALOGCNV_H

// Include files
#include "GaudiKernel/ClassID.h"

#include <dom/DOM_Node.hpp>
#include <dom/DOMString.hpp>

#include "DetDesc/XmlGenericCnv.h"

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
  virtual ~XmlCatalogCnv() {}

  /** Creates the transient representation of an object from a DOM_Element.
   * Overrides the default method in XmlGenericCnv
   * @param element the DOM_Element to be used to builds the object
   * @param refpObject the object to be built
   * @return status depending on the completion of the call
   */
  virtual StatusCode i_createObj (DOM_Element element,
                                  DataObject*& refpObject);

  /** Fills the current object for its child element childElement.
   * Overrides the default method in XmlGenericCnv
   * @param element the child processed here
   * @param refpObject the object to be filled
   * @return status depending on the completion of the call
   */
  virtual StatusCode i_fillObj (DOM_Element childElement,
                                DataObject* refpObject);

  /**
   * This method checks that a converter exists for a given clID.
   * If the converter exists, nothing is done. If not, an exception
   * is raised.
   * @param clsID the clID to check
   */
  void checkConverterExistence(const CLID& clsID);

};

#endif // DETDESC_XMLCNVSVC_XMLCATALOGCNV_H

