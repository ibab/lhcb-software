// $Id: DumpG4Step.cpp,v 1.3 2004-02-22 16:51:09 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
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
#include "G4Step.hh"
#include "G4StepPoint.hh"
#include "G4VProcess.hh"
// ============================================================================
#include "GiGa/DumpG4Step.h"
#include "GiGa/DumpG4Track.h"
// ============================================================================

/** @file
 *  Implementation file for function GiGaUtil::DumpG4Step
 *
 *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
 *  @date 2004-02-20 
 */

namespace GiGaUtil
{
  /** @fn DumpG4Step 
   * 
   *  simple function to dump G4Step
   *
   *  @param  step pointer to G4Step
   *  @param  stream output stream 
   *  @return output stream 
   *
   *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
   *  @date   2004-02-20
   */
  std::ostream&  DumpG4Step
  ( const G4Step* step   ,
    std::ostream& stream ) { return DumpG4Step ( stream , step ) ; }

  /** @fn DumpG4Step 
   * 
   *  simple function to dump G4Step
   *
   *  @param  stream output stream 
   *  @param  step pointer to G4Step
   *  @return output stream 
   *
   *  @author Vanya BELYAEV Ivan.Belyaev@itep.ru
   *  @date   2004-02-20
   */
  std::ostream&  DumpG4Step  
  ( std::ostream& stream , 
    const G4Step* step   ) 
  {
    static const std::string s_name  = " GiGaUtil::DumpG4Step  " ;
    static const std::string s_stars = std::string ( 50 , '*' ) ;
    //
    stream << s_name << (void*) step << " " << s_stars << std::endl ;
    
    if ( 0 == step  ) 
    { return stream << s_name << " G4Step*  points to NULL" << std::endl ; }

    
    { //  pre 
      const G4StepPoint* point = step->GetPreStepPoint() ;
      if ( 0 == point )
      { stream << s_name 
               << " GetPreStepPoint   : "
               << " G4StepPoint*          points to NULL" << std::endl; }
      else 
      { 
        stream << s_name 
               << " PreStepPointPosition   :" 
               << point -> GetPosition   () << std::endl ; 
        stream << s_name 
               << " PreStepPointGlobalTime :" 
               << point -> GetGlobalTime () << std::endl ; 
        stream << s_name 
               << " PreStepPointLocalTime  :" 
               << point -> GetLocalTime  () << std::endl ; 
        stream << s_name 
               << " PreStepPointProperTime :" 
               << point -> GetProperTime () << std::endl ; 
        stream << s_name 
               << " PreStepPointMomentum   :" 
               << point -> GetMomentum   () << std::endl ; 
        G4Material* mat = point->GetMaterial() ;
        if ( 0 != mat ) { stream << (*mat) << std::endl ; }
        const G4VProcess* proc = point->GetProcessDefinedStep() ;
        if ( 0 != proc )
        {
          stream << s_name 
                 << " Pre.  Process Type     : " 
                 << proc->GetProcessType() << std::endl ;
          stream << s_name 
                 << " Pre.  Process TypeName : " 
                 << G4VProcess::GetProcessTypeName( proc->GetProcessType() ) 
                 << std::endl ;
          stream << s_name
                 << " Pre.  Process Name     : "
                 << proc->GetProcessName() << std::endl ;
          stream << s_name
                 << " Pre.  Process Class    : "
                 << System::typeinfoName( typeid( *proc ) ) 
                 << std::endl ;
        }        
      }
    }

    { //  post 
      const G4StepPoint* point = step->GetPostStepPoint() ;
      if ( 0 == point )
      { stream << s_name 
               << " GetPostStepPoint  : "
               << " G4StepPoint*          points to NULL" << std::endl; }
      else 
      { 
        stream << s_name 
               << " PostStepPointPosition  :" 
               << point -> GetPosition   () << std::endl ; 
        stream << s_name 
               << " PostStepPointGlobalTime:" 
               << point -> GetGlobalTime () << std::endl ; 
        stream << s_name 
               << " PostStepPointLocalTime :" 
               << point -> GetLocalTime  () << std::endl ; 
        stream << s_name 
               << " PostStepPointProperTime:" 
               << point -> GetProperTime () << std::endl ; 
        stream << s_name 
               << " PostStepPointMomentum  :" 
               << point -> GetMomentum   () << std::endl ; 
        G4Material*       mat  = point->GetMaterial() ;
        if ( 0 != mat  ) { stream << (*mat) << std::endl ; }
        const G4VProcess* proc = point->GetProcessDefinedStep() ;
        if ( 0 != proc )
        {
          stream << s_name 
                 << " Post. Process Type     : " 
                 << proc->GetProcessType() << std::endl ;
          stream << s_name 
                 << " Post. Process TypeName : " 
                 << G4VProcess::GetProcessTypeName( proc->GetProcessType() ) 
                 << std::endl ;
          stream << s_name
                 << " Post. Process Name     : "
                 << proc->GetProcessName() << std::endl ;
          stream << s_name
                 << " Post. Process Class    : "
                 << System::typeinfoName( typeid( *proc ) ) 
                 << std::endl ;
        }        
      }      
    }
    
    stream << s_name 
           << " StepLength         :"          
           << step -> GetStepLength         () << std::endl ;
    
    stream << s_name 
           << " TotalEnergyDeposit :"          
           << step -> GetTotalEnergyDeposit () << std::endl ;
    
    stream << s_name 
           << " ControlFlag        :"          
           << step -> GetControlFlag        () << std::endl ;

    stream << s_name 
           << " DeltaPosition      :"          
           << step -> GetDeltaPosition      () << std::endl ;

    stream << s_name 
           << " DeltaMomentum      :"          
           << step -> GetDeltaMomentum      () << std::endl ;
    

    stream << s_name 
           << " DeltaEnergy        :"          
           << step -> GetDeltaEnergy        () << std::endl ;
    
    
    return GiGaUtil::DumpG4Track( stream , step->GetTrack() )  ;
  };
  
};

// ============================================================================
// The END 
// ============================================================================
