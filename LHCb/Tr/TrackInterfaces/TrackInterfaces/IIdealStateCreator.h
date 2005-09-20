#ifndef TRACKINTERFACES_IIDEALSTATECREATOR_H
#define TRACKINTERFACES_IIDEALSTATECREATOR_H 1

// Include files
// -------------
#include "GaudiKernel/IAlgTool.h"

// forward declarations
class MCParticle;
class State;

/** @class IIdealStateCreator IIdealStateCreator.h "TrackInterfaces/IIdealStateCreator.h"
 *
 *  Interface class for IdealStateCreator (in Tr/TrackMCTools).
 *
 *  @author Eduardo Rodrigues (adaptations to new track event model)
 *  @date   2005-04-04
 *
 *  @author Rutger van der Eijk
 *  @date   4-7-2002
 */

static const InterfaceID IID_IIdealStateCreator( "IIdealStateCreator", 1, 0 );

class IIdealStateCreator: virtual public IAlgTool {
public:
  /// Retrieve interface ID
  static const InterfaceID& interfaceID() { return IID_IIdealStateCreator; }
  
  /// create a state at z-position from MCParticle entry/exit points
  virtual StatusCode createState( const MCParticle* mcPart,
                                  double zrec,
                                  State*& pState ) const = 0;

  /// create a state at MCParticle vertex
  virtual StatusCode createStateVertex( const MCParticle* mcPart,
                                        State*& pState ) const = 0;
};

#endif // TRACKINTERFACES_IIDEALSTATECREATOR_H
