// $Id: IParticleTransporter.h,v 1.1 2002-03-27 20:27:46 gcorti Exp $
#ifndef DAVINCITOOLS_IPARTICLETRANSPORTER_H 
#define DAVINCITOOLS_IPARTICLETRANSPORTER_H 1

// Include files
#include "GaudiKernel/IAlgTool.h"

#include "CLHEP/Geometry/Point3D.h"
#include "CLHEP/Matrix/Matrix.h"

// Forward declarations

// from Event
#include "Event/Particle.h"

static const InterfaceID IID_IParticleTransporter("IParticleTransporter", 
                                                  1 , 0); 

/** @class IParticleTransporter IParticleTransporter.h 
 *  DaVinciTools/IParticleTransporter.h
 *  Particle transporter interface.
 *
 *  @author Edgar de Oliveira
 *  @date   22/02/2002
 */

class IParticleTransporter : virtual public IAlgTool {
public:
  /// Retrieve interface ID
  static const InterfaceID& interfaceID() {
    return IID_IParticleTransporter;
  }
  /// Calculate
  virtual StatusCode transport(Particles::const_iterator &, 
                               float znew,
                               HepPoint3D & newPOT,
                               HepSymMatrix & newPOTErr,
                               HepSymMatrix & newSlopesMomErr,
                               HepMatrix & newPosSlopesCorr) = 0; 
};
#endif // DAVINCITOOLS_IPARTICLETRANSPORTER_H
