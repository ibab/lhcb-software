// $Id: LinearTransporter.h,v 1.3 2005-01-06 10:58:17 pkoppenb Exp $
#ifndef LINEARTRANSPORTER_H 
#define LINEARTRANSPORTER_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiAlg/GaudiTool.h"

// Gaudi interfaces
#include "GaudiKernel/IDataProviderSvc.h"

// from DaVinciTools
#include "Kernel/IParticleTransporter.h"

/** @class LinearTransporter LinearTransporter.h 
 *  Transport the particle considering
 *  only linear approximation
 *
 *  @author Edgar de Oliveira
 *  @date   22/02/2002
 */

class LinearTransporter : public GaudiTool, 
                          virtual public IParticleTransporter {

public:

  /// Initialize
  StatusCode initialize();

  /// Standard constructor
  LinearTransporter( const std::string& type, 
                     const std::string& name,
                     const IInterface* parent);
  
  virtual ~LinearTransporter( ){}; ///< Destructor
  
  /// Transport according to linear extrapolation in "void". 
  StatusCode transport(ParticleVector::const_iterator &, 
                       const double znew,
                       Particle &transParticle);
  
  /// Transport according to linear extrapolation in "void". 
  StatusCode transport(const Particle &, 
                       const double znew,
                       Particle &transParticle);
  
protected:
  
private:
  /// Internal method to transport common to all public methods
  StatusCode linTransport(const Particle *&, 
                          const double znew,
                          Particle &transParticle);

  /// In z range?
  bool inZRange(const Particle *&, const double& );

  /// Tolerance for z estimation
  double m_tolerance;                

  /// z boundaries
  double m_minz;                
  double m_maxz;                
 
};
#endif // LINEARTRANSPORTER_H
