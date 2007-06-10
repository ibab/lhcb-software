// $Id: MCFinderDicts.h,v 1.1 2007-06-10 19:59:04 ibelyaev Exp $
// ============================================================================
#ifndef LOKI_MCFINDERDICTS_H 
#define LOKI_MCFINDERDICTS_H 1
// ============================================================================
// Include Files
// ============================================================================
namespace LoKi
{
  namespace Dicts 
  {
    namespace MCFinderDicts
    {
      // ======================================================================
      /// valid object ? 
      bool valid( const LoKi::MCFinder& f ) ;    
      // ======================================================================
      /// find MC-decays according to decay descriptor 
      LoKi::Types::MCRange findDecays 
      ( const LoKi::MCFinder& f       , 
        const std::string&    decay   ) ;
      // ======================================================================
      /// find MC-decays according to decay descriptor 
      LoKi::Types::MCRange findDecays 
      ( const LoKi::MCFinder& f       , 
        const std::string&    decay   ,
        const std::string&    address ) ;
      // ======================================================================
      /// find MC-decays according to decay descriptor 
      LoKi::Types::MCRange findDecays 
      ( const LoKi::MCFinder& f       , 
        const std::string&    decay   ,
        const LHCb::MCParticle::Container* mcps ) ;
      // ======================================================================
      /// find MC-decays according to decay descriptor 
      LoKi::Types::MCRange findDecays 
      ( const LoKi::MCFinder&       f     , 
        const std::string&          decay ,
        const LoKi::Types::MCRange& mcps  ) ;
      // ======================================================================
      /// find MC-decays according to decay descriptor 
      LoKi::Types::MCRange findDecays 
      ( const LoKi::MCFinder& f       , 
        const std::string&    decay   ,
        const LHCb::MCParticle::Vector& mcps ) ;
      // ======================================================================
      /// find MC-decays according to decay descriptor 
      LoKi::Types::MCRange findDecays 
      ( const LoKi::MCFinder& f       , 
        const std::string&    decay   ,
        const LHCb::MCParticle::ConstVector& mcps ) ;
      // ======================================================================
      /// find MC-decays according to decay descriptor 
      LoKi::Types::MCRange findDecays 
      ( const LoKi::MCFinder&       f     , 
        const std::string&          decay ,
        const LoKi::Keeper<LHCb::MCParticle>& mcps  ) ;
      // ======================================================================
      /// find MC-decays according to decay descriptor 
      LoKi::Types::MCRange findDecays 
      ( const LoKi::MCFinder&       f     , 
        const std::string&          decay ,
        const LoKi::UniqueKeeper<LHCb::MCParticle>& mcps  ) ;
    };
  } // end of namespace LoKi::Dicts 
} // end of namespace LoKi
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_MCFINDERDICTS_H
// ============================================================================
