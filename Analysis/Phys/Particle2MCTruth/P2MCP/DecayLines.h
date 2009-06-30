// $Id: DecayLines.h,v 1.1 2009-06-30 08:45:19 jpalac Exp $
#ifndef P2MCP_DECAYLINES_H 
#define P2MCP_DECAYLINES_H 1

// Include files
#include "P2MCP/P2MCPTypes.h"
#include "P2MCP/P2MCPFunctors.h"
/** @class DecayLines DecayLines.h P2MCP/DecayLines.h
 *  
 *  Class containing sorted, segmented sequences of LHCb::MCParticles.
 *  Underying data structure is a container of LHCb::MCParticle::ConstVectors.
 *  Each entry contains MCParticles that come from one line in a decay tree. 
 *  They are sorted according to position in that line, with the parent in
 *  first position and each generation following.
 *
 *  @author Juan PALACIOS
 *  @date   2009-06-30
 */

namespace P2MCP 
{
  
class DecayLines {
public: 

  inline explicit DecayLines(const LHCb::MCParticle::ConstVector& mcParticles )
    :
    m_treeSorter(),
    m_lines( m_treeSorter(mcParticles) )
  {
    
  }

  inline explicit DecayLines(const LHCb::MCParticle::Container& mcParticles )
    :
    m_treeSorter(),
    m_lines( m_treeSorter(mcParticles) )
    {
    }
  
  inline DecayLines( )
    :
    m_lines(0)
  { }

  virtual ~DecayLines( ) {} ///< Destructor

  inline size_t size() { return m_lines.size(); }

  inline bool empty() { return m_lines.empty(); }

  inline LHCb::MCParticle::ConstVector& operator[](size_t index) 
  {
    return m_lines[index];
  }

  inline P2MCP::Types::FlatTrees::const_iterator begin() 
  {
    return m_lines.begin();
  }

  inline P2MCP::Types::FlatTrees::const_iterator end() 
  {
    return m_lines.begin();
  }

private:


  
  P2MCP::Functors::SortIntoTrees<P2MCP::Functors::InTree, 
                                 P2MCP::Functors::SortInTrees > m_treeSorter;

  P2MCP::Types::FlatTrees m_lines;
};
 
} // namespace P2MCP

#endif // P2MCP_DECAYLINES_H
