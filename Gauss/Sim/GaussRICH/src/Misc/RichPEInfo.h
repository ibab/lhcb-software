#ifndef      RICHPEINFO_h 
#define      RICHPEINFO_h 1

#include "globals.hh"
#include "G4ThreeVector.hh"

class RichPEInfo {

public:

  RichPEInfo ();

  virtual ~RichPEInfo ();

  G4int PhotOriginRadiatorNumber() const {return m_PhotOriginRadiatorNumber; }
  void setPhotOriginRadiatorNumber(const G4int aRnum)
  { m_PhotOriginRadiatorNumber= aRnum ; }

  G4int MotherOfPhotonPDGcode() const {return m_MotherOfPhotonPDGcode ; }
  void setMotherOfPhotonPDGcode(const G4int cPDGcode)
  {m_MotherOfPhotonPDGcode=cPDGcode;}

  G4ThreeVector PhotonEmisPoint() const {return m_PhotonEmisPoint ; }
  void setPhotonEmisPoint(const G4ThreeVector & anEmisspt)
  { m_PhotonEmisPoint = anEmisspt ; }

  G4int MotherOfPhotonId() const {return m_MotherOfPhotonId; }
  void setMotherOfPhotonId(const G4int mId) {m_MotherOfPhotonId=mId; }

  G4double PhotonEnergyAtCkvProd() const {return m_PhotonEnergyAtCkvProd; }
  void setPhotonEnergyAtCkvProd( const G4double aPhEnergy)
  {m_PhotonEnergyAtCkvProd=aPhEnergy; }

  G4double CherenkovThetaAtProd() const {return m_CherenkovThetaAtProd; }
  void setCherenkovThetaAtProd(const G4double aCkvTheta)
  {m_CherenkovThetaAtProd=aCkvTheta;}

  G4double CherenkovPhiAtProd() const {return m_CherenkovPhiAtProd; }
  void setCherenkovPhiAtProd(const G4double aCkvPhi)
  { m_CherenkovPhiAtProd= aCkvPhi; }

  G4double MotherofPhotonMomAtProd() const {return m_MotherofPhotonMomAtProd; }
  void setMotherofPhotonMomAtProd(const G4double aChMom)
  {  m_MotherofPhotonMomAtProd=aChMom; }

  const G4ThreeVector & MotherofPhotonMomVectAtProd() const
  {return m_MotherofPhotonMomVectAtProd; }
  void setMotherofPhotonMomVectAtProd( const G4ThreeVector & aChMomVect )
  { m_MotherofPhotonMomVectAtProd=aChMomVect;}

  G4double MotherofPhotonPDGMass() const
  {
    return m_MotherofPhotonPDGMass;
  }
  void setMotherofPhotonPDGMass(const G4double aMotherPDGMass )
  {
    m_MotherofPhotonPDGMass = aMotherPDGMass;
  }

  const G4ThreeVector & MotherofPhotonCkvPreStep() const
  {
    return m_MotherofPhotonCkvPreStep;
  }
  void setMotherofPhotonCkvPreStep( const G4ThreeVector & aMotherofPhotonCkvPreStep )
  {
    m_MotherofPhotonCkvPreStep= aMotherofPhotonCkvPreStep;
  }

  const G4ThreeVector & MotherofPhotonCkvPostStep() const
  {
    return m_MotherofPhotonCkvPostStep;
  }
  void setMotherofPhotonCkvPostStep(const G4ThreeVector & aMotherofPhotonCkvPostStep )
  {
    m_MotherofPhotonCkvPostStep= aMotherofPhotonCkvPostStep;
  }

  G4int PhotonRayleighScatteringFlag() const
  {
    return m_PhotonRayleighScatteringFlag;
  }
  void setPhotonRayleighScatteringFlag( const G4int aPhotonRayleighScatteringFlag)
  {
    m_PhotonRayleighScatteringFlag= aPhotonRayleighScatteringFlag;
  }

  const G4ThreeVector & PhotonAerogelExitPos() const
  {
    return   m_PhotonAerogelExitPos;
  }

  void setPhotonAerogelExitPos(const G4ThreeVector &  aPhotonAerogelExitPos )
  {
    m_PhotonAerogelExitPos = aPhotonAerogelExitPos;
  }

  G4bool VerbosePeTagFlag()  const {return m_VerbosePeTagFlag;}
  void setVerbosePeTagFlag(const G4bool aPeVFlag) {m_VerbosePeTagFlag= aPeVFlag;}

  G4int OptPhotonId() const
  {    return m_OptPhotonId; }
  void setOptPhotonId( const G4int aIdValue){m_OptPhotonId=aIdValue;}

  void setMirror1PhotReflPosition( const  G4ThreeVector & aMirror1PhotReflPosition) 
    { m_Mirror1PhotReflPosition= aMirror1PhotReflPosition;}
  G4ThreeVector Mirror1PhotReflPosition() {return m_Mirror1PhotReflPosition;}
  void setMirror2PhotReflPosition( const  G4ThreeVector & aMirror2PhotReflPosition) 
    { m_Mirror2PhotReflPosition= aMirror2PhotReflPosition;}
  G4ThreeVector Mirror2PhotReflPosition() {return m_Mirror2PhotReflPosition;}
  void setMirror1PhotDetCopyNum(const G4int aMirror1PhotDetCopyNum) {m_Mirror1PhotDetCopyNum=aMirror1PhotDetCopyNum;}
  G4int Mirror1PhotDetCopyNum() {return  m_Mirror1PhotDetCopyNum;}
  void setMirror2PhotDetCopyNum(const G4int aMirror2PhotDetCopyNum) {m_Mirror2PhotDetCopyNum=aMirror2PhotDetCopyNum;}
  G4int Mirror2PhotDetCopyNum() {return  m_Mirror2PhotDetCopyNum;}


private:

  G4int m_PhotOriginRadiatorNumber;
  G4int m_MotherOfPhotonPDGcode;
  G4ThreeVector m_PhotonEmisPoint;
  G4int m_MotherOfPhotonId;
  G4double m_PhotonEnergyAtCkvProd;
  G4double m_CherenkovThetaAtProd;
  G4double m_CherenkovPhiAtProd;
  G4double m_MotherofPhotonMomAtProd;
  G4ThreeVector m_MotherofPhotonMomVectAtProd;
  G4double m_MotherofPhotonPDGMass;
  G4ThreeVector m_MotherofPhotonCkvPreStep;
  G4ThreeVector m_MotherofPhotonCkvPostStep;
  G4int m_PhotonRayleighScatteringFlag;
  G4ThreeVector m_PhotonAerogelExitPos;
  G4bool m_VerbosePeTagFlag;
  G4int m_OptPhotonId;
  G4ThreeVector m_Mirror1PhotReflPosition;
  G4ThreeVector m_Mirror2PhotReflPosition;
  G4int  m_Mirror1PhotDetCopyNum;
  G4int  m_Mirror2PhotDetCopyNum;
};

#endif


