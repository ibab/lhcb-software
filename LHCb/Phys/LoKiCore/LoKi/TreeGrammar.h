// $Id: TreeGrammar.h,v 1.3 2009-06-02 16:47:34 ibelyaev Exp $
// ============================================================================
#ifndef DECAYS_TREEGRAMMAR_H 
#define DECAYS_TREEGRAMMAR_H 1
// ============================================================================
// Include files
// ============================================================================
// LoKi
// ============================================================================
#include "LoKi/NodeGrammar.h"
#include "LoKi/TreeHelpers.h"
// ============================================================================
namespace Decays
{
  // ==========================================================================
  namespace Grammars 
  {
    // ========================================================================
    typedef    Decays::Parsers::Tree                                    Tree_ ;
    // ========================================================================
    /** @struct TreeClosure 
     *  helper closure class to keep/construct tree.
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2009--09-12
     */
    struct TreeClosure 
      : public boost::spirit::classic::closure <TreeClosure,Tree_>
    {
      // ======================================================================
      /// the actual holder for the node 
      member1 tree     ;                     //  the actual holder for the node 
      // ======================================================================
    };
    // ========================================================================
    /** @struct Tree
     *  grammar class to keep/construct trees 
     *  @author Vanya BELYAEV Ivan.Belyaev@nikhef.nl
     *  @date 2009-05-22
     */
    class Tree : public boost::spirit::classic::grammar 
    <Tree,TreeClosure::context_t>
    {
      // ======================================================================
    public:
      // ======================================================================
      typedef Tree_                                                   ResultT ;
      typedef std::vector<std::string>                               Symbols_ ;
      // ======================================================================
    public:
      // ======================================================================
      Tree ( const Symbols_* sym   ,
             const Symbols_* parts ) 
        : m_symbols   ( sym   ) 
        , m_particles ( parts )
        
      {}
      // ======================================================================
    public:
      // ======================================================================
      template <class ScannerT>
      struct definition
      {
        typedef rule<ScannerT,TreeClosure::context_t> tree_rule ;
        
        // the constructor: define the actual grammar
        definition ( const Tree& self)
          : node ( self.m_symbols   , 
                   self.m_particles ) 
        {
          
          head = 
            ( str_p("[") >> node [ head.tree = construct_<Tree_> ( arg1 ) ] >> "]nos" )
            [ head.tree += Decays::Trees::NotOscillated ] |  
            ( str_p("[") >> node [ head.tree = construct_<Tree_> ( arg1 ) ] >> "]os"  )
            [ head.tree += Decays::Trees::Oscillated    ] |  
            (               node [ head.tree = construct_<Tree_> ( arg1 ) ]           )  ;
          
          expression = 
            tree      [ expression.tree = arg1 ] | 
            operation [ expression.tree = arg1 ] |
            head      [ expression.tree = arg1 ] |
            ( str_p ( "(") >> expression [ expression.tree = arg1 ] >> ')') | 
            ( str_p ( "^") >> expression [ expression.tree = arg1 ] ) [ expression.tree /= true ] |
            ( str_p ( "~") >> operation  [ expression.tree = arg1 ] ) [ expression.tree *= true ] |
            ( str_p ( "~") >> tree       [ expression.tree = arg1 ] ) [ expression.tree *= true ] ;

          tree = str_p("(") 
            >> head [ tree.tree = arg1 ] 
            >> ( str_p ( "==x>" ) [ tree.tree += Decays::Trees::LongDoubleX ] |
                 str_p ( "=x>"  ) [ tree.tree += Decays::Trees::DoubleX     ] |
                 str_p ( "--x>" ) [ tree.tree += Decays::Trees::LongSingleX ] |
                 str_p ( "-x>"  ) [ tree.tree += Decays::Trees::SingleX     ] |
                 str_p ( "==>"  ) [ tree.tree += Decays::Trees::LongDouble  ] |
                 str_p ( "=>"   ) [ tree.tree += Decays::Trees::Double      ] |
                 str_p ( "-->"  ) [ tree.tree += Decays::Trees::LongSingle  ] |
                 str_p ( "->"   ) [ tree.tree += Decays::Trees::Single      ] )
            >>    expression [ tree.tree += arg1 ]  // the first child (mandatory!) 
            >> *( expression [ tree.tree += arg1 ] |                           // children 
                  ( str_p("{") >> expression [ tree.tree %= arg1 ] >> '}' ) )  // optional
            >> !( str_p("...") [ tree.tree += true ]        )                  // inclusive 
            >> ')' ;
          
          operation  = 
            ( str_p ( "(" ) 
              >> expression [ operation.tree = arg1 ] 
              >> +( ( str_p( "&&" ) >> expression [ operation.tree  &= arg1 ] ) | 
                    ( str_p( "||" ) >> expression [ operation.tree  |= arg1 ] ) ) 
              >> ')' ) | 
            ( str_p ( "[" ) 
              >> expression [ operation.tree = arg1 ] 
              >> +( str_p( "," ) >> expression [ operation.tree  |= arg1 ] )
              >> ']' ) ;
              
          res = expression [ self.tree = arg1 ] ; 
          
        }      
        // return the final contructed rule 
        const rule<ScannerT>& start() const { return res ; }
        // data members:
        //
        Node        node       ;                         //            the node 
        
        tree_rule   head       ;                         //      the decay head 
        tree_rule   expression ;                         //          expression 
        tree_rule   tree       ;                         //       the full tree
        tree_rule   operation  ;                         //  logical operations 
        //
        rule<ScannerT> res   ;                           //    the final result
        // ====================================================================
      } ; // end of deninition 
      // ======================================================================
    private:
      // ======================================================================
      /// symbols 
      const Symbols_* m_symbols   ;                               //   symbols 
      /// particles 
      const Symbols_* m_particles ;                               // particles 
      // ======================================================================
    } ; // end of class Tree
    // ========================================================================
  } // end of namespace Grammars 
  // ==========================================================================
} // end of namespace Decays 
// ============================================================================
// The  END 
// ============================================================================
#endif // DECAYS_TREEGRAMMAR_H
// ============================================================================
