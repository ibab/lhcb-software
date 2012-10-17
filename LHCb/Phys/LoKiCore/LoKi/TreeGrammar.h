// ============================================================================
#ifndef LOKI_TREEGRAMMAR_H 
#define LOKI_TREEGRAMMAR_H 1
// ============================================================================
// Include files
// ============================================================================
#include "LoKi/NodeGrammar.h"
#include "LoKi/TreeHelpers.h"
// ============================================================================
/** @file LoKi/TreeGrammar.h
 *
 *  The grammars for decay trees, 
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
    // ========================================================================
    namespace sp  = boost::spirit;
    namespace ph  = boost::phoenix;
    namespace qi  = sp::qi;
    namespace enc = sp::ascii;
    namespace dt  = Decays::Trees;
    // ========================================================================
    // Typedefs
    // ========================================================================
    typedef Decays::Parsers::Tree Tree_;
    // =========================================================================
    /** @class Tree
     *  Main grammar for decay trees 
     *  @author Alexander Mazurov
     *  @date   2011-12-11
     */
    template<typename Iterator, typename Skipper>
    struct Tree: qi::grammar<Iterator, Tree_(), Skipper> 
    {
      // ======================================================================
      typedef Tree_ ResultT;
      typedef std::vector<std::string> VectorOfStringsT;
      // ======================================================================
      struct Operations
      {
        template <typename A, typename B = boost::fusion::unused_type,
                  typename C = boost::fusion::unused_type,
                  typename D = boost::fusion::unused_type>
        struct result { typedef void type; };
        // ====================================================================
        void operator()( Tree_&       res   , 
                         const Node_& value , 
                         bool stable=false  ) const
        {
          res = Tree_(value, stable);
        }    
        // ====================================================================
      };
      // ======================================================================
      Tree ( const VectorOfStringsT& symbols   , 
             const VectorOfStringsT& particles )
        : Tree::base_type(res)
        , node( symbols , particles ) 
      {
        // ====================================================================
        res %= expression;
        // ====================================================================
        expression %= tree | operation | head | unary;
        // ====================================================================
        head = 
          ( qi::lit("[") 
            >>  node [op(qi::_val, qi::_1)][qi::_val += dt::NotOscillated] 
            >> "]nos"
            )
          |
          ( qi::lit("[") 
            >>  node [op(qi::_val, qi::_1)][qi::_val += dt::Oscillated] 
            >> "]os"
            ) 
          |
          ( qi::lit("<") >> node[op(qi::_val, qi::_1, true)] >> ">")
          |
          node[op(qi::_val, qi::_1)]
          ;
        // ====================================================================
        operation  = 
          ( qi::lit("(") 
            >> expression [qi::_val = qi::_1] 
            >> +(
                 ("&&" >> expression[qi::_val &= qi::_1]) 
                 | 
                 ("||" >> expression[qi::_val |= qi::_1])
                 ) 
            >> ')' 
            ) 
          | 
          ( qi::lit("[") 
            >> expression[qi::_val = qi::_1] 
            >> +("," >> expression[qi::_val |= qi::_1])
            >> "]"
            ) ;
        // ====================================================================
        tree = 
          qi::lit("(") 
          >> head [ qi::_val = qi::_1] 
          >>  ( qi::lit("==x>")[qi::_val += Decays::Trees::LongDoubleX] |
                qi::lit("=x>")[qi::_val += Decays::Trees::DoubleX] |
                qi::lit("--x>")[qi::_val += Decays::Trees::LongSingleX] |
                qi::lit("-x>")[qi::_val += Decays::Trees::SingleX] |
                qi::lit("==>")[qi::_val += Decays::Trees::LongDouble] |
                qi::lit("=>")[qi::_val += Decays::Trees::Double] |
                qi::lit("-->")[qi::_val += Decays::Trees::LongSingle] |
                qi::lit("->")[qi::_val += Decays::Trees::Single] 
                )
          >> expression[qi::_val += qi::_1] // the first child (mandatory!) 
          >> *( expression[qi::_val += qi::_1] 
                |                           // children 
                (qi::lit("{") >> expression [qi::_val %= qi::_1] >> "}" ) 
                )  // optional
          >> -(qi::lit("...")[qi::_val += true]) // inclusive 
          >> ')' ;   
        unary = 
          (qi::lit("(") >> expression[qi::_val = qi::_1] >> ")") 
          | 
          ("^" >> expression[qi::_val = qi::_1][qi::_val /= true ])
          |
          ("~" >> operation[qi::_val = qi::_1][qi::_val *= true ]) 
          |
          ("~" >> tree[qi::_val = qi::_1][qi::_val *= true ]);  
        // ====================================================================
      }
        // ====================================================================
        qi::rule<Iterator, Tree_(), Skipper> 
          res        , 
          expression , 
          tree       , 
          head       , 
          operation  ,
          unary      ;
        Node<Iterator, Skipper>  node ;    
        ph::function<Operations> op   ;
        // ====================================================================
  } ;
    // ========================================================================
  } //                                        end of namespace Decays::Grammars
  // ==========================================================================
} //                                                    end of namespace Decays 
// ============================================================================
//                                                                      The END 
// ============================================================================
#endif // LOKI_TREEGRAMMAR_H
// ============================================================================
