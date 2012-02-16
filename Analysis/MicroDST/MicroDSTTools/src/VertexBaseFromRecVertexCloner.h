// $Id: VertexBaseFromRecVertexCloner.h,v 1.1 2010-08-11 12:48:36 jpalac Exp $
#ifndef VERTEXBASEFROMRECVERTEXCLONER_H
#define VERTEXBASEFROMRECVERTEXCLONER_H 1

// Include files
// from Gaudi
#include <MicroDST/MicroDSTTool.h>
#include <MicroDST/ICloneVertexBase.h>            // Interface

// local
#include "RecVertexClonerFunctors.h"

/** @class VertexBaseFromRecVertexCloner VertexBaseFromRecVertexCloner.h
 *
 *  MicroDSTTool that clones an LHCb::RecVertex for storage on the MicroDST.
 *  The LHCb::RecVertex's constituent LHCb::Tracks are not cloned for storage.
 *  SmartRefs to them are stored instead.
 *
 *  @author Juan PALACIOS
 *  @date   2007-12-05
 */
class VertexBaseFromRecVertexCloner : public extends1<MicroDSTTool, ICloneVertexBase>
{

public:

  /// Standard constructor
  VertexBaseFromRecVertexCloner( const std::string& type,
                                 const std::string& name,
                                 const IInterface* parent);

  virtual ~VertexBaseFromRecVertexCloner( ); ///< Destructor

public:

  virtual LHCb::VertexBase* operator() (const LHCb::VertexBase* vertex);

private:

  LHCb::RecVertex* operator() (const LHCb::RecVertex* vertex);

  typedef MicroDST::RecVertexClonerShallowTracks PVCloner;

protected:

  virtual LHCb::RecVertex* clone (const LHCb::RecVertex* vertex);

};

#endif // VERTEXBASEFROMRECVERTEXCLONER_H
