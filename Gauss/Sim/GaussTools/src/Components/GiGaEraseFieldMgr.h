// $Id: GiGaEraseFieldMgr.h,v 1.1 2003-04-06 19:07:33 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// ============================================================================
#ifndef GAUSSTOOLS_GIGAERASEFIELDMGR_H 
#define GAUSSTOOLS_GIGAERASEFIELDMGR_H 1
// Include files
// STD & STL 
#include <string>
// GiGa
#include "GiGa/GiGaToolBase.h"
#include "GiGa/GiGaFactory.h"
// Geant4
class G4LogicalVolume ;

/** @class GiGaEraseFieldMgr GiGaEraseFieldMgr.h Components/GiGaEraseFieldMgr.h
 *
 *  Concrete tool which allows to perform the erasing of 
 *  G4 field manager for specified logical volumes, e.g. 
 *  HPD schielded volumes 
 *
 *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
 *  @date   2003-04-05
 */
class GiGaEraseFieldMgr : public GiGaToolBase
{
  // friend factory for instantiation
  friend class GiGaFactory<GiGaEraseFieldMgr>;
public:
  
  /** the only one method, perform the erasure of Field Manager 
   *  @see IGiGaTool
   *  @param par parameters 
   *  @return status code
   */
  virtual StatusCode process( const std::string& parameter = "" ) const ;
  
protected:
  
  /** standard constructor
   *  @see GiGaToolBase 
   *  @see GiGaBase 
   *  @see AlgTool 
   *  @param type type of the object (?)
   *  @param name name of the object
   *  @param parent  pointer to parent object
   */
  GiGaEraseFieldMgr
  ( const std::string& type   , 
    const std::string& name   , 
    const IInterface*  parent ) ; 
  
  /// virtual destructor 
  virtual ~GiGaEraseFieldMgr() ;

protected:
  
  /** get G4 Logical Volume by name 
   *  @param name  name of G4 Logical Volume 
   *  @return pointer to G4Logical Volume 
   */
  G4LogicalVolume* volume ( const std::string& name ) const ;
  
private:
  
  // default constructor is disabled 
  GiGaEraseFieldMgr ();  
  // copy construc tor is disabled 
  GiGaEraseFieldMgr           ( const GiGaEraseFieldMgr& );
  // assignement operator is disabled 
  GiGaEraseFieldMgr& operator=( const GiGaEraseFieldMgr& );
  
private:

  typedef std::vector<std::string> Volumes ;
  
  // list of volumes, for whcih the field manager need to be removed 
  Volumes                          m_volumes ;
  
};

// ============================================================================
// The END 
// ============================================================================
#endif // COMPONENTS_GIGAERASEFIELDMGR_H
// ============================================================================
