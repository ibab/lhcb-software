// $Id: P2MCPFunctors.h,v 1.2 2009-04-28 11:42:51 jpalac Exp $
#ifndef P2MCP_P2MCPFUNCTORS_H 
#define P2MCP_P2MCPFUNCTORS_H 1

// Include files
#include "P2MCP/P2MCPTypes.h"
#include "LoKi/BuildMCTrees.h"
#include <algorithm>
#include <functional>
/** @namespace P2MCP::Functors P2MCP/P2MCPFunctors.h
 *  
 *
 *  @author Juan Palacios
 *  @date   2009-03-13
 */
namespace P2MCP{
  namespace Functors {
    
    struct SortInTrees : public std::binary_function<const LHCb::MCParticle*,const LHCb::MCParticle* , bool>
    {
    
      bool operator() ( const LHCb::MCParticle* p1 , 
                        const LHCb::MCParticle* p2 ) const
      {
        LoKi::MCParticles::FromMCDecayTree fromDecay( p1 ) ;
        return fromDecay ( p2 );
      }
    };


    struct InTree : public std::unary_function<const LHCb::MCParticle*, bool>
    {

      InTree(const LHCb::MCParticle* head) : m_head(head) { }
    
      bool operator() (const LHCb::MCParticle* mc) const 
      {
        P2MCP::Functors::SortInTrees sorter;
        return sorter(m_head, mc);
      }
    private:
      InTree() { }
    
    private:
      const LHCb::MCParticle* m_head;
    
    };

    template <class Selector, class Sorter>
    struct SortIntoTrees
    {

      template <class InputContainer>
      P2MCP::Types::FlatTrees operator() (const InputContainer& mcps) const
      {
        LHCb::MCParticle::ConstVector heads = LoKi::MCTrees::buildTrees(mcps);

        P2MCP::Types::FlatTrees output;

        LHCb::MCParticle::ConstVector::iterator itHead = heads.begin();

        for ( ; itHead != heads.end() ; ++itHead) {
          Selector selector(*itHead);
          P2MCP::Types::FlatTree tree;
          std::copy_if(mcps.begin(), 
                       mcps.end(), 
                       std::back_inserter(tree), 
                       std::cref(selector));
          std::stable_sort( tree.begin() , 
                            tree.end() , 
                            Sorter() ) ;
          output.push_back(tree);
        }
        return output;
      }
    };

  }
  
}
#endif // P2MCP_P2MCPFUNCTORS_H
