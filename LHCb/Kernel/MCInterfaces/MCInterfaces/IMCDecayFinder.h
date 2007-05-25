// $Id: IMCDecayFinder.h,v 1.1.1.1 2007-05-25 16:24:33 cattanem Exp $
#ifndef TOOLS_IMCDECAYFINDER_H 
#define TOOLS_IMCDECAYFINDER_H 1

// Include files
// from STL
#include <string>
#include <vector>

// from Gaudi
#include "GaudiKernel/IAlgTool.h"
#include "Event/MCParticle.h"

// Declaration of the interface ID (interface id, major version, minor version)
static const InterfaceID IID_IMCDecayFinder("IMCDecayFinder", 4, 0);

/** @class IMCDecayFinder IMCDecayFinder.h DaVinciTools/IMCDecayFinder.h
 *
 *  Please see the file MCDecayFinder.h
 *  in the src directory of package Phys/DaVinciTools.
 *
 *  @author Olivier Dormond
 *  @date   12/03/2002
 */
class IMCDecayFinder : virtual public IAlgTool {
public:
  /// Retrieve interface ID
  static const InterfaceID& interfaceID() { return IID_IMCDecayFinder; }

  /// Get/Set the decay string to find
  virtual std::string decay( void ) = 0;
  virtual StatusCode setDecay( std::string decay ) = 0;

  virtual std::string revert( void ) = 0;

  /// Does the described decay exists in the event?
  virtual bool hasDecay(  const LHCb::MCParticle::ConstVector& event ) = 0;
  virtual bool findDecay( const LHCb::MCParticle::ConstVector& event,
                          const LHCb::MCParticle*& previous_result ) = 0;

  virtual bool hasDecay(  const LHCb::MCParticle::Container& event ) = 0;
  virtual bool findDecay( const LHCb::MCParticle::Container& event,
                          const LHCb::MCParticle*& previous_result ) = 0;

  virtual bool hasDecay( void ) = 0;
  virtual bool findDecay( const LHCb::MCParticle*& previous_result ) = 0;

  virtual void descendants( const LHCb::MCParticle *head,
                            LHCb::MCParticle::Vector& result,
                            bool leaf=false ) = 0;
  virtual void decayMembers( const LHCb::MCParticle* head,
                             LHCb::MCParticle::Vector& members ) = 0;
  virtual void decaySubTrees( const LHCb::MCParticle* head,
                              std::vector<std::pair<const LHCb::MCParticle*,
                                                    LHCb::MCParticle::Vector >
                                         > & subtrees ) = 0;

};
#endif // TOOLS_IMCDECAYFINDER_H
