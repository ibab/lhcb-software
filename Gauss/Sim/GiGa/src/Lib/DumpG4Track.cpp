// $Id: DumpG4Track.cpp,v 1.4 2004-03-17 08:54:59 ranjard Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.3  2004/02/22 16:51:09  ibelyaev
//  add DumpG4Event
//
// Revision 1.2  2004/02/22 13:38:19  ibelyaev
//  relocate DumpG4*.h files
//
// Revision 1.1  2004/02/20 18:13:35  ibelyaev
//  major update in GiGaBase and GiGaTrajectory
// 
// ============================================================================
// Include files
// ============================================================================
// Gaudi
// ============================================================================
#include "GaudiKernel/System.h"
// ============================================================================
// Geant4
// ============================================================================
#include "G4Track.hh"
#include "G4ParticleDefinition.hh"
#include "G4VProcess.hh"
#include "G4LogicalVolume.hh"
#include "G4Material.hh"     
// ============================================================================
// GiGa 
// ============================================================================
#include "GiGa/DumpG4Track.h"
// ============================================================================

/** @file
 *  Implementation file for function GiGaUtil::DumpG4Track
 *
 *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
 *  @date 2004-02-20 
 */

namespace GiGaUtil
{
  /** @fn DumpG4Track
   * 
   *  simple function to dump G4Track
   *
   *  @param  track  pointer to G4Track
   *  @param  stream output stream 
   *  @return output stream 
   *
   *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
   *  @date   2004-02-20
   */
  std::ostream& DumpG4Track 
  ( const G4Track* track  , 
    std::ostream&  stream ) { return DumpG4Track( stream , track ) ; }
  
  /**  simple function to dump G4Track
   *
   *  @param  stream output stream 
   *  @param  track  pointer to G4Track
   *  @return output stream 
   *
   *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
   *  @date   2004-02-20
   */
  std::ostream&  DumpG4Track 
  ( std::ostream& stream , 
    const G4Track* track ) 
  {
    static const std::string s_name  = " GiGaUtil::DumpG4Track " ;
    static const std::string s_stars = std::string ( 50 , '*' ) ;
    //
    //stream << s_name << (void*) track << " " << s_stars << std::endl  ;

    if ( 0 == track ) 
    { return stream << s_name << " G4Track* points to NULL" << std::endl  ; }
    
    stream << s_name 
           << " TrackID/ParentID " << track->GetTrackID  () 
           << " / "                << track->GetParentID () << std::endl  ;
    
    { // particle 
      const G4DynamicParticle* dp = track->GetDynamicParticle() ;
      if (  0 == dp ) 
      { stream << s_name 
               << " GetDynamicParticle: " 
               << " G4DynamicParticle*    points to NULL" << std::endl ; }
    }
    
    { // definition 
      const G4ParticleDefinition* pd = track->GetDefinition() ;
      if ( 0 == pd  ) 
      { stream << s_name 
               << " GetDefinition:      "
               << " G4ParticleDefinition* points to NULL" << std::endl ; }
      else 
      { stream << s_name 
               << " Particle      : " << pd->GetParticleName () << std::endl  ; }
      
    }
    
    stream << s_name 
           << " Position      : " << track -> GetPosition      () << std::endl  ;
    stream << s_name 
           << " GlobalTime    : " << track -> GetGlobalTime    () << std::endl  ;
    stream << s_name 
           << " LocalTime     : " << track -> GetLocalTime     () << std::endl  ;
    stream << s_name 
           << " ProperTime    : " << track -> GetProperTime    () << std::endl  ;

           
    stream << s_name 
           << " KineticEnergy : " << track -> GetKineticEnergy () << std::endl  ;
    stream << s_name 
           << " Direction     : " << track -> GetMomentumDirection () 
           << std::endl  ;
    stream << s_name 
           << " Momentum      : " << track -> GetMomentum     ()  << std::endl  ;
    stream << s_name 
           << " Velocity      : " << track -> GetVelocity     ()  << std::endl  ;
    stream << s_name 
           << " Polarization  : " << track -> GetPolarization ()  << std::endl  ;
    
    stream << s_name 
           << " TrackStatus   : " << track -> GetTrackStatus  ()  << std::endl  ;
    
    stream << s_name 
           << " BelowThreshold: " << track -> IsBelowThreshold()  << std::endl  ;
    stream << s_name 
           << " GoodForTracking " << track -> IsGoodForTracking() << std::endl  ;
    
    stream << s_name 
           << " TrackLength   : " << track -> GetTrackLength  ()  << std::endl  ;
    
    { // step 
      const G4Step* step = track -> GetStep () ;
      if ( 0 == step ) 
      { stream << s_name 
               << " GetStep:            "
               << " G4Step:               points to NULL" << std::endl ; }
      else 
      { stream << s_name 
               << " Step          : " << (void*) step << std::endl  ; }
    }  
    
    stream << s_name 
           << " StepNumber    : " << track -> GetCurrentStepNumber ()  
           << std::endl  ;
    
    stream << s_name 
           << " StepLength    : " << track -> GetStepLength    () << std::endl  ;
    
    stream << s_name 
           << " VertexPosition: " << track -> GetVertexPosition() << std::endl  ;
    stream << s_name 
           << " VertexMomentumDirection " << 
      track -> GetVertexMomentumDirection() << std::endl  ;
    stream << s_name 
           << " VertexKineticEnergy     " << track -> GetVertexKineticEnergy() 
           << std::endl  ;
    
    { // logical volume at vertex 
      const G4LogicalVolume* lVol = track->GetLogicalVolumeAtVertex() ;
      if ( 0 == lVol ) 
      { stream << s_name 
               << " GetLogicalVolumeAtVertex: "
               << " G4LogicalVolume*    points to NULL" << std::endl ; }
      else 
      {
        stream << s_name
               << " LogicalVolumeAtVertex : "
               << lVol->GetName() << std::endl  ;
        if( 0 != lVol->GetMaterial() )
        {
          stream << s_name 
                 << " Material of the LogicalVolumeAtVertex " 
                 << std::endl  ;
          stream << (*(lVol->GetMaterial())) << std::endl  ;
        }
        else 
        {
          stream << s_name 
                 << " Material of the LogicalVolumeAtVertex " 
                 << " NULL " << std::endl  ;
        } 
      }
    }
    
    
    { // process creator 
      const G4VProcess* proc = track->GetCreatorProcess() ;
      if ( 0 == proc ) 
      { stream << s_name 
               << " GetCreatorProcess   "
               << " G4VProcess*         points to NULL" << std::endl ; }
      else 
      {
        stream << s_name 
               << " CreatorProcess Type     : " 
               << proc->GetProcessType() << std::endl  ;
        stream << s_name 
               << " CreatorProcess TypeName : " 
               << G4VProcess::GetProcessTypeName( proc->GetProcessType() ) 
               << std::endl  ;
        stream << s_name
               << " CreatorProcess Name     : "
               << proc->GetProcessName() << std::endl  ;
        stream << s_name
               << " CreatorProcess Class    : "
               << System::typeinfoName( typeid( *proc ) ) 
               << std::endl  ;
      }
    }
    
    stream << s_name 
           << " Weight        : " << track -> GetWeight        () << std::endl  ;
    
    return stream ;
  };

};

// ============================================================================
// The END 
// ============================================================================
