// $Id: GiGaSensDetBudget.cpp,v 1.4 2008-07-26 15:43:52 robbep Exp $
// Include files

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h" 
#include "GaudiKernel/MsgStream.h" 
#include "GaudiKernel/PropertyMgr.h" 
// GiGa
//#include "GiGa/GiGaMACROs.h"
// local
#include "GiGaSensDetBudget.h"
// G4 
#include "G4Step.hh"
#include "G4TouchableHistory.hh"
#include "G4VPhysicalVolume.hh"
#include "G4LogicalVolume.hh"
#include "G4Material.hh"

// ============================================================================
/** @file
 *  
 *  Implementation file for class GiGaSensDetBudget.
 *  Sensitive Detector, which is able to calculate the material budget 
 *
 *  @author Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date   04/05/2002
 */
// ============================================================================

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( GiGaSensDetBudget )


// ============================================================================
/** standard constructor 
 *  @see GiGaSensDetBase 
 *  @see GiGaBase 
 *  @see AlgTool 
 *  @param type type of the object (?)
 *  @param name name of the object
 *  @param parent  pointer to parent object
 */
// ============================================================================
GiGaSensDetBudget::GiGaSensDetBudget
( const std::string& type   ,
  const std::string& name   ,
  const IInterface*  parent ) 
  : G4VSensitiveDetector ( name  )
  , GiGaSensDetBase      ( type , name , parent ) 
  , m_budgetL            (       )
  , m_budgetP            (       )
  , m_budgetM            (       )
  , m_lvolume            ( true  ) // accumulate by logical  volume name 
  , m_pvolume            ( true  ) // accumulate by physical volume name 
  , m_material           ( true  ) // accumulate by material name 
{
  declareProperty( "Logical"  , m_lvolume  );
  declareProperty( "Physical" , m_pvolume  );
  declareProperty( "Material" , m_material );
}



// ============================================================================
// destructor 
// ============================================================================
GiGaSensDetBudget::~GiGaSensDetBudget() {} 


// ============================================================================
/** process the hit
 *  @param step     pointer to current Geant4 step 
 *  @param history  pointer to touchable history 
 */
// ============================================================================
bool GiGaSensDetBudget::ProcessHits
( G4Step*                step       , 
  G4TouchableHistory* /* history */ ) 
{
  // check argument
  if( 0 == step ) { Error(" G4Step* points to  NULL!"    ) ; return false ; }
  
  G4TouchableHistory* TT =  
    (G4TouchableHistory*)( step -> GetPreStepPoint () -> GetTouchable () );
  if( 0 == TT   ) { Error(" TouchableHistory* is NULL!"  ) ; return false ; }
  const G4VPhysicalVolume*  PV =   TT->GetVolume();
  if( 0 == PV   ) { Error(" G4VPhysicalVolume* is NULL!" ) ; return false ; }
  const G4LogicalVolume*    LV =   PV->GetLogicalVolume();
  if( 0 == PV   ) { Error(" G4LogicalVolume*   is NULL!" ) ; return false ; }
  const G4Material*         MT = LV->GetMaterial() ;
  if( 0 == MT   ) { Error(" G4Material*        is NULL!" ) ; return false ; }
  
  // step length 
  const double length = 
    ( step->GetPostStepPoint()->GetPosition() - 
      step->GetPreStepPoint ()->GetPosition() ).r();
  
  // radiation length of material  
  const double radlen = MT->GetRadlen();
  
  // accumulate material budget 
  if( m_lvolume  ) { m_budgetL[ LV->GetName() ] += length / radlen ; }
  if( m_pvolume  ) { m_budgetP[ PV->GetName() ] += length / radlen ; }
  if( m_material ) { m_budgetM[ MT->GetName() ] += length / radlen ; }
  
  return false ;
  
}
// ============================================================================


namespace Local
{  
  inline std::string print( const double value            , 
                            const char*  format = "%9.3g" )
  {
    static char buf[64];
    return std::string( buf , buf + sprintf( buf , format , value ) ) ;
  }
}


// ============================================================================
/** finalize the sensitive detector 
 *  @see GiGaSensDetBase 
 *  @see GiGaBase 
 *  @return status code 
 */
// ============================================================================
StatusCode GiGaSensDetBudget::finalize   () 
{
  // printout 
  MsgStream log( msgSvc() , name() ) ;
  const std::string stars( 80 , '*' );
  log << MSG::INFO   << stars                                << endreq ;
  log << MSG::ALWAYS << " Sorted Material budget listing "             ;
  if( m_material ) { log << " Materials " ; }
  if( m_lvolume  ) { log << " LogVols   " ; }
  if( m_pvolume  ) { log << " PhysVols  " ; }
  log << endreq ;
  typedef std::pair<double,const std::string*> Entry   ;
  typedef std::vector<Entry>                   Entries ;
  if( m_material )
    {
      log << MSG::INFO   << stars                                << endreq ;
      Entries tmp  ;      
      double total = 0 ;
      for( Budget::const_iterator it1 = m_budgetM.begin() ; 
           it1 != m_budgetM.end() ; ++it1 )
        {
          total += it1 -> second ;
          tmp.push_back( Entry( it1->second , &(it1->first) ) );
        }
      std::stable_sort( tmp.begin() , tmp.end() );
      for( Entries::reverse_iterator ri = tmp.rbegin() ;
           tmp.rend() != ri ; ++ri )
        {
          log << MSG::INFO 
              << " Budget: "       << Local::print( ri->first )
              << " Material   : '" <<            *(ri->second)  
              << "'" << endreq ;
        }
      log << MSG::ALWAYS << " Total Evaluated Budget ( Materials   ) = " ;
      log << Local::print( total ) << endreq  ;
      tmp.clear();
    }
  if( m_lvolume )
    {    
      log << MSG::INFO   << stars                                << endreq ;
      Entries tmp  ;      
      double total = 0 ;
      for( Budget::const_iterator it1 = m_budgetL.begin() ; 
           it1 != m_budgetL.end() ; ++it1 )
        {
          total += it1 -> second ;
          tmp.push_back( Entry( it1->second , &(it1->first) ) );
        }
      std::stable_sort( tmp.begin() , tmp.end() );
      for( Entries::reverse_iterator ri = tmp.rbegin() ;
           tmp.rend() != ri; ++ri )
        {
          log << MSG::INFO 
              << " Budget: "       << Local::print( ri->first )
              << " LogVolume  : '" <<            *(ri->second)  
              << "'" << endreq ;
        }
      log << MSG::ALWAYS << " Total Evaluated Budget ( LogVolumes  ) = " ;
      log << Local::print( total ) << endreq  ;
      tmp.clear();
    }
  if( m_pvolume )
    {    
      log << MSG::INFO   << stars                                << endreq ;
      Entries tmp  ;      
      double total = 0 ;
      for( Budget::const_iterator it1 = m_budgetP.begin() ; 
           it1 != m_budgetP.end() ; ++it1 )
        {
          total += it1 -> second ;
          tmp.push_back( Entry( it1->second , &(it1->first) ) );
        }
      std::stable_sort( tmp.begin() , tmp.end() );
      for( Entries::reverse_iterator ri = tmp.rbegin() ;
           tmp.rend() != ri   ; ++ri )
        {
          log << MSG::INFO 
              << " Budget: "       << Local::print( ri->first )
              << " PhysVolume : '" <<            *(ri->second)  
              << "'" << endreq ;
        }
      log << MSG::ALWAYS << " Total Evaluated Budget ( PhysVolumes ) = " ;
      log << Local::print( total ) << endreq  ;
      tmp.clear();
    }
  log << MSG::INFO << stars << endreq ;  
  // clear the budget tables 
  m_budgetL.clear () ;  
  m_budgetP.clear () ;
  m_budgetM.clear () ;
  // finalize the base class 
  return GiGaSensDetBase::finalize ();
}


// ============================================================================
// The END 
// ============================================================================
