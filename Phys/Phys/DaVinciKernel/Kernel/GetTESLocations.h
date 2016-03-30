// $Id:$ 
// ============================================================================
#ifndef KERNEL_GETTESLOCATIONS_H 
#define KERNEL_GETTESLOCATIONS_H 1
// ============================================================================
// Include files
// ============================================================================
// STD& STL
// ============================================================================
#include <map>
#include <vector>
#include <string>
// ============================================================================
// GaudiKernel
// ============================================================================
#include "GaudiKernel/Kernel.h"
// ============================================================================
// PhysEvent
// ============================================================================
#include "Event/Particle.h"
// ============================================================================
namespace DaVinci
{
  // ==========================================================================
  namespace Utils 
  {
    // ========================================================================
    /** @class GetTESLocations Kernel/GetTESLocations.h
     *
     *  @author Vanya Belyaev
     *  @date   2016-02-16
     */
    class GAUDI_API GetTESLocations 
    {
    public: 
      // ======================================================================
      ///  Standard constructor
      GetTESLocations ( const DataObject*          o = 0 ) 
        : m_mapping   () 
        , m_locations () { if ( nullptr != o ) { _collect ( o ) ; } }      
      ///  constructor from constained obejct 
      GetTESLocations ( const LHCb::Particle*      o     ) 
        : m_mapping   () 
        , m_locations () { if ( nullptr != o ) {  collect ( o ) ; } }
      template <class OBJECT> 
        GetTESLocations ( OBJECT begin , OBJECT end ) 
        : m_mapping   () 
        , m_locations () { collect ( begin , end ) ; }
      // ======================================================================      
      /// destructor 
      ~GetTESLocations() ;
      // ======================================================================
    public:
      // ======================================================================
      /// add TES-location 
      inline   std::size_t collect ( const DataObject*     o ) ;
      /// add TES-location 
      inline   std::size_t collect ( const LHCb::Particle* p ) ;
      /// add TES-location 
      template <class OBJECT> 
        inline std::size_t collect ( OBJECT begin , 
                                     OBJECT end   ) ;
      // ======================================================================
    public:  // get the collected locations 
      // ======================================================================
      const std::vector<std::string>& locations() const
      { return m_locations ; }
      // ======================================================================
    protected:
      // ======================================================================
      /// add TES-location 
      std::size_t _collect ( const DataObject*     o ) ;
      // ======================================================================
    private:
      // ======================================================================
      /// mapping:  data object -> tes location 
      std::map<const DataObject*,std::string>  m_mapping   ;
      /// the list of locations 
      std::vector<std::string>                 m_locations ;
      // ======================================================================      
    };
    // ========================================================================
    // add TES-location for the particle 
    // ========================================================================
    inline std::size_t GetTESLocations::collect 
    ( const LHCb::Particle* p  ) 
    {
      std::size_t _added = 0 ;
      if ( nullptr == p ) { return 0 ; }              // nothing to add 
      const DataObject* c = p->parent() ; 
      if ( nullptr == c ) { return 0 ; }              // nothing to add
      if ( m_mapping.end() == m_mapping.find ( c ) ) 
      { _added += _collect ( c ) ; }
      const SmartRefVector<LHCb::Particle>& dd = p->daughters() ;
      return  _added + collect ( dd.begin() , dd.end() ) ;
    }
    // ========================================================================
    // add TES-location for parent container 
    // ========================================================================
    inline std::size_t GetTESLocations::collect ( const DataObject* c )
    {
      return
        nullptr         == c                    ? 0 :
        m_mapping.end() != m_mapping.find ( c ) ? 0 : _collect ( c ) ;
    }
    // ========================================================================
    // add TES-location for particle 
    // ========================================================================
    template <class OBJECT> 
    inline std::size_t GetTESLocations::collect ( OBJECT begin ,
                                                  OBJECT end   )
    {
      std::size_t _added = 0 ;
      for ( ; begin != end ; ++begin ) { _added += collect ( *begin ) ; }
      return _added ;
    }
    // ========================================================================
  } //                                          end of namespace DaVinci::Utils 
  // ==========================================================================
} //                                                   end of namespace DaVinci
// ============================================================================
//                                                                      The END 
// ============================================================================
#endif // KERNEL_GETTESLOCATIONS_H
// ============================================================================
