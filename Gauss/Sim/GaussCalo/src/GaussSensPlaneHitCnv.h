// $Id: GaussSensPlaneHitCnv.h,v 1.1 2003-07-07 16:09:39 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// ============================================================================
#ifndef GAUSSCALO_GAUSSSENSPLANEHITCNV_H 
#define GAUSSCALO_GAUSSSENSPLANEHITCNV_H 1
// Include files
// GiGaCnv 
#include "GiGaCnv/GiGaCnvBase.h"
/// forward declaration
template <class CNV> class CnvFactory;

/** @class GaussSensPlaneHitCnv GaussSensPlaneHitCnv.h
 *  
 *  Converter for Calorimeter Sensitive Plane Hits  
 *
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date   2002-12-06
 */

class GaussSensPlaneHitCnv : public GiGaCnvBase
{
  
  /// friend factory for instantiation
  friend class CnvFactory<GaussSensPlaneHitCnv>;
  
public:
  
  /** initialize the converter 
   *  @return status code
   */
  virtual StatusCode initialize() ;
  
  /** create the Object 
   *  @param Address address 
   *  @param Object object itself 
   *  @return status code 
   */
  virtual StatusCode createObj     
  ( IOpaqueAddress*     Address   , 
    DataObject*&        Object    ) ;
  
  /** update the Object 
   *  @param Address address 
   *  @param Object object itself 
   *  @return status code 
   */
  virtual StatusCode updateObj     
  ( IOpaqueAddress*     Address   , 
    DataObject*         Object    ) ; 
  
  /** retrieve the class identifier  for created object 
   *  @return class idenfifier for created object 
   */
  static const CLID&         classID();
  
  /** retrieve the storage type for created object 
   *  @return storage type  for created object 
   */
  static const unsigned char storageType() ;
  
protected:
  
  /** Standard constructor
   *  @param svc pointer to Service Locator 
   */
  GaussSensPlaneHitCnv( ISvcLocator* svc ); 
  
  /// destructor virtual and protected 
  virtual ~GaussSensPlaneHitCnv();

private:
  
  /// default constructor  is private
  GaussSensPlaneHitCnv() ;
  /// copy constructor     is private 
  GaussSensPlaneHitCnv           ( const GaussSensPlaneHitCnv& );
  /// assignement operator is private 
  GaussSensPlaneHitCnv& operator=( const GaussSensPlaneHitCnv& );
  
private:
  
};
// ============================================================================
// The END 
// ============================================================================
#endif // GAUSSCALO_GAUSSSENSPLANEHITCNV_H
// ============================================================================
