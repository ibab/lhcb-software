// $Id: RelyConverter.h,v 1.5 2006-12-14 12:56:16 ranjard Exp $
#ifndef COMPONENT_RELYCONVERTER_H 
#define COMPONENT_RELYCONVERTER_H 1

// Include files
#include "DetCond/CondDBGenericCnv.h"

// Forward and external declarations
class ISvcLocator;
template <class TYPE> class CnvFactory;

/** @class RelyConverter RelyConverter.h component/RelyConverter.cpp
 *  
 *  ConditionsDBCnvSvc rely on the functionalities provided by the XmlCnvSvc.
 *  RelyConverter delegate the creation of the object to the XmlCnvSvc
 *  (via DetectorPersistencySvc).
 *
 *  @author Marco CLEMENCIC
 *  @date   2004-12-03
 */
class RelyConverter: public CondDBGenericCnv {

  /// Friend needed for instantiation
  friend class CnvFactory<RelyConverter>;

public: 

  /// Operations that can be performed by delegation
  enum Operation {
    CreateObject,
    FillObjectRefs,
    UpdateObjectRefs
  };

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
   * Resolve the references of the created transient object.
   * @param  pAddress the address of the object representation
   * @param  pObject the object created
   * @return status depending on the completion of the call
   */
  virtual StatusCode fillObjRefs (IOpaqueAddress *pAddress,
                                  DataObject *pObject);
  /**
   * Resolve the references of the just updated transient object.
   * @param  pAddress the address of the object representation
   * @param  pObject the object created
   * @return status depending on the completion of the call
   */
  virtual StatusCode updateObjRefs (IOpaqueAddress *pAddress,
                                    DataObject *pObject);
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
  static const CLID& classID () { return CLID_Any; }

protected:
  /// Standard constructor
  RelyConverter(ISvcLocator* svc); 
  virtual ~RelyConverter( ); ///< Destructor

private:

  /**
   * Do the needed steps to perform a creation by delegation.
   */
  StatusCode i_delegatedCreation(IOpaqueAddress* pAddress,
                                 DataObject *&pObject,
                                 Operation op = CreateObject);

public:
  /**
   * Extract the storage_type from the folder or description.
   */
  static long getStorageType(const std::string &path, const std::string &desc);

private:
  /// Handle to the IConversionSvc interface of the DetectorPersistencySvc
  IConversionSvc*      m_detPersSvc;

};
#endif // COMPONENT_XMLRELYCNV_H
