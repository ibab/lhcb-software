// $Id: ParticleFilter.h,v 1.1 2002-03-27 20:35:00 gcorti Exp $
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
 *
 *  @author Paul colrain
 *  @date   14/03/2002
 */
class ParticleFilter : public AlgTool, virtual public IParticleFilter {

public:

  /// Standard constructor
  ParticleFilter( const std::string& type, 
                  const std::string& name,
                  const IInterface* parent);
  
  /// Desctructor
  virtual ~ParticleFilter( ){ }; 
  
  /// Initialize
  StatusCode initialize( );

  /// Filter
  StatusCode filter( const ParticleVector&, ParticleVector& ); 
  
  /// Filter positive particles
  StatusCode filterPositive( const ParticleVector&, ParticleVector& ); 
  
  /// Filter negative particles
  StatusCode filterNegative( const ParticleVector&, ParticleVector& ); 
  
protected:
  
private:

  /// The FilterCriterion names (properties)
  std::vector< std::string >     m_criteriaNames; 

  /// The Filter Criterions
  std::vector< IFilterCriterion* > m_criteria;  

};

#endif // PARTICLEFILTER_H
