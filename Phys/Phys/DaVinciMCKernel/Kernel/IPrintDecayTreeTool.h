// $Id: IPrintDecayTreeTool.h,v 1.1 2007-10-09 16:37:00 jpalac Exp $
#ifndef DAVINCIMCTOOLS_IPRINTDECAYTREETOOL_H
#define DAVINCIMCTOOLS_IPRINTDECAYTREETOOL_H 1

// Include files
// from STL
#include <vector>
#include <string>

// from Gaudi
#include "GaudiKernel/IAlgTool.h"

#include "Event/Particle.h"
#include "Event/MCParticle.h"

// From DaVinci
#include "Kernel/Particle2MCLinker.h"

// Declaration of the interface ID (interface id, major version, minor version)
static const InterfaceID IID_IPrintDecayTreeTool("IPrintDecayTreeTool", 1, 0);

/** @class IPrintDecayTreeTool IPrintDecayTreeTool.h DaVinciMCTools/IPrintDecayTreeTool.h
 *  
 *
 *  @author Juan Palacios juancho@nikhef.nl
 *  @date   09/10/2007
 */
class IPrintDecayTreeTool : virtual public IAlgTool {
public:

  /// Retrieve interface ID
  static const InterfaceID& interfaceID() { return IID_IPrintDecayTreeTool; }

  /// Print decay tree for a given particle
  virtual void printTree( const LHCb::Particle* mother, 
                          int maxDepth = -1 ) = 0;

  /// Print side by side decay tree for a given MC particle and it's
  /// reconstructed items
  virtual void printTree( const LHCb::MCParticle* mother, 
                          Particle2MCLinker* assoc,
                          int maxDepth = -1) = 0;

  /// Print side by side decay tree for a given particle and it's
  /// associated MC particle.
  virtual void printTree( const LHCb::Particle* mother, 
                          Particle2MCLinker* assoc, 
                          int maxDepth = -1 ) = 0;

  /// Also print the reconstructed info if available.
  virtual void printAsTree( const LHCb::MCParticle::ConstVector& event,
                            Particle2MCLinker* assoc ) = 0;
  /// Same from a keyed container.
  virtual void printAsTree( const LHCb::MCParticles& event ) = 0;
  virtual void printAsTree( const LHCb::MCParticles& event,
                            Particle2MCLinker* assoc ) = 0;

  /// Print all the Particles in the event as a flat list.
  virtual void printAsList( const LHCb::Particle::ConstVector &event ) = 0;
  /// Also print the truth/reconstructed info if available.
  virtual void printAsList( const LHCb::Particle::ConstVector &event,
                            Particle2MCLinker* assoc ) = 0;
  virtual void printAsList( const LHCb::MCParticle::ConstVector &event,
                            Particle2MCLinker* assoc ) = 0;
  /// Same from a keyed container.
  virtual void printAsList( const LHCb::Particles &event ) = 0;
  virtual void printAsList( const LHCb::Particles &event,
                            Particle2MCLinker* assoc ) = 0;
  virtual void printAsList( const LHCb::MCParticles &event,
                            Particle2MCLinker* assoc ) = 0;
};
#endif // DAVINCIMCTOOLS_IPRINTDECAYTREETOOL_H
