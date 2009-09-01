// $Id: NodeGrammar.h,v 1.4 2009-09-01 09:02:43 ibelyaev Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $, version $Revision: 1.4 $
// ============================================================================
#ifndef LOKI_NODEGRAMMAR_H 
#define LOKI_NODEGRAMMAR_H 1
// ============================================================================
// Include files
// ============================================================================
// PartProp
// ============================================================================
#include "Kernel/Nodes.h"
#include "Kernel/NodesPIDs.h"
#include "Kernel/IParticlePropertySvc.h"
// ============================================================================
// Boost/Spirit
// ============================================================================
#include "boost/spirit/include/classic_core.hpp"
#include "boost/spirit/include/classic_attribute.hpp"
#include "boost/spirit/include/phoenix1.hpp"
#include <boost/spirit/include/classic_stored_rule.hpp>
#include <boost/spirit/include/classic_actor.hpp>
#include "boost/bind.hpp"
// ============================================================================
namespace Decays 
{
  // ==========================================================================
  namespace Grammars 
  {
    // ========================================================================
    using namespace boost::spirit::classic ;
    using namespace phoenix                ;
    // ========================================================================
    typedef    Decays::Nodes::_Node                                 Node_     ;
    typedef    Decays::NodeList                                     NodeList_ ;
    // ========================================================================
    /** @struct NodeClosure 
     *  helper closure class to keep/construct "single" nodes..
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2009--09-12
     */
    struct NodeClosure 
      : public boost::spirit::classic::closure <NodeClosure,Node_>
    {
      // ======================================================================
      /// the actual holder for the node 
      member1 node     ;                     //  the actual holder for the node 
      // ======================================================================
    };
    // ========================================================================
    /** @struct Node
     *  grammar class to keep/construct parametric nodes 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2009--09-12
     */
    class Node : public boost::spirit::classic::grammar 
    <Node,NodeClosure::context_t>
    {
      // ======================================================================
    public:
      // ======================================================================
      typedef Node_                                                   ResultT ;
      typedef std::vector<std::string>                               Symbols_ ;
      // ======================================================================
    public:
      // ======================================================================
      Node ( const Symbols_* sym   ,
             const Symbols_* parts ) 
        : m_symbols   ( sym   ) 
        , m_particles ( parts ) 
      {}
      // ======================================================================
    public:
      // ======================================================================
      /// internal class to deal with scanners
      template <typename ScannerT>
      struct definition
      {
        typedef strlit<std::string::const_iterator> str_p_ ;
        typedef rule<ScannerT,NodeClosure::context_t>        node_rule       ;
        typedef stored_rule<ScannerT>                        node_stored_rule ;
        
        node_stored_rule make_rule ( const Symbols_* sym ) const 
        {
          node_stored_rule _rule ;
          if ( 0 == sym || sym->empty() ) { return _rule ; }
          for ( Symbols_::const_iterator i = sym->begin() ; 
                sym->end() != i ; ++i ) 
          {
            if ( sym->begin() == i ) { _rule = str_p_ ( i->begin() , i->end() ) ; }
            else {      _rule = _rule.copy() | str_p_ ( i->begin() , i->end() ) ; }
          }
          return _rule ;
        }        
        // the constructor: define the actual grammar
        definition ( const Node& self)
        {
          using namespace Decays::Nodes ;
          // 
          spin   = 
            ( str_p ( "JSpin" ) >> '(' >> min_limit_d(1u)[uint_p[ spin.node = construct_<JSpin>( arg1 ) ]] >> ')' ) |
            ( str_p ( "LSpin" ) >> '(' >> min_limit_d(1u)[uint_p[ spin.node = construct_<LSpin>( arg1 ) ]] >> ')' ) |
            ( str_p ( "SSpin" ) >> '(' >> min_limit_d(1u)[uint_p[ spin.node = construct_<SSpin>( arg1 ) ]] >> ')' ) ;
          
          tm = 
            ( str_p ( "LongLived_"  ) >> '(' >> real_p[ tm.node  = construct_<LongLived_>  ( arg1 ) ] >> ')') | 
            ( str_p ( "ShortLived_" ) >> '(' >> real_p[ tm.node  = construct_<ShortLived_> ( arg1 ) ] >> ')') | 
            ( str_p ( "Light"       ) >> '(' >> real_p[ tm.node  = construct_<Light>       ( arg1 ) ] >> ')') | 
            ( str_p ( "Heavy"       ) >> '(' >> real_p[ tm.node  = construct_<Heavy>       ( arg1 ) ] >> ')') ;
          
          quark  = str_p("HasQuark") >> '(' >> 
            ( str_p ( "up"      ) [ quark.node = construct_<HasQuark>( LHCb::ParticleID::up      ) ] |
              str_p ( "down"    ) [ quark.node = construct_<HasQuark>( LHCb::ParticleID::down    ) ] | 
              str_p ( "strange" ) [ quark.node = construct_<HasQuark>( LHCb::ParticleID::strange ) ] |
              str_p ( "charm"   ) [ quark.node = construct_<HasQuark>( LHCb::ParticleID::charm   ) ] |
              str_p ( "beauty"  ) [ quark.node = construct_<HasQuark>( LHCb::ParticleID::bottom  ) ] | 
              str_p ( "bottom"  ) [ quark.node = construct_<HasQuark>( LHCb::ParticleID::bottom  ) ] |  
              str_p ( "top"     ) [ quark.node = construct_<HasQuark>( LHCb::ParticleID::top     ) ] )
                                     >> ')' ;
          
          // particles:
          parts = this->make_rule ( self.m_particles ).copy() ;
          // symbols:
          symbs = this->make_rule ( self.m_symbols   ).copy() ;

          // the atomic node:
          atomic = 
            //
            spin       [ atomic.node = arg1 ] |
            // 
            tm         [ atomic.node = arg1 ] |
            // quark  
            quark      [ atomic.node = arg1 ] |
            // particles 
            parts      [ atomic.node = construct_<Pid>   ( construct_<std::string> ( arg1 , arg2 ) ) ] |
            // particles 
            ( str_p("[") >> parts [ atomic.node = construct_<CC>   ( construct_<std::string> ( arg1 , arg2 ) ) ] >> "]cc" ) |
            // symbols 
            symbs      [ atomic.node = construct_<Symbol>( construct_<std::string> ( arg1 , arg2 ) ) ] ;
          ;
          
          expression  =
            // boolean operation 
            operation [ expression.node = arg1 ] |
            // negation 1 :
            ( str_p("!") >>        expression[ expression.node = construct_<Not> ( arg1 ) ]        ) | 
            // negation 2 :
            ( str_p("~") >> '(' >> expression[ expression.node = construct_<Not> ( arg1 ) ] >> ')' ) | 
            // allow extra "()"
            ( str_p("(") >>        expression[ expression.node =                   arg1   ] >> ')' ) |
            // simple atomic expression 
            atomic    [ expression.node = arg1 ] ;
          
          operation  = str_p ( "(" ) >> 
            expression [ operation.node = arg1 ] >>
            +( ( str_p( "&" ) >> expression [ operation.node  &= arg1 ] ) | 
               ( str_p( "|" ) >> expression [ operation.node  |= arg1 ] ) ) >> ')' ;
          
          res = expression [ self.node = arg1 ] ; 
          
        }      
        // return the final contructed rule 
        const rule<ScannerT>& start() const { return res ; }
        // data members:
        //
        node_rule spin       ; // processing of  "<x>Spin ( <int> )" 
        node_rule tm         ; // processing of  "LongLived_/ShortLived_/Light/Heavy ( <double> ) "
        node_rule quark      ; // procession of  "HasQuark ( <string> ) "
        node_rule atomic     ; // the basic atomic node 
        node_rule expression ; // expresion 
        node_rule operation  ; // logical operation 
        // 
        node_stored_rule symbs ; // match the list of symbols 
        node_stored_rule parts ; // match the list of particles 
        //
        rule<ScannerT> res  ;        
        // ====================================================================
      } ; // end of deninition 
      // ======================================================================
    private :
      // ======================================================================
      mutable double       m_low       ;
      mutable double       m_high      ;
      mutable std::string  m_s_string  ;
      mutable std::string  m_p_string  ;
      const Symbols_*      m_symbols   ;
      const Symbols_*      m_particles ;
      // ======================================================================
    } ; // end of class ParNode  
    // ========================================================================
  } // end of namespace Decays::Grammars
  // ==========================================================================
} // end of namespace Decays 
// ============================================================================
// The END 
// ============================================================================
#endif // LOKI_NODEGRAMMAR_H
// ============================================================================
