// $Id: CombinedTransporter.h,v 1.2 2004-12-16 14:35:42 pkoppenb Exp $
#ifndef COMBINEDTRANSPORTER_H 
#define COMBINEDTRANSPORTER_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiAlg/GaudiTool.h"

// from DaVinciTools
#include "DaVinciTools/IParticleTransporter.h"

/** @class CombinedTransporter CombinedTransporter.h 
 *  Transport the particle with one of the following possibilities:
 *  
 *  1) linear or multiple scattering in Velo region (-500. < z < 950.)
 *  
 *  2) linear or magnetic field in parabolic approximation out of the
 *     Velo region
 *
 *  @author Edgar de Oliveira
 *  @date   22/02/2002
 */

class CombinedTransporter : public GaudiTool, 
                            virtual public IParticleTransporter {
public:
  /// Standard constructor
  CombinedTransporter( const std::string& type, 
                       const std::string& name,
                       const IInterface* parent);
  
  virtual ~CombinedTransporter( ){}; ///< Destructor
  
  /// Initialize
  StatusCode initialize();

  /// Transport with linear or parabolic extrapolations depending
  /// on regions of the detector.  
  StatusCode transport(ParticleVector::const_iterator &, 
                       const double znew,
                       Particle &transParticle);
  
  /// Transport with linear or parabolic extrapolations depending
  /// on regions of the detector.  
  StatusCode transport(const Particle &, 
                       const double znew,
                       Particle &transParticle);
  
 
protected:
  
private:
  
  /// Type of Transporter to be used in Velo region (property).   
  std::string pVeloTransporterName;
  /// Type of Transporter to be used in Magnetic Field region (property).
  std::string pMFTransporterName;
  /// Pointer to transporter to use in Velo region.
  IParticleTransporter* m_pVeloTransporter;
  /// Pointer to transporter to use Magnetic field region.
  IParticleTransporter* m_pMFTransporter;  

};
#endif // COMBINEDTRANSPORTER_H
