// $Id: ParticleStuffer.h,v 1.7 2006-05-10 12:27:38 pkoppenb Exp $
#ifndef PARTICLESTUFFER_H
#define PARTICLESTUFFER_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiAlg/GaudiTool.h"

// from DaVinciTools
#include "Kernel/IParticleStuffer.h"
#include "Kernel/IParticleTransporter.h"

// Forward declarations
class IParticlePropertySvc;

/** @class ParticleStuffer ParticleStuffer.h
 *  Fill a particle given a vertex and a particle ID
 *  @author Paul Colrain
 *  @date   14/03/2002
 *  Modified by S. Amato to transport parameters to the vertex position 11/10/2002
 */
class ParticleStuffer : public GaudiTool,
                        virtual public IParticleStuffer
{

public:

  /// Standard constructor
  ParticleStuffer( const std::string& type,
                   const std::string& name,
                   const IInterface* parent);

  /// Destructor
  virtual ~ParticleStuffer( );

  /// Retrieve  the ParticlePropertyService.
  StatusCode initialize();

  /// Fill Composite Particle from Vertex
  StatusCode fillParticle( const LHCb::Particle::ConstVector& daughters,
                           const LHCb::Vertex&,
                           const LHCb::ParticleID&,
                           LHCb::Particle&);

  /// Fill Composite Particle from Vertex
  StatusCode fillParticle( const LHCb::Particle::ConstVector& daughters,
                           const LHCb::Vertex&,
                           LHCb::Particle&);

private:

  LHCb::IParticlePropertySvc* m_ppSvc;  ///< Reference to ParticlePropertySvc
  IParticleTransporter* m_pTransporter; ///< Reference to ParticleTransporter
  std::string m_transporterType;        ///< Type of transporter to use

};

#endif // PARTICLESTUFFER_H
