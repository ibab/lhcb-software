// $Id: GiGaCaloHitCnv.h,v 1.2 2002-12-13 16:52:58 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.1  2002/12/07 14:41:45  ibelyaev
//  add new Calo stuff
// 
// ============================================================================
#ifndef CALO_GIGACALOHITCNV_H 
#define CALO_GIGACALOHITCNV_H 1
// Include files
// GiGaCnv 
#include "GiGaCnv/GiGaCnvBase.h"
/// forward declaration
template <class CNV> class CnvFactory;

/** @class GiGaCaloHitCnv GiGaCaloHitCnv.h Calo/GiGaCaloHitCnv.h
 *  
 *  Converter for Calorimeter Hits 
 *
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date   2002-12-06
 */

class GiGaCaloHitCnv : public GiGaCnvBase
{
  
  /// friend factory for instantiation
  friend class CnvFactory<GiGaCaloHitCnv>;
  
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
  GiGaCaloHitCnv( ISvcLocator* svc ); 
  
  /// destructor virtual and protected 
  virtual ~GiGaCaloHitCnv();

private:
  
  /// default constructor  is private
  GiGaCaloHitCnv() ;
  /// copy constructor     is private 
  GiGaCaloHitCnv           ( const GiGaCaloHitCnv& );
  /// assignement operator is private 
  GiGaCaloHitCnv& operator=( const GiGaCaloHitCnv& );
  
private:

};
// ============================================================================
// The END 
// ============================================================================
#endif // CALO_GIGACALOHITCNV_H
// ============================================================================
