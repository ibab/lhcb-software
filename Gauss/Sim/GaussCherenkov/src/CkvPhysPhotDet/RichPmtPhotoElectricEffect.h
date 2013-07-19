#ifndef RichPmtPhotoElectricEffect_h
#define RichPmtPhotoElectricEffect_h 1
#include <vector>
#include "globals.hh"
#include "Randomize.hh"
#include "G4ios.hh"
#include "G4VDiscreteProcess.hh"
#include "G4DynamicParticle.hh"
#include "G4OpticalPhoton.hh"
#include "G4Electron.hh"
#include "G4Step.hh"

#include "GiGa/GiGaBase.h"

#include "GaussCherenkov/RichPmtProperties.h"

//
//  The class definition should access the class for
//  conditions db in the future to get the PMT QE. For now,
//  hardcoded in the xml tabulated properties for 1 hpd. SE 24-2-2002.
//
class RichPmtPhotoElectricEffect : public G4VDiscreteProcess {

public:

    RichPmtPhotoElectricEffect(const GiGaBase*, const G4String&, 
                          G4ProcessType   aType = fOptical);
  // RichPmtPhotoElectricEffect(const G4String&,G4ProcessType   aType = fOptical);

  virtual ~RichPmtPhotoElectricEffect();

  G4bool IsApplicable(const G4ParticleDefinition&  );
  // is applicable for optical photon only

  G4double GetMeanFreePath(const G4Track& ,
                           G4double ,
                           G4ForceCondition* condition);
  // returns infinity (max integer possible) . This means the process does
  // not limit the step, but sets the Forced condition for the DoIt to be
  // invoked at every step. But only at the boundary between Pmt quartz
  // window and the Pmt photocathode any action be taken.
  // this may have to be improved in the future for optimization of
  // cpu time.

  G4VParticleChange* PostStepDoIt(const G4Track& aTrack,
                                  const G4Step&  aStep);
  //The method for implementing the PmtPhotoelectric effect.



  double getCurrentPmtQE(int ipmtnum, int richdetnum,
                         double photonenergy);
  // To interpolate the QE from the QE data for a specific pmt.



  G4double PmtPhElectronKE()   {return m_PmtPhElectronKE; }
  //To get the PhotoElectron energy at emission, which is from the
  // HV applied to the PMT tube.

  void setPmtPhElectronKE(double PmtPhElecKE );

  double PhCathodeToSilDetMaxDist()
  {return m_PhCathodeToSilDetMaxDist; }

  void setPhCathodeToSilDetMaxDist(double PhCathodeToSilZ );


  double getCurrentPmtPSFSigma(int pmtnumb, int richdetnumb);
  // get the Point spread function for the current pmt.
  // in the future this may be a function of the radius from the
  // axis of pmt.

  std::vector<double> getCurrentPmtPSFXY ( int pmtnumb, int richdetnumb);
  //  std::vector<double> getCurrentPmtPSFFullParamXY ( int pmtnumb, int richdetnumb, double photRadPos, double PhotEn );
  std::vector<double> getCurrentPmtPSFFullParamXY ( int pmtnumb, int richdetnumb );
  std::vector<double> GetPSFXYForASigma(double aPSFSigma) ;

  std::vector<double> getCurrentPmtDemag(const int pmtnumber,
                                         const int richdetnumber);

  

  G4String PrePhotoElectricLogVolName() const
  {return m_PrePhotoElectricLogVolName; }

  void setPrePhotoElectricLogVolName( const G4String PrePhotoElecLogVolName);

  G4String PostPhotoElectricLogVolName() const
  {return m_PostPhotoElectricLogVolName; }

  void setPostPhotoElectricLogVolName( const G4String PostPhotoElecLogVolName);

  void setPrePhotoElectricMatNameSec( const G4String aPrePhotoElectricMatNameSec ) 
  {    m_PrePhotoElectricMatNameSec=aPrePhotoElectricMatNameSec;}
  G4String PrePhotoElectricMatNameSec()  {  return  m_PrePhotoElectricMatNameSec;}
  

  //  RichPmtProperties* PmtProperty() {return  m_PmtProperty; }

  RichPmtProperties* PmtProperty() {return  RichPmtProperties::getRichPmtPropertiesInstance(); }

  int NumRichDet() {return m_NumRichDet; }
  void setNumRichDet(int numrichDetect) ;

  const std::vector<int> & numTotPmt() const {return  m_numTotPmt; }
  int numTotPmtInCurrentRichDet(int richdetnum);
  G4String Rich1PhysVolNameA() const {return  m_Rich1PhysVolNameA; }
  G4String Rich2PhysVolNameA() const {return  m_Rich2PhysVolNameA; }
  G4String Rich1PhysVolNameB() const {return  m_Rich1PhysVolNameB; }
  G4String Rich2PhysVolNameB() const {return  m_Rich2PhysVolNameB; }

  double MaxZHitInRich1() const {return m_MaxZHitInRich1; }
  void setMaxZHitInRich1 (double MaxZHitWithinRich1);

  double MaxAnyPmtQEff() const
  {
    return m_MaxAnyPmtQEff;
  }

  
  G4bool UsePmtMagDistortions() {
    return m_UsePmtMagDistortions;
  } 
  
  void setUsePmtMagDistortions(G4bool aflg ) { m_UsePmtMagDistortions= aflg;}
  void setPmtPhElecParam();
  void setPSFPreDc06Flag(G4bool aFlagPsf ) { m_PSFPreDc06Flag = aFlagPsf; }
  void setPmtQEUsingNominalTable(G4bool apmtqetableoption) 
  {m_PmtQEUsingNominalTable=apmtqetableoption;}  
  G4bool PSFPreDc06Flag() {return m_PSFPreDc06Flag;}
  G4bool PmtQEUsingNominalTable()  {  return m_PmtQEUsingNominalTable;}
  void  SetPmtQESourceTable(int asou);
  void setPmtModuleSupFlag3(G4bool aFlag3) 
  { m_PmtModuleSupFlag3=aFlag3;}
  void setPmtModuleSupFlag4(G4bool aFlag4)
  { m_PmtModuleSupFlag4=aFlag4;}
  void setPmtModuleSupFlag5(G4bool aFlag5) 
  { m_PmtModuleSupFlag5=aFlag5;}
  void setPmtModuleSupFlag6(G4bool aFlag6)
  { m_PmtModuleSupFlag6=aFlag6;}
  
private:

  //  G4ThreeVector getCathodeToAnodeDirection(int ipmtnum , int richdetnum , 
  //                                        G4ThreeVector aLocalElnOrigin ,
  //                                         std::vector<double>apsfVectInXY  );

  G4ThreeVector getCathodeToAnodeDirectionDefault();


  G4String m_PrePhotoElectricLogVolName;
  G4String m_PostPhotoElectricLogVolName;
  G4String m_PrePhotoElectricMatNameSec;

  G4String m_PrePhotoElectricLogVolNameWLens;
  G4String m_PostPhotoElectricLogVolNameWLens;
  
  double m_PmtPhElectronKE;
  double m_PhCathodeToSilDetMaxDist;

  std::vector<int>m_numTotPmt;
  int m_NumRichDet;
  G4String  m_Rich1PhysVolNameA;
  G4String  m_Rich2PhysVolNameA;
  G4String  m_Rich1PhysVolNameB;
  G4String  m_Rich2PhysVolNameB;
  G4String  m_SuperRichPhysVolNameA;
  G4String  m_SuperRichPhysVolNameB;
  

  double m_MaxZHitInRich1;
  double  m_MaxAnyPmtQEff;

  G4bool m_UsePmtMagDistortions;

  G4bool m_PSFPreDc06Flag;  // if true the old psf implementation
                            // if false the new psf implementation which
                            // has a worse sigma than the old version.
  G4bool m_PmtQEUsingNominalTable; // if true using old Nominal QE Table for all pmts from sqldddb
                                   // if false using the new measured QE tables for the Pmts. 
  G4int m_PmtQESourceTable;

  G4bool m_PmtModuleSupFlag3;
  G4bool m_PmtModuleSupFlag4;
  G4bool m_PmtModuleSupFlag5;
  G4bool m_PmtModuleSupFlag6;

         
};

inline G4bool
RichPmtPhotoElectricEffect::IsApplicable(const G4ParticleDefinition& aParticleType)
{
  return ( &aParticleType == G4OpticalPhoton::OpticalPhoton() );
}

inline G4double
RichPmtPhotoElectricEffect::GetMeanFreePath(const G4Track& ,
                                            G4double ,
                                            G4ForceCondition* condition)
{
  *condition = Forced;
  return DBL_MAX;
}

inline void RichPmtPhotoElectricEffect::setPmtPhElectronKE(double PmtPhElecKE) {
  m_PmtPhElectronKE=PmtPhElecKE;
}

inline void RichPmtPhotoElectricEffect::setPhCathodeToSilDetMaxDist
(double PhCathodeToSilZ){
  m_PhCathodeToSilDetMaxDist= PhCathodeToSilZ;
}

inline double
RichPmtPhotoElectricEffect::getCurrentPmtPSFSigma(int pmtnumb,
                                                  int richdetnumb)
{
  return PmtProperty()->getRichPmtPSF(pmtnumb,richdetnumb)
    ->pmtPointSpreadFunction();
}




inline void
RichPmtPhotoElectricEffect::setPrePhotoElectricLogVolName( const G4String PrePhotoElecLogVolName)
{
  m_PrePhotoElectricLogVolName= PrePhotoElecLogVolName;
}

inline void
RichPmtPhotoElectricEffect::setPostPhotoElectricLogVolName(const G4String PostPhotoElecLogVolName)
{
  m_PostPhotoElectricLogVolName=PostPhotoElecLogVolName;
}

inline void  RichPmtPhotoElectricEffect::setNumRichDet(int numrichDetect )
{
  m_NumRichDet= numrichDetect;
}

inline int RichPmtPhotoElectricEffect::numTotPmtInCurrentRichDet(int richdetnum) {
  int curPmtTot=0;
  if(richdetnum >=0 &&   richdetnum < (int) m_numTotPmt.size()  ) {
    curPmtTot = m_numTotPmt[richdetnum];
  }
  return  curPmtTot;
}

inline void RichPmtPhotoElectricEffect::setMaxZHitInRich1(double MaxZHitWithinRich1 ){
  m_MaxZHitInRich1 =  MaxZHitWithinRich1;

}
inline void RichPmtPhotoElectricEffect::SetPmtQESourceTable(int asou) 
{  
  m_PmtQESourceTable=asou;
}


#endif
