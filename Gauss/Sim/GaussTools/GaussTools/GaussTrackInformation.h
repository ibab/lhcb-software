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
#include "GaussTools/DetTrackInfo.h"

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
public:
  // the actual tyep of hit conatiner 
  typedef std::vector<GaussHitBase*> Hits;
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
  bool appendStep () const { return m_appendStep ; }
  
  /** set new value for flag to append step
   *  @param value new value of the flag 
   */
  GaussTrackInformation& setAppendStep ( const bool value )
  { m_appendStep = value ; return *this ; }
  
  /// flag to force the saving of track into traectory
  bool toBeStored () const { return m_toBeStored ; }
  
  /** set new value for flag to force the saving track into trajectory
   *  @param value new value of the flag 
   */
  GaussTrackInformation& setToBeStored ( const bool value ) 
  { m_toBeStored = value ; return *this ; }
  
  /// track created a hit
  bool createdHit() const { return m_createdHit ; }
  
  /** set new value for flag 
   *  @param value new value of the flag 
   */
  GaussTrackInformation& setCreatedHit ( const bool value )
  { m_createdHit = value ; return *this ; }

  // Retrieve if direct parent particle has been stored or not
  bool directParent() const {return m_directParent;}

  // Set if direct parent particle has been stored or not
  GaussTrackInformation& setDirectParent(const bool value)
  { m_directParent = value; return *this;}
  
  /// Get flag to store or not in internal HepMC structure
  bool storeHepMC() const {return m_storeHepMC;}

  /// Set value of flag to store or not in internal HepMC structure
  GaussTrackInformation& setStoreHepMC(const bool value)
  { m_storeHepMC = value; return *this;}
  
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
  
  // get the container of hits 
  const Hits& hits() const { return m_hits ; }
  
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
  
  /** get the pointer to the detInfo
   */
  DetTrackInfo* detInfo() const { return  m_detInfo; }
  
  /** set the pointer to the detInfo
   *  @param aDetInfo pointer to DetTrackInfo
   */
  void setDetInfo ( DetTrackInfo* aDetInfo ) 
  { m_detInfo = aDetInfo; }

  
private:
  
  /// flag indicating that TrajectoryPoint should be appended
  bool m_appendStep ;
  /// flag indicating that track is forced to be stored into trajectory  
  bool m_toBeStored ;
  /// flag indicating that track created a hit
  bool m_createdHit ;
  /// flag indicating that the direct parent particle was not stored 
  /// in HepMC event this will be represented by a special 'dummy' link
  bool m_directParent;
  // flag indicating that the track should be stored in HepMC record
  bool m_storeHepMC;

  /// vector of pointers to hits created by that track
  Hits  m_hits;

  /// pointer to a specialised DetTrackInfo object containing detector-specific
  /// track information
  DetTrackInfo* m_detInfo;
  
};


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
}


// ===========================================================================
/** @fn  gaussTrackInformation
 *  @param  g4  pointer to G4VUserTrackInformation object
 *  @return cast (dynamic or static to GaussTrackInformation
 *  @author  Vanya Belyaev Ivan.Belyaev@itep.ru
 *  @date    2002-12-07
 */
// ============================================================================
inline const GaussTrackInformation* 
gaussTrackInformation ( const G4VUserTrackInformation* g4 )
{
  GiGaUtil::FastCast< 
    const G4VUserTrackInformation,const GaussTrackInformation> cast;
  return cast( g4 );
}

// ============================================================================
#endif   ///<  GIGA_GaussTrackInformation_H 

