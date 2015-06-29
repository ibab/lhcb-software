// $Id: $
#ifndef RADLENGTHCOLL_RADLENGTHCOLL_H 
#define RADLENGTHCOLL_RADLENGTHCOLL_H 1

// Include files
/// Geant4
#include "G4Timer.hh"
#include "G4VProcess.hh"
#include "G4ProcessManager.hh"
#include "G4Event.hh"
// GiGa
#include "GiGa/GiGaStepActionBase.h"
#include "G4SteppingManager.hh"
#include "GaudiKernel/IHistogramSvc.h"
#include "GaudiKernel/NTuple.h"
#include "GaudiKernel/INTupleSvc.h"
//GaussTools
#include "GaussTools/GaussTrackInformation.h"
/// stream
#include <fstream>
#include <cstdlib>

/** @class RadLengthColl RadLengthColl.h RadLengthColl/RadLengthColl.h
 *  

 *
 *  @author Yasmine Sarah Amhis
 *  @date   2005-07-15
 */

class RadLengthColl : virtual public GiGaStepActionBase {

public:
  
  RadLengthColl( const std::string& type, 
                   const std::string& name,
                   const IInterface*  parent );

  virtual ~RadLengthColl( ); ///< Destructor
  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode finalize  ();    ///< Algorithm finalization
  virtual void UserSteppingAction ( const G4Step* );
  
private:

  G4Track* track;
  G4ParticleDefinition* partdef;
  G4StepPoint *thePreStepPoint;

  const G4VPhysicalVolume* Vol;
  const G4LogicalVolume* VOL;
  
  std::string VolName, VolumeName, ParticleName, IDPlane;
  std::string ntname;

  // Radiation Length
  G4double MaterialRadiationLength;
  G4double theRadLength, thePlane2PlaneRadLength, theCumulatedRadLength;
  // Interation Length
  G4double MaterialInterLength;
  G4double theInterLength, thePlane2PlaneInterLength, theCumulatedInterLength;
  
  G4double StepLength;
  G4ThreeVector initial_position;
  int  index, IDP;
  NTuple::Item < long >   m_ntrk ;
  NTuple::Array< float >  m_Xpos, m_Ypos, m_Zpos;
  NTuple::Array< float >  m_cumradlgh, m_p2pradlgh, m_cuminterlgh, m_p2pinterlgh;
  NTuple::Array< float >  m_eta, m_phi;
  NTuple::Array< long >   m_planeID;
  NTuple::Item< float >  m_origx;
  NTuple::Item< float >  m_origy;
  NTuple::Item< float >  m_origz;
  //std::vector< float >   mp_origx;
  //std::vector< float >   mp_origy;
  //std::vector< float >   mp_origz;
  float    mp_origx;
  float    mp_origy;
  float    mp_origz;

  NTuple::Tuple* m_matScan_PlaneDatas ;

  StatusCode status, writestatus;
 
};
#endif // RADLENGTHCOLL_RADLENGTHCOLL_H
