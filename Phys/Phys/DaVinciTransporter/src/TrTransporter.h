// $Id: TrTransporter.h,v 1.2 2004-12-16 14:35:42 pkoppenb Exp $
#ifndef TRTRANSPORTER_H 
#define TRTRANSPORTER_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiAlg/GaudiTool.h"

// from DaVinciTools
#include "DaVinciTools/IParticleTransporter.h"

class ITrExtrapolator ; ///< from Tr/TrKernel package
/** @class TrTransporter TrTransporter.h
 *  
 *
 *  @author Edgar C. de Oliveira
 *  @date   2002-06-14
 */
class TrTransporter : public GaudiTool,
                      virtual public IParticleTransporter {

public:
  /// Standard constructor
  TrTransporter( const std::string& type, 
                 const std::string& name,
                 const IInterface* parent);

  virtual ~TrTransporter( ){}; ///< Destructor

  virtual StatusCode initialize();

  /// Transport according to 'tr' extrapolation in "void". 
  StatusCode transport(ParticleVector::const_iterator &, 
                       const double znew,
                       Particle &transParticle);
  
  /// Transport according to 'tr' extrapolation in "void". 
  StatusCode transport(const Particle &, 
                       const double znew,
                       Particle &transParticle);

protected:

private:
  /// Internal method to transport common to all public methods
  StatusCode trTransport(const Particle *, 
                         const double znew,
                         Particle &transParticle);

  /// Tolerance for z estimation
  double m_tolerance;

  std::string      m_extrapolatorName ;
  ITrExtrapolator*  m_extrapolator;
  IParticleTransporter*  m_LinearTransporter;
};
#endif // TRTRANSPORTER_H
