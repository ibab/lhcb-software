#ifndef      GaussRICH_RICHPEINFO_h 
#define      GaussRICH_RICHPEINFO_h 1

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

  G4int  QW2PCreflFlagSave() {return m_QW2PCreflFlagSave;}
  void setQW2PCreflFlagSave(G4int afl) 
  { m_QW2PCreflFlagSave=afl;}
  
  G4int  BackscatteredPeFlag()  {  return m_BackscatteredPeFlag;}
  void setBackscatteredPeFlag(G4int aFlag)
  {m_BackscatteredPeFlag=aFlag;}
  void bumpBackscatteredFlag()
  {m_BackscatteredPeFlag++;}
  
    
  G4int PhotoElectricFlag() 
  {  return m_PhotoElectricFlag;}
  void setPhotoElectricFlag(G4int apflag) 
  {  m_PhotoElectricFlag=apflag;}
  
  G4int HpdPhotonReflectionFlag()
  {  return m_HpdPhotonReflectionFlag;}
  void setHpdPhotonReflectionFlag( G4int aFlagA)
  {m_HpdPhotonReflectionFlag=aFlagA;}
  
  G4ThreeVector HpdQWExtPhotIncidentPosition(){return  m_HpdQWExtPhotIncidentPosition;}
  void setHpdQWExtPhotIncidentPosition( const  G4ThreeVector & aHpdQWExtPhotIncidentPosition){
    m_HpdQWExtPhotIncidentPosition=  aHpdQWExtPhotIncidentPosition;    
  }  


  G4ThreeVector PmtLensPhotIncidentPosition () {return m_PmtLensPhotIncidentPosition;}
  void setPmtLensPhotIncidentPosition (const G4ThreeVector & aPmtLensPhotIncidentPosition ) 
  { m_PmtLensPhotIncidentPosition = aPmtLensPhotIncidentPosition;}
  
  

  G4ThreeVector HpdPeLocalOriginPosition()
  { return m_HpdPeLocalOriginPosition;}
  void setHpdPeLocalOriginPosition(const  G4ThreeVector & aHpdPeOriginLocalLocation ) 
  {  m_HpdPeLocalOriginPosition=aHpdPeOriginLocalLocation;}
  G4int PhotonSourceInformation()  {  return m_PhotonSourceInformation;}
  void setPhotonSourceInformation(G4int aSourceInformation) 
  { m_PhotonSourceInformation=aSourceInformation;  }
  
  
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
  G4int  m_QW2PCreflFlagSave;
  G4int  m_BackscatteredPeFlag; // 0 means track is not backscattered
                                // 1or more means this track is backscattered.
                                // the number indicates the number times it backscattered.
  G4int m_PhotoElectricFlag;    // 0 means track not created by Photoelectric process 
                                // 1 means track created in Photoelectric process.
                                // 2 means this track is not created by photoelectric process, but
                                // its parent track (or some grand parent track) is created by photoelectric process.

  G4int m_HpdPhotonReflectionFlag;
  
  G4ThreeVector m_HpdQWExtPhotIncidentPosition;
  G4ThreeVector m_PmtLensPhotIncidentPosition;

  G4ThreeVector m_HpdPeLocalOriginPosition;
  G4int m_PhotonSourceInformation; // 0 mean unknown, 1 mean cherenkov process , 2 mean scintillation process.
  

};

#endif //GaussRICH_RichPEInfo


