// $Id: HltFilterFittedVertices.h,v 1.1 2010-05-19 04:05:36 gligorov Exp $
#ifndef HLTFILTERFITTEDVERTICES_H 
#define HLTFILTERFITTEDVERTICES_H 1

// Include files
// from DaVinci, this is a specialized GaudiAlgorithm
#include "HltBase/HltSelectionContainer.h"
#include "HltBase/HltAlgorithm.h"
#include "Event/Particle.h"
#include "Kernel/IVertexFit.h"
#include <Kernel/IDistanceCalculator.h>
#include "Kernel/IParticle2State.h"

/** @class HltFilterFittedVertices HltFilterFittedVertices.h 
 *  
 *
 *  @author V. Gligorov
 *  @date   2010-05-14
 */

class HltFilterFittedVertices : public HltAlgorithm {
public: 
  /// Standard constructor
  HltFilterFittedVertices( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~HltFilterFittedVertices(); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization


protected:

  double m_minVDChi2;

  IVertexFit* m_vertexFitter;
  const IDistanceCalculator* m_dist;
  const IParticle2State* m_p2s ;

  Hlt::SelectionContainer3<LHCb::RecVertex,LHCb::RecVertex,LHCb::RecVertex> m_selections;

  AIDA::IHistogram1D* histochi2;
  AIDA::IHistogram1D* histodist;
  AIDA::IHistogram1D* histochi2best;
  AIDA::IHistogram1D* histodistbest;

};

#endif // HLTFILTERFITTEDVERTICES_H
