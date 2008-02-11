// $Id: IPrintDecayTreeTool.h,v 1.5 2008-02-11 12:33:48 jpalac Exp $
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
#include "Kernel/Particle2MCLinker.h"

// Declaration of the interface ID (interface id, major version, minor version)
static const InterfaceID IID_IPrintDecayTreeTool("IPrintDecayTreeTool", 1, 0);

/** @class IPrintDecayTreeTool IPrintDecayTreeTool.h Kernel/IPrintDecayTreeTool.h
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

  /**
   * Print side by side a decay tree for an MCParticle 
   * and it's associated particles
   *
   * @param mother The MCParticle who's tree is to be printed
   * @param assoc Associator linking mother to reconstructed LHCb::Particles
   * @param maxDepth maximum depth of printing in daughter-space. Default 
   *                 value of -1 is there to allow trickery in the 
   *                 implementations.
   * @todo Does this print two side-=by side trees, or a tree
   * of side-by side items?
  */
  virtual void printTree( const LHCb::MCParticle* mother, 
                          Particle2MCLinker* assoc,
                          int maxDepth = -1) = 0;

  /**
   * Print side by side a decay tree for a Particle 
   * and it's associated MCParticles
   *
   * @param mother The Particle who's tree is to be printed
   * @param assoc Associator linking mother to LHCb::MCParticles
   * @param maxDepth maximum depth of printing in daughter-space. Default 
   *                 value of -1 is there to allow trickery in the 
   *                 implementations.
   * @todo Does this print two side-=by side trees, or a tree
   * of side-by side items?
  */
  virtual void printTree( const LHCb::Particle* mother, 
                          Particle2MCLinker* assoc, 
                          int maxDepth = -1 ) = 0;

  /**
   * Print the contents of an MCParticle::ConstVector as a decay tree.
   * Print the associated reconstructed Particles if available.
   *
   * @param particles The container of MCParticles to be printed.
   * @param assoc Associator linking mother to LHCb::MCParticles
   *
  */
  virtual void printAsTree( const LHCb::MCParticle::ConstVector& particles,
                            Particle2MCLinker* assoc ) = 0;

  /**
   * Print the contents of an MCParticle keyed container as a decay tree.
   * Print the associated reconstructed Particles if available.
   *
   * @param particles The keyed container of MCParticles to be printed.
   * @param assoc Associator linking mother to LHCb::MCParticles
   *
  */
  virtual void printAsTree( const LHCb::MCParticles& particles,
                            Particle2MCLinker* assoc ) = 0;

  /**
   * Print the contents of an Particle::ConstVector as a flat list.
   * @param particles The vector of Particles to be printed.
   *
   */
  virtual void printAsList( const LHCb::Particle::ConstVector& particles ) = 0;

  /**
   * Print the contents of an Particle::ConstVector as a flat list.
   * Print the associated MCParticles if available.
   *
   * @param particles The vector of Particles to be printed.
   * @param assoc Associator linking mother to LHCb::MCParticles
   *
  */
  virtual void printAsList( const LHCb::Particle::ConstVector& particles,
                            Particle2MCLinker* assoc ) = 0;

  /**
   * Print the contents of an MCParticle::ConstVector as a flat list.
   * Print the associated recunstructed Particles if available.
   *
   * @param particles The vector of MCParticles to be printed.
   * @param assoc Associator linking mother to LHCb::Particles
   *
  */
  virtual void printAsList( const LHCb::MCParticle::ConstVector& particles,
                            Particle2MCLinker* assoc ) = 0;

  /**
   * Print the contents of a Particle keyed container as a flat list.
   * @param particles The keyed container of Particles to be printed.
   *
   */
  virtual void printAsList( const LHCb::Particles& particles ) = 0;

  /**
   * Print the contents of an Particle keyed container as a flat list.
   * Print the associated MCParticles if available.
   *
   * @param particles The keyed container of Particles to be printed.
   * @param assoc Associator linking mother to LHCb::MCParticles
   *
  */
  virtual void printAsList( const LHCb::Particles& particles,
                            Particle2MCLinker* assoc ) = 0;

  /**
   * Print the contents of an MCParticle keyed container as a flat list.
   * Print the associated reconstructed Particles if available.
   *
   * @param particles The keyed container of MCParticles to be printed.
   * @param assoc Associator linking mother to LHCb::Particles
   *
  */
  virtual void printAsList( const LHCb::MCParticles& particles,
                            Particle2MCLinker* assoc ) = 0;
};
#endif // DAVINCIMCTOOLS_IPRINTDECAYTREETOOL_H
