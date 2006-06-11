// $Id: Get.h,v 1.3 2006-06-11 15:23:45 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $ 
// ============================================================================
// $Log: not supported by cvs2svn $
// ============================================================================
#ifndef RELATIONS_GET_H 
#define RELATIONS_GET_H 1
// ============================================================================
// Include files
// ============================================================================
// STD&STL
// ============================================================================
#include <set>
// ============================================================================
// Relations 
// ============================================================================
#include "Relations/RelationTypeTraits.h"
#include "Relations/RelationWeightedTypeTraits.h"
// ============================================================================

// ============================================================================
/** @file
 *
 *  The collection of trivial utilities to manipulate with 
 *  relation tabels/ranges/iterators
 * 
 *
 *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
 *  @date 2006-02-21
 */
// ============================================================================

namespace Relations 
{  
  /** @fn getTo
   *  simple function to extract all values of "TO" 
   *  from relation Ranges into the plain container
   *  
   *  @code 
   *
   *  typedef IRelationWeighted<LHCb::CaloDigit,LHCb::MCParticle,float> Table ;
   *  // get the relation table:   
   *  const ITable* table = ...  ;
   *  
   *  const LCHb::CaloDigit* digit = ... ;
   *  // get all links from the given LHCb::CaloDigit obejct:
   *  Table::Range links = table->relations( digit ) ;
   *  
   *  // get all MC-particles into one plain container 
   *  typedef std:vector<const LHCb::MCParticle*> MCPARTICLES ;
   *  MCPARTICLES mcparticles ;
   *  Relations::getTo ( links.begin () , 
   *                     links.end   () ,
   *                     std::back_inserter( mcparticles ) ) ;
   *
   *  @endcode 
   *  
   *  @see IRelationWeighted 
   *  
   *  For extraction of the list of "unique" objects see getUniqueTo
   *  @see Relations::getUniqueTo 
   *
   *  @param first 'begin'-iterator for the sequence of input links 
   *  @param last  'end'-iterator for the sequence of input links 
   *  @param output the posision of the output iterator 
   *  @return the updated position fo output iterator 
   *
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date 2006-02-21
   */
  template <class LINK,class OUTPUT>
  inline OUTPUT getTo
  ( LINK   first  , 
    LINK   last   , 
    OUTPUT output )
  {
    for ( ; first != last ; ++first ) 
    {
      *output =  first->to()  ;   // FILL THE OUTPUT ITERATOR 
      ++output;                   // ADVANCE OUTPUT ITERATOR
    }
    return output ;
  } ;
  
  /** @fn getTo
   *  simple function to extract all values of "TO" 
   *  from relation Ranges into the plain container
   *  
   *  @code 
   *
   *  typedef IRelationWeighted<LHCb::CaloDigit,LHCb::MCParticle,float> Table ;
   *  // get the relation table:   
   *  const ITable* table = ...  ;
   *  
   *  const LCHb::CaloDigit* digit = ... ;
   *  // get all links from the given LHCb::CaloDigit obejct:
   *  Table::Range links = table->relations( digit ) ;
   *  
   *  // get all MC-particles into one plain container 
   *  typedef std:vector<const LHCb::MCParticle*> MCPARTICLES ;
   *  MCPARTICLES mcparticles ;
   *  Relations::getTo ( links , std::back_inserter( mcparticles ) ) ;
   *
   *  @endcode 
   *  
   *  @see IRelationWeighted 
   *  
   *  @param links  the sequence of input links 
   *  @param output the posision of the output iterator 
   *  @return the updated position fo output iterator 
   *
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date 2006-02-21
   */
  template <class LINKS,class OUTPUT>
  inline OUTPUT getTo
  ( LINKS  links  , 
    OUTPUT output )
  { return getTo ( links.begin() , links.end() , output ) ; } ;
  
  /** @fn getWeight
   *
   *  simple function to extract all values of "WEIGHT" 
   *  from relation Ranges into the plain container
   *  
   *  @code 
   *
   *  typedef IRelationWeighted<LHCb::CaloDigit,LHCb::MCParticle,float> Table ;
   *  // get the relation table:   
   *  const ITable* table = ...  ;
   *  
   *  const LCHb::CaloDigit* digit = ... ;
   *  // get all links from the given LHCb::CaloDigit obejct:
   *  Table::Range links = table->relations( digit ) ;
   *  
   *  // get all weight into one plain container 
   *  typedef std:vector<double> WEIGHTS ;
   *  WEIGHTS weight ;
   *  Relations::getWeight ( links.begin () , 
   *                         links.end   () ,
   *                         std::back_inserter( weights ) ) ;
   *
   *  @endcode 
   *  
   *  @see IRelationWeighted 
   *  
   *  @param first 'begin'-iterator for the sequence of input links 
   *  @param last  'end'-iterator for the sequence of input links 
   *  @param output the posision of the output iterator 
   *  @return the updated position fo output iterator 
   *
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date 2006-02-21
   */
  template <class LINK,class OUTPUT>
  inline OUTPUT getWeight
  ( LINK   first  , 
    LINK   last   , 
    OUTPUT output )
  {
    for ( ; first != last ; ++first ) 
    {
      *output = first->weight()  ;   // FILL THE OUTPUT ITERATOR 
      ++output;                     // ADVANCE OUTPUT ITERATOR
    }
    return output ;
  } ;
  
  /** @fn getWeight
   *
   *  simple function to extract all values of "WEIGHT" 
   *  from relation Ranges into the plain container
   *  
   *  @code 
   *
   *  typedef IRelationWeighted<LHCb::CaloDigit,LHCb::MCParticle,float> Table ;
   *  // get the relation table:   
   *  const ITable* table = ...  ;
   *  
   *  const LCHb::CaloDigit* digit = ... ;
   *  // get all links from the given LHCb::CaloDigit obejct:
   *  Table::Range links = table->relations( digit ) ;
   *  
   *  // get all weight into one plain container 
   *  typedef std:vector<double> WEIGHTS ;
   *  WEIGHTS weight ;
   *  Relations::getWeight ( links , std::back_inserter( weights ) ) ;
   *
   *  @endcode 
   *  
   *  @see IRelationWeighted 
   *  
   *  @param links the sequence of input links 
   *  @param output the posision of the output iterator 
   *  @return the updated position fo output iterator 
   *
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date 2006-02-21
   */
  template <class LINKS,class OUTPUT>
  inline OUTPUT getWeight
  ( LINKS  links  , 
    OUTPUT output )
  { return getWeight ( links.begin() , links.end() , output ) ; } ;

  /** @fn sumWeight 
   *
   *  Trivial algorithm whcih allow to accumulate the
   *  "WEIGHT" from the relations ranges 
   * 
   *  @code 
   *
   *  Range links = .... ; 
   *
   *  // find a sum of all weights
   *  const double weight = 
   *    Relations::sumWeight( links.begin() , links.end() , 0.0 ) 
   *
   *  @endcode 
   * 
   *  @param first 'begin'-iterator for the sequence of input links 
   *  @param last  'end'-iterator for the sequence of input links 
   *  @param weight the initial value for the accumulated weight 
   *  @return the updated value for the accumulated weight 
   *
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date 2006-02-21
   */
  template <class LINK,class WEIGHT>
  inline WEIGHT sumWeight
  ( LINK   first  , 
    LINK   last   ,
    WEIGHT weight ) 
  {
    static const std::plus<WEIGHT> _plus ;
    for ( ; first != last ; ++first )   
    { weight = _plus( weight , first->weight() ) ; }     // ACCUMULATE the weight 
    return weight ;
  } ;

  /** @fn getUniqueTo
   *
   *  simple function to extract all *UNIQUE* values of "TO" 
   *  from relation Ranges into the plain container
   *  
   *  @code 
   *
   *  typedef IRelationWeighted<LHCb::CaloCluster,LHCb::MCParticle,float> Table ;
   *  // get the relation table:   
   *  const ITable* table = ...  ;
   *  
   *  // get ALL links 
   *  Table::Range links = table->relations() ;
   *  
   *  // get all *UNIQUE* MC-particles into one plain container 
   *  /// not very efficient: 
   *  // typedef std:vector<const LHCb::MCParticle*> MCPARTICLES ;  
   *  /// efficient: 
   *  typedef std:vector<const LHCb::MCParticle*> MCPARTICLES ;  
   *
   *  MCPARTICLES mcparticles ;
   *  Relations::getUniqueTo ( links.begin () , 
   *                           links.end   () ,
   *                           mcparticles    ) ;
   *
   *  @endcode 
   *  
   *  @see IRelationWeighted 
   *
   * @attention IMPORTANT
   * // ///////////////////////////////////////////////////////////////////////
   * // IMPORTANT!!!
   * // This utility/function/algorihtm is not very efficient for 
   * // "std::vector" containers. but it is efficient for std::set 
   * // containers:
   * // //////////////////////////////////////////////////////////////////////
   *
   *  @param first 'begin'-iterator for the sequence of input links 
   *  @param last  'end'-iterator for the sequence of input links 
   *  @param cont  the contained to be updated 
   *  @return the number of added unique elements 
   *
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date 2006-02-21
   */
  template <class LINK,class CONTAINER>
  inline size_t getUniqueTo
  ( LINK       first  , 
    LINK       last   , 
    CONTAINER& cont   ) 
  {
    // store the current size 
    const size_t size = cont.size() ;
    for ( ; first != last ; ++first ) 
    { cont.push_back ( first->to() ) ;  }
    typename CONTAINER::iterator begin = cont.begin () ;
    typename CONTAINER::iterator end   = cont.end   () ;
    // sort the subsequence 
    std::stable_sort    ( begin , end ) ;
    // find duplicates 
    begin = std::unique ( begin , end ) ;
    // erase clones 
    if ( begin != end ) { cont.erase ( begin , end ) ; }
    // return the new container size 
    return cont.size() - size ;
  } ;
  /** @fn getUniqueTo
   *
   *  simple function to extract all *UNIQUE* values of "TO" 
   *  from relation Ranges into the "std::set" container 
   *  (It is a partial specialization of gfeneric getUniqueTo function)
   *  
   *  @code 
   *
   *  typedef IRelationWeighted<LHCb::CaloCluster,LHCb::MCParticle,float> Table ;
   *  // get the relation table:   
   *  const ITable* table = ...  ;
   *  
   *  // get ALL links
   *  Table::Range links = table->relations( cluster ) ;
   *  
   *  // get all *UNIQUE* MC-particles into one plain container 
   *  typedef std::set<const LHCb::MCParticle*> MCPARTICLES ;
   *  MCPARTICLES mcparticles ;
   *
   *  Relations::getUniqueTo ( links.begin () , 
   *                           links.end   () ,
   *                           mcparticles    ) ;
   *
   *  @endcode 
   *  
   *  @see IRelationWeighted 
   *  @see Relations::getUniqueTo
   *  @see Relations::getTo
   *  @see std:set 
   *  
   *  @param  first 'begin'-iterator for the sequence of input links 
   *  @param  last  'end'-iterator for the sequence of input links 
   *  @param  cont  the set to be updated 
   *  @return the number of added unique elements 
   *
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date 2006-02-21
   */
  template <class LINK,class OBJECT>
  inline size_t getUniqueTo
  ( LINK              first  , 
    LINK              last   , 
    std::set<OBJECT>& cont   ) 
  {
    // store the current size 
    const size_t size = cont.size() ;
    for ( ; first != last ; ++first ) { cont.insert( first->to() ) ; }
    // return number of newly added elements 
    return cont.size() - size ;
  } ;
  /** @fn getUniqueTo
   *
   *  simple function to extract all *UNIQUE* values of "TO" 
   *  from relation Ranges into the "std::set" container 
   *  (It is a partial specialization of gfeneric getUniqueTo function)
   *  
   *  @code 
   *
   *  typedef IRelationWeighted<LHCb::CaloCluster,LHCb::MCParticle,float> Table ;
   *  // get the relation table:   
   *  const ITable* table = ...  ;
   *  
   *  // get ALL links
   *  Table::Range links = table->relations( cluster ) ;
   *  
   *  // get all *UNIQUE* MC-particles into one plain container 
   *  // e.g.: typedef std::set<const LHCb::MCParticle*> MCPARTICLES ;
   *  typedef std::vector<const LHCb::MCParticle*> MCPARTICLES ;
   *  MCPARTICLES mcparticles ;
   *
   *  Relations::getUniqueTo ( links , mcparticles    ) ;
   *
   *  @endcode 
   *  
   *  @see IRelationWeighted 
   *  @see Relations::getUniqueTo
   *  @see Relations::getTo
   *  @see std:set 
   *  
   *  @param  link the sequence of input links 
   *  @param  cont  the set to be updated 
   *  @return the number of added unique elements 
   *
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date 2006-02-21
   */
  template <class LINKS,class CONTAINER>
  inline size_t getUniqueTo
  ( LINKS      links  , 
    CONTAINER& cont   ) 
  { return getUniqueTo ( links.begin() , links.end() , cont ) ; } ;
  /** @fn sumWeight 
   *
   *  Trivial algorithm whcih allow to accumulate the
   *  "WEIGHT" form relations ranges 
   * 
   *  @code 
   *
   *  Range links = .... ; 
   *
   *  // find a sum of all weights
   *  const double weight = 
   *    Relations::sumWeight( links.begin() , links.end() , 0.0 ) 
   *
   *  @endcode 
   * 
   *  @param links the sequence of input links 
   *  @param weight the initial value for the accumulated weight 
   *  @return the updated value for the accumulated weight 
   *
   *  @author Vanya BELYAEV ibelyaev@physics.syr.edu
   *  @date 2006-02-21
   */
  template <class LINKS,class WEIGHT>
  inline WEIGHT sumWeight
  ( LINKS  links  , 
    WEIGHT weight ) 
  { return sumWeight( links.begin() , links.end() , weight ) ; } ;

} ; // end of the namespace Relations

// ============================================================================
// The END 
// ============================================================================
#endif // RELATIONS_GET_H
// ============================================================================
