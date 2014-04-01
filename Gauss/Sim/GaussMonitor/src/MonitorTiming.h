// ============================================================================
#ifndef       GaussTools_MonitorTiming_H
#define       GaussTools_MonitorTiming_H 1 
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
// forward declarations 
template <class TYPE> class GiGaFactory;

/** @class MonitorTiming MonitorTiming.h
 *   
 *  @author  James McCarthy
 *  @date    12/2013
 */

class MonitorTiming: virtual public GiGaStepActionBase
{
  /// friend factory for instantiation
  friend class GiGaFactory<MonitorTiming>;
  
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
  
  MonitorTiming
  ( const std::string& type   ,
    const std::string& name   ,
    const IInterface*  parent ) ;
  
  /// destructor 
  ~MonitorTiming();
  
  virtual StatusCode initialize () ; 
  virtual StatusCode finalize   () ;
  
public:
  
  /* stepping action
   *  @see G4UserSteppingAction
   *  @param step Geant4 step
   */
  
  /* MonitorTiming(); ///< no default constructor
  MonitorTiming( const MonitorTiming& ); ///< no copy  
  MonitorTiming& operator=( const MonitorTiming& ) ; ///< no =*/

  virtual void UserSteppingAction ( const G4Step* );
  void OutputDatas();
  void MSInit();
  
 private:
 
  //Step information variables
  G4StepPoint *thePreStepPoint,*thePostStepPoint;

  //Timers
  G4Timer *totalTimer, *stepTimer; //*trackTimer;

  //Volume variables
  const G4VPhysicalVolume* Vol;
  std::vector<double> cumulated_time_vol;
  std::vector<std::string> volumes;
  
  std::vector<std::string> subVolumes;
  std::vector<double> cumulated_time_subvol;
  
  //std::vector<std::vector<std::string> > vec_subVolumes;
  
  std::vector<std::string> knownVolumes;
  std::vector<std::string> knownVolumes_2;
  std::vector<std::string> otherVolumes;
  std::string VolName, VolumeName;

  G4double stepTime, totalTime, totalCumTime;

  bool firstStep;


  //process variables
  G4SteppingManager* theSteppingManager;

  std::string processName;
  std::vector<double> cumulated_time_proc;
  std::vector<std::string> processes;
    
  //Particle Control Variables
  //G4Track* track;
  
  //G4VUserTrackInformation* uinf;
  
  //G4ParticleDefinition* partdef;
  
  //GaussTrackInformation* ginf;

  //const G4VProcess* theCProcess;
  //std ::string oldvolname, ParticleName, old_name, initial_volume,CPname;
  
  //G4ThreeVector DeltaPos, position, initial_position;
  //G4Track *oldTrack;
  //G4double energy, radius, max_radius, time, time0, timecumul, initial_energy;
  //int r, loops, volchanged; // r is a boolean variable to check if the clock already Runs
  //int keep, stepnb;
    
public:
  std::ofstream Vmsgr;
  
  
private:
  //std::string  m_OutputFileName;
  std::string  m_TimerFileName;
  //const char*  m_OutputFileName2;
  const char*  m_TimerFileName2;

  //std::vector<sdt::string> m_Volumes;
  /*
  NTuple::Tuple* m_datas;
  
  NTuple::Item<long>              m_ntrk;
  NTuple::Item<float>             m_energy;
  // Items for the column wise n-tuple
  NTuple::Array<long>             m_iNumbers;
  NTuple::Array<float>            m_fNumbers;
  NTuple::Item<long>              m_n;
  */
};

// ============================================================================

// ============================================================================
// The END 
// ============================================================================
#endif  ///<    GaussTools_MonitorTiming_H
// ============================================================================
