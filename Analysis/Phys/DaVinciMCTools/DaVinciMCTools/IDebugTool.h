// $Id: IDebugTool.h,v 1.1.1.1 2002-05-21 07:32:02 gcorti Exp $
#ifndef DAVINCIMCTOOLS_IDEBUGTOOL_H
#define DAVINCIMCTOOLS_IDEBUGTOOL_H 1

// Include files
// from STL
#include <vector>
#include <string>

// from Gaudi
#include "GaudiKernel/IAlgTool.h"

// from LHCbKernel
#include "DaVinciAssociators/Particle2MCAsct.h"
#include "Event/Particle.h"
#include "Event/MCParticle.h"

// Forward declarations

// Declaration of the interface ID (interface id, major version, minor version)
static const InterfaceID IID_IDebugTool("IDebugTool", 1, 0);

/** @class IDebugTool IDebugTool.h DaVinciMCTools/IDebugTool.h
 *  
 *  Please see the file DebugTool.h
 *  in the src directory of package Phys/DaVinciMCTools.
 *
 *  @author Olivier Dormond
 *  @date   29/03/2002
 */
class IDebugTool : virtual public IAlgTool {
public:

  /// Retrieve interface ID
  static const InterfaceID& interfaceID() { return IID_IDebugTool; }

  /// In the following function a maxDepth is provided with a default value
  /// of -1. That way the tool "PrintDepth" property is used.
  /// Print decay tree for a given MC particle
  virtual void printTree( const MCParticle* mother, int maxDepth = -1 ) = 0;

  /// Print decay tree for a given particle
  virtual void printTree( const Particle* mother, int maxDepth = -1 ) = 0;

  /// Print side by side decay tree for a given MC particle and it's
  /// reconstructed items
  virtual void printTree( const MCParticle* mother, 
                          Particle2MCAsct::IAsct *assoc,
                          int maxDepth = -1) = 0;

  /// Print side by side decay tree for a given particle and it's
  /// associated MC particle.
  virtual void printTree( const Particle* mother, 
                          Particle2MCAsct::IAsct *assoc, 
                          int maxDepth = -1 ) = 0;

  /// Print all the MC particles leading to this one.
  virtual void printAncestor( const MCParticle *child ) = 0;

  /// Print all the (MC)Particles in event as a tree.
  /// Top level particles are those without parent.
  virtual void printEventAsTree( const std::vector<MCParticle*> &event ) = 0;
  /// Also print the reconstructed info if available.
  virtual void printEventAsTree( const std::vector<MCParticle*> &event,
                                 Particle2MCAsct::IAsct *assoc ) = 0;

  /// Print all the (MC)Particles in the event as a flat list.
  virtual void printEventAsList( const ParticleVector &event ) = 0;
  virtual void printEventAsList( const std::vector<MCParticle*> &event ) = 0;
  /// Also print the truth/reconstructed info if available.
  virtual void printEventAsList( const ParticleVector &event,
                                 Particle2MCAsct::IAsct *assoc ) = 0;
  virtual void printEventAsList( const std::vector<MCParticle*> &event,
                                 Particle2MCAsct::IAsct *assoc ) = 0;
};
#endif // DAVINCIMCTOOLS_IDEBUGTOOL_H
