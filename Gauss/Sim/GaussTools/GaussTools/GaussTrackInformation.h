// $Id: GaussTrackInformation.h,v 1.4 2003-04-24 13:07:23 witoldp Exp $ 
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// Revision 1.3  2003/04/09 12:07:37  witoldp
// added pointer to RICHInfo
//
// Revision 1.2  2002/12/07 21:19:13  ibelyaev
//  few optimization updates
//
// ============================================================================
#ifndef      GaussTools_GaussTrackInformation_H 
#define      GaussTools_GaussTrackInformation_H 1 
// ============================================================================
/// STL
#include <string>
#include <vector>
// GiGa
#include "GiGa/GiGaUtil.h"
/// Geant4 
#include "G4VUserTrackInformation.hh" 
/// GaussTools
#include "GaussTools/GaussHitBase.h"

// forward declaration
class RichInfo;

/** @class GaussTrackInformation GaussTrackInformation.h 
 *
 *  Class which is used to store auxilliary information about G4Track,
 *      
 *  @author  Witek Pokorski Witold.Pokorski@cern.ch
 *  @author  Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date    23/02/2001
 */

///
class GaussTrackInformation : public G4VUserTrackInformation
{
  ///
public:

  /// default (empty) constructor
  GaussTrackInformation() ;
  
  /** copy constructor
   *  @param right object to be copied 
   */
  GaussTrackInformation( const GaussTrackInformation& right );
  
  /// destructor 
  virtual ~GaussTrackInformation() ; 
  
  /// clone (virtual constructor)
  virtual GaussTrackInformation* clone() const ;
  
  /// overloaded operator new 
  void* operator new    ( size_t );
  
  /// overloaded operator delete
  void  operator delete ( void*  );

  /// needed by base class 
  virtual void Print      () const {};
  
  /// flag to append the step 
  const   bool appendStep () const { return m_appendStep ; }
  
  /** set new value for flag to append step
   *  @param value new value of the flag 
   */
  GaussTrackInformation& setAppendStep ( const bool value )
  { m_appendStep = value ; return *this ; }
  
  /// flag to force the saving of track into traectory
  const bool toBeStored () const { return m_toBeStored ; }
  
  /** set new value for flag to force the saving track into trajectory
   *  @param value new value of the flag 
   */
  GaussTrackInformation& setToBeStored ( const bool value ) 
  { m_toBeStored = value ; return *this ; }
  
  /// track created a hit
  const bool createdHit() const { return m_createdHit ; }
  
  /** set new value for flag 
   *  @param value new value of the flag 
   */
  GaussTrackInformation& setCreatedHit ( const bool value )
  { m_createdHit = value ; return *this ; }
  
  /** add hit pointer
   *  @param hit hit to be added into list of connected hits
   */
  GaussTrackInformation& addHit    ( GaussHitBase* hit ) 
  { if ( 0 != hit ) { m_hits.push_back( hit ) ; } ; return *this ; }
  
  /** add hit pointer
   *  @param hit hit to be added into list of connected hits
   */
  GaussTrackInformation& addToHits ( GaussHitBase* hit ) 
  { return addHit( hit ) ; }
  
  /** update Tracks IDs in hits
   *  (set the new track ID for all connected hits)
   *  @param trackID new value of trackID 
   */
  GaussTrackInformation& updateHitsTrackID( G4int trackID )
  {
    for( Hits::iterator ihit = m_hits.begin() ; m_hits.end() != ihit ; ++ihit )
      {
        GaussHitBase* hit = *ihit ;
        if( 0 != hit ) { hit->setTrackID( trackID ); }
      }
    return *this ;
  }

  /** get the pointer to the richInfo
   */

  RichInfo* richInfo() 
  {
    return  m_richInfo;
  }
  
  /** set the pointer to the richInfo
   *  @param aRichInfo pointer to RichInfo
   */
  
  void setRichInfo ( RichInfo* aRichInfo ) 
  {
    m_richInfo = aRichInfo;
  }

  
private:
  
  /// flag indicating that TrajectoryPoint should be appended
  bool m_appendStep ;
  /// flag indicating that track is forced to be stored into trajectory  
  bool m_toBeStored ;
  /// flag indicating that track created a hit
  bool m_createdHit ;

  /// vector of pointers to hits created by that track
  typedef std::vector<GaussHitBase*> Hits;
  Hits  m_hits;

  /// pointer to a specialised RICHInfo object containing RICH-specific
  /// track information
  RichInfo* m_richInfo;
  
};
// ===========================================================================

// ===========================================================================
/** @fn  gaussTrackInformation
 *  @param  g4  pointer to G4VUserTrackInformation object
 *  @return cast (dynamic or static to GaussTrackInformation
 *  @author  Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date    2002-12-07
 */
// ============================================================================
inline GaussTrackInformation* 
gaussTrackInformation ( G4VUserTrackInformation* g4 )
{
  GiGaUtil::FastCast<G4VUserTrackInformation,GaussTrackInformation> cast;
  return cast( g4 );
};
// ============================================================================

// ============================================================================
#endif   ///<  GIGA_GaussTrackInformation_H 
// ============================================================================
