// $Id: ParticleFilter.h,v 1.4 2003-06-26 16:51:51 gcorti Exp $
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

// Forward declarations
class IHistogram1D;

/** @class ParticleFilter ParticleFilter.h
 *  Given a vector of Particles, provides a sub-vector of Particles 
 *  satisfying a set of FilterCriteria
 *  @author Paul colrain
 *  @date   14/03/2002
 *  Sandra Amato: Modified to include counters 
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

  /// Finalize: Release all the Filter Criterion Tools. 
  StatusCode finalize( );

  /// Apply the filter to the input vector of Particles and outputs the 
  /// result in another vector.
  StatusCode filter( const ParticleVector&, ParticleVector& ); 
  
  /// Apply the filter and selects positive particles.
  StatusCode filterPositive( const ParticleVector&, ParticleVector& ); 
  
  /// Apply the filter and selects negative particles.
  StatusCode filterNegative( const ParticleVector&, ParticleVector& ); 


  /// Return the number of events that passed each criterium independently.
  std::vector< int > independentCounters();
  
  /// Return the number of events that passed each criterium cumulatively.
  std::vector< int > cumulativeCounters();


protected:
  
private:

  /// The FilterCriterion names (properties)
  std::vector< std::string >     m_criteriaNames; 

  /// The Filter Criterions
  std::vector< IFilterCriterion* > m_criteria;  

	/// Counter of events that passed each criterium independently
  std::vector< int > m_independentCounter;  

	/// Counter of events that passed each criterium cumulatively
  std::vector< int > m_cumulativeCounter;

  bool                  m_produceHistogram; ///< flag for histo production
  IHistogram1D*         m_hIndCounter;
  IHistogram1D*         m_hCumCounter;  

};

#endif // PARTICLEFILTER_H
