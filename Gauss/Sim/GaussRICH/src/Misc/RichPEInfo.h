#ifndef      RICHPEINFO_h 
#define      RICHPEINFO_h 1 

#include "globals.hh"
#include "G4ThreeVector.hh"

class RichPEInfo {

 public:
  RichPEInfo ();
  virtual ~RichPEInfo ();
  G4int PhotOriginRadiatorNumber() {return m_PhotOriginRadiatorNumber; }
  void setPhotOriginRadiatorNumber(G4int aRnum)
    { m_PhotOriginRadiatorNumber= aRnum ; }

  G4int MotherOfPhotonPDGcode() {return m_MotherOfPhotonPDGcode ; }
  void setMotherOfPhotonPDGcode(G4int cPDGcode) 
                               {m_MotherOfPhotonPDGcode=cPDGcode;}

  G4ThreeVector PhotonEmisPoint() {return m_PhotonEmisPoint ; }
  void setPhotonEmisPoint(G4ThreeVector  anEmisspt) 
    { m_PhotonEmisPoint = anEmisspt ; }


  G4int MotherOfPhotonId() {return m_MotherOfPhotonId; }
  void setMotherOfPhotonId(G4int mId) {m_MotherOfPhotonId=mId; }

  G4double PhotonEnergyAtCkvProd() {return m_PhotonEnergyAtCkvProd; }
  void setPhotonEnergyAtCkvProd( G4double aPhEnergy) 
                  {m_PhotonEnergyAtCkvProd=aPhEnergy; }
  G4double CherenkovThetaAtProd() {return m_CherenkovThetaAtProd; }
  void setCherenkovThetaAtProd(G4double aCkvTheta) 
                 {m_CherenkovThetaAtProd=aCkvTheta;}

  G4double CherenkovPhiAtProd() {return m_CherenkovPhiAtProd; }
  void setCherenkovPhiAtProd(G4double aCkvPhi)
                 { m_CherenkovPhiAtProd= aCkvPhi; }

  G4double MotherofPhotonMomAtProd() {return m_MotherofPhotonMomAtProd; }
  void setMotherofPhotonMomAtProd(G4double aChMom)
                 {  m_MotherofPhotonMomAtProd=aChMom; }

  G4ThreeVector MotherofPhotonMomVectAtProd() 
                {return m_MotherofPhotonMomVectAtProd; }
  void setMotherofPhotonMomVectAtProd( G4ThreeVector  aChMomVect )
                { m_MotherofPhotonMomVectAtProd=aChMomVect;}
  G4double MotherofPhotonPDGMass()
  {
    return m_MotherofPhotonPDGMass;
  }
  void setMotherofPhotonPDGMass(G4double aMotherPDGMass )
  {
    m_MotherofPhotonPDGMass = aMotherPDGMass;
  }

  G4ThreeVector MotherofPhotonCkvPreStep() 
  {
    return m_MotherofPhotonCkvPreStep;
    
  }
  void setMotherofPhotonCkvPreStep( G4ThreeVector  aMotherofPhotonCkvPreStep )
  {
    m_MotherofPhotonCkvPreStep= aMotherofPhotonCkvPreStep;
    
  }
  

   G4ThreeVector MotherofPhotonCkvPostStep() 
  {
    return m_MotherofPhotonCkvPostStep;
    
  }
  void setMotherofPhotonCkvPostStep(G4ThreeVector  aMotherofPhotonCkvPostStep )
  {
    m_MotherofPhotonCkvPostStep= aMotherofPhotonCkvPostStep;
    
  }
  
  G4int PhotonRayleighScatteringFlag() 
  {
    return m_PhotonRayleighScatteringFlag;
  }
  void setPhotonRayleighScatteringFlag( G4int aPhotonRayleighScatteringFlag) 
  {
    m_PhotonRayleighScatteringFlag= aPhotonRayleighScatteringFlag;
    
  }
  G4ThreeVector PhotonAerogelExitPos() 
  {
    return   m_PhotonAerogelExitPos;
  }

  void setPhotonAerogelExitPos(G4ThreeVector  aPhotonAerogelExitPos ) 
  {
    m_PhotonAerogelExitPos = aPhotonAerogelExitPos;
    
  }
  G4bool VerbosePeTagFlag()  {return m_VerbosePeTagFlag;}
  void setVerbosePeTagFlag(G4bool aPeVFlag) {m_VerbosePeTagFlag= aPeVFlag;}
  
  G4int OptPhotonId() 
  {    return m_OptPhotonId;}
  void setOptPhotonId( G4int aIdValue){m_OptPhotonId=aIdValue;}
  
    
  
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
  
};

#endif


