// $Id: TestDST.h,v 1.2 2007-02-21 09:12:49 ukerzel Exp $
#ifndef TESTDST_H 
#define TESTDST_H 1

// Include files
// from DaVinci, this is a specialized GaudiAlgorithm
#include "Kernel/DVAlgorithm.h"

#include <GaudiKernel/IRegistry.h>

#include "Linker/LinkerTool.h"

#include "DaVinciAssociators/Particle2MCLinker.h"


/** @class TestDST TestDST.h
 *   
 *
 *  @author Ulrich Kerzel
 *  @date   2007-02-06
 */
class TestDST : public DVAlgorithm {
public: 
  /// Standard constructor
  TestDST( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~TestDST( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

private:

  typedef LinkerTool< LHCb::Particle, LHCb::RecVertex > Part2Vertex;
  typedef Part2Vertex::DirectType                       Part2VertexTable;
  typedef Part2VertexTable::Range                       Part2VertexRange;
  typedef Part2VertexTable::iterator                    Part2VertexIterator;

  typedef LinkerTool< LHCb::Track, LHCb::MCParticle >   Track2MCPart;
  typedef Track2MCPart::DirectType                      Track2MCPartTable;
  typedef Track2MCPartTable::Range                      Track2MCPartRange;
  typedef Track2MCPartTable::iterator                   Track2MCPartIterator;


  StatusCode PrintDefaultPV();
  StatusCode PrintRelatedPV(const LHCb::Particle* particle);
  StatusCode PrintParticleInfo(const LHCb::Particle* particle);
  StatusCode PrintParticles();

  StatusCode PrintMCParticles();        ///< print all MC particles found in container
  StatusCode PrintMCVertices();         ///<              vertices
  StatusCode PrintMCParticle(const LHCb::Particle* particle);

  std::vector<const LHCb::MCParticle*> GetMCParticle (const LHCb::Particle* particle,
                                                      Particle2MCLinker* particleLinker);


  inline std::string objectLocation(const DataObject *pObject) const 
  {
    return (!pObject ? "Null DataObject" :
            (pObject->registry() ? pObject->registry()->identifier() : "UnRegistered"));
  }


  bool        m_printPVtracks;            ///< print information about tracks at PV
  bool        m_printCaloElectrons;
  bool        m_printCaloPRS;     
  bool        m_printMCContainer;


  std::string        m_nameMCAsct;                ///< Name of MC associator
  Particle2MCLinker* m_part2MCLinkerComposite ;   ///< associator using composite method
  Particle2MCLinker* m_part2MCLinkerLinks     ;   ///< associator       links
};
#endif // TESTDST_H
