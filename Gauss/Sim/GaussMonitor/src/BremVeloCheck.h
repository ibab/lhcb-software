// ============================================================================
#ifndef       BREMCHECK_H
#define       BREMCHECK_H 1 
// ============================================================================

/*
// STL 
#include <string>
#include <vector>
//
#include "GaudiKernel/IHistogramSvc.h"
#include "GaudiKernel/NTuple.h"
//#include "GaudiKernel/INTupleSvc.h"
#include "GaudiAlg/GaudiHistoAlg.h"
#include "GaudiAlg/IHistoTool.h"
#include "AIDA/IHistogram1D.h"
// GiGa

#include "G4Step.hh"
#include "G4OpticalPhoton.hh"
#include "G4Electron.hh"
#include "G4Positron.hh"
// to enable neutrinos
#include "G4QNeutrinoPhysics.hh"
#include "G4Gamma.hh"
#include "G4Timer.hh"
///
*/

// Include files
/// Geant4
#include "G4Timer.hh"
#include "G4VProcess.hh"
#include "G4ProcessManager.hh"
#include "G4Event.hh"
#include "G4Step.hh"
#include "G4OpticalPhoton.hh"
#include "G4Electron.hh"
#include "G4Positron.hh"
#include "G4TrackingManager.hh"
// GiGa
#include "GiGa/GiGaStepActionBase.h"
#include "GiGa/GiGaTrackActionBase.h"
#include "G4SteppingManager.hh"
#include "GaudiKernel/IHistogramSvc.h"
#include "GaudiKernel/NTuple.h"
#include "GaudiKernel/INTupleSvc.h"
//GaussTools
#include "GaussTools/GaussTrackInformation.h"
/// stream
#include <fstream>
#include <cstdlib>





// forward declarations 
//template <class TYPE> class GiGaFactory;
class G4Track;
class G4ParticleDefinition;

/** @class GaussPostTrackAction GaussPostTrackAction.h 
 *
 * Gauss tracking action deals with storing tracks
 * and handles track information objects
 *
 *  @author  Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @author  Witek Pokorski Witold.Pokorski@cern.ch
 *  
 *  @date    23/01/2001
 */


class BremVeloCheck: virtual public GiGaTrackActionBase
{
  /// friend factory for instantiation 
  //  friend class GiGaFactory<GaussPostTrackAction>;

public: 
  /// useful typedefs
  typedef  std::vector<std::string>                  TypeNames;
  typedef  std::vector<const G4ParticleDefinition*>  PartDefs;
  ///
  //protected:

  /** standard constructor 
   *  @see GiGaTrackActionBase 
   *  @see GiGaBase 
   *  @see AlgTool 
   *  @param type type of the object (?)
   *  @param name name of the object
   *  @param parent  pointer to parent object
   */
  BremVeloCheck
  ( const std::string& type   ,
    const std::string& name   ,
    const IInterface*  parent ) ;
  
  /// destructor (virtual and protected)
  virtual ~BremVeloCheck();

public: 

  
  /** initialize the track action  
   *  @see GiGaTrackActionBase 
   *  @see GiGaBase 
   *  @see  AlgTool 
   *  @see IAlgTool 
   *  @return status code 
   */
  virtual StatusCode initialize () ; 
  
  /** finalize the action object 
   *  @see GiGaTrackActionBase 
   *  @see GiGaBase 
   *  @see  AlgTool 
   *  @see IAlgTool 
   *  @return status code
   */
  virtual StatusCode finalize   () ;
  
  /** perform action 
   *  @see G4UserTrackingAction
   *  @param pointer to new track opbject 
   */
  virtual void PreUserTrackingAction  ( const G4Track* ) ;

  /** perform action 
   *  @see G4UserTrackingAction
   *  @param pointer to new track opbject 
   */
  virtual void PostUserTrackingAction ( const G4Track* ) ;

protected:

  ///

private:


  BremVeloCheck() ; ///< no default constructor
  BremVeloCheck( const BremVeloCheck& ) ; ///< no copy 
  BremVeloCheck& operator=( const BremVeloCheck& ) ; 

  /// Utility function to check multiplicity
  //void checkIntRegion( const G4Track * track );
  int checkIntRegion( const G4Track* );


  NTuple::Tuple*    m_ntuple;

  // track-level variables
  NTuple::Item< int >		m_trackid;
  NTuple::Item< int >		m_trackpar;
  NTuple::Array< int >		m_cptype;
  NTuple::Array< int >		m_cpstype;
  //daughters variables
  NTuple::Item< int >		m_ndaugh;
  NTuple::Array< double >	m_dau_e;
  NTuple::Array< int >		m_daughID;
  NTuple::Array< int >		m_dpstype;
  NTuple::Array< int >		m_dptype;
  NTuple::Array< int >		m_dirparent;

  
  
  AIDA::IHistogram1D* h_photon_E;
  AIDA::IHistogram1D* h_nphotons;
//  AIDA::IHistogram3D* h_photon_posXYZ;
  AIDA::IHistogram2D* h_photon_posXZ;
};

// ============================================================================
//#include "BremVeloCheck.cpp"
// ============================================================================

// ============================================================================
#endif  //  
// ============================================================================
















