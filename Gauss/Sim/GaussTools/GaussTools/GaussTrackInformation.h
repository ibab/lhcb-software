// ============================================================================
/// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
/// $Log: not supported by cvs2svn $
/// Revision 1.1  2002/10/30 14:00:36  witoldp
/// changes following introduction vector of pointers to hits in 
/// GiGaTrackInformation
///
/// Revision 1.9  2002/10/28 14:47:12  witoldp
/// added info whether hit was created by the track
///
/// Revision 1.8  2001/08/12 15:42:44  ibelyaev
/// improvements with Doxygen comments
///
/// Revision 1.7  2001/07/23 13:11:44  ibelyaev
/// the package restructurisation(II)
/// 
// ============================================================================
#ifndef      GaussTools_GaussTrackInformation_H 
#define      GaussTools_GaussTrackInformation_H 1 
// ============================================================================
/// STL
#include <string>
#include <vector>
/// Geant4 
#include "G4VUserTrackInformation.hh" 
/// GaussTools
#include "GaussTools/GaussHitBase.h"

/** @class GaussTrackInformation GaussTrackInformation.h 
 *
 *  Class which is used to strore auxiallry information about G4Track,
 *  could be used to control storage of track 
 *   fromm G4Track objects into G4VTrajectory objects. 
 *   
 *  Class to be extenden in the future to take into account hit patterns. 
 *   
 *  @author  Vanya Belyaev
 *  @date    23/02/2001
 */

///
class GaussTrackInformation : public G4VUserTrackInformation
{
  ///
public:
  ///
  inline          GaussTrackInformation() ;
  virtual inline ~GaussTrackInformation() ; 
  ///
  inline void* operator new    ( size_t );
  inline void  operator delete ( void*  );
  ///
  virtual void Print() const;   /// from base class
  ///
  /// flag to append step
  inline GaussTrackInformation& setAppendStep( const bool );
  inline const bool appendStep();
  /// track is forced to be saved into trajectory 
  inline GaussTrackInformation&  setToBeStored               ( const bool ) ;
  inline const bool             toBeStored                  () const ;  
  ///
  /// track created a hit
  inline GaussTrackInformation& setCreatedHit(const bool);
  inline const bool            createdHit() const;
  ///
  /// add hit pointer
  inline void addHit(GaussHitBase*);
  ///
  /// update Tracks IDs in hits
  inline void updateHitsTrackID(G4int);

private:
  ///
  /// flag indicating that TrajectoryPoint should be appended
  bool m_appendStep;
  /// flag indicating that track is forced to be stored into trajectory  
  bool m_toBeStored;
  /// flag indicating that track creted a hit
  bool m_createdHit;
  ///
  /// vector of pointers to hits created by that track
  std::vector<GaussHitBase*> m_hits;
  ///

};
///
#include "GaussTools/GaussTrackInformation.icpp"  
///

// ============================================================================
#endif   ///<  GIGA_GaussTrackInformation_H 
// ============================================================================
