// $Id: LinearTransporter.h,v 1.1 2002-05-15 23:18:06 gcorti Exp $
#ifndef LINEARTRANSPORTER_H 
#define LINEARTRANSPORTER_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/AlgTool.h"

// Gaudi interfaces
#include "GaudiKernel/IDataProviderSvc.h"

// from DaVinciTools
#include "DaVinciTools/IParticleTransporter.h"

/** @class LinearTransporter LinearTransporter.h 
 *  Transport the particle considering
 *  only linear approximation
 *
 *  @author Edgar de Oliveira
 *  @date   22/02/2002
 */

class LinearTransporter : public AlgTool, 
                          virtual public IParticleTransporter {

public:
  /// Standard constructor
  LinearTransporter( const std::string& type, 
                     const std::string& name,
                     const IInterface* parent);
  
  virtual ~LinearTransporter( ){}; ///< Destructor
  
  /// Transport according to linear extrapolation in "void". 
  StatusCode transport(Particles::const_iterator &, 
                       double znew,
                       Particle &transParticle);
  
  /// Transport according to linear extrapolation in "void". 
  StatusCode transport(const Particle &, 
                       double znew,
                       Particle &transParticle);
  
  /// Transport according to linear extrapolation in "void". 
  StatusCode transport(Particle &, 
                       double znew,
                       Particle &transParticle);  
protected:
  
private:
  /// Internal method to transport common to all public methods
  StatusCode linTransport(Particle *&, 
                          double znew,
                          Particle &transParticle);

  /// Tolerance for z estimation
  double m_tolerance;                
 
};
#endif // LINEARTRANSPORTER_H
