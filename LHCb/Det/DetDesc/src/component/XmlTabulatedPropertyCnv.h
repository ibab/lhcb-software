// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Det/DetDesc/src/component/XmlTabulatedPropertyCnv.h,v 1.3 2001-05-14 15:13:43 sponce Exp $

#ifndef     DETDESC_XMLTABULATEDPROPERTYCNV_H
#define     DETDESC_XMLTABULATEDPROPERTYCNV_H 1 

// Include files
#include "DetDesc/XmlGenericCnv.h"
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
  virtual ~XmlTabulatedPropertyCnv() {};

  /** Creates the transient representation of an object from a DOM_Element.
   * Overrides the default method in XmlGenericCnv
   * @param element the DOM_Element to be used to builds the object
   * @param refpObject the object to be built
   * @return status depending on the completion of the call
   */
  virtual StatusCode i_createObj (DOM_Element element,
                                  DataObject*& refpObject);

  /** This fills the current object for its child childElement.
   * Overrides the default method in XmlGenericCnv
   * @param element the DOM_Element that addr represents and from which we
   * will get the informations to creates new XmlAddresses
   * @return status depending on the completion of the call
   */
  virtual StatusCode i_fillObj (DOM_Element childElement,
                                DataObject* refpObject);

  /** This fills the current object for its child text node childText.
   * Overrides the default method in XmlGenericCnv
   * @param childText the child processed here
   * @param refpObject the object to be filled
   * @return status depending on the completion of the call
   */
  virtual StatusCode i_fillObj (DOM_Text childText,
                                DataObject* refpObject);

  /** This processes the current object.
   * Overrides the default method in XmlGenericCnv
   * @param refpObject the object to be processed
   * @return status depending on the completion of the call
   */
  virtual StatusCode i_processObj (DataObject* refpObject);
  

 private:

  /// user defined unit for the x axe
  double m_xunit;

  /// user defined unit for the y axe
  double m_yunit;

};


#endif   // DETDESC_XMLTABULATEDPROPERTYCNV_H

