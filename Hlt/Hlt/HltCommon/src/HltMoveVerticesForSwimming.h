// $Id: HltMoveVerticesForSwimming.h,v 1.6 2010-03-25 02:44:26 gligorov Exp $
#ifndef HLTMOVEVERTICESFORSWIMMING_H
#define HLTMOVEVERTICESFORSWIMMING_H 1

// Include files
// from DaVinci, this is a specialized GaudiAlgorithm
#include "HltBase/HltSelectionContainer.h"
#include "HltBase/HltAlgorithm.h"
#include "Event/Particle.h"
#include "Event/RecVertex.h"
#include "Event/VertexBase.h"
#include "Event/Vertex.h"
#include <Kernel/ILifetimeFitter.h>
#include "Kernel/IRelatedPVFinder.h"
#include <Kernel/IDistanceCalculator.h>
#include "Relations/IRelation.h"
#include "Relations/Relations.h"
/** @class HltMoveVerticesForSwimming HltMoveVerticesForSwimming.h
 *
 *
 *  @author v. Gligorov
 *  @date   2007-02-12
 */

class HltMoveVerticesForSwimming : public HltAlgorithm
{
  public:
    /// Standard constructor
    HltMoveVerticesForSwimming( const std::string& name, ISvcLocator* pSvcLocator );

    virtual ~HltMoveVerticesForSwimming(); ///< Destructor

    virtual StatusCode initialize(); ///< Algorithm initialization
    virtual StatusCode execute();    ///< Algorithm execution

  private:
    StatusCode move_PVs( const LHCb::Particle*, LHCb::VertexBase* );

    Hlt::SelectionContainer2<LHCb::RecVertex, LHCb::RecVertex> m_selections;

    double m_swimmingDistance; // How far to move the primary vertex. The sign is
                               // positive if moving in the direction of the B
                               // momentum
    double m_bLifetime; // the measured B lifetime
    double m_bLifetimeErr; // the error on the measured B lifetime
    double m_bIP; // The B IP wrt. the best PV
    double m_bIPChi2; // The B IP Chi2 wrt. the best PV
    double m_bFDChi2; // The B FD Chi2 wrt. the best PV
    double m_bDIRA; // The B pointing angle wrt. the best PV
    bool m_badEvent;

    ILifetimeFitter* m_fit;
    IDistanceCalculator* m_dist;
    IRelatedPVFinder* m_finder;

    std::string m_Bcontainer;
    std::string m_lifeToolName;
    std::string m_distToolName;
    std::string m_finderToolName;
    std::string m_offlinePVs;
};

#endif
