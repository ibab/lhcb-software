// $Id: RecVertexCloner.h,v 1.5 2009-04-16 07:43:06 jpalac Exp $
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
class RecVertexCloner : public MicroDSTTool, 
                        virtual public ICloneRecVertex {
public: 
  /// Standard constructor
  RecVertexCloner( const std::string& type, 
                   const std::string& name,
                   const IInterface* parent);

  virtual ~RecVertexCloner( ); ///< Destructor

  virtual LHCb::RecVertex* operator() (const LHCb::VertexBase* vertex);

  virtual LHCb::RecVertex* operator() (const LHCb::RecVertex* vertex);

  StatusCode initialize();

private:

  typedef MicroDST::RecVertexClonerShallowTracks PVCloner;

  LHCb::RecVertex* clone (const LHCb::RecVertex* vertex);

};
#endif // RECVERTEXCLONER_H
