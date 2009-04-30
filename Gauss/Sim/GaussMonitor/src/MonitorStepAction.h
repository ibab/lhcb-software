
// ============================================================================
#ifndef       GaussMonitor_MonitorStepAction_H
#define       GaussMonitor_MonitorStepAction_H 1 
// ============================================================================
// include files 
/// Geant4
#include "G4Timer.hh"
#include "G4VProcess.hh"
#include "G4ProcessManager.hh"
// GiGa
#include "GiGa/GiGaStepActionBase.h"
#include "G4SteppingManager.hh"
// Gaudi Kernel
#include "GaudiKernel/IHistogramSvc.h"
#include "GaudiKernel/NTuple.h"
//GaussTools
#include "GaussTools/GaussTrackInformation.h"
/// stream
#include <fstream>
#include <cstdlib>

/** @class MonitorStepAction MonitorStepAction.h
 *   
 *  
 *  
 */

class MonitorStepAction: virtual public GiGaStepActionBase
{

  //protected:

  /** standard constructor 
   *  @see GiGaStepActionBase 
   *  @see GiGaBase 
   *  @see AlgTool 
   *  @param type type of the object (?)
   *  @param name name of the object
   *  @param parent  pointer to parent object
   */
 
 public: 
 
  MonitorStepAction
    ( const std::string& type   ,
      const std::string& name   ,
      const IInterface*  parent ) ;
  
  /// destructor 
  ~MonitorStepAction();
  

 virtual StatusCode initialize () ;
 
 virtual StatusCode finalize   () ;

public:

/* stepping action
*  @see G4UserSteppingAction
*  @param step Geant4 step
*/
  
  /* MonitorStepAction(); ///< no default constructor
  MonitorStepAction( const MonitorStepAction& ); ///< no copy  
  MonitorStepAction& operator=( const MonitorStepAction& ) ; ///< no =*/
 

  virtual void UserSteppingAction ( const G4Step* );
  void OutputDatas();
  void MSInit();
  
 private:
 
  G4StepPoint *thePreStepPoint,*thePostStepPoint;
  const G4VPhysicalVolume* Vol;
  const G4VProcess* theCProcess;
  std::string m_oldvolname;
  std::string ParticleName, old_name, initial_volume,CPname;

  std::string volumes[20];

  // variables
  G4ThreeVector DeltaPos, position,postposition, initial_position;
  G4Track *oldTrack;
  G4double energy, radius, max_radius,  initial_energy;

  // Counters
  int m_nTracks;
  int m_nSteps;

  //
  G4Track* track;
  
  G4VUserTrackInformation* uinf;
  
  G4ParticleDefinition* partdef;
  
  GaussTrackInformation* ginf;

 public:
  std::ofstream Vmsgr;

 private:
  bool m_first;
  

  // Histograms to fill
  IHistogram1D* m_hStepNumberMF;
  IHistogram1D* m_hStepLengthMF;

};

// ============================================================================
// The END 
// ============================================================================
#endif  ///<    GaussMonitor_MonitorStepAction_H
// ============================================================================
