// $Id: IDecayFinder.h,v 1.8 2004-07-29 14:32:15 pkoppenb Exp $
#ifndef TOOLS_IDECAYFINDER_H 
#define TOOLS_IDECAYFINDER_H 1

// Include files
// from STL
#include <string>
#include <vector>

// from Gaudi
#include "GaudiKernel/IAlgTool.h"

#include "Event/Particle.h"
//class Particle;

// Declaration of the interface ID (interface id, major version, minor version)
static const InterfaceID IID_IDecayFinder("IDecayFinder", 3, 0);

/** @class IDecayFinder IDecayFinder.h DaVinciTools/IDecayFinder.h
 *  
 *  Please see the file DecayFinder.h
 *  in the src directory of package Phys/DaVinciTools.
 *
 *  @author Olivier Dormond
 *  @date   23/04/2002
 */
class IDecayFinder : virtual public IAlgTool {
public:
  /// Retrieve interface ID
  static const InterfaceID& interfaceID() { return IID_IDecayFinder; }

  /// Get/Set the decay string to find
  virtual std::string decay( void ) = 0;
  virtual StatusCode setDecay( std::string decay ) = 0;

  virtual std::string revert( void ) = 0;

  /// Does the described decay exists in the event?
  virtual bool hasDecay( const ParticleVector &event ) = 0;
  virtual bool findDecay( const ParticleVector &event,
                          const Particle *&previous_result ) = 0;

  virtual bool hasDecay( const Particles &event ) = 0;
  virtual bool findDecay( const Particles &event,
                          const Particle *&previous_result ) = 0;

  virtual bool hasDecay( void ) = 0;
  virtual bool findDecay( const Particle*&previous_result ) = 0;

  virtual void descendants( const Particle *head,
                            std::vector<Particle *>&result,
                            bool leaf=false ) = 0;
  virtual void decayMembers( const Particle *head,
                             std::vector<Particle*>&members ) = 0;
  virtual void decaySubTrees( const Particle *head,
                      std::vector<std::pair<const Particle*,
                                            std::vector<Particle*> >
                                 > & subtrees ) = 0;

};
#endif // TOOLS_IDECAYFINDER_H
