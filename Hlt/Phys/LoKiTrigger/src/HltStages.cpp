// $Id$
// ============================================================================
// $URL$
// ============================================================================
// Include files 
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/ToStream.h"
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/TriggerTypes.h"
#include "LoKi/HltStages.h"
#include "LoKi/Constants.h"
// ============================================================================
/** @file
 *  Implementation for Hlt-Stages functors 
 * 
 *  This file is part of LoKi project: 
 *   ``C++ ToolKit for Smart and Friendly Physics Analysis''
 * 
 *  The package has been designed with the kind help from
 *  Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas, 
 *  contributions and advices from G.Raven, J.van Tilburg, 
 *  A.Golutvin, P.Koppenburg have been used in the design.
 *
 *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
 *  @date 2010-08-01
 *
 *  @see LoKi::TriggerTypes 
 *  @see Hlt::Candidate 
 *
 *  $Revision$
 *  Last Modification $Date$ 
 *                 by $Author$ 
 */
// ============================================================================
// MANDATORY: virtual destructor 
// ============================================================================
LoKi::Stages::IsTrack::~IsTrack(){}
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::Stages::IsTrack*
LoKi::Stages::IsTrack::clone() const 
{ return new LoKi::Stages::IsTrack(*this) ; }
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::Stages::IsTrack::result_type 
LoKi::Stages::IsTrack::operator() 
  ( LoKi::Stages::IsTrack::argument a ) const 
{
  if ( !a ) { Error ("Hlt::Stage* points to NULL, return false " ) ; }
  return a ? a->is<LHCb::Track>() : false ;
}
// ============================================================================
// OPTIONAL: the ince printout 
// ============================================================================
std::ostream& LoKi::Stages::IsTrack::fillStream ( std::ostream& s ) const 
{ return s << "TS_TRACK" ; }


// ============================================================================
// MANDATORY: virtual destructor 
// ============================================================================
LoKi::Stages::IsL0Muon::~IsL0Muon(){}
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::Stages::IsL0Muon*
LoKi::Stages::IsL0Muon::clone() const 
{ return new LoKi::Stages::IsL0Muon(*this) ; }
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::Stages::IsL0Muon::result_type 
LoKi::Stages::IsL0Muon::operator() 
  ( LoKi::Stages::IsL0Muon::argument a ) const 
{
  if ( !a ) { Error ("Hlt::Stage points to NULL, return false " ) ; }
  return a ? a->is<LHCb::L0MuonCandidate>() : false ;
}
// ============================================================================
// OPTIONAL: the ince printout 
// ============================================================================
std::ostream& LoKi::Stages::IsL0Muon::fillStream ( std::ostream& s ) const 
{ return s << "TS_L0MUON" ; }


// ============================================================================
// MANDATORY: virtual destructor 
// ============================================================================
LoKi::Stages::IsL0DiMuon::~IsL0DiMuon(){}
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::Stages::IsL0DiMuon*
LoKi::Stages::IsL0DiMuon::clone() const 
{ return new LoKi::Stages::IsL0DiMuon(*this) ; }
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::Stages::IsL0DiMuon::result_type 
LoKi::Stages::IsL0DiMuon::operator() 
  ( LoKi::Stages::IsL0DiMuon::argument a ) const 
{
  if ( !a ) { Error ("Hlt::Stage points to NULL, return false " ) ; }
  return a ? a->is<Hlt::L0DiMuonCandidate>() : false ;
}
// ============================================================================
// OPTIONAL: the ince printout 
// ============================================================================
std::ostream& LoKi::Stages::IsL0DiMuon::fillStream ( std::ostream& s ) const 
{ return s << "TS_L0DIMUON" ; }


// ============================================================================
// MANDATORY: virtual destructor 
// ============================================================================
LoKi::Stages::IsL0Calo::~IsL0Calo(){}
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::Stages::IsL0Calo*
LoKi::Stages::IsL0Calo::clone() const 
{ return new LoKi::Stages::IsL0Calo(*this) ; }
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::Stages::IsL0Calo::result_type 
LoKi::Stages::IsL0Calo::operator() 
  ( LoKi::Stages::IsL0Calo::argument a ) const 
{
  if ( !a ) { Error ("Hlt::Stage points to NULL, return false " ) ; }
  return a ? a->is<LHCb::L0CaloCandidate>() : false ;
}
// ============================================================================
// OPTIONAL: the ince printout 
// ============================================================================
std::ostream& LoKi::Stages::IsL0Calo::fillStream ( std::ostream& s ) const 
{ return s << "TS_L0CALO" ; }
// ============================================================================

// ============================================================================
// MANDATORY: virtual destructor 
// ============================================================================
LoKi::Stages::IsVertex::~IsVertex (){}
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::Stages::IsVertex*
LoKi::Stages::IsVertex::clone() const 
{ return new LoKi::Stages::IsVertex(*this) ; }
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::Stages::IsVertex::result_type 
LoKi::Stages::IsVertex::operator() 
  ( LoKi::Stages::IsVertex::argument a ) const 
{
  if ( !a ) { Error ("Hlt::Stage points to NULL, return false " ) ; }
  return a ? a->is<LHCb::RecVertex>() : false ;
}
// ============================================================================
// OPTIONAL: the ince printout 
// ============================================================================
std::ostream& LoKi::Stages::IsVertex::fillStream ( std::ostream& s ) const 
{ return s << "TS_VERTEX" ; }
// ============================================================================



// ============================================================================
// MANDATORY: virtual destructor 
// ============================================================================
LoKi::Stages::IsMultiTrack::~IsMultiTrack (){}
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::Stages::IsMultiTrack*
LoKi::Stages::IsMultiTrack::clone() const 
{ return new LoKi::Stages::IsMultiTrack(*this) ; }
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::Stages::IsMultiTrack::result_type 
LoKi::Stages::IsMultiTrack::operator() 
  ( LoKi::Stages::IsMultiTrack::argument a ) const 
{
  if ( !a ) { Error ("Hlt::Stage points to NULL, return false " ) ; }
  return a ? a->is<Hlt::MultiTrack>() : false ;
}
// ============================================================================
// OPTIONAL: the ince printout 
// ============================================================================
std::ostream& LoKi::Stages::IsMultiTrack::fillStream ( std::ostream& s ) const 
{ return s << "TS_MULTITRACK" ; }
// ============================================================================


// ============================================================================
// MANDATORY: virtual destructor 
// ============================================================================
LoKi::Stages::IsStage::~IsStage (){}
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::Stages::IsStage*
LoKi::Stages::IsStage::clone() const 
{ return new LoKi::Stages::IsStage(*this) ; }
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::Stages::IsStage::result_type 
LoKi::Stages::IsStage::operator() 
  ( LoKi::Stages::IsStage::argument a ) const 
{
  if ( !a ) { Error ("Hlt::Stage points to NULL, return false " ) ; }
  return a ? a->is<Hlt::Stage>() : false ;
}
// ============================================================================
// OPTIONAL: the nice printout 
// ============================================================================
std::ostream& LoKi::Stages::IsStage::fillStream ( std::ostream& s ) const 
{ return s << "TS_STRAGE" ; }
// ============================================================================


// ============================================================================
// MANDATORY: virtual destructor 
// ============================================================================
LoKi::Stages::Type::~Type(){}
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::Stages::Type*
LoKi::Stages::Type::clone() const 
{ return new LoKi::Stages::Type(*this) ; }
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::Stages::Type::result_type 
LoKi::Stages::Type::operator() 
  ( LoKi::Stages::Type::argument a ) const 
{
  if ( !a ) 
  { 
    Error ("Hlt::Stage points to NULL, return Unknown" ) ; 
    return Hlt::Stage::Unknown ;
  }
  return a->stageType() ;
}
// ============================================================================
// OPTIONAL: the ince printout 
// ============================================================================
std::ostream& LoKi::Stages::Type::fillStream ( std::ostream& s ) const 
{ return s << "TS_TYPE" ; }
// ============================================================================



// ============================================================================
// constructor 
// ============================================================================
LoKi::Stages::TrFun::TrFun
( const LoKi::TrackTypes::TrFunc& fun , 
  double                          bad )
  : LoKi::BasicFunctors<const Hlt::Stage*>::Function () 
  , m_fun { fun } 
  , m_bad { bad } 
{}
// ============================================================================
// constructor 
// ============================================================================
LoKi::Stages::TrFun::TrFun
( const LoKi::TrackTypes::TrFunc& fun )
  : LoKi::BasicFunctors<const Hlt::Stage*>::Function () 
  , m_fun ( fun ) 
  , m_bad ( LoKi::Constants::NegativeInfinity ) 
{}
// ============================================================================
// MANDATORY: virtual destructor 
// ============================================================================
LoKi::Stages::TrFun::~TrFun(){}
// ============================================================================
// MANDATORY: clone method ("virtual constructor") 
// ============================================================================
LoKi::Stages::TrFun*
LoKi::Stages::TrFun::clone() const 
{ return new LoKi::Stages::TrFun ( *this ) ; }
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::Stages::TrFun::result_type 
LoKi::Stages::TrFun::operator() 
  ( LoKi::Stages::TrFun::argument a ) const
{
  //
  if ( !a ) 
  { 
    Error ("Hlt::Stage points to NULL, return 'bad'" ) ; 
    return m_bad ;
  }
  //
  auto track = a->get<LHCb::Track>() ;  
  if ( !track ) 
  { 
    Error ("LHCb::Track* points to NULL, return 'bad'" ) ; 
    return m_bad ;
  }
  //
  return m_fun ( track ) ;
}
// ============================================================================
// OPTIONAL: the ince printout 
// ============================================================================
std::ostream& LoKi::Stages::TrFun::fillStream ( std::ostream& s ) const 
{
  s << " TS_TrFUN(" << m_fun ;
  if ( LoKi::Constants::NegativeInfinity != m_bad ) 
  { s << "," << m_bad ; }
  return s << ") " ;
}
// ============================================================================



// ============================================================================
// constructor 
// ============================================================================
LoKi::Stages::TrCut::TrCut
( const LoKi::TrackTypes::TrCuts& fun , 
  const bool                      bad )
  : LoKi::BasicFunctors<const Hlt::Stage*>::Predicate () 
  , m_fun ( fun ) 
  , m_bad ( bad ) 
{}
// ============================================================================
// constructor 
// ============================================================================
LoKi::Stages::TrCut::TrCut
( const LoKi::TrackTypes::TrCuts& fun )
  : LoKi::BasicFunctors<const Hlt::Stage*>::Predicate () 
  , m_fun ( fun   ) 
  , m_bad ( false ) 
{}
// ============================================================================
// MANDATORY: virtual destructor 
// ============================================================================
LoKi::Stages::TrCut::~TrCut(){}
// ============================================================================
// MANDATORY: clone method ("virtual constructor") 
// ============================================================================
LoKi::Stages::TrCut*
LoKi::Stages::TrCut::clone() const 
{ return new LoKi::Stages::TrCut ( *this ) ; }
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::Stages::TrCut::result_type 
LoKi::Stages::TrCut::operator() 
  ( LoKi::Stages::TrCut::argument a ) const
{
  //
  if ( !a ) 
  { 
    Error ("Hlt::Stage points to NULL, return 'bad'" ) ; 
    return m_bad ;
  }
  //
  const LHCb::Track* track = a->get<LHCb::Track>() ;  
  if ( !track ) 
  { 
    Error ("LHCb::Track* points to NULL, return 'bad'" ) ; 
    return m_bad ;
  }
  //
  return m_fun ( track ) ;
}
// ============================================================================
// OPTIONAL: the ince printout 
// ============================================================================
std::ostream& LoKi::Stages::TrCut::fillStream ( std::ostream& s ) const 
{
  s << " TS_TrCut(" << m_fun ;
  if ( m_bad ) { s << ",True" ; }
  return s << ") " ;
}
// ============================================================================


 

// ============================================================================
// MANDATORY: virtual destructor 
// ============================================================================
LoKi::Stages::Locked::~Locked(){}
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::Stages::Locked*
LoKi::Stages::Locked::clone() const 
{ return new LoKi::Stages::Locked(*this) ; }
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::Stages::Locked::result_type 
LoKi::Stages::Locked::operator() 
  ( LoKi::Stages::Locked::argument a ) const 
{
  if ( !a ) 
  {
    Error ("Hlt::Stage* points to NULL, return false " ) ; 
    return false ;
  }
  return 0 != a->locked() ;
}
// ============================================================================
// OPTIONAL: the ince printout 
// ============================================================================
std::ostream& LoKi::Stages::Locked::fillStream ( std::ostream& s ) const
{ return s << "TS_LOCKED" ; }




// ============================================================================
// constructot from the algorithm name
// ============================================================================
LoKi::Stages::History::History ( std::string alg ) 
  :  LoKi::BasicFunctors<const Hlt::Stage*>::Predicate () 
  , m_algorithm ( std::move(alg) ) 
{}
// ============================================================================
// MANDATORY: virtual destructor 
// ============================================================================
LoKi::Stages::History::~History(){}
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::Stages::History*
LoKi::Stages::History::clone() const 
{ return new LoKi::Stages::History(*this) ; }
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::Stages::History::result_type 
LoKi::Stages::History::operator() 
  ( LoKi::Stages::History::argument a ) const 
{
  if ( !a ) 
  {
    Error ("Hlt::Stage* points to NULL, return false " ) ; 
    return false ;
  }
  auto& history = a->history() ;
  return history.end() != std::find ( history.begin () , 
                                      history.end   () , 
                                      m_algorithm      ) ;
}
// ============================================================================
// OPTIONAL: the ince printout 
// ============================================================================
std::ostream& LoKi::Stages::History::fillStream ( std::ostream& s ) const 
{ 
  s << " TS_HISTORY(" ; 
  Gaudi::Utils::toStream ( m_algorithm , s ) ;
  return s << ") " ;
}

// ============================================================================
// constructor from the algorithm name
// ============================================================================
LoKi::Stages::HistorySub::HistorySub ( const std::string alg ) 
  :  LoKi::Stages::History ( std::move(alg) ) 
{}
// ============================================================================
// MANDATORY: virtual destructor 
// ============================================================================
LoKi::Stages::HistorySub::~HistorySub () {}
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::Stages::HistorySub*
LoKi::Stages::HistorySub::clone() const 
{ return new LoKi::Stages::HistorySub(*this) ; }
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::Stages::HistorySub::result_type 
LoKi::Stages::HistorySub::operator() 
  ( LoKi::Stages::HistorySub::argument a ) const 
{
  if ( !a ) 
  {
    Error ("Hlt::Stage* points to NULL, return false " ) ; 
    return false ;
  }
  for ( const auto& item : a->history() ) 
  {
    if ( m_algorithm.size() <= item.size() && 
         std::string::npos != item.find ( m_algorithm ) ) { return true ; }
  }
  return false ;
}
// ============================================================================
// OPTIONAL: the ince printout 
// ============================================================================
std::ostream& LoKi::Stages::HistorySub::fillStream ( std::ostream& s ) const 
{ 
  s << " TS_HISTORY_SUB(" ; 
  Gaudi::Utils::toStream ( m_algorithm , s ) ;
  return s << ") " ;
}


// ============================================================================
// constructor from the algorithm name
// ============================================================================
LoKi::Stages::HistoryRegex::HistoryRegex ( const std::string& alg ) 
  :LoKi::Stages::HistorySub ( alg ) 
  , m_expression ( alg ) 
{}
// ============================================================================
// MANDATORY: virtual destructor 
// ============================================================================
LoKi::Stages::HistoryRegex::~HistoryRegex () {}
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::Stages::HistoryRegex*
LoKi::Stages::HistoryRegex::clone() const 
{ return new LoKi::Stages::HistoryRegex(*this) ; }
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::Stages::HistoryRegex::result_type 
LoKi::Stages::HistoryRegex::operator() 
  ( LoKi::Stages::HistoryRegex::argument a ) const 
{
  if ( !a ) 
  {
    Error ("Hlt::Stage* points to NULL, return false " ) ; 
    return false ;
  }
  //
  for ( const auto& item : a->history() )
  {
    if ( boost::regex_match ( item.str() , m_expression ) ) { return true ; }
  }
  //
  return false ;
}
// ============================================================================
// OPTIONAL: the ince printout 
// ============================================================================
std::ostream& LoKi::Stages::HistoryRegex::fillStream ( std::ostream& s ) const 
{ 
  s << " TS_HISTORY_RE(" ; 
  Gaudi::Utils::toStream ( m_algorithm , s ) ;
  return s << ") " ;
}



// ============================================================================
// constructor from the key and data type
// ============================================================================
LoKi::Stages::HasCache::HasCache
( std::string        key , 
  Hlt::Cache::Values typ ) 
  : LoKi::BasicFunctors<const Hlt::Stage*>::Predicate () 
  , m_key { std::move(key) }
  , m_typ { typ } 
{}
// ============================================================================
// MANDATORY: virtual destructor 
// ============================================================================
LoKi::Stages::HasCache::~HasCache(){}
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::Stages::HasCache*
LoKi::Stages::HasCache::clone() const 
{ return new LoKi::Stages::HasCache ( *this ) ; }
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::Stages::HasCache::result_type 
LoKi::Stages::HasCache::operator() 
  ( LoKi::Stages::HasCache::argument a ) const 
{
  if ( !a ) 
  {
    Error ("Hlt::Stage* points to NULL, return false " ) ; 
    return false ;
  }
  //
  if       ( Hlt::Cache::Bool    == m_typ )  
  { return a -> hasInfo_< Hlt::CacheValues<Hlt::Cache::Bool >::Type>( m_key ) ; }
  else if  ( Hlt::Cache::Int     == m_typ )  
  { return a -> hasInfo_<Hlt::CacheValues<Hlt::Cache::Int   >::Type>( m_key ) ; }
  else if  ( Hlt::Cache::Double  == m_typ )  
  { return a -> hasInfo_<Hlt::CacheValues<Hlt::Cache::Double>::Type>( m_key ) ; }
  else if  ( Hlt::Cache::String  == m_typ )  
  { return a -> hasInfo_<Hlt::CacheValues<Hlt::Cache::String>::Type>( m_key ) ; }
  //
  Error ("The Invalid type has been specified, return 'false'") ;
  return false ;
}
// ============================================================================
// OPTIONAL: the nice printout 
// ============================================================================
std::ostream& LoKi::Stages::HasCache::fillStream ( std::ostream& s ) const 
{
  s << "TS_HASCACHE('" << m_key << "'" ;
  if      ( Hlt::Cache::Bool   == m_typ ) { return s << ",Hlt.Cache.Bool)"   ; }
  else if ( Hlt::Cache::Int    == m_typ ) { return s << ",Hlt.Cache.Int)"    ; }
  else if ( Hlt::Cache::Double == m_typ ) { return s << ",Hlt.Cache.Double)" ; }
  else if ( Hlt::Cache::String == m_typ ) { return s << ",Hlt.Cache.String)" ; }
  //
  return s << ")" ;
}


// ============================================================================
// constructor from the key and data type
// ============================================================================
LoKi::Stages::Cache1::Cache1
( std::string  key , 
  double       def ) 
  : LoKi::BasicFunctors<const Hlt::Stage*>::Function()
  , m_key { std::move(key) } 
  , m_def { def }
{}
// ============================================================================
// MANDATORY: virtual destructor 
// ============================================================================
LoKi::Stages::Cache1::~Cache1(){}
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::Stages::Cache1*
LoKi::Stages::Cache1::clone() const 
{ return new LoKi::Stages::Cache1 ( *this ) ; }
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::Stages::Cache1::result_type 
LoKi::Stages::Cache1::operator() 
  ( LoKi::Stages::Cache1::argument a ) const 
{
  if ( !a ) 
  {
    Error ("Hlt::Stage* points to NULL, return default" ) ; 
    return m_def ;
  }
  //
  return a -> info ( m_key , m_def ) ;
}
// ============================================================================
// OPTIONAL: the ince printout 
// ============================================================================
std::ostream& LoKi::Stages::Cache1::fillStream ( std::ostream& s ) const 
{ return s << " TS_CACHE_DOUBLE('" << m_key << "," << m_def << ")" ; }

// ============================================================================
// constructor from the key and data type
// ============================================================================
LoKi::Stages::Cache2::Cache2
( std::string  key , 
  bool         def ) 
  : LoKi::BasicFunctors<const Hlt::Stage*>::Predicate()
  , m_key { std::move(key) }
  , m_def { def }
{}
// ============================================================================
// MANDATORY: virtual destructor 
// ============================================================================
LoKi::Stages::Cache2::~Cache2(){}
// ============================================================================
// MANDATORY: clone method ("virtual constructor")
// ============================================================================
LoKi::Stages::Cache2*
LoKi::Stages::Cache2::clone() const 
{ return new LoKi::Stages::Cache2 ( *this ) ; }
// ============================================================================
// MANDATORY: the only one essential method 
// ============================================================================
LoKi::Stages::Cache2::result_type 
LoKi::Stages::Cache2::operator() 
  ( LoKi::Stages::Cache2::argument a ) const 
{
  if ( !a ) 
  {
    Error ("Hlt::Stage* points to NULL, return default" ) ; 
    return m_def ;
  }
  //
  return a -> info ( m_key , m_def ) ;
}
// ============================================================================
// OPTIONAL: the ince printout 
// ============================================================================
std::ostream& LoKi::Stages::Cache2::fillStream ( std::ostream& s ) const 
{ return s << " TS_CACHE_BOOL('" << m_key << "," << m_def << ")" ; }
// ============================================================================




// ==================================================================
//  constructor from the predicate and the fake integer argument 
// ==================================================================
LoKi::Stages::Cut_<Hlt::MultiTrack>::Cut_ 
( const LoKi::BasicFunctors<const LHCb::Track*>::CutVal& cut )
  : LoKi::BasicFunctors<const Hlt::Stage*>::Predicate ()
  , m_cut  ( cut  )
{}



// ============================================================================
/*  perform the actual evaluation 
 *  @attention predicate is appleid only for VALID stages
 *  @param stage (INPUT) the input stage 
 *  @return result of predicate evaluation for VALID stages
 */
// ============================================================================
bool LoKi::Stages::Cut_<Hlt::MultiTrack>::filterStage 
( const Hlt::Stage* stage ) const 
{
  //
  if ( !stage ) 
  {
    Error ( "Invalid Stage, return false" ) ;
    return false ;                                         // RETURN 
  }
  //
  // get the object from the stage 
  //
  auto obj = stage->get<Hlt::MultiTrack>() ;
  //
  if ( !obj ) 
  {
    Error ( "Hlt::Stage is NOT Hlt::MultiTrack, return false" ) ;
    return false ;                                         // RETURN 
    
  }
  //
  return m_cut.fun ( LHCb::Track::ConstVector{ obj->tracks().begin() , 
                                               obj->tracks().end  () } ) ;
} 
// ============================================================================
// OPTIONAL: the ince printout 
// ============================================================================
std::ostream& 
LoKi::Stages::Cut_<Hlt::MultiTrack>::fillStream ( std::ostream& s ) const 
{ return s << m_cut ; }
// ============================================================================

// ============================================================================
// constructor from the function and "bad"-value 
// ============================================================================
LoKi::Stages::Fun_<Hlt::MultiTrack>::Fun_
( const LoKi::BasicFunctors<const LHCb::Track*>::FunVal& fun , 
  const double                                           bad )
  : LoKi::BasicFunctors<const Hlt::Stage*>::Function ()
  , m_fun  ( fun )
  , m_bad  ( bad )
{}
// ============================================================================
/*  perform the actual evaluation 
 *  @attention predicate is appleid only for VALID stages
 *  @param stage (INPUT) the input stage 
 *  @return result of predicate evaluation for VALID stages
 */
// ============================================================================
double LoKi::Stages::Fun_<Hlt::MultiTrack>::evalStage 
( const Hlt::Stage* stage ) const 
{
  //
  if ( !stage ) 
  {
    Error ( "Invalid Stage, return 'bad'" ) ;
    return m_bad ;                                         // RETURN 
  }
  //
  // get the object from the stage 
  //
  auto obj = stage->get<Hlt::MultiTrack>() ;
  //
  if ( !obj ) 
  {
    Error ( "Hlt::Stage is NOT Hlt::MultiTrack, return 'bad'" ) ;
    return m_bad ;                                         // RETURN 
    
  }
  //
  return m_fun.fun ( LHCb::Track::ConstVector { obj->tracks().begin() , 
                                                obj->tracks().end  () } ) ;
}
// ============================================================================
// OPTIONAL: the ince printout 
// ============================================================================
std::ostream& 
LoKi::Stages::Fun_<Hlt::MultiTrack>::fillStream ( std::ostream& s ) const 
{ return s << m_fun ; }
// ============================================================================


// ============================================================================
// The END 
// ============================================================================
