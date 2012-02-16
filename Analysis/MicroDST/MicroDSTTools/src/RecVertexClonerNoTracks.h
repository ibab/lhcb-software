// $Id: RecVertexClonerNoTracks.h,v 1.9 2010-08-11 12:51:28 jpalac Exp $
#ifndef RecVertexClonerNoTracks_H
#define RecVertexClonerNoTracks_H 1

// local
#include "RecVertexCloner.h"

/** @class RecVertexClonerNoTracks RecVertexClonerNoTracks.h
 *
 *  MicroDSTTool that clones an LHCb::RecVertex for storage on the MicroDST.
 *  The LHCb::RecVertex's constituent LHCb::Tracks are not cloned, and the
 *  SmartRef vector is cleared.
 *
 *  @author Juan PALACIOS
 *  @date   2007-12-05
 */
class RecVertexClonerNoTracks : public RecVertexCloner
{

public:

  /// Standard constructor
  RecVertexClonerNoTracks( const std::string& type,
                           const std::string& name,
                           const IInterface* parent );

  virtual ~RecVertexClonerNoTracks( ); ///< Destructor

protected:

  virtual LHCb::RecVertex* clone ( const LHCb::RecVertex* vertex );

};

#endif // RecVertexClonerNoTracks_H
