#ifndef TRACKTOOLS_ISTATECREATOR_H
#define TRACKTOOLS_ISTATECREATOR_H 1

// Include files
// -------------
#include "GaudiKernel/IAlgTool.h"

// forward declarations
class MCParticle;
class State;

/** @class IStateCreator IStateCreator.h "TrackTools/IStateCreator.h"
 *
 *  Interface class for TrueStateCreator (in Tr/TrackChecker).
 *
 *  @author Eduardo Rodrigues (adaptations to new track event model)
 *  @date   2005-04-04
 *
 *  @author Rutger van der Eijk
 *  @date   4-7-2002
 */

static const InterfaceID IID_IStateCreator( "IStateCreator", 1, 0 );

class IStateCreator: virtual public IAlgTool {
public:
  /// Retrieve interface ID
  static const InterfaceID& interfaceID() { return IID_IStateCreator; }
  
  /// create a state at z-position from MCParticle entry/exit points
  virtual StatusCode createState( const MCParticle* mcPart,
                                  double zrec,
                                  State*& pState ) const = 0;

  /// create a state at MCParticle vertex
  virtual StatusCode createStateVertex( const MCParticle* mcPart,
                                        State*& pState ) const = 0;
};

#endif // TRACKTOOLS_ISTATECREATOR_H
