// $Id:$ 
// ============================================================================
#ifndef LOKI_TRGTOCPP_H 
#define LOKI_TRGTOCPP_H 1
// ============================================================================
// Include files
// ============================================================================
// STD&STL
// ============================================================================
#include <string>
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/Kernel.h"
// ============================================================================
namespace LoKi 
{
  // ==========================================================================
  namespace Hlt1 
  {
    // ========================================================================
    class Tool                ;
    class TrackFunction       ;
    class TrackBiFunction     ;
    class MatchTVelo          ;
    class TrackVertexFunction ;
    class TrackVertexFunction ;
    class TrackTool           ;
    class MatchConf           ;
    class UpgradeConf         ;
    class UpgradeTool         ;
    class VxMakerConf         ;
    // ========================================================================
  }
  // ==========================================================================
}
// ============================================================================
namespace Gaudi
{
  // ==========================================================================
  namespace Utils 
  {
    // ========================================================================
    GAUDI_API std::string toCpp ( const LoKi::Hlt1::Tool&                o ) ;
    GAUDI_API std::string toCpp ( const LoKi::Hlt1::TrackFunction&       o ) ;
    GAUDI_API std::string toCpp ( const LoKi::Hlt1::TrackBiFunction&     o ) ;
    GAUDI_API std::string toCpp ( const LoKi::Hlt1::MatchTVelo&          o ) ;
    GAUDI_API std::string toCpp ( const LoKi::Hlt1::TrackVertexFunction& o ) ;
    GAUDI_API std::string toCpp ( const LoKi::Hlt1::MatchConf&           o ) ;
    GAUDI_API std::string toCpp ( const LoKi::Hlt1::TrackTool&           o ) ;
    GAUDI_API std::string toCpp ( const LoKi::Hlt1::UpgradeConf&         o ) ;
    GAUDI_API std::string toCpp ( const LoKi::Hlt1::UpgradeTool&         o ) ;
    GAUDI_API std::string toCpp ( const LoKi::Hlt1::VxMakerConf&         o ) ;
    // ========================================================================
  } 
  // ==========================================================================
}
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_TRGTOCPP_H
// ============================================================================
