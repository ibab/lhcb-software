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
  void setPhotonEmisPoint(G4ThreeVector anEmisspt) 
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
  void setMotherofPhotonMomVectAtProd( G4ThreeVector aChMomVect )
                { m_MotherofPhotonMomVectAtProd=aChMomVect;}
  G4double MotherofPhotonPDGMass()
  {
    return m_MotherofPhotonPDGMass;
  }
  void setMotherofPhotonPDGMass(G4double aMotherPDGMass )
  {
    m_MotherofPhotonPDGMass = aMotherPDGMass;
  }

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
 
  
};

#endif


