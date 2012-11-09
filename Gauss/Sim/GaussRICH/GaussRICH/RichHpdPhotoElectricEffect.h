#ifndef RichHpdPhotoElectricEffect_h
#define RichHpdPhotoElectricEffect_h 1
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

#include "GaussRICH/RichHpdProperties.h"

//
//  The class definition should access the class for
//  conditions db in the future to get the HPD QE. For now,
//  hardcoded in the xml tabulated properties for 1 hpd. SE 24-2-2002.
//
class RichHpdPhotoElectricEffect : public G4VDiscreteProcess {

public:

    RichHpdPhotoElectricEffect(const GiGaBase*, const G4String&, 
                          G4ProcessType   aType = fOptical);
  // RichHpdPhotoElectricEffect(const G4String&,G4ProcessType   aType = fOptical);

  virtual ~RichHpdPhotoElectricEffect();

  G4bool IsApplicable(const G4ParticleDefinition&  );
  // is applicable for optical photon only

  G4double GetMeanFreePath(const G4Track& ,
                           G4double ,
                           G4ForceCondition* condition);
  // returns infinity (max integer possible) . This means the process does
  // not limit the step, but sets the Forced condition for the DoIt to be
  // invoked at every step. But only at the boundary between Hpd quartz
  // window and the Hpd photocathode any action be taken.
  // this may have to be improved in the future for optimization of
  // cpu time.

  G4VParticleChange* PostStepDoIt(const G4Track& aTrack,
                                  const G4Step&  aStep);
  //The method for implementing the HpdPhotoelectric effect.



  double getCurrentHpdQE(int ihpdnum, int richdetnum,
                         double photonenergy);
  // To interpolate the QE from the QE data for a specific hpd.



  G4double HpdPhElectronKE()   {return m_HpdPhElectronKE; }
  //To get the PhotoElectron energy at emission, which is from the
  // HV applied to the HPD tube.

  void setHpdPhElectronKE(double HpdPhElecKE );

  double PhCathodeToSilDetMaxDist()
  {return m_PhCathodeToSilDetMaxDist; }

  void setPhCathodeToSilDetMaxDist(double PhCathodeToSilZ );


  double getCurrentHpdPSFSigma(int hpdnumb, int richdetnumb);
  // get the Point spread function for the current hpd.
  // in the future this may be a function of the radius from the
  // axis of hpd.

  std::vector<double> getCurrentHpdPSFXY ( int hpdnumb, int richdetnumb);
  //  std::vector<double> getCurrentHpdPSFFullParamXY ( int hpdnumb, int richdetnumb, double photRadPos, double PhotEn );
  std::vector<double> getCurrentHpdPSFFullParamXY ( int hpdnumb, int richdetnumb );
  std::vector<double> GetPSFXYForASigma(double aPSFSigma) ;

  std::vector<double> getCurrentHpdDemag(const int hpdnumber,
                                         const int richdetenumber);

  double getCurrentHitSmearSigma(const int hpdnumber,
                                                const int richdetenumber,
                                                const int radiatorNum);
  

  G4String PrePhotoElectricLogVolName() const
  {return m_PrePhotoElectricLogVolName; }

  void setPrePhotoElectricLogVolName( const G4String PrePhotoElecLogVolName);

  G4String PostPhotoElectricLogVolName() const
  {return m_PostPhotoElectricLogVolName; }

  void setPostPhotoElectricLogVolName( const G4String PostPhotoElecLogVolName);

  void setPrePhotoElectricMatNameSec( const G4String aPrePhotoElectricMatNameSec ) 
  {    m_PrePhotoElectricMatNameSec=aPrePhotoElectricMatNameSec;}
  G4String PrePhotoElectricMatNameSec()  {  return  m_PrePhotoElectricMatNameSec;}
  

  //  RichHpdProperties* HpdProperty() {return  m_HpdProperty; }

  RichHpdProperties* HpdProperty() {return  RichHpdProperties::getRichHpdPropertiesInstance(); }

  int NumRichDet() {return m_NumRichDet; }
  void setNumRichDet(int numrichDetect) ;

  const std::vector<int> & numTotHpd() const {return  m_numTotHpd; }
  int numTotHpdInCurrentRichDet(int richdetnum);
  G4String Rich1PhysVolNameA() const {return  m_Rich1PhysVolNameA; }
  G4String Rich2PhysVolNameA() const {return  m_Rich2PhysVolNameA; }
  G4String Rich1PhysVolNameB() const {return  m_Rich1PhysVolNameB; }
  G4String Rich2PhysVolNameB() const {return  m_Rich2PhysVolNameB; }
  double hpdPhCathodeInnerRadius() const {return  m_hpdPhCathodeInnerRadius; }
  void sethpdPhCathodeInnerRadius(double hpdphcInnr );
  double MaxZHitInRich1() const {return m_MaxZHitInRich1; }
  void setMaxZHitInRich1 (double MaxZHitWithinRich1);

  double MaxAnyHpdQEff() const
  {
    return m_MaxAnyHpdQEff;
  }

  
  G4bool UseHpdMagDistortions() {
    return m_UseHpdMagDistortions;
  } 
  
  void setUseHpdMagDistortions(G4bool aflg ) { m_UseHpdMagDistortions= aflg;}
  void setHpdPhElecParam();
  void setPSFPreDc06Flag(G4bool aFlagPsf ) { m_PSFPreDc06Flag = aFlagPsf; }
  void setHpdQEUsingNominalTable(G4bool ahpdqetableoption) 
  {m_HpdQEUsingNominalTable=ahpdqetableoption;}  
  G4bool PSFPreDc06Flag() {return m_PSFPreDc06Flag;}
  G4bool HpdQEUsingNominalTable()  {  return m_HpdQEUsingNominalTable;} 

  void setactivateRichHitSmear( G4bool asFl ) 
  {
    m_activateRichHitSmear=asFl;
  }
  G4bool  activateRichHitSmear() 
  {
    return m_activateRichHitSmear;
  }
  void setRichAerogelHitSmearVal(double aAgelVal) 
  { m_RichAerogelHitSmearVal=aAgelVal;  }
  void setRich1GasHitSmearVal(double aR1gasSmVal)
  {m_Rich1GasHitSmearVal=aR1gasSmVal;}
  void setRich2GasHitSmearVal(double aR2gasSmVal)
  {m_Rich2GasHitSmearVal=aR2gasSmVal;  }
  

  
private:

  G4ThreeVector getCathodeToAnodeDirection(int ihpdnum , int richdetnum , 
                                          G4ThreeVector aLocalElnOrigin ,
                                           std::vector<double>apsfVectInXY  );

  G4String m_PrePhotoElectricLogVolName;
  G4String m_PostPhotoElectricLogVolName;
  G4String m_PrePhotoElectricMatNameSec;
  
  double m_HpdPhElectronKE;
  double m_PhCathodeToSilDetMaxDist;
  //   RichHpdProperties* m_HpdProperty;
  std::vector<int>m_numTotHpd;
  int m_NumRichDet;
  G4String  m_Rich1PhysVolNameA;
  G4String  m_Rich2PhysVolNameA;
  G4String  m_Rich1PhysVolNameB;
  G4String  m_Rich2PhysVolNameB;
  double m_hpdPhCathodeInnerRadius;
  double m_MaxZHitInRich1;
  double  m_MaxAnyHpdQEff;

  G4bool m_UseHpdMagDistortions;

  G4bool m_PSFPreDc06Flag;  // if true the old psf implementation
                            // if false the new psf implementation which
                            // has a worse sigma than the old version.
  G4bool m_HpdQEUsingNominalTable; // if true using old Nominal QE Table for all hpds from sqldddb
                                   // if false using the new measured QE tables for the Hpds. 

  G4bool m_activateRichHitSmear;
  double m_RichAerogelHitSmearVal;
  double m_Rich1GasHitSmearVal;
  double m_Rich2GasHitSmearVal;
  
};

inline G4bool
RichHpdPhotoElectricEffect::IsApplicable(const G4ParticleDefinition& aParticleType)
{
  return ( &aParticleType == G4OpticalPhoton::OpticalPhoton() );
}

inline G4double
RichHpdPhotoElectricEffect::GetMeanFreePath(const G4Track& ,
                                            G4double ,
                                            G4ForceCondition* condition)
{
  *condition = Forced;
  return DBL_MAX;
}

inline void RichHpdPhotoElectricEffect::setHpdPhElectronKE(double HpdPhElecKE) {
  m_HpdPhElectronKE=HpdPhElecKE;
}

inline void RichHpdPhotoElectricEffect::setPhCathodeToSilDetMaxDist
(double PhCathodeToSilZ){
  m_PhCathodeToSilDetMaxDist= PhCathodeToSilZ;
}

inline double
RichHpdPhotoElectricEffect::getCurrentHpdPSFSigma(int hpdnumb,
                                                  int richdetnumb)
{
  return HpdProperty()->getRichHpdPSF(hpdnumb,richdetnumb)
    ->hpdPointSpreadFunction();
}

inline std::vector<double>
RichHpdPhotoElectricEffect::getCurrentHpdDemag(const int hpdnumber,
                                               const int richdetnumber) 
{
  return HpdProperty()->getRichHpdDeMag(hpdnumber,richdetnumber)->HpdDemagVect();
}

inline void
RichHpdPhotoElectricEffect::setPrePhotoElectricLogVolName( const G4String PrePhotoElecLogVolName)
{
  m_PrePhotoElectricLogVolName= PrePhotoElecLogVolName;
}

inline void
RichHpdPhotoElectricEffect::setPostPhotoElectricLogVolName(const G4String PostPhotoElecLogVolName)
{
  m_PostPhotoElectricLogVolName=PostPhotoElecLogVolName;
}

inline void  RichHpdPhotoElectricEffect::setNumRichDet(int numrichDetect )
{
  m_NumRichDet= numrichDetect;
}

inline int RichHpdPhotoElectricEffect::numTotHpdInCurrentRichDet(int richdetnum) {
  int curHpdTot=0;
  if(richdetnum >=0 &&   richdetnum < (int) m_numTotHpd.size()  ) {
    curHpdTot = m_numTotHpd[richdetnum];
  }
  return  curHpdTot;
}
inline void RichHpdPhotoElectricEffect::sethpdPhCathodeInnerRadius(double hpdphcInnr ){
  m_hpdPhCathodeInnerRadius= hpdphcInnr;

}

inline void RichHpdPhotoElectricEffect::setMaxZHitInRich1(double MaxZHitWithinRich1 ){
  m_MaxZHitInRich1 =  MaxZHitWithinRich1;

}

#endif
