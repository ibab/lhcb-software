// $Id: TrTrackUse.cpp,v 1.1 2004-10-27 11:58:09 ibelyaev Exp $
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
#include "Event/TrStoredTrack.h"
// ============================================================================
// local
// ============================================================================
#include "CaloUtils/TrTrackUse.h"
// ============================================================================
// Boost 
// ============================================================================
#include "boost/lexical_cast.hpp"
// ============================================================================

// ============================================================================
/** @file 
 *  The implementation file for class : TrTrackUse
 *
 *  @author Vanya BELYAEV belyaev@lapp.in2p3.fr 
 *  @date   2004-10-27
 */
// ============================================================================

// ============================================================================
/// constructor 
// ============================================================================
TrTrackUse::TrTrackUse() 
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
TrTrackUse::~TrTrackUse(){};
// ============================================================================

// ============================================================================
/** the basic method fofr delegation of properties 
 *  @param object property manager 
 *  @return statsu code 
 */
// ============================================================================
template <class TYPE>
inline StatusCode 
TrTrackUse::i_declareProperties ( TYPE* object ) 
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
StatusCode TrTrackUse::declareProperties ( Algorithm* alg )
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
StatusCode TrTrackUse::declareProperties ( Service*  svc ) 
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
StatusCode TrTrackUse::declareProperties ( AlgTool*  tool ) 
{ return i_declareProperties ( tool ) ; };
// ============================================================================

// ============================================================================
/** the basic methdod which defines if track selected fro furtehr processing
 *  @param track pointer to TrStoiredTrack object 
 *  @return decsion
 */
// ============================================================================
bool TrTrackUse::use  ( const TrStoredTrack* track ) const 
{
  if ( 0 == track ) { return false ; }
  
  // unique ?
  if ( uniqueOnly    () &&     !track -> unique    () ) { return false ; }
  
  // use error tracks ? 
  if ( !error        () && 0 != track -> errorFlag () ) { return false ; }
  
  // categories ?
  if ( !isLong       () && track -> isLong         () ) { return false ; }
  if ( !isUpstream   () && track -> isUpstream     () ) { return false ; }
  if ( !isDownstream () && track -> isDownstream   () ) { return false ; }
  if ( !isVelotrack  () && track -> isVelotrack    () ) { return false ; }
  if ( !isBackward   () && track -> isBackward     () ) { return false ; }
  if ( !isTtrack     () && track -> isTtrack       () ) { return false ; }
  
  // algorihtms?
  if ( !velo         () && track -> velo           () ) { return false ; }
  if ( !seed         () && track -> seed           () ) { return false ; }
  if ( !match        () && track -> match          () ) { return false ; }
  if ( !forward      () && track -> forward        () ) { return false ; }
  if ( !follow       () && track -> follow         () ) { return false ; }
  if ( !veloTT       () && track -> veloTT         () ) { return false ; }
  if ( !veloBack     () && track -> veloBack       () ) { return false ; }
  if ( !ksTrack      () && track -> ksTrack        () ) { return false ; }
  
  return true ;  
};
// ============================================================================

// ============================================================================
namespace 
{
  /** @fn prnt 
   *  local function to make a simple TYPE->std::string conversion
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
std::string TrTrackUse::bits ( const TrStoredTrack* track ) const 
{
  if ( 0 == track ) { return std::string("<invalid>") ; }
  
  std::string msg( "bits: ") ;
  msg +=  "E:" + prnt ( track -> errorFlag     () ) ;
  msg += "/U:" + prnt ( track -> unique        () ) ;
  msg += "/H:" + prnt ( track -> history       () ) ;
  //
  msg += "/L:" + prnt ( track -> isLong        () ) ;
  msg += "/U:" + prnt ( track -> isUpstream    () ) ;
  msg += "/D:" + prnt ( track -> isDownstream  () ) ;
  msg += "/V:" + prnt ( track -> isVelotrack   () ) ;
  msg += "/B:" + prnt ( track -> isBackward    () ) ;
  msg += "/T:" + prnt ( track -> isTtrack      () ) ;
  //
  msg += "/v:" + prnt ( track -> velo          () ) ;
  msg += "/s:" + prnt ( track -> seed          () ) ;
  msg += "/m:" + prnt ( track -> match         () ) ;
  msg += "/f:" + prnt ( track -> forward       () ) ;
  msg += "/f:" + prnt ( track -> follow        () ) ;
  msg += "/v:" + prnt ( track -> veloTT        () ) ;
  msg += "/b:" + prnt ( track -> veloBack      () ) ;
  msg += "/k:" + prnt ( track -> ksTrack       () ) ;
  //
  return msg ;
};
// ============================================================================

// ============================================================================
/// get the list of rejected track types 
// ============================================================================
std::string TrTrackUse::rejectedTypes() const 
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
std::string TrTrackUse::rejectedCategories () const 
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
std::string  TrTrackUse::rejectedAlgorithms () const 
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
