// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Det/DetDesc/src/component/XmlElementCnv.h,v 1.2 2001-05-14 15:13:42 sponce Exp $

#ifndef DETDESC_XMLCNVSVC_XMLELEMENTCNV_H
#define DETDESC_XMLCNVSVC_XMLELEMENTCNV_H

// Include files
#include "DetDesc/XmlGenericCnv.h"
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

  /** This processes the current object.
   * Overrides the default method in XmlGenericCnv
   * @param refpObject the object to be processed
   * @return status depending on the completion of the call
   */
  virtual StatusCode i_processObj (DataObject* refpObject);

};


#endif // DETDESC_XMLCNVSVC_XMLELEMENTCNV_H

