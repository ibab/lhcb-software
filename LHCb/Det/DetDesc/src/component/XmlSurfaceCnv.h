//  $Header: /afs/cern.ch/project/cvs/reps/lhcb/Det/DetDesc/src/component/XmlSurfaceCnv.h,v 1.3 2001-05-14 15:13:43 sponce Exp $

#ifndef     DETDESC_XMLSurfaceCNV_H
#define     DETDESC_XMLSurfaceCNV_H 1 


// Include files
#include "DetDesc/XmlGenericCnv.h"
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
  virtual ~XmlSurfaceCnv() {};

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

};


#endif   // DETDESC_XMLSurfaceCNV_H

