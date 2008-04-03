// $Id: RecVertexCloner.h,v 1.2 2008-04-03 14:45:08 jpalac Exp $
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

  virtual LHCb::RecVertex* operator() (const LHCb::RecVertex* vertex);

  StatusCode initialize();

private:

  typedef MicroDST::RecVertexClonerShallowTracks PVCloner;

  LHCb::RecVertex* clone (const LHCb::RecVertex* vertex);

};
#endif // RECVERTEXCLONER_H
