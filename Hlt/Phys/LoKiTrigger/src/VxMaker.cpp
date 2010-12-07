// $Id$
// ============================================================================
// Include files 
// ============================================================================
// HltBase 
// ============================================================================
#include "HltBase/HltUtils.h"
// ============================================================================
// GaudiAlg
// ============================================================================
#include "GaudiAlg/GaudiAlgorithm.h"
// ============================================================================
// KalmanFilter 
// ============================================================================
#include "KalmanFilter/FastVertex.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/Combiner.h"
#include "LoKi/VxMakerConf.h"
#include "LoKi/VxMaker.h"
#include "LoKi/Hlt1.h"
#include "LoKi/Constants.h"
// ============================================================================
/** @file 
 *
 *  Implementation file for class LoKi::Hlt1::VxMaker
 *  This file is part of LoKi project: 
 *   ``C++ ToolKit for Smart and Friendly Physics Analysis''
 * 
 *  The package has been designed with the kind help from
 *  Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas, 
 *  contributions and advices from G.Raven, J.van Tilburg, 
 *  A.Golutvin, P.Koppenburg have been used in the design.
 *
 *  By usage of this code one clearly states the disagreement 
 *  with the campain of Dr.O.Callot et al.: 
 *  ``No Vanya's lines are allowed in LHCb/Gaudi software.''
 *  
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date   2010-10-28
 *
 *                    $Revision$
 *  Last Modification $Date$ 
 *                 by $Author$
 */ 
// ============================================================================
namespace 
{
  // ==========================================================================
  const Gaudi::StringKey s_CHI2 = Gaudi::StringKey ( "Chi2" ) ;
  const Gaudi::StringKey s_DOCA = Gaudi::StringKey ( "DOCA" ) ;
  // ==========================================================================
}
// ============================================================================
//  consructor from the verter making parameters 
// ============================================================================
LoKi::Hlt1::VxMakerBase::VxMakerBase
( const LoKi::Hlt1::VxMakerConf& conf ) 
  : LoKi::Hlt1::HelperTool ( 1 ) 
  , m_conf ( conf ) 
{}
// ============================================================================
// virtual destructor 
// ============================================================================
LoKi::Hlt1::VxMakerBase::~VxMakerBase(){}
// ============================================================================
// make the vertex from  two tracks 
// ============================================================================
const LHCb::RecVertex*
LoKi::Hlt1::VxMakerBase::makeVertex 
( const LHCb::Track* track1 , 
  const LHCb::Track* track2 , 
  const bool         apply  ) const 
{
  //
  double doca = LoKi::Constants::NegativeInfinity ;
  double chi2 = LoKi::Constants::NegativeInfinity ;
  //
  return makeVertex ( track1 , track2 , 
                      doca   , chi2   , apply ) ;
}
// ============================================================================
// make the vertex from  two tracks 
// ============================================================================
const LHCb::RecVertex*
LoKi::Hlt1::VxMakerBase::makeVertex 
( const LHCb::Track* track1 , 
  const LHCb::Track* track2 , 
  double&            doca   , 
  double&            chi2   ,
  const bool         apply  ) const 
{
  //
  doca = LoKi::Constants::NegativeInfinity ;
  chi2 = LoKi::Constants::NegativeInfinity ;
  //
  if ( 0      == track1 || 
       0      == track2 ||
       track1 == track2 )                       { return 0 ; } // RETURN 
  //
  // apply cuts for the first track 
  if ( apply && !track1cut ( track1 ) )         { return 0 ;  } // RETURN
  // apply cuts for the first track 
  if ( apply && !track2cut ( track2 ) )         { return 0 ;  } // RETURN 
  //
  // make vertex 
  // 
  const LHCb::RecVertexHolder holder = LoKi::FastVertex::makeVertex
    ( track1 , track2 , docamax() , chi2max() , doca , chi2 , true ) ;
  //
  if (         !holder    )                     { return 0 ; } // RETURN 
  //
  if ( !vxcut ( holder )  )                     { return 0 ; } // RETURN 
  //
  _store ( holder ) ; // ATTENTION! 
  //
  return holder ;
}
// ============================================================================
// check the consistency of two tracks to have common vertex 
// ============================================================================
bool LoKi::Hlt1::VxMakerBase::checkVertex 
( const LHCb::Track* track1 , 
  const LHCb::Track* track2 , 
  const bool         apply  ) const 
{
  if ( 0 == track1 || 0 == track2     ) { return false ; }
  //
  // apply cuts for thr first track 
  if ( apply && !track1cut ( track1 ) ) { return false ;  }
  // apply cuts for thr first track 
  if ( apply && !track2cut ( track2 ) ) { return false ;  }
  //
  return LoKi::FastVertex::checkDistance 
    ( track1 , track2 , docamax() , chi2max() , true ) ; // FIX ME!!! 
}
// ============================================================================


// ============================================================================
// constructor 
// ============================================================================
LoKi::Hlt1::VxMaker::VxMaker 
( const std::string&                 output  ,   // output selection name/key 
  const LoKi::Hlt1::VxMakerConf&     config  )   //        tool configuration 
  : LoKi::BasicFunctors<const Hlt::Candidate*>::Pipe () 
  , LoKi::Hlt1::VxMakerBase ( config ) 
  , m_sink    ( output  ) 
{}
// ============================================================================
// MANDATORY: virtual desctructor 
// ============================================================================
LoKi::Hlt1::VxMaker::~VxMaker () {}
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::Hlt1::VxMaker* LoKi::Hlt1::VxMaker::clone() const 
{ return new LoKi::Hlt1::VxMaker ( *this ) ; }
// ============================================================================
// the only one important method 
// ============================================================================
LoKi::Hlt1::VxMaker::result_type 
LoKi::Hlt1::VxMaker::operator() 
  ( LoKi::Hlt1::VxMaker::argument a ) const 
{
  //
  Assert ( 0 != alg() ,  "Invalid setup!" ) ;
  //
  typedef result_type                CANDIDATES ;
  typedef CANDIDATES::const_iterator ITERATOR   ;
  //
  if ( a.empty()  ) { return result_type () ; } // no action for EMPTY input   
  // 
  // input selections:
  const CANDIDATES* arg1 = &a  ;
  //
  // the output selection 
  //
  //
  CANDIDATES       output ;
  //
  for ( ITERATOR icand1 = arg1->begin() ; arg1->end() != icand1 ; ++icand1 ) 
  {
    const Hlt::Candidate* cand1 = *icand1 ;
    if ( 0 == cand1 ) { continue ; }                              // CONTINUE 
    //
    const Hlt::MultiTrack* mtrack = cand1->get<Hlt::MultiTrack> () ;
    //
    if ( 0 == mtrack                  ) { continue ; }           // CONTINUE 
    //
    if ( 2 != mtrack->tracks().size() ) { continue ; }           // CONTIUNE 
    //
    const LHCb::Track* trk1 = mtrack->tracks()[0] ;
    if ( 0 == trk1                    ) { continue ; }           // CONITNUE 
    //
    const LHCb::Track* trk2 = mtrack->tracks()[1] ;
    if ( 0 == trk2                    ) { continue ; }           // CONITNUE 
    //
    // make vertex ? 
    //
    double doca = 0 ;
    double chi2 = 0 ;
    const LHCb::RecVertex* vertex = makeVertex 
      ( trk1 , trk2 , doca , chi2 , false ) ;
    //
    if ( 0 == vertex ) { continue ; }                            // CONTINUE 
    //
    // start new candidate:
    //
    Hlt::Candidate* candidate = newCandidate() ;
    candidate -> addToWorkers ( alg() ) ;
    //
    {
      //
      // 1. "initiator" stage - copy the first candidate
      //
      /// get new stage 
      Hlt::Stage*     stage   = newStage  () ;
      candidate -> addToStages ( stage ) ;
      /// lock new stage:
      Hlt::Stage::Lock lock ( stage , alg () ) ;
      //
      lock.addToHistory ( cand1->workers() ) ;
      // lock.addToHistory ( myName()         ) ;
      stage -> set ( cand1->currentStage() ) ;
    }
    //
    {
      //
      // 2. "regular" stage : vertex 
      //
      /// get new stage 
      Hlt::Stage*     stage   = newStage () ;
      candidate -> addToStages ( stage ) ;
      /// lock new stage:
      Hlt::Stage::Lock lock ( stage , alg() ) ;
      // lock.addToHistory ( myName()         ) ;
      stage    -> set ( vertex ) ;
      stage    -> updateInfo ( s_DOCA , doca ) ;
      stage    -> updateInfo ( s_CHI2 , chi2 ) ; 
    }
    // add new candidate to the output:
    output.push_back ( candidate ) ;
    // ========================================================================
  } //                                           end of loop over input data
  // ==========================================================================
  //
  // register the selection in Hlt Data Service 
  return !m_sink ? output : m_sink ( output ) ;                       // RETURN 
  // ==========================================================================
}
// ============================================================================
// OPTIONAL: nice printout 
// ============================================================================
std::ostream& LoKi::Hlt1::VxMaker::fillStream ( std::ostream& s ) const 
{
  return 
    s << "TC_VXMAKE("
      << "'" << output () << "',"
      <<        config () << ")" ;  
}
// ============================================================================


// ============================================================================
// constructor 
// ============================================================================
LoKi::Hlt1::VxMaker2::VxMaker2 
( const std::string&                  output  ,   // output selection name/key 
  const LoKi::Hlt1::VxMaker2::Source& tracks2 ,   // tracks to be matched with 
  const LoKi::Hlt1::VxMakerConf&      config  )   //        tool configuration 
  : LoKi::Hlt1::VxMaker ( output , config )   
  , m_source2 ( tracks2 )
{}
// ============================================================================
// constructor 
// ============================================================================
LoKi::Hlt1::VxMaker2::VxMaker2 
( const std::string&                output  ,   // output selection name/key 
  const std::string&                tracks2 ,   // tracks to be matched with 
  const LoKi::Hlt1::VxMakerConf&    config  )   //        tool configuration 
  : LoKi::Hlt1::VxMaker ( output , config )   
  , m_source2    ( LoKi::Hlt1::Selection ( tracks2 ) ) 
{}
// ============================================================================
// MANDATORY: virtual desctructor 
// ============================================================================
LoKi::Hlt1::VxMaker2::~VxMaker2 () {}
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::Hlt1::VxMaker2* LoKi::Hlt1::VxMaker2::clone() const 
{ return new LoKi::Hlt1::VxMaker2 ( *this ) ; }
// ============================================================================
// the only one important method 
// ============================================================================
LoKi::Hlt1::VxMaker2::result_type 
LoKi::Hlt1::VxMaker2::operator() 
  ( LoKi::Hlt1::VxMaker2::argument a ) const 
{
  //
  Assert ( 0 != alg() ,  "Invalid setup!" ) ;
  //
  typedef result_type                CANDIDATES ;
  typedef CANDIDATES::const_iterator ITERATOR   ;
  //
  if ( a.empty()  ) { return result_type () ; } // no action for EMPTY input   
  // 
  const CANDIDATES a2 =  source() () ;
  if ( a2.empty() ) { return result_type () ; } // no action for EMPTY input 
  //
  // input selections:
  const CANDIDATES* arg1 = &a  ;
  const CANDIDATES* arg2 = &a2 ;
  //
  // the output selection 
  CANDIDATES       output ;
  //
  // swap ?? 
  //
  for ( ITERATOR icand1 = arg1->begin() ; arg1->end() != icand1 ; ++icand1 ) 
  {
    const Hlt::Candidate* cand1 = *icand1 ;
    if ( 0 == cand1 ) { continue ; }                              // CONTINUE 
    const LHCb::Track*    trk1  = cand1->get<LHCb::Track> () ;
    //
    if ( !track1cut ( trk1 ) ) { continue ; }                     // CONTINUE 
    //
    if ( 0 == trk1  ) { continue ; }                              // CONTINUE
    //
    for ( ITERATOR icand2 = arg2->begin() ; arg2->end() != icand2 ; ++icand2 ) 
    {
      const Hlt::Candidate* cand2 = *icand2 ;
      if ( 0 == cand2 ) { continue ; }                            // CONITNUE 
      const LHCb::Track*    trk2  = cand2->get<LHCb::Track> () ;
      if ( 0 == trk2  ) { continue ; }                            // CONTINUE 
      //
      if ( !track2cut ( trk2 ) ) { continue ; }                   // CONTINUE 
      //
      // make vertex ? 
      //
      double doca = 0 ;
      double chi2 = 0 ;
      const LHCb::RecVertex* vertex = 
        makeVertex ( trk1 , trk2 , doca , chi2 , false ) ;
      //
      if ( 0 == vertex ) { continue ; }                            // CONTINUE 
      //
      // start new candidate:
      Hlt::Candidate* candidate = newCandidate() ;
      candidate -> addToWorkers ( alg() ) ;
      //
      {
        //
        // 1. "initiator" stage - copy the first candidate
        //
        /// get new stage 
        Hlt::Stage*     stage   = newStage     () ;
        candidate -> addToStages ( stage ) ;
        /// lock new stage:
        Hlt::Stage::Lock lock ( stage , alg () ) ;
        //
        lock.addToHistory ( cand1->workers() ) ;
        // lock.addToHistory ( myName()         ) ;
        stage -> set ( cand1->currentStage() ) ;
      }
      //
      {
        //
        // 2. "regular" stage : vertex 
        //
        /// get new stage 
        Hlt::Stage*     stage   = newStage     () ;
        candidate -> addToStages ( stage ) ;
        /// lock new stage:
        Hlt::Stage::Lock lock ( stage , alg() ) ;
        // lock.addToHistory ( myName()         ) ;
        stage    -> set ( vertex ) ;
        stage    -> updateInfo ( s_DOCA , doca ) ;
        stage    -> updateInfo ( s_CHI2 , chi2 ) ; 
      }
      // add new candidate to the output:
      output.push_back ( candidate ) ;
    } //                                         end of loop over source_2 
    // ========================================================================
  } //                                           end of loop over input data
  // ==========================================================================
  //
  // register the selection in Hlt Data Service 
  return !m_sink ? output : m_sink ( output ) ;                       // RETURN 
  // ==========================================================================
}
// ============================================================================
// OPTIONAL: nice printout 
// ============================================================================
std::ostream& LoKi::Hlt1::VxMaker2::fillStream ( std::ostream& s ) const 
{
  return 
    s << "TC_VXMAKE2("
      << "'" << output () << "',"
      <<        source () << "," 
      <<        config () << ")" ;  
}
// ============================================================================


// ============================================================================
// constructor 
// ============================================================================
LoKi::Hlt1::VxMaker3::VxMaker3 
( const std::string&                   output  ,  // output selection name/key 
  const LoKi::Hlt1::VxMaker3::Source&  tracks1 ,  //       the first selection 
  const LoKi::Hlt1::VxMaker3::Source&  tracks2 ,  //       the first selection 
  const LoKi::Hlt1::VxMakerConf&       config  )  //        tool configuration 
  : LoKi::BasicFunctors<const Hlt::Candidate*>::Source () 
  , m_source1 ( tracks1 ) 
  , m_vxmaker ( output , tracks2 , config ) 
{}
// ============================================================================
// MANTATORY: virtual destructor 
// ============================================================================
LoKi::Hlt1::VxMaker3::~VxMaker3(){}
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::Hlt1::VxMaker3* LoKi::Hlt1::VxMaker3::clone() const 
{ return new LoKi::Hlt1::VxMaker3 ( *this ) ; }
// ============================================================================
// the only one important method 
// ============================================================================
LoKi::Hlt1::VxMaker3::result_type 
LoKi::Hlt1::VxMaker3::operator() ( /* LoKi::Hlt1::VxMaker3::argument */ ) const 
{ return m_vxmaker ( m_source1 ()  ) ; }
// ============================================================================
// OPTIONAL: nice printout 
// ============================================================================
std::ostream& LoKi::Hlt1::VxMaker3::fillStream ( std::ostream& s ) const 
{
  return 
    s << "TC_VXMAKE3("
      << "'" << m_vxmaker.output  () << "',"
      <<        source1           () << "," 
      <<        source2           () << "," 
      <<        m_vxmaker.config  () << ")" ;  
}
// ============================================================================

// ============================================================================
// constructor 
// ============================================================================
LoKi::Hlt1::DiTrackMaker::DiTrackMaker 
( const std::string&                       output  ,   // output selection name/key 
  const LoKi::Hlt1::DiTrackMaker::Source&  tracks2 ,   // tracks to be matched with 
  const LoKi::Hlt1::VxMakerConf&           config  )   //        tool configuration 
  : LoKi::BasicFunctors<const Hlt::Candidate*>::Pipe () 
  , LoKi::Hlt1::VxMakerBase ( config ) 
  , m_source2 ( tracks2 ) 
  , m_sink    ( output  ) 
{}
// ============================================================================
// constructor 
// ============================================================================
LoKi::Hlt1::DiTrackMaker::DiTrackMaker 
( const std::string&                       output  ,   // output selection name/key 
  const std::string&                       tracks2 ,   // tracks to be matched with 
  const LoKi::Hlt1::VxMakerConf&           config  )   //        tool configuration 
  : LoKi::BasicFunctors<const Hlt::Candidate*>::Pipe () 
  , LoKi::Hlt1::VxMakerBase ( config ) 
  , m_source2    ( LoKi::Hlt1::Selection ( tracks2 ) ) 
  , m_sink       ( output  ) 
{}
// ============================================================================
// MANDATORY: virtual desctructor 
// ============================================================================
LoKi::Hlt1::DiTrackMaker::~DiTrackMaker () {}
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::Hlt1::DiTrackMaker* LoKi::Hlt1::DiTrackMaker::clone() const 
{ return new LoKi::Hlt1::DiTrackMaker ( *this ) ; }
// ============================================================================
// the only one important method 
// ============================================================================
LoKi::Hlt1::DiTrackMaker::result_type 
LoKi::Hlt1::DiTrackMaker::operator() 
  ( LoKi::Hlt1::DiTrackMaker::argument a ) const 
{
  //
  Assert ( 0 != alg() ,  "Invalid setup!" ) ;
  //
  //
  Assert ( 0 != alg() ,  "Invalid setup!" ) ;
  //
  typedef result_type                CANDIDATES ;
  typedef CANDIDATES::const_iterator ITERATOR   ;
  //
  if ( a.empty()  ) { return result_type () ; } // no action for EMPTY input   
  // 
  const CANDIDATES a2 =  source() () ;
  if ( a2.empty() ) { return result_type () ; } // no action for EMPTY input 
  //
  // input selections:
  const CANDIDATES* arg1 = &a  ;
  const CANDIDATES* arg2 = &a2 ;
  //
  // the output selection 
  CANDIDATES       output ;
  //
  // swap ?? 
  //
  for ( ITERATOR icand1 = arg1->begin() ; arg1->end() != icand1 ; ++icand1 ) 
  {
    const Hlt::Candidate* cand1 = *icand1 ;
    if ( 0 == cand1 ) { continue ; }                              // CONTINUE 
    const LHCb::Track*    trk1  = cand1->get<LHCb::Track> () ;
    //
    if ( !track1cut ( trk1 ) ) { continue ; }                     // CONTINUE 
    //
    if ( 0 == trk1  ) { continue ; }                              // CONTINUE
    //
    for ( ITERATOR icand2 = arg2->begin() ; arg2->end() != icand2 ; ++icand2 ) 
    {
      const Hlt::Candidate* cand2 = *icand2 ;
      if ( 0 == cand2 ) { continue ; }                            // CONITNUE 
      const LHCb::Track*    trk2  = cand2->get<LHCb::Track> () ;
      if ( 0 == trk2  ) { continue ; }                            // CONTINUE 
      //
      if ( !track2cut ( trk2 ) ) { continue ; }                   // CONTINUE 
      //
      // check the distance 
      if ( !checkVertex ( trk1 , trk2 , false ) ) { continue ; }  // CONTINUE 
      //
      // start new candidate:
      Hlt::Candidate* candidate = newCandidate() ;
      candidate -> addToWorkers ( alg() ) ;
      //      
      {
        //
        // 1. "initiator" stage - copy the first candidate
        //
        /// get new stage 
        Hlt::Stage*     stage   = newStage     () ;
        candidate -> addToStages ( stage ) ;
        /// lock new stage:
        Hlt::Stage::Lock lock ( stage , alg () ) ;
        //
        lock.addToHistory ( cand1->workers() ) ;
        // lock.addToHistory ( myName()         ) ;
        stage -> set ( cand1->currentStage() ) ;
      }
      //
      {
        //
        // 2. "regular" stage - create di-track
        //
        /// get new stage 
        Hlt::Stage*     stage   = newStage     () ;
        candidate -> addToStages ( stage ) ;
        /// lock new stage:
        Hlt::Stage::Lock lock ( stage , alg () ) ;
        // lock.addToHistory ( myName()         ) ;
        //
        Hlt::MultiTrack* mtrack = newMultiTrack() ;
        mtrack -> addToTracks ( trk1 ) ;
        mtrack -> addToTracks ( trk2 ) ;
        stage  -> set ( mtrack ) ;
      }
      // 
      // add new candidate to the output:
      output.push_back ( candidate ) ;
    } //                                         end of loop over source_2 
    // ========================================================================
  } //                                           end of loop over input data
  // ==========================================================================
  //
  // register the selection in Hlt Data Service 
  return !m_sink ? output : m_sink ( output ) ;                       // RETURN
  // ==========================================================================
}
// ============================================================================
// OPTIONAL: nice printout 
// ============================================================================
std::ostream& LoKi::Hlt1::DiTrackMaker::fillStream ( std::ostream& s ) const 
{
  return 
    s << "TC_DITRACKS("
      << "'" << output () << "',"
      <<        source () << "," 
      <<        config () << ")" ;  
}
// ============================================================================




  
  // ============================================================================
  // LoKi::Hlt1::VxMaker::result_type 
// LoKi::Hlt1::VxMaker::operator() () const 
// {
//   typedef TrSource::result_type      Tracks   ;
//   typedef LoKi::Combiner_<Tracks>    Combiner ;
//   typedef Combiner::Range            TRange   ;
//   typedef LHCb::RecVertex::Container Vertices ;
//   typedef result_type                Output   ;
  
//   Output output ;
//   output.reserve ( 50 ) ;
  
//   if ( 0 == m_alg ) { init() ;} 
//   Assert ( 0 != m_alg , "GaudiAlgorithm* points to NULL!" ) ;
  
//   Vertices* overtices = new Vertices() ;
//   m_alg->put ( overtices , "Hlt1/Vertex/" + m_sink.selName() ) ;
  
  
//   // get the first set of tracks
//   Tracks track1 = m_track1() ;
  
//   // get the second set of tracks
//   Tracks track2 ;
//   if ( !m_same ) { track2 = m_track2() ; }
  
//   const bool same = m_same || track1 == track2 ;
  
//   const TRange range1 ( track1 ) ;
//   const TRange range2 ( same ? track1 : track2 ) ;
  
//   const Hlt::VertexCreator creator ;
  
//   /// create the combiner & fill it with data 
//   Combiner loop ;
//   loop.add ( range1 ) ;
//   loop.add ( range2 ) ;
  
//   // make the combinations 
//   for ( ; loop.valid() ; loop.next() ) 
//   {
//     // get the current combination 
//     Tracks tracks ( 2 ) ;
//     loop.current ( tracks.begin() ) ;
    
//     const LHCb::Track* first  = tracks[0] ;
//     const LHCb::Track* second = tracks[1] ;
    
//     // skip invalid 
//     if ( 0 == first || 0 == second ) { continue ; }              // CONTINUE 
//     // skip the same  
//     if ( first == second           ) { continue ; }              // CONTINUE 
//     // reduce the double count :
//     if ( same && first >= second   ) { continue ; }              // CONTINUE

//     // chech the overrlap 
//     if ( HltUtils::matchIDs ( *first , *second ) ) { continue ; }// CONTINUE 
    
//     // apply the cuts on the combination of two tracks:
//     if ( !m_cut2tr_trivial && !m_cut2tr ( *first , *second ) ) { continue ;  }
    
//     // create the vertex 
//     std::auto_ptr<LHCb::RecVertex> vertex ( new LHCb::RecVertex() ) ;
    
//     /// fill it with some information 
//     creator ( *first , *second , *vertex ) ;
    
//     // apply cuts on the vertex 
//     if ( !m_cut4rv_trivial && !m_cut4rv ( *vertex ) ) { continue ; } // CONTINUE
    
//     // good vertex! add it to the seelction! 
//     output.push_back ( vertex.get() ) ;    // "vertex is not valid anymore 
//     overtices->insert( vertex.release() ); // this is in the TES and assumes ownership
   
//   }

//   // use "sink": register object for Hlt Data Service 
//   return m_sink ( output )  ; // RETURN 
// }



// ============================================================================
// The END 
// ============================================================================
