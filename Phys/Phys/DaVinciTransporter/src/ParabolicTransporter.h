// $Id: ParabolicTransporter.h,v 1.2 2004-12-16 14:35:42 pkoppenb Exp $
#ifndef PARABOLICTRANSPORTER_H 
#define PARABOLICTRANSPORTER_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiAlg/GaudiTool.h"

// Gaudi interfaces
#include "GaudiKernel/IDataProviderSvc.h"

// from DaVinciTools
#include "DaVinciTools/IParticleTransporter.h"

/** @class ParabolicTransporter ParabolicTransporter.h 
 *  Transporter in magnetic field considering parabolic aproximation.
 *  Formula used is U = u + u'*dz + (1/2)*u''*dz*dz, u={x,y}, ' = d/dz(u)
 *  (see: L. Bugge and J. Myrheim , NIM 179, 1981, 365-381) and
 *  Matt Needham, LHCb-TrParabolicExtrapolator-Class.
 *
 *  @author Edgar de Oliveira
 *  @date   22/02/2002
 */

class ImagneticFieldSvc;

class ParabolicTransporter : public GaudiTool, 
                             virtual public IParticleTransporter {
public:
  /// Standard constructor
  ParabolicTransporter( const std::string& type, 
                        const std::string& name,
                        const IInterface* parent);
  
  virtual ~ParabolicTransporter( ){}; ///< Destructor
  
  /// Initialize
  StatusCode initialize();
  
  /// Transport according to parabolic extrapolation.
  StatusCode transport(ParticleVector::const_iterator &, 
                       const double znew,
                       Particle &transParticle);
  
  /// Transport according to parabolic extrapolation.
  StatusCode transport(const Particle &, 
                       const double znew,
                       Particle &transParticle);
  
  
protected:
  
private:

  /// Transport according to parabolic extrapolation, common calculations.
  StatusCode magfTransport(const Particle *&, 
                           const double znew,
                           Particle &transParticle);

  IMagneticFieldSvc* m_pIMF; ///< Pointer to the magnetic field service
  double m_tolerance;        ///< Tolerance for z estimation  

};
#endif // PARABOLICTRANSPORTER_H
