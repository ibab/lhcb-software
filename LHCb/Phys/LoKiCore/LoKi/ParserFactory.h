// $Id$
// ============================================================================
#ifndef LOKI_PARSERFACTORY_H 
#define LOKI_PARSERFACTORY_H 1
// ============================================================================
// Include files
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/TreeHelpers.h"
#include "LoKi/iTree.h"
#include "LoKi/Trees.h"
// ============================================================================
namespace Decays 
{
  // ==========================================================================
  namespace Trees 
  {
    // ========================================================================
    template <class PARTICLE>
    StatusCode factory 
    ( typename Decays::Trees::Types_<PARTICLE>::Tree&               tree       ,
      const Decays::iNode&                                          mother     , 
      const Decays::Trees::Oscillation&                             oscillated ,
      const Decays::Trees::Arrow&                                   arrow      ,
      const typename Decays::Trees::Types_<PARTICLE>::TreeList&     daughters  , 
      const bool                                                    inclusive  ,
      const typename Decays::Trees::Types_<PARTICLE>::TreeList&     optional   , 
      std::ostream&                                                 stream     ) ;
  }
  // ========================================================================
  namespace Parsers 
  {
    // ========================================================================
    /** helper function to create the list of trees from 
     *  the list of parsed trees 
     *  @param trees  (OUTPUT) the list of constructed trees 
     *  @param parser (INPUT)  the list of input parsed trees 
     *  @param stream the stream to report the errors 
     *  @author Vanya BELYAEV Ivan.BElyaev@nikhef.nl
     *  @date 2009-05-23
     */
    template <class PARTICLE>
    StatusCode factory
    ( Decays::TreeList_<PARTICLE>&        trees  ,
      const Decays::Parsers::Tree::Trees& parsed , 
      std::ostream&                       stream ) ;
    // =======================================================================
    /** helper function to create the trees from the parsed tree
     *  @param tree   (OUTPUT) the list of constructed trees 
     *  @param parser (INPUT)  the list of input parsed trees 
     *  @param stream the stream to report the errors 
     *  @author Vanya BELYAEV Ivan.BElyaev@nikhef.nl
     *  @date 2009-05-23
     */
    template <class PARTICLE>
    StatusCode factory
    ( Decays::Tree_<PARTICLE>&     tree   ,
      const Decays::Parsers::Tree& parsed ,
      std::ostream&                stream ) ;    
    // =======================================================================
    /** helper function to create the list of trees from 
     *  the list of parsed trees 
     *  @param tree   (OUTPUT) the list of constructed trees 
     *  @param parser (INPUT)  the list of input parsed trees 
     *  @param stream the stream to report the errors 
     *  @author Vanya BELYAEV Ivan.BElyaev@nikhef.nl
     *  @date 2009-05-23
     */
    template <class PARTICLE>
    StatusCode factory
    ( Decays::TreeList_<PARTICLE>&        trees  ,
      const Decays::Parsers::Tree::Trees& parsed , 
      std::ostream&                       stream ) 
    {
      // avoid long names 
      typedef Decays::Trees::Types_<PARTICLE>               Types   ;
      typedef typename Types::Tree                          Tree    ;
      typedef typename Types::Invalid                       Invalid ;
      
      // loop over parsed trees 
      for  ( Decays::Parsers::Tree::Trees::const_iterator  it =
               parsed.begin() ; parsed.end() != it ; ++it )
      {
        // create the invalid tree
        Tree tree = Invalid() ;
        // parse the element of the list
        StatusCode sc = factory ( tree , *it , stream ) ;
        if  ( sc.isFailure() ) 
        { 
          stream << "ERROR: unable to parse element : '"
                 << (*it) << "' " << sc << std::endl ;
          return sc ;                                                 // RETURN 
        }                         
        trees += tree ;                                               // APPEND
      }
      return StatusCode::SUCCESS ;                                    // RETURN 
    }
    // ========================================================================
    /** helper function to create the trees from the parsed tree
     *  @param tree   (OUTPUT) the list of constructed trees 
     *  @param parser (INPUT)  the list of input parsed trees 
     *  @param stream the stream to report the errors 
     *  @author Vanya BELYAEV Ivan.BElyaev@nikhef.nl
     *  @date 2009-05-23
     */
    template <class PARTICLE>
    inline 
    StatusCode factory
    ( Decays::Tree_<PARTICLE>&     tree   ,
      const Decays::Parsers::Tree& parsed ,
      std::ostream&                stream ) 
    {
      typedef Decays::Trees::Types_<PARTICLE>    Types    ;
      typedef typename Types::TreeList           TreeList ;
      typedef typename Types::Invalid            Invalid  ;
      typedef typename Types::Or                 Or       ;
      typedef typename Types::And                And      ;
      typedef Decays::Trees::Marked_<PARTICLE>   Marked   ;
      typedef Decays::Trees::Stable_<PARTICLE>   Stable   ;
      
      // invalidate the tree 
      tree = Invalid() ;
      
      if      ( !parsed.ored() .empty() ) 
      {
        TreeList trees ;
        StatusCode sc = factory  ( trees , parsed.ored() , stream ) ;
        if ( sc.isFailure   () ) 
        {
          stream << "ERROR: unable to parse 'OR'    : '"
                 << parsed << "' " << sc << std::endl ;
          return sc ;                                                 // RETURN 
        }                           
        tree = Or ( trees ) ; 
      }
      else if ( ! parsed.anded() .empty() ) 
      {
        TreeList trees ;
        StatusCode sc = factory ( trees , parsed.anded() , stream ) ;
        if ( sc.isFailure   () ) 
        {
          stream << "ERROR: unable to parse 'AND'   : '"
                 << parsed << "' " << sc << std::endl ;
          return sc ;                                                 // RETURN 
        }                           
        tree = And ( trees ) ; 
      }
      else if ( parsed.stable () && !parsed.children ().empty()  ) 
      {
        StatusCode sc = StatusCode ( Decays::Trees::ChildrenForStable , true ) ;
        stream << "ERROR: Children are specified for stable : '"
               << parsed << "' " << sc << std::endl ;
        return sc ;                                                 // RETURN
      }
      else if ( parsed.stable () && !parsed.optional ().empty()  ) 
      {
        StatusCode sc = StatusCode ( Decays::Trees::OptionalForStable , true ) ;
        stream << "ERROR: Optional are specified for stable : '"
               << parsed << "' " << sc << std::endl ;
        return sc ;                                                 // RETURN
      }
      else if ( parsed.stable () && parsed.inclusive() ) 
      {
        StatusCode sc = StatusCode ( Decays::Trees::InclusiveForStable , true ) ;
        stream << "ERROR: Inclusive  is specified for stable : '"
               << parsed << "' " << sc << std::endl ;
        return sc ;                                                 // RETURN
      }
      else if ( parsed.stable () )
      { 
        //
        tree = Stable ( parsed.head() ) ;
        //
      }
      else if ( parsed.children () . empty() &&
                parsed.optional () . empty() ) 
      {
        StatusCode sc = Decays::Trees::factory 
          ( tree                 , 
            parsed.head       () , 
            parsed.oscillated () ) ;
        if ( sc.isFailure   () ) 
        { 
          stream << "ERROR: unable to create atomic : '"
                 << parsed << "' " << sc << std::endl ;
          return sc ;                                                 // RETURN 
        }                           
      }
      else 
      {
        TreeList children ;
        TreeList optional ;
        StatusCode sc1 = factory ( children , parsed.children () , stream ) ;
        if ( sc1.isFailure   () ) 
        { 
          stream << "ERROR: unable to parse children: '"
                 << parsed << "' " << sc1 << std::endl ;
          return sc1 ;                                                // RETURN 
        }                           
        StatusCode sc2 = factory ( optional , parsed.optional () , stream ) ;
        if ( sc2.isFailure   () ) 
        { 
          stream << "ERROR: unable to parse optional: '"
                 << parsed << "' " << sc2 << std::endl ;
          return sc2 ;                                                // RETURN 
        }
        StatusCode sc  = Decays::Trees::factory 
          ( tree                    , 
            parsed . head        () ,
            parsed . oscillated  () , 
            parsed . arrow       () , 
            children                , 
            parsed . inclusive   () , 
            optional                , 
            stream                  ) ;
        if ( sc.isFailure   () ) 
        { 
          stream << "ERROR: unable to create tree   : '"
                 << parsed << "' " << sc << std::endl ;
          return sc ;                                                 // RETURN 
        }                           
      }
      
      // the final polishing:
      if ( parsed.marked  () ) { tree = Marked ( tree ) ; }
      if ( parsed.negated () ) { tree =         ~tree   ; }
      
      return StatusCode::SUCCESS ;
    }
    // ========================================================================
  } //                                         end of namespace Decays::Parsers  
  // ==========================================================================
} //                                                    end of namespace Decays 
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_PARSERFACTORY_H
// ============================================================================
