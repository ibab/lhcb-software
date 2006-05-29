// $Id: TrackUse.cpp,v 1.6 2006-05-29 15:36:19 odescham Exp $
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
  , m_isBackward   ( true  ) 
  // 
  , m_isLong       ( true  ) 
  , m_isUpstream   ( true  ) 
  , m_isDownstream ( true  ) 
  , m_isVelotrack  ( true  ) 
  , m_isTtrack     ( true  ) 

  //
  , m_patvelo      ( true  )
  , m_patforward   ( true  )
  , m_patveloTT    ( true  )
  , m_patkshort    ( true  )

  , m_seed         ( true  )
  , m_match        ( true  ) 
  , m_veloTT       ( true  )
  , m_ksTrack      ( true  )
  , m_tsa          ( true  ) 
  , m_trgforward   ( true  ) 
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
  object -> declareProperty ( "isBackward"   , m_isBackward   ) ;
  //
  object -> declareProperty ( "isLong"       , m_isLong       ) ;
  object -> declareProperty ( "isUpstream"   , m_isUpstream   ) ;
  object -> declareProperty ( "isDownstream" , m_isDownstream ) ;
  object -> declareProperty ( "isVelotrack"  , m_isVelotrack  ) ;
  object -> declareProperty ( "isTtrack"     , m_isTtrack     ) ;
  //
  object -> declareProperty ( "patVelo"         , m_patvelo         ) ;
  object -> declareProperty ( "patForward"      , m_patforward      ) ;
  object -> declareProperty ( "patVeloTT"       , m_patveloTT       ) ;
  object -> declareProperty ( "patKshort"       , m_patkshort       ) ;

  object -> declareProperty ( "trackSeeding"     , m_seed         ) ;
  object -> declareProperty ( "trackMatching"    , m_match        ) ;
  object -> declareProperty ( "trackVeloTT"      , m_veloTT       ) ;
  object -> declareProperty ( "trackKshort"      , m_ksTrack      ) ;
  object -> declareProperty ( "tsaTrack"         , m_tsa          ) ;
  object -> declareProperty ( "trgForward"       , m_trgforward   ) ;
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
bool TrackUse::use  (  LHCb::Track* track ) const 
{
  if ( 0 == track ) { return false ; }
  // Flag ?
  if ( uniqueOnly    () && track -> checkFlag(LHCb::Track::Clone)    ) { return false ; }
  if ( !error        () && track -> checkFlag(LHCb::Track::Invalid)  ) { return false ; }
  if ( !isBackward   () && track -> checkFlag(LHCb::Track::Backward) ) { return false ; }
  // Type ?
  if ( !isLong       () && track -> checkType (LHCb::Track::Long)       ) { return false ; }
  if ( !isUpstream   () && track -> checkType (LHCb::Track::Upstream)   ) { return false ; }
  if ( !isDownstream () && track -> checkType (LHCb::Track::Downstream) ) { return false ; }
  if ( !isVelotrack  () && track -> checkType (LHCb::Track::Velo)       ) { return false ; }
  if ( !isTtrack     () && track -> checkType (LHCb::Track::Ttrack )    ) { return false ; }
  // History ?
  if ( !PatVelo     () && track -> checkHistory(LHCb::Track::PatVelo)    ) { return false ; }
  if ( !PatVeloTT   () && track -> checkHistory(LHCb::Track::PatVeloTT)  ) { return false ; }
  if ( !PatForward  () && track -> checkHistory(LHCb::Track::PatForward) ) { return false ; }
  if ( !PatKShort   () && track -> checkHistory(LHCb::Track::PatKShort)  ) { return false ; }

  if ( !TrackSeeding   () && track -> checkHistory(LHCb::Track::TrackSeeding) ) { return false ; }
  if ( !TrackMatching  () && track -> checkHistory(LHCb::Track::TrackMatching)) { return false ; }
  if ( !TrackVeloTT    () && track -> checkHistory(LHCb::Track::TrackVeloTT)  ) { return false ; }
  if ( !TrackKShort    () && track -> checkHistory(LHCb::Track::TrackKShort)  ) { return false ; }

  if ( !TsaTrack       () && track -> checkHistory(LHCb::Track::TsaTrack)     ) { return false ; }
  if ( !TrgForward     () && track -> checkHistory(LHCb::Track::TrgForward)   ) { return false ; }


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
std::string TrackUse::bits (LHCb::Track* track ) const 
{
  if ( 0 == track ) { return std::string("<invalid>") ; }
  
  std::string msg( "bits: ") ;
  msg +=  "E:" + prnt ( track -> checkFlag(LHCb::Track::Invalid) ) ;
  msg += "/U:" + prnt ( !track -> checkFlag(LHCb::Track::Clone) ) ;
  msg += "/B:" + prnt ( track -> checkFlag(LHCb::Track::Backward) ) ;
  //
  msg += "/L:" + prnt ( track -> checkType (LHCb::Track::Long) ) ;
  msg += "/U:" + prnt ( track -> checkType (LHCb::Track::Upstream) ) ;
  msg += "/D:" + prnt ( track -> checkType (LHCb::Track::Downstream) ) ;
  msg += "/V:" + prnt ( track -> checkType (LHCb::Track::Velo) ) ;
  msg += "/T:" + prnt ( track -> checkType (LHCb::Track::Ttrack ) ) ;

  msg += "/H:" + prnt ( track -> history       () ) ;
  msg += "/pv:"  + prnt ( track -> checkHistory(LHCb::Track::PatVelo) ) ;
  msg += "/pvTT:"+ prnt ( track -> checkHistory(LHCb::Track::PatVeloTT) ) ;
  msg += "/pf:"  + prnt ( track -> checkHistory(LHCb::Track::PatForward) ) ;
  msg += "/pks:" + prnt ( track -> checkHistory(LHCb::Track::PatKShort) ) ;
  msg += "/ts:"  + prnt ( track -> checkHistory(LHCb::Track::TrackSeeding) ) ;
  msg += "/tm:"  + prnt ( track ->  checkHistory(LHCb::Track::TrackMatching) ) ;
  msg += "/tvTT:"+ prnt ( track ->  checkHistory(LHCb::Track::TrackVeloTT) ) ;
  msg += "/tks:" + prnt ( track ->  checkHistory(LHCb::Track::TrackKShort) ) ;
  msg += "/tsa:" + prnt ( track ->  checkHistory(LHCb::Track::TsaTrack) ) ;
  msg += "/trg:" + prnt ( track ->  checkHistory(LHCb::Track::TrgForward) ) ;
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
  if ( !PatVelo()     ) { names += "'PatVelo' "         ; }
  if ( !PatVeloTT()   ) { names += "'PatVeloTT' "         ; }
  if ( !PatForward()  ) { names += "'PatForward' "         ; }
  if ( !PatKShort()   ) { names += "'PatKShort' "         ; }
  if ( !TrackSeeding()  ) { names += "'TrackSeeding' "        ; }
  if ( !TrackMatching() ) { names += "'TrackMatching' "      ; }
  if ( !TrackVeloTT()   ) { names += "'TrackVeloTT' "       ; }
  if ( !TrackKShort()   ) { names += "'TrackKShort' "       ; }
  if ( !TsaTrack()      ) { names += "'TsaTrack' "     ; }
  if ( !TrgForward()    ) { names += "'TrgForward' "      ; }
  return names ;
};
// ============================================================================

