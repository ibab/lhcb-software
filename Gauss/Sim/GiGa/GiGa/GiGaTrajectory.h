#ifndef    GIGA_GIGATRAJECTORY_H
#define    GIGA_GIGATRAJECTORY_H 1 
/// STL
#include <vector>
/// base "interface" class
#include "G4VTrajectory.hh" 
/// GiGa 
#include "GiGa/GiGaTrajectoryPoint.h"
///
class G4ParticleDefinition;
class G4Track;
class G4Step;
///

/** @class GiGaTrajectory GiGaTrajectory.h GiGa/GiGaTrajectory.h

    Customisation of "standard" G4Trajectory to incorporate TimeOfFlight
    of the each point. Overwise conversion to MCParticle/MCVertex structure is impossible.  
    
    @author  Vanya Belyaev
    @date    22/02/2001
*/


class GiGaTrajectory: public G4VTrajectory                    , 
                      public std::vector<GiGaTrajectoryPoint*>
{
  ///
public:
  ///
  inline GiGaTrajectory (                        );
  inline GiGaTrajectory ( const G4Track*         );
  inline GiGaTrajectory ( const GiGaTrajectory & );
  virtual inline ~GiGaTrajectory();
  ///  
  inline void* operator new    ( size_t ) ;
  inline void  operator delete ( void*  ) ;
  ///
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
  virtual inline void ShowTrajectory  ()                 const ;
  virtual        void DrawTrajectory  ( G4int i_mode=0 ) const ;
  virtual inline void AppendStep      ( const G4Step*  )       ;
  virtual inline void MergeTrajectory ( G4VTrajectory* )       ;
  ///
  virtual int                 GetPointEntries(          ) const ;
  virtual G4VTrajectoryPoint* GetPoint       ( int indx ) const ;  
  ///
private: 
  ///
  int                          m_trackID  ;
  int                          m_parentID ;
  const G4ParticleDefinition*  m_partDef  ; 
  HepLorentzVector             m_4vect    ;   
  /// 
};
///
#include "GiGa/GiGaTrajectory.icpp"
///

#endif  // GIGA_GIGATRAJECTORY_H



