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
  void setChTrackMomentumVector(G4ThreeVector  aMomemtumVector) 
                           {m_ChTrackMomentumVector=aMomemtumVector;}

  G4double MotherChTrackPDGMass() 
  {
    return m_MotherChTrackPDGMass;
  }
  void setMotherChTrackPDGMass( G4double aTrackPDGMass) 
  {
    m_MotherChTrackPDGMass= aTrackPDGMass;
  }

 
  G4ThreeVector ChTrackCkvPrestep() 
  {
    return m_ChTrackCkvPrestep;
    
  }
  void setChTrackCkvPrestep( G4ThreeVector  aChTrackCkvPrestep) 
  {
    m_ChTrackCkvPrestep=  aChTrackCkvPrestep;
    
  }
  
  G4ThreeVector ChTrackCkvPoststep()
  {
    return m_ChTrackCkvPoststep;
    
  }
  void setChTrackCkvPoststep( G4ThreeVector  aChTrackCkvPoststep) 
  {
    m_ChTrackCkvPoststep = aChTrackCkvPoststep;
    
  }
  
  G4int PhotonRayleighScatFlag()
  {
    return m_PhotonRayleighScatFlag;
    
  }

  void setPhotonRayleighScatFlag( G4int aPhotonRayleighScatFlag) 
  {
    m_PhotonRayleighScatFlag = aPhotonRayleighScatFlag;
    
  }
  void bumpPhotonRayleighScatFlag() 
  {

    m_PhotonRayleighScatFlag++;
    
    
  }

  void bumpPhotonRayleighScatFlagByAmount(G4int  aFlagBumpAmount ) 
  {

    m_PhotonRayleighScatFlag += aFlagBumpAmount;
    
  }
   
  G4ThreeVector  AerogelExitPosition() 
  {
    return  m_AerogelExitPosition;
    
  }

  void setAerogelExitPosition( G4ThreeVector  aAgelExitPos) 
  {
    m_AerogelExitPosition = aAgelExitPos;
    
  }
  G4bool  VerbosePhotTagFlag() 
  {    return  m_VerbosePhotTagFlag;}
  
  void setVerbosePhotTagFlag(G4bool aVTFlag)
  { m_VerbosePhotTagFlag=aVTFlag;}
  
    
 private:

  G4int m_MotherChTrackPDGcode;   
  G4double m_CkvPhotonEnergyAtProd;
  G4double m_CkvAngleThetaAtProd;
  G4double m_CkvAnglePhiAtProd;
  G4double m_ChTrackMomAtProd;
  G4ThreeVector m_ChTrackMomentumVector;
  G4double m_MotherChTrackPDGMass;
  G4ThreeVector m_ChTrackCkvPrestep;
  G4ThreeVector m_ChTrackCkvPoststep;
  G4int  m_PhotonRayleighScatFlag;
  G4ThreeVector m_AerogelExitPosition;  // filled only from user step action.
  G4bool m_VerbosePhotTagFlag;
  

};

#endif
