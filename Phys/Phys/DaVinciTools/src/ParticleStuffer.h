// $Id: ParticleStuffer.h,v 1.3 2002-10-15 17:51:27 gcorti Exp $
#ifndef PARTICLESTUFFER_H 
#define PARTICLESTUFFER_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/AlgTool.h"

// from DaVinciTools
#include "DaVinciTools/IParticleStuffer.h"

// Forward declarations
class IParticlePropertySvc;

/** @class ParticleStuffer ParticleStuffer.h
 *  Fill a particle given a vertex and a particle ID
 *  @author Paul Colrain
 *  @date   14/03/2002
 */
class ParticleStuffer : public AlgTool, 
                        virtual public IParticleStuffer {

public:

  /// Standard constructor
  ParticleStuffer( const std::string& type, 
                   const std::string& name,
                   const IInterface* parent);
  
  /// Desctructor
  virtual ~ParticleStuffer( ){ }; 

  /// Retrieve  the ParticlePropertyService.
  StatusCode initialize();    

  
  /// Fill Composite Particle from Vertex
  StatusCode fillParticle( const Vertex&, Particle&, 
                           const ParticleID& );

  // Fill Particle from ProtoParticle
  //StatusCode fillParticle( const ProtoParticle&, Particle&, 
  //                         const ParticleID& );

private:
  IParticlePropertySvc* m_ppSvc;        ///< Reference to ParticlePropertySvc

};

#endif // PARTICLESTUFFER_H
