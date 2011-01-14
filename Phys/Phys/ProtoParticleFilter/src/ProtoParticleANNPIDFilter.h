
//-----------------------------------------------------------------------------
/** @file ProtoParticleANNPIDFilter.h
 *
 * Header file for algorithm ProtoParticleANNPIDFilter
 *
 * @author Chris Jones   Christopher.Rob.Jones@cern.ch
 * @date 2011-01-12
 */
//-----------------------------------------------------------------------------

#ifndef PROTOPARTICLEFILTER_ProtoParticleANNPIDFilter_H
#define PROTOPARTICLEFILTER_ProtoParticleANNPIDFilter_H 1

// base class
#include "ProtoParticleDLLFilter.h"

// Interfaces
#include "Kernel/IProtoParticleFilter.h"

//-----------------------------------------------------------------------------
/** @class ProtoParticleANNPIDFilter ProtoParticleANNPIDFilter.h
 *
 *  Filter tool to provide a simple ProtoParticle selection based
 *  exclusively on the ProtoParticle combined ANN PID information
 *
 *  @author Chris Jones   Christoper.Rob.Jones@cern.ch



 *  @date   2011-01-12
 */
//-----------------------------------------------------------------------------

class ProtoParticleANNPIDFilter : public ProtoParticleDLLFilter,
                                  virtual public IProtoParticleFilter
{

public: // Core Gaudi methods

  /// Standard constructor
  ProtoParticleANNPIDFilter( const std::string& type,
                             const std::string& name,
                             const IInterface* parent );

  virtual ~ProtoParticleANNPIDFilter( ); ///< Destructor

  /// Initialisation
  virtual StatusCode initialize();

protected:

  // Create a cut object from decoded cut options
  virtual const ProtoParticleSelection::Cut *
  createCut( const std::string & tag,
             const std::string & delim,
             const std::string & value ) const;

};

#endif // PROTOPARTICLEFILTER_ProtoParticleANNPIDFilter_H
