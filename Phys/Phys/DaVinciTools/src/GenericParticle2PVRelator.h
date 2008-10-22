// $Id: GenericParticle2PVRelator.h,v 1.4 2008-10-22 13:47:01 jpalac Exp $
#ifndef GENERICPARTICLE2PVRELATOR_H 
#define GENERICPARTICLE2PVRELATOR_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "Kernel/IRelatedPVFinder.h"            // Interface
#include "Kernel/Particle2Vertex.h"

class IDistanceCalculator;

/** @class GenericParticle2PVRelator GenericParticle2PVRelator.h
 *  
 * Template class with the core ingredients for an implementation of
 * the IRelatedPVFinder interface.
 *
 *  <b>Template parameters: </b>
 * 
 *  BestLogic
 *  Stateless class or struct implementing
 *  <code>
 *  static double weight(const LHCb::Particle* particle,
 *                       const LHCb::VertexBase* pv,
 *                       const IDistanceCalculator* distCalc) 
 *  </code>
 *
 *  DistCalcName
 *  Stateless struct containing
 *  const static std::string DistCalcName::value
 *  
 * 
 *  @author Juan PALACIOS
 *  @date   2008-10-15
 */
template <typename BestLogic, typename DistCalcName>
class GenericParticle2PVRelator : public GaudiTool, virtual public IRelatedPVFinder {
public: 
  /// Standard constructor
  GenericParticle2PVRelator( const std::string& type, 
                             const std::string& name,
                             const IInterface* parent)
    :
    GaudiTool ( type, name , parent )
  {
    declareInterface<IRelatedPVFinder>(this);
  }
  
  virtual ~GenericParticle2PVRelator( ) {} ///< Destructor

  StatusCode initialize() 
  {
    
    StatusCode sc( GaudiTool::initialize() );
    if (sc.isSuccess()) {
      m_distCalculator = tool<IDistanceCalculator>(DistCalcName::value);
    }
    return sc;
  }
  

  const Particle2Vertex::LightTable relatedPVs(const LHCb::Particle* particle,
                                               const LHCb::RecVertex::Container& PVs) const
  {
    return relatedPVs<>(particle, PVs.begin(), PVs.end() );
  }
  

  const Particle2Vertex::LightTable relatedPVs(const LHCb::Particle* particle,
                                               const LHCb::RecVertex::ConstVector& PVs) const
  {
    return relatedPVs(particle, PVs.begin(), PVs.end() );
  }

  const Particle2Vertex::LightTable relatedPVs(const LHCb::Particle* particle,
                                               const std::string& PVLocation) const
  {
    LHCb::RecVertex::Container* PVs = get<LHCb::RecVertices>( PVLocation );
    
    if (0!=PVs) {
      return relatedPVs(particle, PVs->begin(), PVs->end() );
    } else {
      Error("No LHcb::RecVertex::Container found at "+PVLocation).ignore();
    }
    return Particle2Vertex::LightTable();
  }

private:

  template <typename Iter> 
  const Particle2Vertex::LightTable relatedPVs(const LHCb::Particle* particle,
                                               Iter begin,
                                               Iter end     ) const
  {
    Particle2Vertex::LightTable table;
    if (0!=particle) {
      for ( Iter iPV = begin ; iPV != end ; ++iPV){
        const double wt = BestLogic::weight(particle, *iPV, m_distCalculator);
        if (wt > 0. ) {
          table.i_push(particle,*iPV, wt );
        }
      }
      table.i_sort();
    } else {
      Warning("No particle!").ignore();
    }
    return table;
  }
  
private:

  IDistanceCalculator* m_distCalculator;

};
#endif // GENERICPARTICLE2PVRELATOR_H
