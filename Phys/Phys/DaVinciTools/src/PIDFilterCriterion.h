// $Id: PIDFilterCriterion.h,v 1.2 2002-05-15 23:26:59 gcorti Exp $
#ifndef PIDFILTERCRITERION_H 
#define PIDFILTERCRITERION_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/AlgTool.h"

// from DaVinciTools
#include "DaVinciTools/IFilterCriterion.h"

// Forward declarations
class IParticlePropertySvc;

/** @class PIDFilterCriterion PIDFilterCriterion.h 
 *  Returns a yes/no depending on the particle ID and their confidence level. 
 *  @author Paul colrain
 *  @date   14/03/2002
 */
class PIDFilterCriterion : public AlgTool,
                           virtual public IFilterCriterion {

public:

  /// Standard constructor
  PIDFilterCriterion( const std::string& type, 
                      const std::string& name,
                      const IInterface* parent);
  
  /// Desctructor
  virtual ~PIDFilterCriterion( ){ };

  /// Initialize
  StatusCode initialize( );

  /// Test if Particle ID Confidence Level filter is satisfied
  inline bool isSatisfied( const Particle* const & part );

  /// Test if Particle ID Confidence Level filter is satisfied  
  inline bool operator()( const Particle* const & part );

private:

  std::vector< std::string > m_partNames;  ///< Particle names
  std::vector< int >         m_partCodes;  ///< Particle codes

  /// Minimum Particle hypothesis confidence levels
  std::vector< double >      m_confLevels; 

  /// Pointer to the ParticlePropertySvc
  IParticlePropertySvc* m_ppSvc;   

};

#endif // PIDFILTERCRITERION_H
