// $Id: GiGaEraseFieldMgr.cpp,v 1.1 2003-04-06 19:07:33 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// ============================================================================
// Include files 
// Geant4 
#include "G4LogicalVolume.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4FieldManager.hh"
// GiGa 
#include "GiGa/GiGaUtil.h"
// local
#include "GiGaEraseFieldMgr.h"

// ============================================================================
/** @file
 *
 *  Implementation file for class : GiGaEraseFieldMgr
 *
 *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
 *  @date   2003-04-05
 */
// ============================================================================

// ============================================================================
/** standard constructor
 *  @see GiGaToolBase 
 *  @see GiGaBase 
 *  @see AlgTool 
 *  @param type type of the object (?)
 *  @param name name of the object
 *  @param parent  pointer to parent object
 */
// ============================================================================
GiGaEraseFieldMgr::GiGaEraseFieldMgr
( const std::string& type   , 
  const std::string& name   , 
  const IInterface*  parent ) 
  : GiGaToolBase ( type , name , parent )
  , m_volumes    ()
{
  declareProperty ( "Volumes" , m_volumes ) ;
};
// ============================================================================

// ============================================================================
/// destructor 
// ============================================================================
GiGaEraseFieldMgr::~GiGaEraseFieldMgr(){};
// ============================================================================

// ============================================================================
/** get G4 Logical Volume by name 
 *  @param name  name of G4 Logical Volume 
 *  @return pointer to G4Logical Volume 
 */
// ============================================================================
G4LogicalVolume* GiGaEraseFieldMgr::volume ( const std::string& name ) const 
{
  // locate volume store 
  const G4LogicalVolumeStore* store =  G4LogicalVolumeStore::GetInstance();
  if( 0 == store ) 
    { Error(" volume(): G4LogicalVolumeStore is invalid!" ) ; return 0 ; }
  for( G4LogicalVolumeStore::const_iterator ivol = store->begin() ; 
       store->end() != ivol ; ++ivol ) 
    {
      G4LogicalVolume* vol = *ivol ;
      if( 0 != vol && vol->GetName() == name ) { return vol ; }
    }
  Error ( "volume(): G4LogicalVolume('" + name + "') does not exist!" ) ;
  return 0 ;
};
// ============================================================================

// ============================================================================
/** the only one method, perform the erasure of Field Manager 
 *  @see IGiGaTool
 *  @return status code 
 */
// ============================================================================
StatusCode GiGaEraseFieldMgr::process ( const std::string& /* par */ ) const 
{
  if( m_volumes.empty() ) 
    { return Warning("process(): empty volume list" , StatusCode::SUCCESS ) ; }
  
  for( Volumes::const_iterator vol = m_volumes.begin() ; 
       m_volumes.end() != vol ; ++vol ) 
    {
      // get G4 volume 
      G4LogicalVolume* g4lv = volume ( *vol );
      if( 0 == g4lv )          { continue ; }                    // CONTINUE
      
      // get existing namager 
      const G4FieldManager* mgr = g4lv -> GetFieldManager() ;
      
      if( 0 != mgr ) 
        {
          Print ( "process('" + g4lv->GetName() + "'): erase FieldManager '" + 
                  GiGaUtil::ObjTypeName( mgr                     )   + "'/'" + 
                  GiGaUtil::ObjTypeName( mgr->GetDetectorField() )   + "'  " ,
                  StatusCode::SUCCESS , MSG::INFO                            ); 
          
          g4lv -> SetFieldManager( 0 , true  ) ;
        }
      else 
        { Warning( "process('" + g4lv -> GetName() +
                   "'): FieldManager is NULL" ) ; }
    }
  
  return StatusCode::SUCCESS ;
}
// ============================================================================

// ============================================================================
// The END
// ============================================================================
