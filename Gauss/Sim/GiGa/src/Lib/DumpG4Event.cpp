// $Id: DumpG4Event.cpp,v 1.1 2004-02-22 16:51:09 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// ============================================================================
// Include files
// ============================================================================
// Gaudi
// ============================================================================
#include "GaudiKernel/System.h"
// ============================================================================
// Geant4
// ============================================================================
#include "G4Event.hh"
#include "G4HCofThisEvent.hh"
#include "G4VHitsCollection.hh"
#include "G4DCofThisEvent.hh"
#include "G4TrajectoryContainer.hh"
// ============================================================================
// GiGa 
// ============================================================================
#include "GiGa/DumpG4Event.h"
// ============================================================================

/** @file 
 *  Implementation file for function DumpG4Event
 *  @date 2004-02-22 
 *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
 */

namespace GiGaUtil 
{
  
  /** @fn DumpG4Event
   * 
   *  simple function to dump class G4Event
   *
   *  @param  stream output stream 
   *  @param  event  pointer to class G4Event
   *  @return output stream 
   *
   *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
   *  @date   2004-02-20
   */
  std::ostream& DumpG4Event 
  ( std::ostream&  stream , 
    const G4Event* event  ) { return DumpG4Event ( event , stream ) ; }
  
  /** @fn DumpG4Event
   * 
   *  simple function to dump class G4Event
   *
   *  @param  event  pointer to class G4Event
   *  @param  stream output stream 
   *  @return output stream 
   *
   *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
   *  @date   2004-02-20
   */
  std::ostream& DumpG4Event
  ( const G4Event* event  , 
    std::ostream&  stream ) 
  {
    static const std::string s_name  = " GiGaUtil::DumpG4Event " ;
    static const std::string s_stars = std::string ( 50 , '*' ) ;
    //
    stream << s_name << (void*) event << " " << s_stars << std::endl ;
    
    if ( 0 == event ) 
    { return stream << s_name << " G4Event* points to NULL" << std::endl ; }
    
    stream << s_name 
           << " EventID                : " 
           << event -> GetEventID               () 
           << std::endl ;
    stream << s_name 
           << " NumberOfPrimaryVertex  : "  
           << event -> GetNumberOfPrimaryVertex () 
           << std::endl ;

    { // tracks
      G4TrajectoryContainer* tracks  = event -> GetTrajectoryContainer () ;
      if( 0 == tracks ) 
      { stream << s_name 
               << " GetTrajectoryContainer : " 
               << " G4TrajectoryContainer*    points to NULL" << std::endl ; }
      else 
      { stream << s_name 
               << " #StoredTrajectories    : "  
               << tracks -> size               () 
               << std::endl ;
      }
    }
    
    { // hits
      G4HCofThisEvent* hits  = event -> GetHCofThisEvent() ;
      if( 0 == hits ) 
      { stream << s_name 
               << " GetHCofThisEvent       : " 
               << " G4HCofThisEvent*          points to NULL" << std::endl ; }
      else 
      { 
        stream << s_name 
               << " # of Hit Collections       : "  
               << hits -> GetNumberOfCollections ()  
               << std::endl ;
        for( int i = 0  ; i < hits->GetCapacity()  ; ++i ) 
        {
          G4VHitsCollection* c = hits->GetHC( i ) ;
          if ( 0 == c ) { continue ; }
          stream << s_name 
                 << " Collection:" 
                 << " ID = "       << i   
                 << " \tName = '"  << c -> GetName   () << "'" 
                 << " \tSD = '"    << c -> GetSDname () << "'" 
                 << " \t#Hits = "  << c -> GetSize   () << std::endl ;
        }
      }
    }
    
    { // digits 
      G4DCofThisEvent* digits  = event -> GetDCofThisEvent() ;
      if( 0 == digits ) 
      { stream << s_name 
               << " GetDCofThisEvent       : " 
               << " G4DCofThisEvent*          points to NULL" << std::endl ; }
    }

    stream << s_name 
           << " IsAborted              : "  
           << event -> IsAborted                () 
           << std::endl ;
    
    
    { // info 
      G4VUserEventInformation* info  = event -> GetUserInformation () ;
      if( 0 == info ) 
      { stream << s_name 
               << " GetUserInformation     : " 
               << " G4VUserEventInformation* points to NULL" << std::endl ; }
    }
    
    return stream ;
  }

} // end of namespace GiGaUtil



// ============================================================================
// The END 
// ============================================================================
