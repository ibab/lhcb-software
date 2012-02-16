// $Id: VertexBaseFromRecVertexClonerNoTracks.h,v 1.1 2010-08-11 12:48:36 jpalac Exp $
#ifndef VertexBaseFromRecVertexClonerNoTracks_H
#define VertexBaseFromRecVertexClonerNoTracks_H 1

// local
#include "VertexBaseFromRecVertexCloner.h"

/** @class VertexBaseFromRecVertexClonerNoTracks VertexBaseFromRecVertexClonerNoTracks.h
 *
 *  MicroDSTTool that clones an LHCb::RecVertex for storage on the MicroDST.
 *  The LHCb::RecVertex's constituent LHCb::Tracks are not cloned for storage
 *  and the SmartRef vector is cleared.
 *
 *  @author Juan PALACIOS
 *  @date   2007-12-05
 */
class VertexBaseFromRecVertexClonerNoTracks : public VertexBaseFromRecVertexCloner
{

public:

  /// Standard constructor
  VertexBaseFromRecVertexClonerNoTracks( const std::string& type,
                                         const std::string& name,
                                         const IInterface* parent);

  virtual ~VertexBaseFromRecVertexClonerNoTracks( ); ///< Destructor

protected:

  virtual LHCb::RecVertex* clone (const LHCb::RecVertex* vertex);

};

#endif // VertexBaseFromRecVertexClonerNoTracks_H
