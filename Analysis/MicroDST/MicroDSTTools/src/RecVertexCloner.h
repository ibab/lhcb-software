// $Id: RecVertexCloner.h,v 1.9 2010-08-11 12:51:28 jpalac Exp $
#ifndef RECVERTEXCLONER_H
#define RECVERTEXCLONER_H 1

// Include files
// from Gaudi
#include <MicroDST/MicroDSTTool.h>
#include <MicroDST/ICloneRecVertex.h>            // Interface

// local
#include "RecVertexClonerFunctors.h"

/** @class RecVertexCloner RecVertexCloner.h
 *
 *  MicroDSTTool that clones an LHCb::RecVertex for storage on the MicroDST.
 *  The LHCb::RecVertex's constituent LHCb::Tracks are not cloned for storage.
 *  SmartRefs to them are stored instead.
 *
 *  @author Juan PALACIOS
 *  @date   2007-12-05
 */
class RecVertexCloner : public extends1<MicroDSTTool, ICloneRecVertex>
{

public:

  /// Standard constructor
  RecVertexCloner( const std::string& type,
                   const std::string& name,
                   const IInterface* parent );

  virtual ~RecVertexCloner( ); ///< Destructor

public:

  virtual LHCb::RecVertex* operator() (const LHCb::RecVertex* vertex);

private:

  typedef MicroDST::RecVertexClonerShallowTracks PVCloner;

protected:

  virtual LHCb::RecVertex* clone (const LHCb::RecVertex* vertex);

};

#endif // RECVERTEXCLONER_H
