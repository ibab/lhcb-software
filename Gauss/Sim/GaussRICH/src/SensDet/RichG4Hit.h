#ifndef RichG4Hit_h
#define RichG4Hit_h 1

#include "G4VHit.hh"
#include "G4THitsCollection.hh"
#include "G4Allocator.hh"
#include "G4ThreeVector.hh"
// This is the hit class for the LHCb-RICH. Only the coordinates of the
// PreStepPoint at the entrance to the SiDet sensitive detector
// are stored in this class.

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




