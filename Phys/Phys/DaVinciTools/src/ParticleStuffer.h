// $Id: ParticleStuffer.h,v 1.4 2002-12-20 13:36:29 gcorti Exp $
#ifndef PARTICLESTUFFER_H 
#define PARTICLESTUFFER_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/AlgTool.h"

// from DaVinciTools
#include "DaVinciTools/IParticleStuffer.h"
#include "DaVinciTools/IParticleTransporter.h"

// Forward declarations
class IParticlePropertySvc;

/** @class ParticleStuffer ParticleStuffer.h
 *  Fill a particle given a vertex and a particle ID
 *  @author Paul Colrain
 *  @date   14/03/2002
 *  Modified by S. Amato to transport parameters to the vertex position 11/10/2002 
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
  IToolSvc* m_pToolSvc;
  IParticleTransporter* m_pTransporter; ///< Reference to ParticleTransporter
  std::string m_transporterType;        ///< Type of transporter to use      

};

#endif // PARTICLESTUFFER_H
