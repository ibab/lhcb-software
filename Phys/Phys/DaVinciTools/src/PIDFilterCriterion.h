// $Id: PIDFilterCriterion.h,v 1.1 2002-03-27 20:35:00 gcorti Exp $
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
 *
 *  @author Paul colrain
 *  @date   14/03/2002
 */
class PIDFilterCriterion : public AlgTool, virtual public IFilterCriterion {

public:

  /// Standard constructor
  PIDFilterCriterion( const std::string& type, 
                      const std::string& name,
                      const IInterface* parent);
  
  /// Desctructor
  virtual ~PIDFilterCriterion( ){ };

  /// Initialize
  StatusCode initialize( );

  /// Test if filter is satisfied
  inline bool isSatisfied( const Particle* const & part );

  /// Test if filter is satisfied  
  inline bool operator()( const Particle* const & part );

private:

  /// Particle names
  std::vector< std::string > m_partNames;

  /// Particle codes
  std::vector< int >         m_partCodes;

  /// Minimum Particle hypothesis confidence levels
  std::vector< double >      m_confLevels;

  /// Pointer to the ParticlePropertySvc
  IParticlePropertySvc* m_ppSvc;

};

#endif // PIDFILTERCRITERION_H
