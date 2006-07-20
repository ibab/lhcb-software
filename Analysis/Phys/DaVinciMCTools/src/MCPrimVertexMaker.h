// $Id: MCPrimVertexMaker.h,v 1.2 2006-07-20 11:27:30 jpalac Exp $
#ifndef MCPRIMVERTEXMAKER_H 
#define MCPRIMVERTEXMAKER_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"
#include "GaudiKernel/RndmGenerators.h"

/** @class MCPrimVertexMaker MCPrimVertexMaker.h
 *  
 *  Produces a primary vertex from primary MC vertices
 *
 *  @author Patrick Koppenburg
 *  @date   2006-05-03
 */
class IVisPrimVertTool;
class MCPrimVertexMaker : public GaudiAlgorithm {
public: 
  /// Standard constructor
  MCPrimVertexMaker( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~MCPrimVertexMaker( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

private:
  StatusCode makePV(const LHCb::MCVertex*, LHCb::RecVertex*) ;

private:
  double m_xSmear ; ///< Error in x per sqrt(tk)
  double m_ySmear ; ///< Error in y per sqrt(tk)
  double m_zSmear ; ///< Error in z per sqrt(tk)
  double m_onlyVisible ; ///< Use only visible vertices and tracks
   
  long m_prods ; ///< total products
  long m_vtx ; ///< total MC pvs
  long m_evts ; ///< number of events
  long m_vis ; ///< number of visible PVs
  
  Rndm::Numbers m_ranGauss;
  IVisPrimVertTool* m_visPV;
};
#endif // MCPRIMVERTEXMAKER_H
