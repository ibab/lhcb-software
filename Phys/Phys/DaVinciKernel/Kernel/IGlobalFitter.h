// $Id: IGlobalFitter.h,v 1.1 2005-11-11 16:02:34 pkoppenb Exp $
#ifndef DAVINCIKERNEL_IGLOBALFITTER_H 
#define DAVINCIKERNEL_IGLOBALFITTER_H 1

// Include files
#include "GaudiKernel/IAlgTool.h"

// Forward declarations
class Vertex;
class Particle;
class StatusCode;
class IParticlePropertySvc;


static const InterfaceID IID_IGlobalFitter("IGlobalFitter", 2 , 1);

/** @class IGlobalFitter
 *  Interface Class for fitting an arbitrary decay topology.
 *
 *  @author G.Balbi, A. Carbone, V. Vagnoni, S.Vecchi . 
 *  
 *  @date   11/11/2005
 */

class IGlobalFitter : virtual public IAlgTool {
  
public:
  /// Retrieve interface ID
  static const InterfaceID& interfaceID() { return IID_IGlobalFitter; }
  
  virtual StatusCode fit(Vertex &inVertex)=0;
  virtual StatusCode fit(Vertex &inVertex,double &time, double &timeerr)=0;
  virtual StatusCode fit(Particle &inParticle)=0;

  virtual StatusCode getFitPull(Vertex&, Vertex&, HepVector&)=0;
  virtual StatusCode getFitPull(Particle&, Particle&, HepVector&)=0;

  virtual void setMassConstraint(Vertex &v)=0;
  virtual void setWhichParticleLifetime(Particle*)=0;	
};

#endif // DAVINCIKERNEL_IGLOBALFITTER_H
