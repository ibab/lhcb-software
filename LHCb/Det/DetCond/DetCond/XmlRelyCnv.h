// $Id: XmlRelyCnv.h,v 1.2 2005-02-09 08:30:53 marcocle Exp $
#ifndef COMPONENT_XMLRELYCNV_H 
#define COMPONENT_XMLRELYCNV_H 1

// Include files
#include "CondDBGenericCnv.h"
#include "GaudiKernel/CnvFactory.h"
#include <string>

// from POOL
#include "AttributeList/AttributeListSpecification.h"

// Forward and external declarations
class ISvcLocator;
template <class TYPE> class CnvFactory;

/** @class XmlRelyCnv XmlRelyCnv.h component/XmlRelyCnv.cpp
 *  
 *  ConditionsDBCnvSvc rely on the functionalities provided by the XmlCnvSvc.
 *  XmlRelyCnv delegate the creation of the object to the XmlCnvSvc
 *  (via DetectorPersistencySvc).
 *
 *  @author Marco CLEMENCIC
 *  @date   2004-12-03
 */
class XmlRelyCnv: public CondDBGenericCnv {

  /// Friend needed for instantiation
  friend class CnvFactory<XmlRelyCnv>;

public: 

  /**
   * Initializes the converter
   *  @return status depending on the completion of the call
   */
  virtual StatusCode initialize();
  
  /**
   * Finalizes the converter
   *  @return status depending on the completion of the call
   */
  virtual StatusCode finalize();

  /**
   * Creates the transient representation of an object.
   * @param  pAddress the address of the object representation
   * @param  pObject the object created
   * @return status depending on the completion of the call
   */
  virtual StatusCode createObj (IOpaqueAddress *pAddress,
                                DataObject *&pObject);
  /**
   * Updates the transient object from the other representation (not implemented).
   * @param pAddress the address of the object representation
   * @param pObject the object updated
   *  @return status depending on the completion of the call
   */
  virtual StatusCode updateObj (IOpaqueAddress *pAddress,
                                DataObject *pObject);
  
  /**
   * Converts the transient object to the requested representation (not implemented).
   * @param refpAddress the address of the object representation
   * @param pObject the object to convert
   *  @return status depending on the completion of the call
   */
  virtual StatusCode createRep (DataObject* pObject,
                                IOpaqueAddress*& refpAddress);
  
  /**
   * Updates the converted representation of a transient object.
   * @param pAddress the address of the object representation
   * @param pObject the object whose representation has to be updated
   *  @return status depending on the completion of the call
   */
  virtual StatusCode updateRep (IOpaqueAddress* pAddress,
                                DataObject* pObject);

  /**
   * Accessor to the type of elements that this converter converts.
   * @return the classID for this type
   */  
  static const CLID& classID () { return s_CLID_any; }  
  
  /** Routine used to easily obtain the AttributeListSpecification
      for this kind of objects. */
  static const pool::AttributeListSpecification& attrListSpec();

protected:
  /// Standard constructor
  XmlRelyCnv(ISvcLocator* svc); 
  virtual ~XmlRelyCnv( ); ///< Destructor

private:

  /// needed to implement classID()
  static const CLID s_CLID_any;

  /// Storage space for the default AttributeListSpecification.
  static pool::AttributeListSpecification m_attlist;

  /**
   * Do the needed steps to perform a creation by delegation.
   */
  StatusCode i_delegatedCreation(IOpaqueAddress* pAddress,
                                 DataObject *&pObject,TimePoint &since,TimePoint &till);

  /// Handle to the IConversionSvc interface of the DetectorPersistencySvc
  IConversionSvc*      m_detPersSvc;

};
#endif // COMPONENT_XMLRELYCNV_H
