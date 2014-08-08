// $Id$
#ifndef GENERICPARTICLE2PVRELATOR_H 
#define GENERICPARTICLE2PVRELATOR_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "Kernel/IRelatedPVFinder.h"            // Interface
#include "Kernel/Particle2Vertex.h"
#include <algorithm>
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
class GenericParticle2PVRelator : public GaudiTool, 
                                  virtual public IRelatedPVFinder 
{

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
    if (sc.isSuccess()) 
    {
      m_distCalculator = tool<IDistanceCalculator>(DistCalcName::value, this);
    }
    return sc;
  }
  
  const Particle2Vertex::LightWTable relatedPVs(const LHCb::Particle* particle,
                                                const LHCb::RecVertex::Range& PVs) const
  {
    return relatedPVs(particle, PVs.begin(), PVs.end() );
  }


  const Particle2Vertex::LightWTable relatedPVs(const LHCb::Particle* particle,
                                                const LHCb::RecVertex::Container& PVs) const
  {
    return relatedPVs(particle, PVs.begin(), PVs.end() );
  }
  

  const Particle2Vertex::LightWTable relatedPVs(const LHCb::Particle* particle,
                                                const LHCb::RecVertex::ConstVector& PVs) const
  {
    return relatedPVs(particle, PVs.begin(), PVs.end() );
  }

  const Particle2Vertex::LightWTable relatedPVs(const LHCb::Particle* particle,
                                                const LHCb::VertexBase::Container& PVs) const
  {
    return relatedPVs(particle, PVs.begin(), PVs.end() );
  }
  

  const Particle2Vertex::LightWTable relatedPVs(const LHCb::Particle* particle,
                                                const LHCb::VertexBase::ConstVector& PVs) const
  {
    return relatedPVs(particle, PVs.begin(), PVs.end() );
  }

  const Particle2Vertex::LightWTable relatedPVs(const LHCb::Particle* particle,
                                                const std::string& PVLocation) const
  {
    LHCb::RecVertex::Range PVs = get<LHCb::RecVertex::Range>( PVLocation );
    
    return relatedPVs(particle, PVs.begin(), PVs.end() );

  }

  virtual const LHCb::VertexBase* relatedPV(const LHCb::Particle* particle,
                                            const LHCb::RecVertex::Range& PVs) const 
  {
    return relatedPV(particle, PVs.begin(), PVs.end());
  }


  virtual const LHCb::VertexBase* relatedPV(const LHCb::Particle* particle,
                                            const LHCb::RecVertex::Container& PVs) const 
  {
    return relatedPV(particle, PVs.begin(), PVs.end());
  }
  
  virtual const LHCb::VertexBase* relatedPV(const LHCb::Particle* particle,
                                            const LHCb::RecVertex::ConstVector& PVs) const 
  {
    return relatedPV(particle, PVs.begin(), PVs.end());
  }
  
  virtual const LHCb::VertexBase* relatedPV(const LHCb::Particle* particle,
                                            const LHCb::VertexBase::Container& PVs) const 
  {
    return relatedPV(particle, PVs.begin(), PVs.end());
  }
  
  virtual const LHCb::VertexBase* relatedPV(const LHCb::Particle* particle,
                                            const LHCb::VertexBase::ConstVector& PVs) const 
  {
    return relatedPV(particle, PVs.begin(), PVs.end());
  }
  
  virtual const LHCb::VertexBase* relatedPV(const LHCb::Particle* particle,
                                            const std::string& PVLocation) const
  {
    LHCb::RecVertex::Range PVs = get<LHCb::RecVertex::Range>( PVLocation );    
    return relatedPV(particle, PVs.begin(), PVs.end() );
  }
  


private:

  template <typename Iter> 
  inline const Particle2Vertex::LightWTable relatedPVs(const LHCb::Particle* particle,
                                                       Iter begin,
                                                       Iter end     ) const
  {
    Particle2Vertex::LightWTable table;
    if (0!=particle) {
      for ( Iter iPV = begin ; iPV != end ; ++iPV){
        const double wt = BestLogic::weight(particle, *iPV, m_distCalculator);
        if (wt > std::numeric_limits<double>::epsilon() ) {
          table.i_relate(particle,*iPV, wt );
        } else {
          Warning("Weight effectively 0. PV not related.").ignore();
        }
      }
    } else {
      Warning("No particle!").ignore();
    }
    return table;
  }

  template <typename Iter>
  const LHCb::VertexBase* relatedPV(const LHCb::Particle* particle,
                                    Iter begin,
                                    Iter end ) const
  {
    const size_t nPVs = end-begin;
    if ( 1 == nPVs ) return *begin;
    if ( 0 == nPVs ) return 0;

    typedef typename std::iterator_traits<Iter>::value_type PV;
    typedef std::pair<PV, double> WeightedPV;
    typedef std::vector< WeightedPV > WeightedPVs;

    WeightedPVs weightedPVs;

    Iter iPV = begin;
    for ( ; iPV != end; ++iPV) {
      const double wt = BestLogic::weight(particle, *iPV, m_distCalculator);
      weightedPVs.push_back(  WeightedPV(*iPV, wt) );
    }

    typename WeightedPVs::const_iterator bestPV = 
      std::max_element(weightedPVs.begin(),
                       weightedPVs.end(),
                       SortByWeight<WeightedPV>());

    return bestPV->first;

  }
  
private:

  template <typename T>
  struct SortByWeight : public std::binary_function<T, T, bool>
  {
    inline bool operator() (const T& pv0, const T& pv1) 
    {
      return pv0.second < pv1.second;
    }
    
  };
  
private:

  IDistanceCalculator* m_distCalculator;

};
#endif // GENERICPARTICLE2PVRELATOR_H
