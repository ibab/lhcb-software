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

  std::vector<double>getCurrentHpdDemag(int hpdnumber, int richdetnumber);


  G4String PrePhotoElectricLogVolName() 
  {return m_PrePhotoElectricLogVolName; }
  void setPrePhotoElectricLogVolName( G4String PrePhotoElecLogVolName);

  G4String PostPhotoElectricLogVolName()
  {return m_PostPhotoElectricLogVolName; }
  void setPostPhotoElectricLogVolName(G4String PostPhotoElecLogVolName);

  RichHpdProperties* HpdProperty() {return  m_HpdProperty; }
  
  int NumRichDet() {return m_NumRichDet; }
  void setNumRichDet(int numrichDetect) ;

  std::vector<int>numTotHpd() {return  m_numTotHpd; }
  int numTotHpdInCurrentRichDet(int richdetnum);
  G4String Rich1PhysVolNameA() {return  m_Rich1PhysVolNameA; }
  G4String Rich2PhysVolNameA() {return  m_Rich2PhysVolNameA; }
  G4String Rich1PhysVolNameB() {return  m_Rich1PhysVolNameB; }
  G4String Rich2PhysVolNameB() {return  m_Rich2PhysVolNameB; }
  double hpdPhCathodeInnerRadius() {return  m_hpdPhCathodeInnerRadius; }
  void sethpdPhCathodeInnerRadius(double hpdphcInnr );
  double MaxZHitInRich1() {return m_MaxZHitInRich1; }
  void setMaxZHitInRich1 (double MaxZHitWithinRich1);


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

};

#endif
