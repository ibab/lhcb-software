#ifndef      RICHPHOTINFO_h 
#define      RICHPHOTINFO_h 1 
// Information carried by the Photons made in RICH
// that are carried along using the G4TrackInformation.
#include "globals.hh"
#include "G4ThreeVector.hh"
class RichPhotInfo {

 public:
  RichPhotInfo ();
  virtual ~RichPhotInfo ();
  G4int MotherChTrackPDGcode() {return m_MotherChTrackPDGcode ; }
  void setMotherChTrackPDGcode(G4int aPDGcode) 
                               {m_MotherChTrackPDGcode=aPDGcode;}
  G4double CkvPhotonEnergyAtProd() {return m_CkvPhotonEnergyAtProd; }
  void setCkvPhotonEnergyAtProd(G4double aPhotonEnergy) 
                              {m_CkvPhotonEnergyAtProd=aPhotonEnergy; }

  G4double CkvAngleThetaAtProd() {return m_CkvAngleThetaAtProd; }
  void setCkvAngleThetaAtProd(G4double aPhTheta )
                                {m_CkvAngleThetaAtProd=aPhTheta ; }
  G4double CkvAnglePhiAtProd() {return m_CkvAnglePhiAtProd; }
  void setCkvAnglePhiAtProd(G4double aPhPhi )
                                {m_CkvAnglePhiAtProd=aPhPhi; }
  G4double ChTrackMomAtProd() {return m_ChTrackMomAtProd; }
  void setChTrackMomAtProd( G4double aChMom ) 
                            { m_ChTrackMomAtProd=aChMom; }
  G4ThreeVector ChTrackMomentumVector() {return  m_ChTrackMomentumVector; }
  void setChTrackMomentumVector(G4ThreeVector aMomemtumVector) 
                           {m_ChTrackMomentumVector=aMomemtumVector;}

 private:

  G4int m_MotherChTrackPDGcode;   
  G4double m_CkvPhotonEnergyAtProd;
  G4double m_CkvAngleThetaAtProd;
  G4double m_CkvAnglePhiAtProd;
  G4double m_ChTrackMomAtProd;
  G4ThreeVector m_ChTrackMomentumVector;

};

#endif
