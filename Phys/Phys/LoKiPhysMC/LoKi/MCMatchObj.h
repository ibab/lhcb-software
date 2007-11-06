// $Id: MCMatchObj.h,v 1.5 2007-11-06 10:53:15 cattanem Exp $
// ============================================================================
#ifndef LOKI_MCMATCHOBJ_H 
#define LOKI_MCMATCHOBJ_H 1
// ============================================================================
// Include files
// ============================================================================
// STD & STL 
// ============================================================================
#include <functional>
// ============================================================================
// LoKiCore
// ============================================================================
#include "LoKi/Base.h"
// ============================================================================
// LoKiPhysMC 
// ============================================================================
#include "LoKi/PhysMCTypes.h"
// ============================================================================

// ============================================================================
/** @file
 *
 *  This file is a part of LoKi project - 
 *    "C++ ToolKit  for Smart and Friendly Physics Analysis"
 *
 *  The package has been designed with the kind help from
 *  Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas, 
 *  contributions and advices from G.Raven, J.van Tilburg, 
 *  A.Golutvin, P.Koppenburg have been used in the design.
 *
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2006-03-10
 */
// ============================================================================

namespace LoKi 
{  
  /** @class MCMatchObj MCMatchObj.h LoKi/MCMatchObj.h
   *  
   *
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date   2006-03-11
   */
  class MCMatchObj 
    : public LoKi::Base                                                     
    , public std::binary_function<const LHCb::Particle*,
                                  const LHCb::MCParticle*,bool>    
  {
  public: 
    /** Standard constructor
     *  @param name object name 
     *  @param reporter error reporter 
     */
    MCMatchObj
    ( const std::string&     name , 
      const LoKi::IReporter* reporter );
  protected:
    /// protected and virtual destructor 
    virtual ~MCMatchObj( ); ///< Destructor
  public:
    /** check the match of MC truth information   (functor interface)
     *  @param  particle    pointer to Particle object  
     *  @param  mcparticle  pointer to MCParticle object 
     *  @return true        if the particle and mcparticle has "match"
     */
    bool operator() ( const LHCb::Particle*   particle   , 
                      const LHCb::MCParticle* mcparticle )  const 
    { return match ( particle , mcparticle ) ; }
    
    /** check the match of MC truth information  
     *  @param  particle    pointer to Particle object  
     *  @param  mcparticle  pointer to MCParticle object 
     *  @return true        if the particle and mcparticle has "match"
     */
    bool       match   
    ( const LHCb::Particle*   particle   , 
      const LHCb::MCParticle* mcparticle )  const ;
    
    /** check the match of MC truth information 
     *  @param  first       begin  iterator for sequence of particles 
     *  @param  last        end    iterator for sequence of particles 
     *  @param  mcparticle  pointer to MCParticle object 
     *  @return iterator to the first matched particle 
     */
    template <class PARTICLE> 
    inline PARTICLE   match     
    ( PARTICLE                first      , 
      PARTICLE                last       , 
      const LHCb::MCParticle* mcparticle ) const ;
    
    /** check the match of MC truth information 
     *  @param  particle    pointer to Particle object 
     *  @param  first       begin  iterator for sequence of MC particles 
     *  @param  last        end    iterator for sequence of MC particles 
     *  @return iterator to the first matched MC particle 
     */
    template <class MCPARTICLE>
    inline MCPARTICLE match  
    ( const LHCb::Particle*   particle   , 
      MCPARTICLE              first      , 
      MCPARTICLE              last       ) const ;
    
    /** check the match of MC truth information 
     *  @param  first       begin  iterator for sequence of MC particles 
     *  @param  last        end    iterator for sequence of MC particles 
     *  @return true if *ALL* 'particles' are matched 
     */
    template <class PARTICLE, class MCPARTICLE>
    inline bool match      
    ( PARTICLE     first     ,
      PARTICLE     last      , 
      MCPARTICLE   firstMC   , 
      MCPARTICLE   lastMC    ) const ;

    /** check the match of MC truth information 
     *  @param  particle    pointer to Particle object 
     *  @param  range       range of MC particles 
     *  @return true if particle matches at least 1 MC particle from range 
     */
    bool match     
    ( const LHCb::Particle*       particle   ,
      const LoKi::Types::MCRange& range      ) const ;
    
  public:
    
    void addMatchInfo
    ( const LoKi::Types::TableP2MC*      table ) 
    { if ( 0 != table) { m_tableP2MC   .push_back ( table ) ; } }
    
    void addMatchInfo
    ( const LoKi::Types::TableP2MCW*     table ) 
    { if ( 0 != table) { m_tableP2MCW  .push_back ( table ) ; } }
    
    void addMatchInfo
    ( const LoKi::Types::TablePP2MC*     table ) 
    { if ( 0 != table) { m_tablePP2MC  .push_back ( table ) ; } }
    
    void addMatchInfo
    ( const LoKi::Types::TableT2MC*      table ) 
    { if ( 0 != table) { m_tableT2MC   .push_back ( table ) ; } }
    
    void addMatchInfo
    ( const LoKi::Types::TableT2MCW*     table ) 
    { if ( 0 != table) { m_tableT2MCW  .push_back ( table ) ; } }
    
    /// clear the internal storage
    void clear() ;
    
  protected:
    
    /** look at MC truth information using "direct" relation table 
     *  @param table      pointer to relation table 
     *  @param object     the object (Particle/ProtoParticle/Track/...)
     *  @param mcparticle pointer to MCParticle object 
     */
    template <class TABLE, class OBJECT, class MCPARTICLE>
    inline bool matchInTable 
    ( const TABLE*  table      , 
      OBJECT        object     ,
      MCPARTICLE    mcparticle ) const ;

    /** look at MC truth information using "direct" relation table 
     *  @param obj   the object (Particle/ProtoParticle/Track/...)
     *  @param mcp   pointer to MCParticle object 
     */
    template <class TABLE, class OBJECT, class MCPARTICLE>
    inline bool matchInTable 
    ( TABLE         first ,
      TABLE         last  ,
      OBJECT        obj   ,
      MCPARTICLE    mcp   ) const ;
    
    /** look at MC truth information using "direct" relation table 
     *  @param tables pointer to relation tables
     *  @param obj    the object (Particle/ProtoParticle/Track/...)
     *  @param mcp    pointer to MCParticle object 
     */
    template <class TABLES, class OBJECT, class MCPARTICLE>
    inline bool matchInTables 
    ( const TABLES& tables,
      OBJECT        obj   ,
      MCPARTICLE    mcp   ) const ;
    
  private:
    /// default constructor is disabled (?) 
    MCMatchObj() ;
    /// copy constructor is disabled 
    MCMatchObj           ( const MCMatchObj& right ) ;
    /// assignement operator is disabled 
    MCMatchObj& operator=( const MCMatchObj& right ) ;
  private:
    //
    typedef std::vector<const LoKi::Types::TableP2MC*>     TablesP2MCs ;
    typedef std::vector<const LoKi::Types::TableP2MCW*>   TablesP2MCWs ;
    typedef std::vector<const LoKi::Types::TablePP2MC*>   TablesPP2MCs ;
    typedef std::vector<const LoKi::Types::TableT2MC*>     TablesT2MCs ;
    typedef std::vector<const LoKi::Types::TableT2MCW*>   TablesT2MCWs ;
    //
    TablesP2MCs     m_tableP2MC ;
    TablesP2MCWs   m_tableP2MCW ;
    TablesPP2MCs   m_tablePP2MC ;
    TablesT2MCs     m_tableT2MC ;
    TablesT2MCWs   m_tableT2MCW ;
    //
  } ; 
  //
} ; // end of the namespace LoKi

// ============================================================================
/** check the match of MC truth information 
 *  @param  first       begin  iterator for sequence of particles 
 *  @param  last        end    iterator for sequence of particles 
 *  @param  mcparticle  pointer to MCParticle object 
 *  @return iterator to the first matched particle 
 */
// ============================================================================
template <class PARTICLE> 
inline PARTICLE LoKi::MCMatchObj::match     
( PARTICLE                first      , 
  PARTICLE                last       , 
  const LHCb::MCParticle* mcparticle ) const 
{
  if ( 0 == mcparticle )
  { Error ( "match(): MCParticle* points to NULL") ; return last; } // RETURN
  for ( ; first != last ; ++first ) 
  { if ( match ( *first , mcparticle ) ) { return first ; } }       // RETURN 
  return last ;                                                     // RETURN 
};
// ============================================================================
/** check the match of MC truth information 
 *  @param  particle    pointer to Particle object 
 *  @param  first       begin  iterator for sequence of MC particles 
 *  @param  last        end    iterator for sequence of MC particles 
 *  @return iterator to the first matched MC particle 
 */
// ============================================================================
template <class MCPARTICLE>
inline MCPARTICLE LoKi::MCMatchObj::match  
( const LHCb::Particle*   particle   , 
  MCPARTICLE              first      , 
  MCPARTICLE              last       ) const 
{
  if ( 0 == particle )
  { Error ("match():   Particle* points to NULL") ; return last ; }  // RETURN     
  for ( ; first != last ; ++first ) 
  { if ( match ( particle , *first ) ) { return first ; } }          // RETURN 
  return  last ;                                                     // RETURN 
};
// ============================================================================
/** check the match of MC truth information 
 *  @param  particle    pointer to Particle object 
 *  @param  first       begin  iterator for sequence of MC particles 
 *  @param  last        end    iterator for sequence of MC particles 
 *  @return true if *ALL* 'particles' are matched 
 */
// ============================================================================
template <class PARTICLE, class MCPARTICLE>
inline bool LoKi::MCMatchObj::match
( PARTICLE     first     ,
  PARTICLE     last      , 
  MCPARTICLE   firstMC   , 
  MCPARTICLE   lastMC    ) const 
{
  // agreement 
  if (   first  == last                                ) { return false ; }
  // match the first particle 
  if (   lastMC == match ( *first , firstMC , lastMC ) ) { return false ; } 
  //  empty sequence ?  
  if ( ++first  == last                                ) { return true  ; }
  // match the rest 
  return    match ( first , last , firstMC , lastMC   ) ; 
};
// ============================================================================
/** look at MC truth information using "direct" relation table 
 *  @param table      pointer to relation table 
 *  @param object     the object (particle, protoparticle, track, etc..)
 *  @param mcparticle pointer to MCParticle object 
 */
// ============================================================================
template <class TABLE, class OBJECT, class MCPARTICLE>
inline bool LoKi::MCMatchObj::matchInTable 
( const TABLE* table      , 
  OBJECT       object     , 
  MCPARTICLE   mcparticle ) const 
{
  if ( !table || !object || !mcparticle ) { return false ; }     // RETURN 
  typename TABLE::Range range = table->relations ( object ) ;
  const LHCb::MCParticle* mcp = mcparticle ;
  for (  typename TABLE::iterator entry = range.begin() ; 
         range.end() != entry ; ++entry ) 
  { 
    const LHCb::MCParticle* _mc = entry->to() ;
    if ( mcp == _mc ) { return true ; }                          // RETURN 
  }  
  return false ;                                                 // RETURN 
};
// ============================================================================
/** look at MC truth information using "direct" relation table 
 *  @param table      pointer to relation table 
 *  @param object     the object (Particle/ProtoParticle/Track/...)
 *  @param mcparticle pointer to MCParticle object 
 */
// ============================================================================
template <class TABLE, class OBJECT, class MCPARTICLE>
inline bool LoKi::MCMatchObj::matchInTable 
( TABLE         first ,
  TABLE         last  ,
  OBJECT        obj   ,
  MCPARTICLE    mcp   ) const 
{
  for ( ; first != last ; ++first ) 
  { if ( matchInTable ( *first , obj , mcp ) ) { return true ; } } // RETURN
  return false ;
} ;
// ============================================================================
/** look at MC truth information using "direct" relation table 
 *  @param table      pointer to relation table 
 *  @param object     the object (Particle/ProtoParticle/Track/...)
 *  @param mcparticle pointer to MCParticle object 
 */
// ============================================================================
template <class TABLES, class OBJECT, class MCPARTICLE>
inline bool LoKi::MCMatchObj::matchInTables 
( const TABLES& tables,
  OBJECT        obj   ,
  MCPARTICLE    mcp   ) const 
{ return matchInTable ( tables.begin() , tables.end() , obj , mcp ) ; }
// ============================================================================
    
  

// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_MCMATCHOBJ_H
// ============================================================================
