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

#include "RichHpdProperties.h"

//
//  The class definition should access the class for
//  conditions db in the future to get the HPD QE. For now,
//  hardcoded in the xml tabulated properties for 1 hpd. SE 24-2-2002.
//
class RichHpdPhotoElectricEffect : public G4VDiscreteProcess {

public:

  RichHpdPhotoElectricEffect(const GiGaBase*, const G4String&);

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

  std::vector<double> getCurrentHpdDemag(const int hpdnumber,
                                         const int richdetnumber) const;

  G4String PrePhotoElectricLogVolName() const
  {return m_PrePhotoElectricLogVolName; }

  void setPrePhotoElectricLogVolName( const G4String PrePhotoElecLogVolName);

  G4String PostPhotoElectricLogVolName() const
  {return m_PostPhotoElectricLogVolName; }

  void setPostPhotoElectricLogVolName( const G4String PostPhotoElecLogVolName);

  RichHpdProperties* HpdProperty() {return  m_HpdProperty; }

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


private:

  G4String m_PrePhotoElectricLogVolName;
  G4String m_PostPhotoElectricLogVolName;
  double m_HpdPhElectronKE;
  double m_PhCathodeToSilDetMaxDist;
  RichHpdProperties* m_HpdProperty;
  std::vector<int>m_numTotHpd;
  int m_NumRichDet;
  G4String  m_Rich1PhysVolNameA;
  G4String  m_Rich2PhysVolNameA;
  G4String  m_Rich1PhysVolNameB;
  G4String  m_Rich2PhysVolNameB;
  double m_hpdPhCathodeInnerRadius;
  double m_MaxZHitInRich1;
  double  m_MaxAnyHpdQEff;


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
  return m_HpdProperty->getRichHpdPSF(hpdnumb,richdetnumb)
    ->hpdPointSpreadFunction();
}

inline std::vector<double>
RichHpdPhotoElectricEffect::getCurrentHpdDemag(const int hpdnumber,
                                               const int richdetnumber) const
{
  return m_HpdProperty->getRichHpdDeMag(hpdnumber,richdetnumber)->HpdDemagVect();
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
