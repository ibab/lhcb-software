// $Id: HltBindersDict.cpp,v 1.3 2008-11-18 09:07:29 ibelyaev Exp $
// ============================================================================
// Include files 
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/HltBinders.h"
#include "LoKi/Hlt1Binders.h"
#include "LoKi/HltBindersDicts.h"
// ============================================================================
/** @file
 *  Implementation file for class LoKi::Dicts::HltBind
 *  @author Vanya BELYAEV ibelayev@physics.syr.edu
 *  @date 2007-08-19 
 */
// ============================================================================
// bindMin:  Track-Track
// ============================================================================
LoKi::Types::TrFun 
LoKi::Dicts::HltBind::bindMin  
( const LoKi::Types::TTrFunc&      fun  , 
  const std::vector<LHCb::Track*>* vct  ,
  const std::string&               nam2 ) 
{
  return LoKi::HltBinders::bindMin ( fun , vct , nam2 ) ;
}
// ============================================================================
LoKi::Types::TrFun 
LoKi::Dicts::HltBind::bindMin  
( const LoKi::Types::TTrFunc&      fun  , 
  const LoKi::Types::TrFunc&       fun2 , 
  const std::vector<LHCb::Track*>* vct  ,
  const std::string&               nam2 ) 
{
  return LoKi::HltBinders::bindMin ( fun , fun2 , vct , nam2 ) ;
}
// ============================================================================
LoKi::Types::TrFun 
LoKi::Dicts::HltBind::bindAbsMin  
( const LoKi::Types::TTrFunc&      fun  , 
  const std::vector<LHCb::Track*>* vct  ,
  const std::string&               nam2 ) 
{
  return LoKi::HltBinders::bindAbsMin ( fun , vct , nam2 ) ;
}
// ============================================================================
LoKi::Types::TrFun 
LoKi::Dicts::HltBind::bindAbsMin  
( const LoKi::Types::TTrFunc&      fun  , 
  const LoKi::Types::TrFunc&       fun2 , 
  const std::vector<LHCb::Track*>* vct  ,
  const std::string&               nam2 ) 
{
  return LoKi::HltBinders::bindAbsMin ( fun , fun2 , vct , nam2 ) ;
}
// ============================================================================
// bindMax:  Track-Track
// ============================================================================
LoKi::Types::TrFun 
LoKi::Dicts::HltBind::bindMax  
( const LoKi::Types::TTrFunc&      fun  , 
  const std::vector<LHCb::Track*>* vct  ,
  const std::string&               nam2 ) 
{
  return LoKi::HltBinders::bindMax ( fun , vct , nam2 ) ;
}
// ============================================================================
LoKi::Types::TrFun 
LoKi::Dicts::HltBind::bindMax  
( const LoKi::Types::TTrFunc&      fun  , 
  const LoKi::Types::TrFunc&       fun2 , 
  const std::vector<LHCb::Track*>* vct  ,
  const std::string&               nam2 ) 
{
  return LoKi::HltBinders::bindMax ( fun , fun2 , vct , nam2 ) ;
}
// ============================================================================
LoKi::Types::TrFun 
LoKi::Dicts::HltBind::bindAbsMax  
( const LoKi::Types::TTrFunc&      fun  , 
  const std::vector<LHCb::Track*>* vct  ,
  const std::string&               nam2 ) 
{
  return LoKi::HltBinders::bindAbsMax ( fun , vct , nam2 ) ;
}
// ============================================================================
LoKi::Types::TrFun 
LoKi::Dicts::HltBind::bindAbsMax  
( const LoKi::Types::TTrFunc&      fun  , 
  const LoKi::Types::TrFunc&       fun2 , 
  const std::vector<LHCb::Track*>* vct  ,
  const std::string&               nam2 ) 
{
  return LoKi::HltBinders::bindAbsMax ( fun , fun2 , vct , nam2 ) ;
}
// ============================================================================
// bindMin:  Track-Vertex
// ============================================================================
LoKi::Types::TrFun 
LoKi::Dicts::HltBind::bindMin  
( const LoKi::Types::TrVFunc&          fun  , 
  const std::vector<LHCb::RecVertex*>* vct  ,
  const std::string&                   nam2 ) 
{
  return LoKi::HltBinders::bindMin ( fun , vct , nam2 ) ;
}
// ============================================================================
LoKi::Types::TrFun 
LoKi::Dicts::HltBind::bindMin  
( const LoKi::Types::TrVFunc&          fun  , 
  const LoKi::Types::RVFunc&           fun2 , 
  const std::vector<LHCb::RecVertex*>* vct  ,
  const std::string&                   nam2 ) 
{
  return LoKi::HltBinders::bindMin ( fun , fun2 , vct , nam2 ) ;
}
// ============================================================================
LoKi::Types::TrFun 
LoKi::Dicts::HltBind::bindAbsMin  
( const LoKi::Types::TrVFunc&          fun  , 
  const std::vector<LHCb::RecVertex*>* vct  ,
  const std::string&                   nam2 ) 
{
  return LoKi::HltBinders::bindAbsMin ( fun , vct , nam2 ) ;
}
// ============================================================================
LoKi::Types::TrFun 
LoKi::Dicts::HltBind::bindAbsMin  
( const LoKi::Types::TrVFunc&          fun  , 
  const LoKi::Types::RVFunc&           fun2 , 
  const std::vector<LHCb::RecVertex*>* vct  ,
  const std::string&                   nam2 ) 
{
  return LoKi::HltBinders::bindAbsMin ( fun , fun2 , vct , nam2 ) ;
}
// ============================================================================
// bindMax:  Track-Vertex
// ============================================================================
LoKi::Types::TrFun 
LoKi::Dicts::HltBind::bindMax  
( const LoKi::Types::TrVFunc&          fun  , 
  const std::vector<LHCb::RecVertex*>* vct  ,
  const std::string&                   nam2 ) 
{
  return LoKi::HltBinders::bindMax ( fun , vct , nam2 ) ;
}
// ============================================================================
LoKi::Types::TrFun 
LoKi::Dicts::HltBind::bindMax  
( const LoKi::Types::TrVFunc&          fun  , 
  const LoKi::Types::RVFunc&           fun2 , 
  const std::vector<LHCb::RecVertex*>* vct  ,
  const std::string&                   nam2 ) 
{
  return LoKi::HltBinders::bindMax ( fun , fun2 , vct , nam2 ) ;
}
// ============================================================================
LoKi::Types::TrFun 
LoKi::Dicts::HltBind::bindAbsMax  
( const LoKi::Types::TrVFunc&          fun  , 
  const std::vector<LHCb::RecVertex*>* vct  ,
  const std::string&                   nam2 ) 
{
  return LoKi::HltBinders::bindAbsMax ( fun , vct , nam2 ) ;
}
// ============================================================================
LoKi::Types::TrFun 
LoKi::Dicts::HltBind::bindAbsMax  
( const LoKi::Types::TrVFunc&          fun  , 
  const LoKi::Types::RVFunc&           fun2 , 
  const std::vector<LHCb::RecVertex*>* vct  ,
  const std::string&                   nam2 ) 
{
  return LoKi::HltBinders::bindAbsMax ( fun , fun2 , vct , nam2 ) ;
}
// ============================================================================
// bindMin:  Vertex-Vertex
// ============================================================================
LoKi::Types::RVFun 
LoKi::Dicts::HltBind::bindMin  
( const LoKi::Types::RVVFunc&          fun  , 
  const std::vector<LHCb::RecVertex*>* vct  ,
  const std::string&                   nam2 ) 
{
  return LoKi::HltBinders::bindMin ( fun , vct , nam2 ) ;
}
// ============================================================================
LoKi::Types::RVFun 
LoKi::Dicts::HltBind::bindMin  
( const LoKi::Types::RVVFunc&          fun  , 
  const LoKi::Types::RVFunc&           fun2 , 
  const std::vector<LHCb::RecVertex*>* vct  ,
  const std::string&                   nam2 ) 
{
  return LoKi::HltBinders::bindMin ( fun , fun2 , vct , nam2 ) ;
}
// ============================================================================
LoKi::Types::RVFun 
LoKi::Dicts::HltBind::bindAbsMin  
( const LoKi::Types::RVVFunc&          fun  , 
  const std::vector<LHCb::RecVertex*>* vct  ,
  const std::string&                   nam2 ) 
{
  return LoKi::HltBinders::bindAbsMin ( fun , vct , nam2 ) ;
}
// ============================================================================
LoKi::Types::RVFun 
LoKi::Dicts::HltBind::bindAbsMin  
( const LoKi::Types::RVVFunc&          fun  , 
  const LoKi::Types::RVFunc&           fun2 , 
  const std::vector<LHCb::RecVertex*>* vct  ,
  const std::string&                   nam2 ) 
{
  return LoKi::HltBinders::bindAbsMin ( fun , fun2 , vct , nam2 ) ;
}
// ============================================================================
// bindMax:  Vertex-Vertex
// ============================================================================
LoKi::Types::RVFun 
LoKi::Dicts::HltBind::bindMax  
( const LoKi::Types::RVVFunc&          fun  , 
  const std::vector<LHCb::RecVertex*>* vct  ,
  const std::string&                   nam2 ) 
{
  return LoKi::HltBinders::bindMax ( fun , vct , nam2 ) ;
}
// ============================================================================
LoKi::Types::RVFun 
LoKi::Dicts::HltBind::bindMax  
( const LoKi::Types::RVVFunc&          fun  , 
  const LoKi::Types::RVFunc&           fun2 , 
  const std::vector<LHCb::RecVertex*>* vct  ,
  const std::string&                   nam2 ) 
{
  return LoKi::HltBinders::bindMax ( fun , fun2 , vct , nam2 ) ;
}
// ============================================================================
LoKi::Types::RVFun 
LoKi::Dicts::HltBind::bindAbsMax  
( const LoKi::Types::RVVFunc&          fun  , 
  const std::vector<LHCb::RecVertex*>* vct  ,
  const std::string&                   nam2 ) 
{
  return LoKi::HltBinders::bindAbsMax ( fun , vct , nam2 ) ;
}
// ============================================================================
LoKi::Types::RVFun 
LoKi::Dicts::HltBind::bindAbsMax  
( const LoKi::Types::RVVFunc&          fun  , 
  const LoKi::Types::RVFunc&           fun2 , 
  const std::vector<LHCb::RecVertex*>* vct  ,
  const std::string&                   nam2 ) 
{
  return LoKi::HltBinders::bindAbsMax ( fun , fun2 , vct , nam2 ) ;
}
// ============================================================================


// ============================================================================
// bindMin Track-Track with source 
// ============================================================================
LoKi::Types::TrFun 
LoKi::Dicts::HltBind::bindMin 
( const LoKi::Types::TTrFunc&   fun  , 
  const LoKi::Types::TrSources& src  ) 
{
  return LoKi::HltBinders::bindMin ( fun , src ) ;
}
// ============================================================================
// bindMin Track-Track with source 
// ============================================================================
LoKi::Types::TrFun 
LoKi::Dicts::HltBind::bindMin 
( const LoKi::Types::TTrFunc&   fun  , 
  const LoKi::Types::TrSources& src  , 
  const LoKi::Types::TrFunc&    fun2 ) 
{
  return LoKi::HltBinders::bindMin ( fun , src , fun2 ) ;
}
// ============================================================================
// bindMax Track-Track with source 
// ============================================================================
LoKi::Types::TrFun 
LoKi::Dicts::HltBind::bindMax
( const LoKi::Types::TTrFunc&   fun  , 
  const LoKi::Types::TrSources& src  )
{
  return LoKi::HltBinders::bindMax ( fun , src ) ;
}
// ============================================================================
// bindMax Track-Track with source 
// ============================================================================
LoKi::Types::TrFun 
LoKi::Dicts::HltBind::bindMax
( const LoKi::Types::TTrFunc&   fun  , 
  const LoKi::Types::TrSources& src  , 
  const LoKi::Types::TrFunc&    fun2 ) 
{
  return LoKi::HltBinders::bindMax ( fun , src , fun2 ) ;
}
// ======================================================================


// ============================================================================
// bindMin Track-Track with source 
// ============================================================================
LoKi::Types::TrFun 
LoKi::Dicts::HltBind::bindAbsMin 
( const LoKi::Types::TTrFunc&   fun  , 
  const LoKi::Types::TrSources& src  ) 
{
  return LoKi::HltBinders::bindAbsMin ( fun , src ) ;
}
// ============================================================================
// bindMin Track-Track with source 
// ============================================================================
LoKi::Types::TrFun 
LoKi::Dicts::HltBind::bindAbsMin 
( const LoKi::Types::TTrFunc&   fun  , 
  const LoKi::Types::TrSources& src  , 
  const LoKi::Types::TrFunc&    fun2 ) 
{
  return LoKi::HltBinders::bindAbsMin ( fun , src , fun2 ) ;
}
// ============================================================================
// bindMax Track-Track with source 
// ============================================================================
LoKi::Types::TrFun 
LoKi::Dicts::HltBind::bindAbsMax
( const LoKi::Types::TTrFunc&   fun  , 
  const LoKi::Types::TrSources& src  ) 
{
  return LoKi::HltBinders::bindAbsMax ( fun , src ) ;
}
// ============================================================================
// bindMax Track-Track with source 
// ============================================================================
LoKi::Types::TrFun 
LoKi::Dicts::HltBind::bindAbsMax
( const LoKi::Types::TTrFunc&   fun  , 
  const LoKi::Types::TrSources& src  , 
  const LoKi::Types::TrFunc&    fun2 ) 
{
  return LoKi::HltBinders::bindAbsMax ( fun , src , fun2 ) ;
}
// ============================================================================
// bindMin Track-Vertex with source 
// ============================================================================
LoKi::Types::TrFun 
LoKi::Dicts::HltBind::bindMin 
( const LoKi::Types::TrVFunc&   fun , 
  const LoKi::Types::RVSources& src ) 
{
  return LoKi::HltBinders::bindMin ( fun , src ) ;
}
// ============================================================================
// bindMin Track-Vertex with source 
// ============================================================================
LoKi::Types::TrFun 
LoKi::Dicts::HltBind::bindMin 
( const LoKi::Types::TrVFunc&   fun  , 
  const LoKi::Types::RVSources& src  , 
  const LoKi::Types::RVFunc&    fun2 ) 
{
  return LoKi::HltBinders::bindMin ( fun , src , fun2 ) ;
}
// ============================================================================
// bindMax Track-Vertex with source 
// ============================================================================
LoKi::Types::TrFun 
LoKi::Dicts::HltBind::bindMax 
( const LoKi::Types::TrVFunc&   fun , 
  const LoKi::Types::RVSources& src ) 
{
  return LoKi::HltBinders::bindMax ( fun , src ) ;
}
// ============================================================================
// bindMax Track-Vertex with source 
// ============================================================================
LoKi::Types::TrFun 
LoKi::Dicts::HltBind::bindMax
( const LoKi::Types::TrVFunc&   fun  , 
  const LoKi::Types::RVSources& src  , 
  const LoKi::Types::RVFunc&    fun2 ) 
{
  return LoKi::HltBinders::bindMax ( fun , src , fun2 ) ;
}
// ============================================================================
// bindAbsMin Track-Vertex with source 
// ============================================================================
LoKi::Types::TrFun 
LoKi::Dicts::HltBind::bindAbsMin 
( const LoKi::Types::TrVFunc&   fun , 
  const LoKi::Types::RVSources& src ) 
{
  return LoKi::HltBinders::bindAbsMin ( fun , src ) ;
}
// ============================================================================
// bindAbsMin Track-Vertex with source 
// ============================================================================
LoKi::Types::TrFun 
LoKi::Dicts::HltBind::bindAbsMin 
( const LoKi::Types::TrVFunc&   fun  , 
  const LoKi::Types::RVSources& src  , 
  const LoKi::Types::RVFunc&    fun2 ) 
{
  return LoKi::HltBinders::bindAbsMin ( fun , src , fun2 ) ;
}
// ============================================================================
// bindAbsMax Track-Vertex with source 
// ============================================================================
LoKi::Types::TrFun 
LoKi::Dicts::HltBind::bindAbsMax 
( const LoKi::Types::TrVFunc&   fun , 
  const LoKi::Types::RVSources& src ) 
{
  return LoKi::HltBinders::bindAbsMax ( fun , src ) ;
}
// ============================================================================
// bindAbsMax Track-Vertex with source 
// ============================================================================
LoKi::Types::TrFun 
LoKi::Dicts::HltBind::bindAbsMax
( const LoKi::Types::TrVFunc&   fun  , 
  const LoKi::Types::RVSources& src  , 
  const LoKi::Types::RVFunc&    fun2 ) 
{
  return LoKi::HltBinders::bindAbsMax ( fun , src , fun2 ) ;
}
// ============================================================================
// bindMin Vertex-Vertex with source 
// ============================================================================
LoKi::Types::RVFun 
LoKi::Dicts::HltBind::bindMin 
( const LoKi::Types::RVVFunc&   fun , 
  const LoKi::Types::RVSources& src ) 
{
  return LoKi::HltBinders::bindAbsMax ( fun , src ) ;
}
// ============================================================================
// bindMin Vertex-Vertex with source 
// ============================================================================
LoKi::Types::RVFun 
LoKi::Dicts::HltBind::bindMin 
( const LoKi::Types::RVVFunc&   fun  , 
  const LoKi::Types::RVSources& src  , 
  const LoKi::Types::RVFunc&    fun2 ) 
{
  return LoKi::HltBinders::bindAbsMax ( fun , src , fun2 ) ;
}
// ===========================================================================
// bindMax Vertex-Vertex with source 
// ============================================================================
LoKi::Types::RVFun 
LoKi::Dicts::HltBind::bindMax 
( const LoKi::Types::RVVFunc&   fun , 
  const LoKi::Types::RVSources& src ) 
{
  return LoKi::HltBinders::bindAbsMax ( fun , src ) ;
}
// ============================================================================
// bindMax Vertex-Vertex with source 
// ============================================================================
LoKi::Types::RVFun 
LoKi::Dicts::HltBind::bindMax
( const LoKi::Types::RVVFunc&   fun  , 
  const LoKi::Types::RVSources& src  , 
  const LoKi::Types::RVFunc&    fun2 ) 
{
  return LoKi::HltBinders::bindAbsMax ( fun , src , fun2 ) ;
}
// ============================================================================
// bindAbsMin Vertex-Vertex with source 
// ============================================================================
LoKi::Types::RVFun 
LoKi::Dicts::HltBind::bindAbsMin 
( const LoKi::Types::RVVFunc&   fun , 
  const LoKi::Types::RVSources& src ) 
{
  return LoKi::HltBinders::bindAbsMax ( fun , src ) ;
}
// ============================================================================
// bindAbsMin Vertex-Vertex with source 
// ============================================================================
LoKi::Types::RVFun 
LoKi::Dicts::HltBind::bindAbsMin 
( const LoKi::Types::RVVFunc&   fun  , 
  const LoKi::Types::RVSources& src  , 
  const LoKi::Types::RVFunc&    fun2 ) 
{
  return LoKi::HltBinders::bindAbsMax ( fun , src , fun2 ) ;
}
// ============================================================================
// bindAbsMax Vertex-Vertex with source 
// ============================================================================
LoKi::Types::RVFun 
LoKi::Dicts::HltBind::bindAbsMax 
( const LoKi::Types::RVVFunc&   fun , 
  const LoKi::Types::RVSources& src ) 
{
  return LoKi::HltBinders::bindAbsMax ( fun , src ) ;
}
// ============================================================================
// bindAbsMax Vertex-Vertex with source 
// ============================================================================
LoKi::Types::RVFun 
LoKi::Dicts::HltBind::bindAbsMax
( const LoKi::Types::RVVFunc&   fun  , 
  const LoKi::Types::RVSources& src  , 
  const LoKi::Types::RVFunc&    fun2 ) 
{
  return LoKi::HltBinders::bindAbsMax ( fun , src , fun2 ) ;
}
// ============================================================================
// The END 
// ============================================================================
