// $Id: ParticleTransporter.h,v 1.1 2002-03-27 20:35:01 gcorti Exp $
#ifndef PARTICLETRANSPORTER_H 
#define PARTICLETRANSPORTER_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/AlgTool.h"

// Gaudi interfaces
#include "GaudiKernel/IDataProviderSvc.h"

// from DaVinciTools
#include "DaVinciTools/IParticleTransporter.h"

/** @class ParticleTransporter ParticleTransporter.h 
 *  
 *
 *  @author Edgar de Oliveira
 *  @date   22/02/2002
 */

class ParticleTransporter : public AlgTool,
                            virtual public IParticleTransporter {
public:
  /// Standard constructor
  ParticleTransporter( const std::string& type, 
                       const std::string& name,
                       const IInterface* parent);
  
  virtual ~ParticleTransporter( ){}; ///< Destructor
  
  /// The standard detector data service. 
  IDataProviderSvc*   detSvc() const ;
  
  StatusCode initialize();
  
  StatusCode transport(Particles::const_iterator &, 
                       float znew,
                       HepPoint3D & newPOT,
                       HepSymMatrix & newPOTErr,
                       HepSymMatrix & newSlopesMomErr,
                       HepMatrix & newPosSlopesCorr);
  StatusCode finalize();
  
protected:
  
private:
  unsigned int m_vStations;
  double m_tolerance;
  double m_k;
  std::vector<double> m_zVelo;
  IDataProviderSvc* m_DDS;           ///< Detector Data Service  
};
#endif // PARTICLETRANSPORTER_H
