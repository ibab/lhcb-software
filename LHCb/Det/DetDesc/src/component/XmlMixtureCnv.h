// $Header: /afs/cern.ch/project/cvs/reps/lhcb/Det/DetDesc/src/component/XmlMixtureCnv.h,v 1.4 2001-05-14 15:13:43 sponce Exp $

#ifndef DETDESC_XMLMIXTURECNV_H
#define DETDESC_XMLMIXTURECNV_H

// Include files
#include "DetDesc/XmlGenericCnv.h"
#include "DetDesc/Mixture.h"
#include <string>

// Forward declarations
class     ISvcLocator;
template <class TYPE> class CnvFactory;


/** @class XmlLMixtureCnv
 *
 * XML converter for Mixtures
 *
 * @author Sebastien Ponce
 * @author Radovan Chytracek
 * @author Pere Mato
 */
class XmlMixtureCnv : public XmlGenericCnv {
  
  /// Friend needed for instantiation
  friend class CnvFactory<XmlMixtureCnv>;
  
 public:

  /**
   * accessor to the type of elements that this converter converts
   * @return the classID for this type
   */
  static const CLID& classID () { return CLID_Mixture; }  

  
 protected:
  
  /**
   * Constructor for this converter
   * @param svcs a ISvcLocator interface to find services
   */
  XmlMixtureCnv( ISvcLocator* svcs );
  
  /**
   * Default destructor
   */
  virtual ~XmlMixtureCnv() {}

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

  /**
   * This is used to describe the kind of ponderation used so far in the
   * mixture. MM_undefined means that the mixture is empty at this time,
   * MM_byFractionMass means that fraction of mass is used, MM_byNAtoms
   * means that number of atoms is used
   */
  enum MixMode { MM_undefined, MM_byFractionMass, MM_byNAtoms };


 private:

  /// This tells what kind of ponderation has been used so far in the mixture
  MixMode m_mixMode;

  /**
   * compacts a path. This removes every ".." in the path.
   * @param path the path to be processed
   * @return the result
   */
  std::string compactPath(std::string path);

};

#endif // DETDESC_XMLCNVSVC_XMLMIXTURECNV_H

