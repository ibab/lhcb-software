// $Id: PrintEventAlg.h,v 1.4 2007-01-12 15:23:42 ranjard Exp $
#ifndef    GaussAlgs_PrintEventAlg_H 
#define    GaussAlgs_PrintEventAlg_H 1 

// Include files
#include "GaudiAlg/GaudiAlgorithm.h"

namespace LHCb {
  class IParticlePropertySvc;
}

/** @class PrintEventAlg.h 
 *
 *  Almost empty/primitive algorithm to print debugging info for
 *  MC event
 *
 *  @author  Witold Pokorski
 *  @author  Gloria Corti
 *  @date    2002-12-09, last modified 2007-01-11
 */
class PrintEventAlg : public GaudiAlgorithm {
public:
  /// Standard constructor
  PrintEventAlg( const std::string& name, ISvcLocator* SvcLoc ); 

  virtual ~PrintEventAlg(); ///< Destructor
  
  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

private:

  std::string    m_particles;   ///< Location in TES of MCParticles
  std::string    m_vertices;    ///< Location in TES of MCVertices

  ///Depth to with to print the MCtrees (default = 99)
  long m_depth;
  int  m_licznik;               ///< Total number of MCParticles (all events)
  int  m_liczevent;             ///< Number of events processed

  /** recursive method to print a decay tree from an MCParticle
   * @param depth  depth of tree to reach in printout
   * @param prefix prefix to add to a given line for an MCParticle
   * @param part   pointer to MCParticle to print
   */   
  void printDecayTree(long depth, const std::string& prefix,
                      const LHCb::MCParticle*);
  
  LHCb::IParticlePropertySvc* m_ppSvc; ///<< Reference to particle property service

};
#endif  // GaussAlgs_PrintEventAlg_H
