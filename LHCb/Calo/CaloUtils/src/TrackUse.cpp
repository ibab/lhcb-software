// $Id: TrackUse.cpp,v 1.2 2005-12-08 13:00:22 odescham Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log:
// ============================================================================
// Include files 
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/Algorithm.h"
#include "GaudiKernel/Service.h"
#include "GaudiKernel/AlgTool.h"
// ============================================================================
// Event 
// ============================================================================
#include "Event/Track.h"
// ============================================================================
// local
// ============================================================================
#include "CaloUtils/TrackUse.h"
// ============================================================================
// Boost 
// ============================================================================
#include "boost/lexical_cast.hpp"
// ============================================================================

// ============================================================================
/** @file 
 *  The implementation file for class : TrackUse
 *
 *  @author Vanya BELYAEV belyaev@lapp.in2p3.fr 
 *  @date   2004-10-27
 */
// ============================================================================

// ============================================================================
/// constructor 
// ============================================================================
TrackUse::TrackUse() 
  : m_uniqueOnly   ( false )  // match ALL tracks, including clones 
  , m_error        ( false )  // USE ERROR TRACKS ALSO  
  // 
  , m_isLong       ( true  ) 
  , m_isUpstream   ( true  ) 
  , m_isDownstream ( true  ) 
  , m_isVelotrack  ( true  ) 
  , m_isBackward   ( true  ) 
  , m_isTtrack     ( true  ) 
  //
  , m_velo         ( true  )
  , m_seed         ( true  )
  , m_match        ( true  ) 
  , m_forward      ( true  ) 
  , m_follow       ( true  ) 
  , m_veloTT       ( true  )
  , m_veloBack     ( true  ) 
  , m_ksTrack      ( true  )
{};
// ============================================================================

// ============================================================================
/// destructors 
// ============================================================================
TrackUse::~TrackUse(){};
// ============================================================================

// ============================================================================
/** the basic method fofr delegation of properties 
 *  @param object property manager 
 *  @return statsu code 
 */
// ============================================================================
template <class TYPE>
inline StatusCode 
TrackUse::i_declareProperties ( TYPE* object ) 
{
  if ( 0 == object ) { return StatusCode::FAILURE ; }
  // 
  object -> declareProperty ( "UseUniqueOnly" , m_uniqueOnly ) ;
  object -> declareProperty ( "UseErrorAlso"  , m_error      ) ;
  //
  object -> declareProperty ( "isLong"       , m_isLong       ) ;
  object -> declareProperty ( "isUpstream"   , m_isUpstream   ) ;
  object -> declareProperty ( "isDownstream" , m_isDownstream ) ;
  object -> declareProperty ( "isVelotrack"  , m_isVelotrack  ) ;
  object -> declareProperty ( "isBackward"   , m_isBackward   ) ;
  object -> declareProperty ( "isTtrack"     , m_isTtrack     ) ;
  //
  object -> declareProperty ( "velo"         , m_velo         ) ;
  object -> declareProperty ( "seed"         , m_seed         ) ;
  object -> declareProperty ( "match"        , m_match        ) ;
  object -> declareProperty ( "forward"      , m_forward      ) ;
  object -> declareProperty ( "follow"       , m_follow       ) ;
  object -> declareProperty ( "veloTT"       , m_veloTT       ) ;
  object -> declareProperty ( "veloBack"     , m_veloBack     ) ;
  object -> declareProperty ( "ksTrack"      , m_ksTrack      ) ;
  //
  return StatusCode::SUCCESS ;
};
// ============================================================================


// ============================================================================
/** declare 'own' properties for the algorithm 
 * 
 *  @code 
 *
 *  // algorithm contructor 
 *  MyAlg::MyAlg  ( const std::string& name , 
 *                  ISvcLocator*       svc  )
 *   : Algorithm  ( name , svc ) 
 *   , m_trackUse ()
 *   {
 *      m_trackUse.declareProperties( this ) 
 *   };
 *
 *  @endcode 
 * 
 *  @param alg algorithm 
 *  @return status code 
 */
// ============================================================================
StatusCode TrackUse::declareProperties ( Algorithm* alg )
{ return i_declareProperties ( alg ) ; };
// ============================================================================

// ============================================================================
/** declare 'own' properties for the service 
 * 
 *
 *  @code 
 *
 *  // service contructor 
 *  MySvc::MySvc  ( const std::string& name  , 
 *                  ISvcLocator*       isvc  )
 *   : Service    ( name , isvc ) 
 *   , m_trackUse ()
 *   {
 *      m_trackUse.declareProperties( this ) 
 *   };
 *
 *  @endcode 
 * 
 *  @param svc service 
 *  @return status code 
 */
// ============================================================================
StatusCode TrackUse::declareProperties ( Service*  svc ) 
{ return i_declareProperties ( svc ) ; };
// ============================================================================
  
// ============================================================================
/** declare 'own' properties for the tool 
 * 
 *
 *  @code 
 *
 *  // tool contructor 
 *  MyTool::MyTool  ( const std::string& type , 
 *                    const std::string& name , 
 *                    const IInterface*  parent  )
 *   : AlgTool      ( type , name , parent ) 
 *   , m_trackUse ()
 *   {
 *      m_trackUse.declareProperties( this ) 
 *   };
 *
 *  @endcode 
 * 
 *  @param tool tool
 *  @return status code 
 */
// ============================================================================
StatusCode TrackUse::declareProperties ( AlgTool*  tool ) 
{ return i_declareProperties ( tool ) ; };
// ============================================================================

// ============================================================================
/** the basic methdod which defines if track selected fro furtehr processing
 *  @param track pointer to Track object 
 *  @return decsion
 */
// ============================================================================
bool TrackUse::use  (  Track* track ) const 
{
  if ( 0 == track ) { return false ; }
  // Flag ?
  if ( uniqueOnly    () && !track -> checkFlag(Track::Unique) ) { return false ; }
  if ( !error        () && track -> checkFlag(Track::Invalid) ) { return false ; }
  if ( !isBackward   () && track -> checkFlag(Track::Backward) ) { return false ; }
  // Type ?
  if ( !isLong       () && track -> checkType (Track::Long) ) { return false ; }
  if ( !isUpstream   () && track -> checkType (Track::Upstream) ) { return false ; }
  if ( !isDownstream () && track -> checkType (Track::Downstream) ) { return false ; }
  if ( !isVelotrack  () && track -> checkType (Track::Velo) ) { return false ; }
  if ( !isTtrack     () && track -> checkType (Track::Ttrack ) ) { return false ; }
  // History : use Cnv track for the time being ...
  if ( !veloBack     () && track -> heckHistory(Track::CnvVeloBack)  { return false ; }
  if ( !velo         () && track -> checkHistory(Track::CnvVelo) ) { return false ; }
  if ( !seed         () && track -> checkHistory(Track::CnvSeed) ) { return false ; }
  if ( !match        () && track -> checkHistory(Track::CnvMatch) ) { return false ; }
  if ( !forward      () && track -> checkHistory(Track::CnvForward) ) { return false ; }
  if ( !veloTT       () && track -> checkHistory(Track::CnvVeloTT) ) { return false ; }
  if ( !ksTrack      () && track -> checkHistory(Track::CnvKsTrack) ) { return false ; }
   return true ;  
};
// ============================================================================

// ============================================================================
namespace 
{
  /** Local function to make a simple TYPE->std::string conversion
   *  @param object 
   *  @return string representation
   */
  template <class TYPE> 
  inline std::string prnt ( const TYPE& object ) 
  { return boost::lexical_cast<std::string> (object ) ; } ;
};
// ============================================================================

// ============================================================================
/// 'bit'-representation of track flags/types/categories/algorithms
// ============================================================================
std::string TrackUse::bits (Track* track ) const 
{
  if ( 0 == track ) { return std::string("<invalid>") ; }
  
  std::string msg( "bits: ") ;
  msg +=  "E:" + prnt ( track -> checkFlag(Track::Invalid) ) ;
  msg += "/U:" + prnt ( track -> checkFlag(Track::Unique) ) ;
  msg += "/H:" + prnt ( track -> history       () ) ;
  //
  msg += "/L:" + prnt ( track -> checkType (Track::Long) ) ;
  msg += "/U:" + prnt ( track -> checkType (Track::Upstream) ) ;
  msg += "/D:" + prnt ( track -> checkType (Track::Downstream) ) ;
  msg += "/V:" + prnt ( track -> checkType (Track::Velo) ) ;
  msg += "/B:" + prnt ( track -> checkFlag(Track::Backward) ) ;
  msg += "/T:" + prnt ( track -> checkType (Track::Ttrack ) ) ;
  // History : use Cnv track for the time being ...
  msg += "/b:" + prnt ( track -> checkHistory(Track::CnvVeloBack) ;
  msg += "/v:" + prnt ( track -> checkHistory(Track::CnvVelo) ) ;
  msg += "/s:" + prnt ( track -> checkHistory(Track::CnvSeed) ) ;
  msg += "/m:" + prnt ( track -> checkHistory(Track::CnvMatch) ) ;
  msg += "/f:" + prnt ( track -> checkHistory(Track::CnvForward) ) ;
  msg += "/v:" + prnt ( track -> checkHistory(Track::CnvVeloTT) ) ;
  msg += "/k:" + prnt ( track ->  checkHistory(Track::CnvKsTrack) ) ;
  //
  return msg ;
};
// ============================================================================


// ============================================================================
/// get the list of rejected track types 
// ============================================================================
std::string TrackUse::rejectedTypes() const 
{
  std::string names ;
  if ( uniqueOnly()    ) { names += "'clone' " ; }
  if ( !error()        ) { names += "'error' "  ; }
  return names ;
};
// ============================================================================

// ============================================================================
/// get the list of rejected track categories 
// ============================================================================
std::string TrackUse::rejectedCategories () const 
{
  std::string names ;
  if ( !isLong()        ) { names += "'isLong' "       ; }
  if ( !isUpstream()    ) { names += "'isUpstream' "   ; }
  if ( !isDownstream()  ) { names += "'isDownstream' " ; }
  if ( !isVelotrack()   ) { names += "'isVelotrack' "  ; }
  if ( !isBackward()    ) { names += "'isBackward' "   ; }
  if ( !isTtrack()      ) { names += "'isTtrack' "     ; }
  return names ;
};
// ============================================================================

// ============================================================================
/// get the list of rejected track algorithms  
// ============================================================================
std::string  TrackUse::rejectedAlgorithms () const 
{
  std::string names =  "" ;
  if ( !velo()         ) { names += "'velo' "         ; }
  if ( !seed()         ) { names += "'seed' "         ; }
  if ( !match()        ) { names += "'match' "        ; }
  if ( !forward()      ) { names += "'forward' "      ; }
  if ( !follow()       ) { names += "'follow' "       ; }
  if ( !veloTT()       ) { names += "'veloTT' "       ; }
  if ( !veloBack()     ) { names += "'veloBack' "     ; }
  if ( !ksTrack()      ) { names += "'ksTrack' "      ; }
  return names ;
};
// ============================================================================

// ============================================================================
// The END 
// ============================================================================
