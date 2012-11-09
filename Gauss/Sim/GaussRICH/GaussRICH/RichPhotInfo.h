#ifndef      GaussRICH_RICHPHOTINFO_h 
#define      GaussRICH_RICHPHOTINFO_h 1

// Information carried by the Photons made in RICH
// that are carried along using the G4TrackInformation.

#include "globals.hh"
#include "G4ThreeVector.hh"

class RichPhotInfo {

public:

  RichPhotInfo ();
  virtual ~RichPhotInfo ();

  G4int MotherChTrackPDGcode() const {return m_MotherChTrackPDGcode ; }
  void setMotherChTrackPDGcode(const G4int aPDGcode)
  {m_MotherChTrackPDGcode=aPDGcode;}

  G4double CkvPhotonEnergyAtProd() const {return m_CkvPhotonEnergyAtProd; }
  void setCkvPhotonEnergyAtProd(const G4double aPhotonEnergy)
  {m_CkvPhotonEnergyAtProd=aPhotonEnergy; }

  G4double CkvAngleThetaAtProd() const {return m_CkvAngleThetaAtProd; }
  void setCkvAngleThetaAtProd(const G4double aPhTheta )
  {m_CkvAngleThetaAtProd=aPhTheta ; }

  G4double CkvAnglePhiAtProd() const {return m_CkvAnglePhiAtProd; }
  void setCkvAnglePhiAtProd(const G4double aPhPhi )
  {m_CkvAnglePhiAtProd=aPhPhi; }

  G4double ChTrackMomAtProd() const {return m_ChTrackMomAtProd; }
  void setChTrackMomAtProd( const G4double aChMom )
  { m_ChTrackMomAtProd=aChMom; }

  const G4ThreeVector & ChTrackMomentumVector() const {return  m_ChTrackMomentumVector; }
  void setChTrackMomentumVector(const G4ThreeVector & aMomemtumVector)
  {m_ChTrackMomentumVector=aMomemtumVector;}

  G4double MotherChTrackPDGMass() const
  {
    return m_MotherChTrackPDGMass;
  }
  void setMotherChTrackPDGMass( const G4double aTrackPDGMass)
  {
    m_MotherChTrackPDGMass= aTrackPDGMass;
  }

  const G4ThreeVector & ChTrackCkvPrestep() const
  {
    return m_ChTrackCkvPrestep;
  }
  void setChTrackCkvPrestep( const G4ThreeVector & aChTrackCkvPrestep)
  {
    m_ChTrackCkvPrestep=  aChTrackCkvPrestep;
  }

  const G4ThreeVector & ChTrackCkvPoststep() const
  {
    return m_ChTrackCkvPoststep;
  }
  void setChTrackCkvPoststep( const G4ThreeVector & aChTrackCkvPoststep)
  {
    m_ChTrackCkvPoststep = aChTrackCkvPoststep;

  }

  G4int PhotonRayleighScatFlag() const
  {
    return m_PhotonRayleighScatFlag;
  }
  void setPhotonRayleighScatFlag( const G4int aPhotonRayleighScatFlag)
  {
    m_PhotonRayleighScatFlag = aPhotonRayleighScatFlag;

  }
  void bumpPhotonRayleighScatFlag()
  {
    m_PhotonRayleighScatFlag++;
  }

  void bumpPhotonRayleighScatFlagByAmount(const G4int  aFlagBumpAmount )
  {
    m_PhotonRayleighScatFlag += aFlagBumpAmount;
  }

  G4int QW2PCreflFlag() const
  {
    return m_QW2PCreflFlag;
  }
  void setQW2PCreflFlag(const G4int aFlag)
  {
    m_QW2PCreflFlag = aFlag;
  }
 G4int QW2PCreflFlagSave() const
  {
    return m_QW2PCreflFlagSave;
  }
  void setQW2PCreflFlagSave(const G4int aFlag)
  {
    m_QW2PCreflFlagSave = aFlag;
  }


  const G4ThreeVector & AerogelExitPosition() const
  {
    return m_AerogelExitPosition;
  }

  void setAerogelExitPosition( const G4ThreeVector & aAgelExitPos)
  {
    m_AerogelExitPosition = aAgelExitPos;
  }

 
  void setMirror1ReflPosition( const  G4ThreeVector & aMirror1ReflPosition) 
    { m_Mirror1ReflPosition= aMirror1ReflPosition;}
  G4ThreeVector Mirror1ReflPosition() {return m_Mirror1ReflPosition;}
  void setMirror2ReflPosition( const  G4ThreeVector & aMirror2ReflPosition) 
    { m_Mirror2ReflPosition= aMirror2ReflPosition;}
  G4ThreeVector Mirror2ReflPosition() {return m_Mirror2ReflPosition;}
  void setMirror1DetCopyNum(const G4int aMirror1DetCopyNum) {m_Mirror1DetCopyNum=aMirror1DetCopyNum;}
  G4int Mirror1DetCopyNum() {return  m_Mirror1DetCopyNum;}
  void setMirror2DetCopyNum(const G4int aMirror2DetCopyNum) {m_Mirror2DetCopyNum=aMirror2DetCopyNum;}
  G4int Mirror2DetCopyNum() {return  m_Mirror2DetCopyNum;}


  G4bool  VerbosePhotTagFlag()const
  {    return  m_VerbosePhotTagFlag;}

  void setVerbosePhotTagFlag(const G4bool aVTFlag)
  { m_VerbosePhotTagFlag=aVTFlag;}

  
  G4int PhotProdRadiatorNum() {return m_PhotProdRadiatorNum;}
  void setPhotProdRadiatorNum(G4int aRadiatorNum ){m_PhotProdRadiatorNum=aRadiatorNum;}
  G4int PhotonHpdReflectionFlag(){ return m_PhotonHpdReflectionFlag;}
  void setPhotonHpdReflectionFlag(G4int aFlagA){ m_PhotonHpdReflectionFlag=aFlagA;}
  
    
  G4ThreeVector HpdQWPhotIncidentPosition() {return m_HpdQWPhotIncidentPosition;}
  void setHpdQWPhotIncidentPosition(const  G4ThreeVector & aHpdQWPhotIncidentPosition) {
    m_HpdQWPhotIncidentPosition=aHpdQWPhotIncidentPosition;
  }
  
  G4int PhotonSourceInfo()  {  return m_PhotonSourceInfo;}
  void setPhotonSourceInfo(G4int aSourceInfo) 
  { m_PhotonSourceInfo=aSourceInfo;  }
  

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
  G4ThreeVector m_Mirror1ReflPosition;
  G4ThreeVector m_Mirror2ReflPosition;
  G4int m_Mirror1DetCopyNum;
  G4int m_Mirror2DetCopyNum;
  G4bool m_VerbosePhotTagFlag;
  G4int m_PhotProdRadiatorNum;
  G4int m_QW2PCreflFlag; //added by RWL 9.11.06 force reflections indep of QE
  G4int m_QW2PCreflFlagSave; //added by SE 26.10.07 to store the info on photon reflection
                             // at the hpd QW-photocathode interface. 0 means no reflection
                             // and 1 means with reflection.
  
  G4int m_PhotonHpdReflectionFlag; // bit packed flag to indicate if a photon is reflected in
                                   // some part of the hpd

  G4ThreeVector m_HpdQWPhotIncidentPosition;
  G4int m_PhotonSourceInfo; // 0 means unknown,  1 means cherenkov process , 2 means scintillation process
  
};

#endif //GaussRICH_RichPhotInfo
