// ============================================================================
/// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
/// $Log: not supported by cvs2svn $
/// Revision 1.10  2001/07/23 13:11:44  ibelyaev
/// the package restructurisation(II)
/// 
// ============================================================================
#ifndef    GIGA_GIGATRAJECTORY_H
#define    GIGA_GIGATRAJECTORY_H 1 
// ============================================================================
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
class G4SteppingManager;
///

/** @class GiGaTrajectory GiGaTrajectory.h GiGa/GiGaTrajectory.h
 *
 *  Customisation of "standard" G4Trajectory to incorporate TimeOfFlight
 *   of the each point. Overwise conversion to MCParticle/MCVertex 
 *    structure is impossible.  
 *  
 *  @author  Vanya Belyaev
 *  @date    22/02/2001
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
  inline const int&                  trackID     () const 
  { return m_trackID      ; } 
  inline const int&                  GetTrackID  () const 
  { return trackID ()     ; }
  inline const int&                  parentID    () const 
  { return m_parentID     ; }  
  inline const int&                  GetParentID () const 
  { return parentID()     ; }
  inline const HepLorentzVector&     fourMomentum() const 
  { return m_4vect        ; } 
  inline const HepLorentzVector&     Get4Momentum() const 
  { return fourMomentum() ; } 
  /// 
  inline const G4ParticleDefinition* partDef     () const 
  { return m_partDef ; } 
  ///
  virtual        void ShowTrajectory  ()                 const ;
  virtual        void DrawTrajectory  ( G4int i_mode=0 ) const ;
  virtual        void AppendStep      ( const G4Step*  )       ;
  virtual        void MergeTrajectory ( G4VTrajectory* )       ;
  ///
  virtual inline int                 GetPointEntries(          ) const ;
  virtual inline G4VTrajectoryPoint* GetPoint       ( int indx ) const ;  
  ///
  ///
  inline       void               setStepMgr( const G4SteppingManager* ) ; 
  inline const G4SteppingManager*    stepMgr() const ; 
  ///
private: 
  ///
  int                          m_trackID  ;
  int                          m_parentID ;
  const G4ParticleDefinition*  m_partDef  ; 
  HepLorentzVector             m_4vect    ;   
  /// 
  const G4SteppingManager*     m_stepMgr  ; 
  ///
};
///
#include "GiGa/GiGaTrajectory.icpp"
///

// ============================================================================
#endif  ///< GIGA_GIGATRAJECTORY_H
// ============================================================================







