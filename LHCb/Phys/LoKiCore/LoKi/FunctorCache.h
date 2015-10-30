// $Id:$ 
// ============================================================================
#ifndef LOKI_FUNCTOR_CACHE_H
#define LOKI_FUNCTOR_CACHE_H
// ============================================================================
// Include files 
// ============================================================================
#include "Gaudi/PluginService.h"
// ============================================================================
namespace LoKi 
{
  // ==========================================================================
  template <class T>
  struct CacheFactory 
  {
    typedef T CutType;
    typedef ::Gaudi::PluginService::Factory<CutType*> Factory;
    static std::string id(unsigned int hash)
    {
      return "loki_functor_" + std::to_string(hash);
    }
  };
  // ==========================================================================
  namespace Details 
  {
    // ========================================================================
    template <class T, unsigned int HASH>
    struct CacheFactory: public ::LoKi::CacheFactory<T> 
    {
      typedef T CutType;
      typedef ::Gaudi::PluginService::Factory<CutType*> Factory;
      static typename Factory::ReturnType create();
    };
    // ========================================================================
  } //                                           end of namespace LoKi::Details 
  // ==========================================================================
} //                                                      end of namespace LoKi
// ============================================================================
namespace Gaudi 
{
  // ==========================================================================
  namespace PluginService 
  {
    // ========================================================================
    namespace Details 
    {
      // ======================================================================
      template <class CutType, unsigned int HASH>
      class Factory<LoKi::Details::CacheFactory<CutType, HASH> > 
      {
      public:
        template <typename S>
        static typename S::ReturnType create() 
        {
          return LoKi::Details::CacheFactory<CutType, HASH>::create();
        }
      };
      // ======================================================================
    } //                         end of namespace Gaudi:PluginServiuce::Details
    // ========================================================================
  } //                                    end of namespace Gaudi::PluginService
  // ==========================================================================
} //                                                     end of namespace Gaudi 
// ============================================================================
#define DECLARE_LOKI_FUNCTOR(CutType, Hash) \
  namespace { \
    typedef LoKi::Details::CacheFactory<CutType, Hash> loki_cache_entry ## Hash; \
    DECLARE_COMPONENT_WITH_ID(loki_cache_entry ## Hash, loki_cache_entry ## Hash::id(Hash)) \
  }

// ============================================================================
// The END 
// ============================================================================
#endif
// ============================================================================
