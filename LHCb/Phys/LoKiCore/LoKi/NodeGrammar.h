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
#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_fusion.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/fusion/include/unused.hpp>
// ============================================================================
/** @file 
 *
 *  The grammars for decay tree nodes, 
 *  reimplementation using Boost.Spirit version 2.
 *
 *  This file is a part of LoKi project - 
 *  ``C++ ToolKit  for Smart and Friendly Physics Analysis''
 *
 *  The package has been designed with the kind help from
 *  Galina PAKHLOVA and Sergey BARSUK.  Many bright ideas, 
 *  contributions and advices from G.Raven, J.van Tilburg, 
 *  A.Golutvin, P.Koppenburg have been used in the design.
 *
 *  @author Alexander Mazurov
 *  @date   2011-12-11
 */
// ============================================================================
namespace Decays 
{ 
  // ==========================================================================
  namespace Grammars
  {
    // ========================================================================
    // Namespace Aliases
    namespace sp  = boost::spirit;
    namespace ph  = boost::phoenix;
    namespace qi  = sp::qi;
    namespace enc = sp::ascii;
    namespace dn  = Decays::Nodes;
    //namespace rep = sp::repository;  
    // Typedefs
    typedef Decays::Nodes::_Node Node_;
    //typedef Decays::NodeList NodeList_ ;  
    struct quarks : qi::symbols<char, LHCb::ParticleID::Quark> 
    {
      // Constructor
      quarks() {
        add("up",       LHCb::ParticleID::up)
          ("down",     LHCb::ParticleID::down)
          ("strange",  LHCb::ParticleID::strange)
          ("charm",    LHCb::ParticleID::charm)
          ("beauty",   LHCb::ParticleID::bottom)
          ("bottom",   LHCb::ParticleID::bottom)
          ("top",      LHCb::ParticleID::top);
      }
    };
    // ========================================================================
    /** @class Node 
     *  Main grammar foe decay nodes 
     *  @author Alexander Mazurov
     *  @date   2011-12-11
     */
    template<typename Iterator, typename Skipper>
    struct Node: qi::grammar<Iterator, Node_(), Skipper> 
    {
      // ======================================================================
      typedef Node_ ResultT;
      typedef std::vector<std::string> VectorOfStringsT;
      // ======================================================================
      struct tag_pid        {} ;
      struct tag_symbol     {} ;
      struct tag_cc         {} ;
      struct tag_jspin      {} ;
      struct tag_lspin      {} ;
      struct tag_sspin      {} ;
      struct tag_longlived  {} ;
      struct tag_shortlived {} ;
      struct tag_light      {} ;
      struct tag_heavy      {} ;
      struct tag_quarks     {} ;
      struct tag_not        {} ;
      // ======================================================================
      struct Operations
      {
        // ====================================================================
        template <typename A, typename B = boost::fusion::unused_type,
                  typename C = boost::fusion::unused_type,
                  typename D = boost::fusion::unused_type>
        struct result { typedef void type; };
        // ====================================================================
        void operator()(Node_& res, const std::string& value, tag_pid) const
        {
          res = dn::Pid(value);
        }
        // ====================================================================
        void operator()(Node_& res, const std::string& value, tag_symbol) const
        {
          res = dn::Symbol(value);
        }
        // ====================================================================
        void operator()(Node_& res, const std::string& value, tag_cc) const
        {
          res = dn::CC(value);
        }
        // ====================================================================
        void operator()(Node_& res, int value, tag_jspin) const
        {
          res = dn::JSpin(value);
        }
        // ====================================================================
        void operator()(Node_& res, int value, tag_lspin) const
        {
          res = dn::LSpin(value);
        }
        // ====================================================================
        void operator()(Node_& res, int value, tag_sspin) const
        {
          res = dn::SSpin(value);
        }
        // ====================================================================
        void operator()(Node_& res, double value, tag_longlived) const
        {
          res = dn::LongLived_(value);
        }
        // ====================================================================
        void operator()(Node_& res, double value, tag_shortlived) const
        {
          res = dn::ShortLived_(value);
        }
        // ====================================================================
        void operator()(Node_& res, double value, tag_light) const
        {
          res = dn::Light(value);
        }
        // ====================================================================
        void operator()(Node_& res, double value, tag_heavy) const
        {
          res = dn::Heavy(value);
        }
        // ====================================================================
        void operator()(Node_& res  , 
                        const LHCb::ParticleID::Quark& value, 
                        tag_quarks  ) const
        {
          res = dn::HasQuark(value);
        }
        // ====================================================================        
        void operator()(Node_& res, const Node_& value, tag_not) const 
        {
          res = dn::Not(value);
        }  
        // ====================================================================
      };
      // ======================================================================
      void make_symbols( qi::symbols<char,std::string>& output, 
                         const VectorOfStringsT&        symbols) 
      {
        for(VectorOfStringsT::const_iterator curr = symbols.begin();
            curr != symbols.end(); ++curr) {
          //symbols_.add(curr->c_str(), curr->c_str());
          //std::cerr << "-> " << *curr << std::endl;
          output.add(curr->c_str(), *curr);
        }
        //std::cerr << "<-" << std::endl;
      }
      // ======================================================================
      Node ( const VectorOfStringsT& symbols   , 
             const VectorOfStringsT& particles ) 
        : Node::base_type(res) 
      {
        //
        make_symbols ( parts , particles ) ;
        //
        make_symbols ( symbs , symbols   ) ;
        //
        res %= expression;  
        //
        expression  =
          // boolean operation 
          operation[qi::_val = qi::_1]
          |
          // negation 1 :
          ("!" >> expression[op(qi::_val, qi::_1, tag_not())]) 
          | 
          // negation 2 :
          (qi::lit("~") >> 
           "(" >> expression[op(qi::_val, qi::_1, tag_not())] >> ")") 
          | 
          // allow extra "()"
          ("(" >> expression[qi::_val = qi::_1] >> ")" ) |
          // simple atomic expression 
          atomic[qi::_val = qi::_1] ;
        // 
        operation  = 
          ( qi::lit( "(" ) 
            >> 
            expression[qi::_val = qi::_1] >>
            +(('&' >> expression[qi::_val &= qi::_1]) 
              | 
              ("|" >> expression[qi::_val |= qi::_1])) 
            >> ")"
            ) 
          |
          ("[" 
           >> expression[qi::_val = qi::_1] 
           >> +("," >> expression[qi::_val |= qi::_1])
           >> 
           "]"
           );
        // 
        atomic =  
          spin  [qi::_val = qi::_1]
          |
          tm    [qi::_val = qi::_1]
          | 
          qs    [op(qi::_val, qi::_1, tag_quarks())]
          |
          parts [op(qi::_val, qi::_1, tag_pid())] 
          |
          ("[" >> parts[op(qi::_val, qi::_1, tag_cc())] >> "]cc")
          | 
          symbs[op(qi::_val, qi::_1, tag_symbol())];
        
        spin = 
          ( qi::lit("JSpin") >> '(' 
            >> min_limit(1)[op(qi::_val, qi::_1, tag_jspin())] >> ')') 
          |
          ( qi::lit("LSpin") >> '(' 
            >> min_limit(1)[op(qi::_val, qi::_1, tag_lspin())] >> ')')
          |
          ( qi::lit("SSpin") >> '(' 
            >> min_limit(1)[op(qi::_val, qi::_1, tag_sspin())] >> ')');
        
        tm = 
          ( qi::lit("LongLived_") >> '(' 
            >> qi::double_[op(qi::_val, qi::_1, tag_longlived())] >> ')') 
          | 
          ( qi::lit("ShortLived_") >> '(' 
            >> qi::double_[op(qi::_val, qi::_1, tag_shortlived())] >> ')')
          | 
          ( qi::lit("Light") >> '(' 
            >> qi::double_[op(qi::_val, qi::_1, tag_light())] >> ')') 
          | 
          ( qi::lit("Heavy") >> '(' 
            >> qi::double_[op(qi::_val, qi::_1, tag_heavy())] >> ')');
        
        min_limit %=   qi::int_[qi::_a = qi::_1] >>
          qi::eps( qi::_a >= qi::_r1 && qi::_a >= 0);                 
      }
        // ====================================================================
        qi::rule<Iterator, Node_(), Skipper> 
          res        , 
          atomic     ,  
          spin       , 
          tm         , 
          expression ,
          operation  ;
        qi::symbols<char,std::string> symbs , parts;
        qi::rule<Iterator,int(int), qi::locals<int>, Skipper>  min_limit;
        
        quarks qs;
        ph::function<Operations> op;
        // ====================================================================
    };
    // ========================================================================
  } //                                        end of namespace Decays::Grammars
  // ==========================================================================
} //                                                    end of namespace Decays
// ============================================================================
//                                                                      The END 
// ============================================================================
#endif // LOKI_NODEGRAMMAR_H
// ============================================================================
