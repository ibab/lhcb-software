// $Id: DumpG4Track.cpp,v 1.1 2004-02-20 18:13:35 ibelyaev Exp $
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
#include "G4Track.hh"
#include "G4ParticleDefinition.hh"
#include "G4VProcess.hh"
#include "G4LogicalVolume.hh"
#include "G4Material.hh"     
// ============================================================================
#include "DumpG4Track.h"
// ============================================================================

/** @file
 *  Implementation file for function GiGaUtil::DumpG4Track
 *
 *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
 *  @date 2004-02-20 
 */

namespace GiGaUtil
{
  /**  simple function to dump G4Track
   *
   *  @param stream output stream 
   *  @param track pointer to G4Track
   *  @return outptut stream 
   *
   *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
   *  @date   2004-02-20
   */
  std::ostream&  DumpG4Track ( std::ostream& stream , 
                               const G4Track* track ) 
  {
    static const std::string s_name  = " GiGaUtil::DumpG4Track " ;
    static const std::string s_stars = std::string ( 50 , '*' ) ;
    //
    stream << s_name << (void*) track << s_stars << std::endl ;
    if ( 0 == track ) 
    { return stream << s_name << " G4Track* points to NULL" << std::endl ; }
    
    stream << s_name 
           << " TrackID/ParentID " << track->GetTrackID  () 
           << " / "                << track->GetParentID () << std::endl ;
    
    { // particle 
      const G4DynamicParticle* dp = track->GetDynamicParticle() ;
      if (  0 == dp ) 
      { stream << s_name 
               << " GetDynamicParticle: " 
               << " G4DynamicParticle*    points to NULL" << std::endl; }
    }
    
    { // definition 
      const G4ParticleDefinition* pd = track->GetDefinition() ;
      if ( 0 == pd  ) 
      { stream << s_name 
               << " GetDefinition:      "
               << " G4ParticleDefinition* points to NULL" << std::endl; }
      else 
      { stream << s_name 
               << " Particle      : " << pd->GetParticleName () << std::endl ; }
      
    }
    
    stream << s_name 
           << " Position      : " << track -> GetPosition      () << std::endl ;
    stream << s_name 
           << " GlobalTime    : " << track -> GetGlobalTime    () << std::endl ;
    stream << s_name 
           << " LocalTime     : " << track -> GetLocalTime     () << std::endl ;
    stream << s_name 
           << " ProperTime    : " << track -> GetProperTime    () << std::endl ;
    
    { // volume 
      const G4VPhysicalVolume* pVol = track->GetVolume() ;
      if ( 0 == pVol ) 
      { stream << s_name 
               << " GetVolume:          "
               << " G4VPhysicalVolume*    points to NULL" << std::endl; }
    }
    
    { // next volume (?)
      const G4VPhysicalVolume* pVol = track->GetNextVolume() ;
      if ( 0 == pVol ) 
      { stream << s_name 
               << " GetNextVolume:      "
               << " G4VPhysicalVolume*    points to NULL" << std::endl; }
    }
    
    { // material
      G4Material* mat = track->GetMaterial() ;
      if ( 0 == mat ) 
      { stream << s_name 
               << " GetMaterial:        "
               << " G4Material*           points to NULL" << std::endl; }
      else 
      { stream << s_name 
               << " GetMaterial:        " << (*mat) << std::endl ; } 
    }
    
    { // next material (?)
      G4Material* mat = track->GetNextMaterial() ;
      if ( 0 == mat ) 
      { stream << s_name 
               << " GetNextMaterial:    "
               << " G4Material*           points to NULL" << std::endl; }
      else 
      { stream << s_name 
               << " GetNextMaterial:    " << (*mat) << std::endl ; } 
    }
    
    { // material & cuts 
      const G4MaterialCutsCouple* mat = track->GetMaterialCutsCouple() ;
      if ( 0 == mat ) 
      { stream << s_name 
               << " GetMaterialcutsCouple: "
               << " G4MaterialCutsCouple* points to NULL" << std::endl; }
    }
    
    { // next material & cuts (?)
      const G4MaterialCutsCouple* mat = track->GetNextMaterialCutsCouple() ;
      if ( 0 == mat ) 
      { stream << s_name 
               << " GetNextMaterialcutsCouple: "
               << " G4MaterialCutsCouple* points to NULL" << std::endl; }
    }

    { // touchable
      const G4VTouchable* mat = track->GetTouchable() ;
      if ( 0 == mat ) 
      { stream << s_name 
               << " GetTouchable:       "
               << " G4VTouchable*         points to NULL" << std::endl; }
    }
    
    { // next touchable (?)
      const G4VTouchable* mat = track->GetNextTouchable() ;
      if ( 0 == mat ) 
      { stream << s_name 
               << " GetNextTouchable:   "
               << " G4VTouchable*         points to NULL" << std::endl; }
    }

    
    stream << s_name 
           << " KineticEnergy : " << track -> GetKineticEnergy () << std::endl ;
    stream << s_name 
           << " Direction     : " << track -> GetMomentumDirection () 
           << std::endl ;
    stream << s_name 
           << " Momentum      : " << track -> GetMomentum     ()  << std::endl ;
    stream << s_name 
           << " Velocity      : " << track -> GetVelocity     ()  << std::endl ;
    stream << s_name 
           << " Polarization  : " << track -> GetPolarization ()  << std::endl ;
    
    stream << s_name 
           << " TrackStatus   : " << track -> GetTrackStatus  ()  << std::endl ;
    
    stream << s_name 
           << " BelowThreshold: " << track -> IsBelowThreshold()  << std::endl ;
    stream << s_name 
           << " GoodForTracking " << track -> IsGoodForTracking() << std::endl ;
    
    stream << s_name 
           << " TrackLength   : " << track -> GetTrackLength  ()  << std::endl ;
    
    { // step 
      const G4Step* step = track -> GetStep () ;
      if ( 0 == step ) 
      { stream << s_name 
               << " GetStep:            "
               << " G4Step:               points to NULL" << std::endl; }
      else 
      { stream << s_name 
               << " Step          : " << (void*) step << std::endl ; }
    }  
    
    stream << s_name 
           << " StepNumber    : " << track -> GetCurrentStepNumber ()  
           << std::endl ;
    
    stream << s_name 
           << " StepLength    : " << track -> GetStepLength    () << std::endl ;
    
    stream << s_name 
           << " VertexPosition: " << track -> GetVertexPosition() << std::endl ;
    stream << s_name 
           << " VertexMomentumDirection " << 
      track -> GetVertexMomentumDirection() << std::endl ;
    stream << s_name 
           << " VertexKineticEnergy     " << track -> GetVertexKineticEnergy() 
           << std::endl ;
    
    { // logical volume at vertex 
      const G4LogicalVolume* lVol = track->GetLogicalVolumeAtVertex() ;
      if ( 0 == lVol ) 
      { stream << s_name 
               << " GetLogicalVolumeAtVertex: "
               << " G4LogicalVolume*    points to NULL" << std::endl; }
      else 
      {
        stream << s_name
               << " LogicalVolumeAtVertex : "
               << lVol->GetName() << std::endl ;
        if( 0 != lVol->GetMaterial() )
        {
          stream << s_name 
                 << " Material of the LogicalVolumeAtVertex " 
                 << std::endl ;
          stream << (*(lVol->GetMaterial())) << std::endl ;
        }
        else 
        {
          stream << s_name 
                 << " Material of the LogicalVolumeAtVertex " 
                 << " NULL " << std::endl ;
        } 
      }
    }
    
    
    { // process creator 
      const G4VProcess* proc = track->GetCreatorProcess() ;
      if ( 0 == proc ) 
      { stream << s_name 
               << " GetCreatorProcess   "
               << " G4VProcess*         points to NULL" << std::endl; }
      else 
      {
        stream << s_name 
               << " CreatorProcess Type     : " 
               << proc->GetProcessType() << std::endl ;
        stream << s_name 
               << " CreatorProcess TypeName : " 
               << G4VProcess::GetProcessTypeName( proc->GetProcessType() ) 
               << std::endl ;
        stream << s_name
               << " CreatorProcess Name     : "
               << proc->GetProcessName() << std::endl ;
        stream << s_name
               << " CreatorProcess Class    : "
               << System::typeinfoName( typeid( *proc ) ) 
               << std::endl ;
      }
    }
    
    stream << s_name 
           << " Weight        : " << track -> GetWeight        () << std::endl ;
    
    return stream ;
  };
  
};

// ============================================================================
// The END 
// ============================================================================
