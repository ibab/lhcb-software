#ifndef RichG4Hit_h
#define RichG4Hit_h 1

#include "G4VHit.hh"
#include "G4THitsCollection.hh"
#include "G4Allocator.hh"
#include "G4ThreeVector.hh"
// This is the hit class for the LHCb-RICH. The coordinates of the
// PreStepPoint at the entrance to the SiDet sensitive detector
// are stored as the Rich hit coordinates in this class.

class RichG4Hit : public G4VHit
{
  public:

      RichG4Hit();
      virtual ~RichG4Hit();
      RichG4Hit(const RichG4Hit &right);
      const RichG4Hit& operator=(const RichG4Hit &right);
      int operator==(const RichG4Hit &right) const;


      inline void *operator new(size_t);
      inline void operator delete(void *aHit);

      void Draw();
      void Print();
      void DrawPEOrigin();

  private:
      G4double m_edep;           //energy deposited in the Si Det.
      G4ThreeVector m_GlobalPos; // Global coordinate in the LHCb system.
      G4ThreeVector m_LocalPos;  //Local coordinate in the HPD System.
      G4ThreeVector m_GlobalPEOriginPos; // Global coord of Photoelectron origin
      G4ThreeVector m_LocalPEOriginPos; // Local coord of Photoelectron origin
      G4int m_CurRichNum;        //this is 0 for RICH1 and 1 for RICH2
      G4int m_CurSectorNum;      // this is the Quadrant or Half of a RICH.
      G4int m_CurHpdNum;         // Global Hpd number within a RICH.
      G4int m_CurPixelXNum;       // Pixel Column number (0-31) within an HPD.
      G4int m_CurPixelYNum;       // Pixel Row number (0-31) within an HPD.
      G4int m_OptPhotID;          // ID of the optical photon.
      G4int m_ChTrackID;          // ID of the mother of the optical photon.
      G4int m_PETrackID;          // ID of the track(normally a photoelectron) 
      G4int m_ChTrackPDG;         // PDG code of the mother of the optical photon.
      G4int m_PETrackPDG;          // PDG code of the track
      G4int m_RadiatorNumber;      // radiator number (0,1,2) which produced the photon.
      G4ThreeVector m_PhotEmisPt;  //emiss pt coord of the photon.
      G4double m_PhotEnergyAtProd;  //Photon energy in MeV at Ckv Production.
      G4double m_ThetaCkvAtProd;    // Cherenkov Angle Theta (rad) at Ckv production.
      G4double m_PhiCkvAtProd;      // Cherenkov Angle Phi (rad) at Ckv production.
      G4double m_ChTrackTotMom;   //Total momentum (MeV) of the mother of Photon at Ckv Production.
      G4ThreeVector m_ChTrackMomVect; // Three Momentum of the mother of Photon at Ckv Production.
      
  public:
  inline void SetEdep(G4double de)      
  { m_edep = de; };

  inline G4double GetEdep()      
  { return m_edep; };

  inline void SetGlobalPos(G4ThreeVector xyz)      
  { m_GlobalPos = xyz; };

  inline G4ThreeVector GetGlobalPos()      
  { return m_GlobalPos; };

  inline void SetLocalPos(G4ThreeVector localxyz)      
  { m_LocalPos = localxyz; };

  inline G4ThreeVector GetLocalPos()      
  { return m_LocalPos; };

  inline G4ThreeVector GetGlobalPEOriginPos()      
  { return m_GlobalPEOriginPos; };
  
  inline void SetGlobalPEOriginPos(G4ThreeVector PExyz)      
  { m_GlobalPEOriginPos = PExyz; };
  
  inline G4ThreeVector GetLocalPEOriginPos()      
  { return m_LocalPEOriginPos; };
  
  inline void SetLocalPEOriginPos(G4ThreeVector PELocalxyz)      
  { m_LocalPEOriginPos = PELocalxyz; };
 
  inline void SetCurRichDetNum(G4int IRichNum ) 
  { m_CurRichNum = IRichNum ; }
  
  inline G4int GetCurRichDetNum() 
  {return  m_CurRichNum; }

  inline void SetCurHpdNum (G4int ihp ) 
  { m_CurHpdNum = ihp; } ;

  inline G4int GetCurHpdNum()           
  { return m_CurHpdNum ; };

  inline void SetCurSectorNum (G4int isector ) 
  { m_CurSectorNum = isector; } ;

  inline G4int GetCurSectorNum()           
  { return m_CurSectorNum ; };

  inline void SetCurPixelXNum (G4int ipx ) 
  { m_CurPixelXNum = ipx; };

  inline G4int GetCurPixelXNum()           
  { return m_CurPixelXNum; };

  inline void SetCurPixelYNum (G4int ipy ) 
  { m_CurPixelYNum = ipy; };

  inline G4int GetCurPixelYNum()           
  { return m_CurPixelYNum; };

  inline void AddEdep( G4double addenergy ) 
  { m_edep += addenergy; }  

  inline void SetOptPhotID(G4int aPhotID) {m_OptPhotID= aPhotID; }
  inline G4int GetOptPhotID() {return m_OptPhotID; }
  inline void SetChTrackID(G4int aChTrackID) {m_ChTrackID= aChTrackID; }
  inline G4int GetChTrackID() {return m_ChTrackID; }
  inline void SetChTrackPDG(G4int aChTrackPDG) {m_ChTrackPDG= aChTrackPDG; }
  inline G4int GetChTrackPDG() {return m_ChTrackPDG; }
  inline void SetPETrackID(G4int aPETrackID) {m_PETrackID= aPETrackID; }
  inline G4int GetPETrackID() {return m_PETrackID; }
  inline void SetPETrackPDG(G4int aPETrackPDG) {m_PETrackPDG= aPETrackPDG; }
  inline G4int GetPETrackPDG() {return m_PETrackPDG; }

  inline void SetRadiatorNumber(G4int aRadiatorNumber) 
                               {m_RadiatorNumber=aRadiatorNumber;}
  inline G4int GetRadiatorNumber() {return m_RadiatorNumber; }
  inline void SetPhotEmisPt(G4ThreeVector aEmisspt) 
                             {m_PhotEmisPt=aEmisspt; }
  inline G4ThreeVector GetPhotEmisPt() {return m_PhotEmisPt; }
  inline void SetPhotEnergyAtProd(G4double aPhotEnergy) 
       { m_PhotEnergyAtProd=aPhotEnergy; }
  inline G4double PhotEnergyAtProd() {return m_PhotEnergyAtProd; }

  inline void SetThetaCkvAtProd(G4double aThetaCkv )
    {m_ThetaCkvAtProd=aThetaCkv;}
  inline G4double ThetaCkvAtProd() {return  m_ThetaCkvAtProd; }

  inline void SetPhiCkvAtProd(G4double aPhiCkv )
    {m_PhiCkvAtProd=aPhiCkv;}
  inline G4double PhiCkvAtProd() {return  m_PhiCkvAtProd; }

  inline void SetChTrackTotMom(G4double aChTrackTotMom)
    { m_ChTrackTotMom=aChTrackTotMom; }
  inline G4double ChTrackTotMom() {return m_ChTrackTotMom; }

  inline void SetChTrackMomVect( G4ThreeVector aChTrackMomVect )
    {m_ChTrackMomVect=aChTrackMomVect; }
  inline G4ThreeVector ChTrackMomVect() {return m_ChTrackMomVect; }
  

};

typedef G4THitsCollection<RichG4Hit> RichG4HitsCollection;

extern G4Allocator<RichG4Hit> RichG4HitAllocator;

inline void* RichG4Hit::operator new(size_t)
{
  void *aHit;
  aHit = (void *) RichG4HitAllocator.MallocSingle();
  return aHit;
}

inline void RichG4Hit::operator delete(void *aHit)
{
  RichG4HitAllocator.FreeSingle((RichG4Hit*) aHit);
}

#endif




