#ifndef    GIGA_GIGATRAJECTORY_H
#define    GIGA_GIGATRAJECTORY_H 1 

///
#include <vector>

/// base "interface" class
#include "G4VTrajectory.hh" 

#include "GiGa/GiGaTrajectoryPoint.h"

class G4ParticleDefinition;
class G4Track;
class G4Step;

/** @class GiGaTrajectory GiGaTrajectory.h GiGa/GiGaTrajectory.h

    Customisation of "standard" G4Trajectory to incorporate TimeOfFlight
    of the each point. Overwise conversion to MCParticle/MCVertex structure is impossible.  
    
    @author  Vanya Belyaev
    @date    22/02/2001
*/


class GiGaTrajectory: public G4VTrajectory
{
  ///
public:
  ///
  typedef   std::vector<GiGaTrajectoryPoint*>  POINTS;
  typedef   POINTS::iterator                   IT    ;
  typedef   POINTS::const_iterator             CI    ;
  typedef   POINTS::reverse_iterator           RI    ;
  typedef   POINTS::const_reverse_iterator     CRI   ;
  ///  
  GiGaTrajectory ();
  GiGaTrajectory ( const G4Track* aTrack  );
  GiGaTrajectory ( const GiGaTrajectory & );
  virtual ~GiGaTrajectory();
  ///  
  inline void* operator new(size_t);
  inline void  operator delete(void*);
  inline int operator == (const GiGaTrajectory& right) const
  {return ( &right == this );} 
  ///
  /// accessors - a'la Gaudi and a'la Geant4 
  inline const int&                  trackID     () const { return m_trackID      ; } 
  inline const int&                  GetTrackID  () const { return trackID ()     ; }
  inline const int&                  parentID    () const { return m_parentID     ; }  
  inline const int&                  GetParentID () const { return parentID()     ; }
  inline const HepLorentzVector&     fourMomentum() const { return m_4vect        ; } 
  inline const HepLorentzVector&     Get4Momentum() const { return fourMomentum() ; } 
  /// 
  inline const G4ParticleDefinition* partDef     () const { return m_partDef ; } 
  ///
  virtual void ShowTrajectory  ()                 const ;
  virtual void DrawTrajectory  ( G4int i_mode=0 ) const ;
  virtual void AppendStep      ( const G4Step*  )       ;
  virtual void MergeTrajectory ( G4VTrajectory* )       ;
  ///
  virtual int                 GetPointEntries(          ) const 
  { return m_points.size(); } 
  virtual G4VTrajectoryPoint* GetPoint       ( int indx ) const  
  { return indx < m_points.size() ? m_points[indx] : 0 ; }
  ///
  inline  POINTS::iterator                begin ()       { return m_points.begin () ; };
  inline  POINTS::const_iterator          begin () const { return m_points.begin () ; };
  inline  POINTS::reverse_iterator        rbegin()       { return m_points.rbegin() ; };
  inline  POINTS::const_reverse_iterator  rbegin() const { return m_points.rbegin() ; };
  inline  POINTS::iterator                end   ()       { return m_points.end   () ; };
  inline  POINTS::const_iterator          end   () const { return m_points.end   () ; };
  inline  POINTS::reverse_iterator        rend  ()       { return m_points.rend  () ; };
  inline  POINTS::const_reverse_iterator  rend  () const { return m_points.rend  () ; };
  ///
private: 
  ///
  POINTS                       m_points   ;
  ///
  int                          m_trackID  ;
  int                          m_parentID ;
  const G4ParticleDefinition*  m_partDef  ; 
  HepLorentzVector             m_4vect    ;   
  /// 
};
///
extern G4Allocator<GiGaTrajectory> s_GiGaTrajectoryAllocator;
///
inline void* GiGaTrajectory::operator new(size_t)
{ return (void*) s_GiGaTrajectoryAllocator.MallocSingle(); };
///
inline void  GiGaTrajectory::operator delete(void* aTrajectory)
{ s_GiGaTrajectoryAllocator.FreeSingle( (GiGaTrajectory*) aTrajectory ); };
///

#endif  // GIGA_GIGATRAJECTORY_H


