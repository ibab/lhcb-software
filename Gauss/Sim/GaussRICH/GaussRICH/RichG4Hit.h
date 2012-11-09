#ifndef GaussRICH_RichG4Hit_h
#define GaussRICH_RichG4Hit_h 1

#include "GaussTools/GaussHitBase.h"
#include "G4VHit.hh"
#include "G4THitsCollection.hh"
#include "G4Allocator.hh"
#include "G4ThreeVector.hh"
#include "RichG4RadiatorMaterialIdValues.h"
#include "RichG4HpdReflectionFlag.h"
#include <vector>
#include <bitset>

// From Kernel
#include "Kernel/RichRadiatorType.h"
#include "Kernel/RichDetectorType.h"

// This is the Geant4 hit class for the LHCb-RICH.
// The coordinates of the PreStepPoint at the entrance to the
// SiDet sensitive detector are stored as the Rich hit coordinates
// in this class.

class RichG4Hit : public GaussHitBase
{

public:

  RichG4Hit() {}
  virtual ~RichG4Hit();
  RichG4Hit(const RichG4Hit &right);

  const RichG4Hit& operator=(const RichG4Hit &right);
  int operator==(const RichG4Hit &right) const;

  inline void *operator new(size_t);
  inline void operator delete(void *aHit);

  void Draw();
  void Print();
  void DrawPEOrigin();
  std::vector<bool> DecodeRichHpdReflectionFlag() const;

  /// Sorting operator
  inline bool operator< ( const RichG4Hit & hit ) const
  { 
    // sort by z coord of emission point
    // doesn't really matter what, as long as it is predictable
    return ( this->GetPhotEmisPt().z() < hit.GetPhotEmisPt().z() );
  }

private:

  G4double m_edep;           ///< energy deposited in the Si Det.
  G4ThreeVector m_GlobalPos; ///<  Global coordinate in the LHCb system.
  G4ThreeVector m_LocalPos;  ///< Local coordinate in the HPD System.
  G4ThreeVector m_GlobalPEOriginPos; ///<  Global coord of Photoelectron origin
  G4ThreeVector m_LocalPEOriginPos; ///<  Local coord of Photoelectron origin
  G4int m_CurRichNum;        ///< this is 0 for RICH1 and 1 for RICH2
  G4int m_CurSectorNum;      ///< this is the Quadrant or Half of a RICH.
  G4int m_CurHpdNum;         ///< Global Hpd number within a RICH.
  G4int m_CurPixelXNum;       ///< Pixel Column number (0-31) within an HPD.
  G4int m_CurPixelYNum;       ///< Pixel Row number (0-31) within an HPD.
  G4int m_OptPhotID;          ///< ID of the optical photon.
  G4int m_ChTrackID;          ///< ID of the mother of the optical photon.
  G4int m_PETrackID;          ///< ID of the track(normally a photoelectron)
  G4int m_ChTrackPDG;         ///< PDG code of the mother of the optical photon.
  G4int m_PETrackPDG;          ///< PDG code of the track
  G4int m_RadiatorNumber;      ///< radiator number which produced the photon.
                               // c4f10 =1, cf4=2. aerogelFullTile =10-25,Gasqw=6-7(rich1,rich2)
                               // , hpdqw=8, filtergeneric=4,
                               // filterd263=5, nitrogen=30-31(rich1,rich2).
                               // CO2=33  
                               // aerogelSubTiles = 1000->1299
  G4ThreeVector m_PhotEmisPt;   ///< emiss pt coord of the photon.
  G4double m_PhotEnergyAtProd;  ///< Photon energy in MeV at Ckv Production.
  G4double m_ThetaCkvAtProd;    ///< Cherenkov Angle Theta (rad) at Ckv production.
  G4double m_PhiCkvAtProd;      ///< Cherenkov Angle Phi (rad) at Ckv production.
  G4double m_ChTrackTotMom;     ///< Total momentum (MeV) of the mother of Photon at Ckv Production.
  G4ThreeVector m_ChTrackMomVect; ///< Three Momentum of the mother of Photon at Ckv Production.
  // Only with RichVerboseTag mode
  G4double m_RichHitGlobalTime;    ///< Global time for RichHit.
  G4double m_RichChTrackMass;          ///< PDG mass of the Mother of the optical photon.
  // Only with RichVerboseTag mode

  G4ThreeVector m_ChTrackCkvPreStepPos; ///< Prestep position of charged track during cherenkov  step.
  // Only with RichVerboseTag mode

  G4ThreeVector m_ChTrackCkvPostStepPos; ///< Poststep position of charged track during cherenkov  step.
  // Only with RichVerboseTag mode

  G4int m_OptPhotRayleighFlag;           ///< number of times optical photon is Rayleigh scattered.
  // Zero means not Rayleigh scattered.
  // Only with RichVerboseTag mode

  G4ThreeVector m_OptPhotAgelExitPos;   ///< Position where photon exited from Aerogel.
  // Only with RichVerboseTag mode and RichStepAnalysis4

  G4ThreeVector m_Mirror1PhotonReflPosition; ///< Photon Reflection Point on Spherical mirror which is mirror1.
  // Only with RichVerboseTag mode and RichStepAnalysis5

  G4ThreeVector m_Mirror2PhotonReflPosition; ///< Photon Reflection Point on Flat mirror which is mirror2.
  // Only with RichVerboseTag mode and RichStepAnalysis5

  G4int m_Mirror1PhotonDetectorCopyNum;      ///< For the reflection point on Sperical mirror, this is
  //  1000* mirrortype + 100*richdetnum+ MirrorCopyNum
  // where mirrortype =0 for spherical mirror.
  // richdetnum=0 for rich1 and  1 for rich2.
  // MirrorCopyNum = copy number of the spherical mirror.
  // Only with RichVerboseTag mode and RichStepAnalysis5


  G4int m_Mirror2PhotonDetectorCopyNum;      ///< For the reflection point on Flat mirror, this is
  // 1000*mirrortype + 100*richdetnum+ MirrorCopyNum
  // where mirrortype = 1 for flat mirror.
  // richdetnum=0 for rich1 and  1 for rich2.
  // MirrorCopyNum = copy number of the flat mirror.
  // Only with RichVerboseTag mode and RichStepAnalysis5

  G4int m_RichVerboseHitInfo;        ///< if 0 the variables with  RichVerboseTag are disabled and have
  // no meaningful values. If not 0 they have the expected values.

  G4int m_RichHpdQW2PhCathReflFlag; // if 0 no photon reflection at Hpd QW-PhCathode boundary (normal case)
                                    // if 1 phtoton reflection happened at the Hpd QW-PhCathode boundary.

  G4int m_ElectronBackScatterFlag; // 0 means the track (electron) which created the hit, was not backscattered
                                   // 1 or more means the track created the this hit was backscattered.
                                   // this number indicates the number times the track backscattered.

  G4int m_PhotoElectricProductionFlag; // 0 means the track created the hit is NOT created by Photoelectric process
  // 1 means the track created the hit was produced by Photoelectric process(normal case)
  // 2 means the although the track is not created by Photoelectric process,
  // its parent or some grand parent is created by Photoelectric process.

  G4int m_RichHpdPhotonReflectionFlag; // bit packed word which indicates if and where a photon might have
                                       // reflected in an hpd before creating photoelectron.
                                       // if it is zero, no reflection occured, which is the normal case for signal hits.
                                       // if it is non zero one can unpack the bits to see where it reflected.

  G4ThreeVector m_HpdQuartzWindowExtSurfPhotIncidentPosition; // Photon incidence point on the external surface of the HPD Quartz Window.
  G4int m_PhotonSourceProcessInfo; // 0 means unknown, 1 means Cherenkov process, 2 means Scintillation process
                                   // This is the process which created the corresponding optical photon.
                                   // for hits from backscattered pe, this is 0.
  G4bool m_FlagHitAsDuplicate; // if true the hit is a duplicate hit. If false it is a unique hit which is the case for most of the hits.
                              // this helps to study the effect of binary readout 

public:

  inline void SetEdep(const G4double de)
  { m_edep = de; };

  inline G4double GetEdep() const
  { return m_edep; };

  inline void SetGlobalPos( const G4ThreeVector & xyz)
  { m_GlobalPos = xyz; };

  inline const G4ThreeVector & GetGlobalPos() const
  { return m_GlobalPos; };

  inline void SetLocalPos(const G4ThreeVector & localxyz)
  { m_LocalPos = localxyz; };

  inline const G4ThreeVector & GetLocalPos() const
  { return m_LocalPos; };

  inline const G4ThreeVector & GetGlobalPEOriginPos() const
  { return m_GlobalPEOriginPos; };

  inline void SetGlobalPEOriginPos( const G4ThreeVector & PExyz )
  { m_GlobalPEOriginPos = PExyz; };

  inline const G4ThreeVector & GetLocalPEOriginPos() const
  { return m_LocalPEOriginPos; };

  inline void SetLocalPEOriginPos( const G4ThreeVector & PELocalxyz)
  { m_LocalPEOriginPos = PELocalxyz; };

  inline void SetCurRichDetNum( const G4int IRichNum )
  { m_CurRichNum = IRichNum ; }

  inline G4int GetCurRichDetNum() const
  { return m_CurRichNum; }

  inline void SetCurHpdNum ( const G4int ihp )
  { m_CurHpdNum = ihp; } ;

  inline G4int GetCurHpdNum() const
  { return m_CurHpdNum ; };

  inline void SetCurSectorNum ( const G4int isector )
  { m_CurSectorNum = isector; } ;

  inline G4int GetCurSectorNum() const
  { return m_CurSectorNum ; };

  inline void SetCurPixelXNum ( const G4int ipx )
  { m_CurPixelXNum = ipx; };

  inline G4int GetCurPixelXNum() const
  { return m_CurPixelXNum; };

  inline void SetCurPixelYNum ( const G4int ipy )
  { m_CurPixelYNum = ipy; };

  inline G4int GetCurPixelYNum() const
  { return m_CurPixelYNum; };

  inline void AddEdep( const G4double addenergy )
  { m_edep += addenergy; }

  inline void SetOptPhotID( const G4int aPhotID ) {m_OptPhotID= aPhotID; }
  inline G4int GetOptPhotID() const {return m_OptPhotID; }

  inline void SetChTrackID(const G4int aChTrackID) {m_ChTrackID= aChTrackID; }
  inline G4int GetChTrackID() const {return m_ChTrackID; }

  inline void SetChTrackPDG(const G4int aChTrackPDG) {m_ChTrackPDG= aChTrackPDG; }
  inline G4int GetChTrackPDG() const {return m_ChTrackPDG; }

  inline void SetPETrackID(const G4int aPETrackID) {m_PETrackID= aPETrackID; }
  inline G4int GetPETrackID() const {return m_PETrackID; }

  inline void SetPETrackPDG(const G4int aPETrackPDG) {m_PETrackPDG= aPETrackPDG; }
  inline G4int GetPETrackPDG() const {return m_PETrackPDG; }

  inline void SetRadiatorNumber(const G4int aRadiatorNumber)
  {m_RadiatorNumber=aRadiatorNumber;}

  inline G4int GetRadiatorNumber() const {return m_RadiatorNumber; }

  inline void SetPhotEmisPt(const G4ThreeVector & aEmisspt)
  {m_PhotEmisPt=aEmisspt; }

  inline const G4ThreeVector & GetPhotEmisPt() const {return m_PhotEmisPt; }

  inline void SetPhotEnergyAtProd(const G4double aPhotEnergy)
  { m_PhotEnergyAtProd = aPhotEnergy; }

  inline G4double PhotEnergyAtProd() const {return m_PhotEnergyAtProd; }

  inline void SetThetaCkvAtProd(const G4double aThetaCkv )
  {m_ThetaCkvAtProd=aThetaCkv;}

  inline G4double ThetaCkvAtProd() const { return m_ThetaCkvAtProd; }

  inline void SetPhiCkvAtProd(const G4double aPhiCkv )
  {m_PhiCkvAtProd=aPhiCkv;}

  inline G4double PhiCkvAtProd() const {return m_PhiCkvAtProd; }

  inline void SetChTrackTotMom(const G4double aChTrackTotMom)
  { m_ChTrackTotMom=aChTrackTotMom; }

  inline G4double ChTrackTotMom() const {return m_ChTrackTotMom; }

  inline void SetChTrackMomVect( const G4ThreeVector & aChTrackMomVect )
  {m_ChTrackMomVect=aChTrackMomVect; }

  inline const G4ThreeVector & ChTrackMomVect() const {return m_ChTrackMomVect; }

  inline void SetRichHitGlobalTime(const G4double aRichHitGlobalTime )
  {  m_RichHitGlobalTime= aRichHitGlobalTime;}

  inline G4double RichHitGlobalTime() const
  { return  m_RichHitGlobalTime; }

  inline void SetRichChTrackMass (const G4double aRichTrackMass )
  { m_RichChTrackMass=aRichTrackMass; }

  inline G4double RichChTrackMass() const
  { return  m_RichChTrackMass; }

  inline void setChTrackCkvPreStepPos( const G4ThreeVector & aChTrackCkvPreStepPos)
  { m_ChTrackCkvPreStepPos = aChTrackCkvPreStepPos; }

  inline const G4ThreeVector & ChTrackCkvPreStepPos() const
  { return m_ChTrackCkvPreStepPos; }

  inline void setChTrackCkvPostStepPos( const G4ThreeVector & aChTrackCkvPostStepPos)
  { m_ChTrackCkvPostStepPos = aChTrackCkvPostStepPos; }

  inline const G4ThreeVector & ChTrackCkvPostStepPos() const
  { return m_ChTrackCkvPostStepPos; }

  inline void setOptPhotRayleighFlag( const G4int aOptPhotRayleighFlag)
  { m_OptPhotRayleighFlag = aOptPhotRayleighFlag; }

  inline G4int OptPhotRayleighFlag() const
  { return  m_OptPhotRayleighFlag; }

  inline void setOptPhotAgelExitPos( const G4ThreeVector & aOptPhotAgelExitPos)
  { m_OptPhotAgelExitPos = aOptPhotAgelExitPos; }

  inline const G4ThreeVector & OptPhotAgelExitPos() const
  { return m_OptPhotAgelExitPos; }

  inline void  setMirror1PhotonReflPosition( const G4ThreeVector & aMirror1PhotonReflPosition )
  {m_Mirror1PhotonReflPosition= aMirror1PhotonReflPosition;}

  inline const G4ThreeVector & Mirror1PhotonReflPosition() const
  {return   m_Mirror1PhotonReflPosition;}

  inline void  setMirror2PhotonReflPosition( const G4ThreeVector & aMirror2PhotonReflPosition )
  {m_Mirror2PhotonReflPosition= aMirror2PhotonReflPosition;}

  inline const G4ThreeVector & Mirror2PhotonReflPosition() const
  {return   m_Mirror2PhotonReflPosition;}

  inline void setMirror1PhotonDetectorCopyNum(const G4int aMirror1PhotonDetectorCopyNum)
  {m_Mirror1PhotonDetectorCopyNum = aMirror1PhotonDetectorCopyNum;}
  inline G4int Mirror1PhotonDetectorCopyNum() const {return m_Mirror1PhotonDetectorCopyNum;}

  inline void setMirror2PhotonDetectorCopyNum(const G4int aMirror2PhotonDetectorCopyNum)
  {m_Mirror2PhotonDetectorCopyNum = aMirror2PhotonDetectorCopyNum;}
  inline G4int Mirror2PhotonDetectorCopyNum() const {return m_Mirror2PhotonDetectorCopyNum;}

  inline void setRichVerboseHitInfo( const G4int aVerboseValue )
  {m_RichVerboseHitInfo = aVerboseValue; }
  inline G4int RichVerboseHitInfo() const {return m_RichVerboseHitInfo;}


  void setRichHpdQW2PhCathReflFlag(const G4int aFlag)
  {m_RichHpdQW2PhCathReflFlag=aFlag;}

  inline G4int RichHpdQW2PhCathReflFlag()const
  {  return m_RichHpdQW2PhCathReflFlag;}

  void setElectronBackScatterFlag(const G4int aFlagB)
  {m_ElectronBackScatterFlag=aFlagB;}
  G4int ElectronBackScatterFlag() const
  {  return m_ElectronBackScatterFlag;}

  void setPhotoElectricProductionFlag(const G4int aFlagC)
  {m_PhotoElectricProductionFlag= aFlagC;}
  G4int  PhotoElectricProductionFlag()const
  {  return m_PhotoElectricProductionFlag;}


  G4int RichHpdPhotonReflectionFlag() const
  {  return m_RichHpdPhotonReflectionFlag;}
  void setRichHpdPhotonReflectionFlag(const G4int aFlagD)
  {m_RichHpdPhotonReflectionFlag=aFlagD;}


  inline const G4ThreeVector & HpdQuartzWindowExtSurfPhotIncidentPosition () const
  {return   m_HpdQuartzWindowExtSurfPhotIncidentPosition;}

  void setHpdQuartzWindowExtSurfPhotIncidentPosition (const G4ThreeVector & aHpdQuartzWindowExtSurfPhotIncidentPosition) {
    m_HpdQuartzWindowExtSurfPhotIncidentPosition=aHpdQuartzWindowExtSurfPhotIncidentPosition;
  }

  inline G4int PhotonSourceProcessInfo() const 
  {
    return m_PhotonSourceProcessInfo;
  }

  void setPhotonSourceProcessInfo(const G4int aSourceProcCode ) 
  { m_PhotonSourceProcessInfo=aSourceProcCode;}


  inline Rich::DetectorType detectorType() const
  {
    return ( GetCurRichDetNum() < 0 ?
             (GetGlobalPos().z() < 4000 ? Rich::Rich1 : Rich::Rich2) :
             static_cast<Rich::DetectorType>(GetCurRichDetNum()) );
  }

  Rich::RadiatorType radiatorType() const;

  // HPD Reflections

  inline bool isHpdQwPCRefl() const
  {
    return ( 0 != reflectionBits().test(hpdRefFlags()->HpdQwPCRefl()) );
  }
  inline bool isHpdChromiumRefl() const
  {
    return ( 0 != reflectionBits().test(hpdRefFlags()->HpdChromiumRefl()) );
  }
  inline bool isHpdAirQwRefl() const
  {
    return ( 0 != reflectionBits().test(hpdRefFlags()->HpdAirQwRefl()) );
  }
  inline bool isHpdAirPCRefl() const
  {
    return ( 0 != reflectionBits().test(hpdRefFlags()->HpdAirPCRefl()) );
  }
  inline bool isHpdSiliconRefl() const
  {
    return ( 0 != reflectionBits().test(hpdRefFlags()->HpdSiliconRefl()) );
  }
  inline bool isHpdKovarRefl() const
  {
    return ( 0 != reflectionBits().test(hpdRefFlags()->HpdKovarRefl()) );
  }
  inline bool isHpdKaptonRefl() const
  {
    return ( 0 != reflectionBits().test(hpdRefFlags()->HpdKaptonRefl()) );
  }
  inline bool isHpdPCQwRefl() const
  {
    return ( 0 != reflectionBits().test(hpdRefFlags()->HpdPCQwRefl()) );
  }

  // scintillation

  inline bool isScintillation() const
  {
    return this->PhotonSourceProcessInfo() == 2;
  }
  void setCurrentHitAsDuplicate(G4bool aDupFlag ) 
  {
    m_FlagHitAsDuplicate=aDupFlag;
  }
  
  inline bool FlagHitAsDuplicate() const
  {
    return ( (bool) m_FlagHitAsDuplicate);
  }
  
   
private:

  RichG4HpdReflectionFlag * hpdRefFlags() const
  {
    return RichG4HpdReflectionFlag::RichG4HpdReflectionFlagInstance();
  }

  std::bitset<16> reflectionBits() const
  {
    return std::bitset<16>((unsigned long)m_RichHpdPhotonReflectionFlag);
  }

};

typedef G4THitsCollection<RichG4Hit> RichG4HitsCollection;

extern G4Allocator<RichG4Hit> RichG4HitAllocator;

inline void* RichG4Hit::operator new(size_t)
{
  return static_cast<void*>( RichG4HitAllocator.MallocSingle() );
}

inline void RichG4Hit::operator delete(void *aHit)
{
  RichG4HitAllocator.FreeSingle( static_cast<RichG4Hit*>(aHit) );
}

#endif //GaussRICH_RichG4Hit

