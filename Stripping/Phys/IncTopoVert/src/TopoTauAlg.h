// $Id: $
#ifndef TOPOTAUALG_H 
#define TOPOTAUALG_H 1

// Include files
#include "Kernel/IEventTupleTool.h"            // Interface
#include "GaudiKernel/ToolFactory.h" 
#include "IncTopoVert/ITopoVertexTool.h"
#include "Kernel/IParticleCombiner.h"

// from Gaudi
#include "Kernel/DaVinciAlgorithm.h"

/** @class TopoTauAlg TopoTauAlg.h
 *  
 *
 *  @author Julien Cogan, Giampiero Mancinelli
 *  @date   2013-12-17
 */
class TopoTauAlg : public DaVinciAlgorithm {
public: 
  /// Standard constructor
  typedef std::vector<LHCb::RecVertex*> RecVertexVector;

  TopoTauAlg( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~TopoTauAlg( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

  ITopoVertexTool  * m_topoVertexTool ;
  IParticleCombiner* m_combiner;
  

private:

  std::string m_VFparticlesInputLocation;
  std::string m_particlesInputLocation;
  std::string m_outputLocation;

  IDistanceCalculator*  m_Geom;
//   double m_cut_ghost;
//   double m_cut_ips_VF;
//   double m_cut_ips;
  double m_cut_ntrk;
  int             m_nEvents;            ///< Number of events
  int             m_nAccepted;          ///< Number of events accepted
  int             m_nCandidates;        ///< Number of candidates
};
#endif // TOPOTAUTALG_H
