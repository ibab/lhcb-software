// $Id: ParticleFilter.h,v 1.2 2002-05-15 23:26:20 gcorti Exp $
#ifndef PARTICLEFILTER_H 
#define PARTICLEFILTER_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/AlgTool.h"

// from DaVinciTools
#include "DaVinciTools/IParticleFilter.h"
#include "DaVinciTools/IFilterCriterion.h"

/** @class ParticleFilter ParticleFilter.h
 *  Given a vector of Particles, provides a sub-vector of Particles 
 *  satisfying a set of FilterCriteriums
 *  @author Paul colrain
 *  @date   14/03/2002
 */
class ParticleFilter : public AlgTool, 
                       virtual public IParticleFilter {

public:

  /// Standard constructor
  ParticleFilter( const std::string& type, 
                  const std::string& name,
                  const IInterface* parent);
  
  /// Desctructor
  virtual ~ParticleFilter( ){ }; 
  
  /// Initialize: Retrieve all the Filter Criterion Tools. 
  StatusCode initialize( );

  /// Apply the filter to the input vector of Particles and outputs the 
  /// result in another vector.
  StatusCode filter( const ParticleVector&, ParticleVector& ); 
  
  /// Apply the filter and selects positive particles.
  StatusCode filterPositive( const ParticleVector&, ParticleVector& ); 
  
  /// Apply the filter and selects negative particles.
  StatusCode filterNegative( const ParticleVector&, ParticleVector& ); 
  
protected:
  
private:

  /// The FilterCriterion names (properties)
  std::vector< std::string >     m_criteriaNames; 

  /// The Filter Criterions
  std::vector< IFilterCriterion* > m_criteria;  

};

#endif // PARTICLEFILTER_H
