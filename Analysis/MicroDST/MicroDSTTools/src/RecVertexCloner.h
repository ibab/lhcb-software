// $Id: RecVertexCloner.h,v 1.7 2009-07-29 21:38:20 jpalac Exp $
#ifndef RECVERTEXCLONER_H 
#define RECVERTEXCLONER_H 1

// Include files
// from Gaudi
#include <MicroDST/MicroDSTTool.h>
#include <MicroDST/ICloneVertexBase.h>            // Interface

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
                        virtual public ICloneVertexBase {
public: 
  /// Standard constructor
  RecVertexCloner( const std::string& type, 
                   const std::string& name,
                   const IInterface* parent);

  virtual ~RecVertexCloner( ); ///< Destructor

  virtual LHCb::VertexBase* operator() (const LHCb::VertexBase* vertex);

  StatusCode initialize();

private:

  LHCb::RecVertex* operator() (const LHCb::RecVertex* vertex);

  typedef MicroDST::RecVertexClonerShallowTracks PVCloner;

  LHCb::RecVertex* clone (const LHCb::RecVertex* vertex);

};
#endif // RECVERTEXCLONER_H
