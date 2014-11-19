#ifndef LUMICOUNTVELOWITHZRCUTS_H
#define LUMICOUNTVELOWITHZRCUTS_H 1

#include "GaudiAlg/GaudiAlgorithm.h"
#include "GaudiKernel/Point3DTypes.h"
#include <cmath>

/** @class LumiCountVeloWithZRCuts LumiCountVeloWithZRCuts.h
 *
 *  @author Vladislav Balagura
 *  @date   2011-09-20
 *
 *  (copied from LumiCountMuons)
 */
class LumiCountVeloWithZRCuts : public GaudiAlgorithm
{
  public:
    LumiCountVeloWithZRCuts( std::string name, ISvcLocator* pSvcLocator );
    ~LumiCountVeloWithZRCuts() override = default;

    StatusCode initialize() override;
    StatusCode execute() override;

  private:
    std::string m_TrackCounterName, m_VertexCounterName;
    std::string m_TrackInputSelectionName, m_VertexInputSelectionName;
    std::string m_OutputContainerName;
    double m_AbsZCut, m_RCut;
    int m_TrackCounter, m_VertexCounter;

    inline bool fiducial(const Gaudi::XYZPoint&  p) const {
         return std::fabs( p.z() )   < m_AbsZCut &&
                           p.Perp2() < m_RCut * m_RCut;
    }

    template <typename T, typename P> int count_if(const std::string& loc, P predicate) const {
        auto* c = getIfExists<T>( loc );
        return c ? std::count_if( std::begin( *c ), std::end( *c ), predicate )
                 : -1;
    }
};
#endif
