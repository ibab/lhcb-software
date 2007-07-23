// $Id: MCFinderDicts.cpp,v 1.2 2007-07-23 17:27:31 ibelyaev Exp $
// ============================================================================
// Include files 
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/MCTypes.h"
#include "LoKi/MCFinderObj.h"
#include "LoKi/MCFinder.h"
#include "LoKi/MCFinderDicts.h"
// ============================================================================
/** @file 
 *  Implementation file for class LoKi::Dicts::MCFinder
 *  @date 2007-06-06 
 *  @author Vanya BELYAEV
 */
// ============================================================================
namespace 
{
  /// empty range 
  const LoKi::Types::MCRange s_EMPTY = LoKi::Types::MCRange() ;
} 
// ===========================================================================
// valid object?
// ===========================================================================
bool LoKi::Dicts::MCFinderDicts::valid 
( const LoKi::MCFinder& m ) 
{ return m.validPointer() ; }
// ===========================================================================
// find MC-decays according to decay descriptor 
// ===========================================================================
LoKi::Types::MCRange LoKi::Dicts::MCFinderDicts::findDecays 
( const LoKi::MCFinder& f       , 
  const std::string&    decay   ) 
{ return f.validPointer() ? f->findDecays ( decay ) : s_EMPTY ;}
// ===========================================================================
// find MC-decays according to decay descriptor 
// ===========================================================================
LoKi::Types::MCRange LoKi::Dicts::MCFinderDicts::findDecays 
( const LoKi::MCFinder& f       , 
  const std::string&    decay   ,
  const std::string&    address ) 
{ return f.validPointer() ? f->findDecays ( decay , address ) : s_EMPTY ;}
// ===========================================================================
// find MC-decays according to decay descriptor 
// ===========================================================================
LoKi::Types::MCRange LoKi::Dicts::MCFinderDicts::findDecays 
( const LoKi::MCFinder& f       , 
  const std::string&    decay   ,
  const LHCb::MCParticle::Container* mcps ) 
{ return f.validPointer() ? f->findDecays ( decay , mcps ) : s_EMPTY ;}
// ===========================================================================
// find MC-decays according to decay descriptor 
// ===========================================================================
LoKi::Types::MCRange LoKi::Dicts::MCFinderDicts::findDecays 
( const LoKi::MCFinder& f       , 
  const std::string&    decay   ,
  const LHCb::MCParticle::Vector& mcps ) 
{ return f.validPointer() ? f->findDecays ( decay , mcps ) : s_EMPTY ;}
// ===========================================================================
// find MC-decays according to decay descriptor 
// ===========================================================================
LoKi::Types::MCRange LoKi::Dicts::MCFinderDicts::findDecays 
( const LoKi::MCFinder& f       , 
  const std::string&    decay   ,
  const LHCb::MCParticle::ConstVector& mcps ) 
{ return f.validPointer() ? f->findDecays ( decay , mcps ) : s_EMPTY ;}
// ===========================================================================
// find MC-decays according to decay descriptor 
// ===========================================================================
LoKi::Types::MCRange LoKi::Dicts::MCFinderDicts::findDecays 
( const LoKi::MCFinder&       f     , 
  const std::string&          decay ,
  const LoKi::Types::MCRange& mcps  ) 
{ return f.validPointer() ? f->findDecays ( decay , mcps ) : s_EMPTY ;}
// ===========================================================================
// find MC-decays according to decay descriptor 
// ===========================================================================
LoKi::Types::MCRange LoKi::Dicts::MCFinderDicts::findDecays 
( const LoKi::MCFinder&       f     , 
  const std::string&          decay ,
  const LoKi::Keeper<LHCb::MCParticle>& mcps  ) 
{ return f.validPointer() ? f->findDecays ( decay , mcps ) : s_EMPTY ;}
// ===========================================================================
// find MC-decays according to decay descriptor 
// ===========================================================================
LoKi::Types::MCRange LoKi::Dicts::MCFinderDicts::findDecays 
( const LoKi::MCFinder&       f     , 
  const std::string&          decay ,
  const LoKi::UniqueKeeper<LHCb::MCParticle>& mcps  ) 
{ return f.validPointer() ? f->findDecays ( decay , mcps ) : s_EMPTY ;}
// ===========================================================================

// ============================================================================
// The END 
// ============================================================================
