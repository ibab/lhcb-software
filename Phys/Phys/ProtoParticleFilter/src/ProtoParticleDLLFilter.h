
//-----------------------------------------------------------------------------
/** @file ProtoParticleDLLFilter.h
 *
 * Header file for algorithm ProtoParticleDLLFilter
 *
 * @author Chris Jones   Christopher.Rob.Jones@cern.ch
 * @date 2006-05-03
 */
//-----------------------------------------------------------------------------

#ifndef PROTOPARTICLEFILTER_ProtoParticleDLLFilter_H
#define PROTOPARTICLEFILTER_ProtoParticleDLLFilter_H 1

// base class
#include "ProtoParticleFilterBase.h"

// Interfaces
#include "Kernel/IProtoParticleFilter.h"

// Boost
#include "boost/lexical_cast.hpp"

//-----------------------------------------------------------------------------
/** @class ProtoParticleDLLFilter ProtoParticleDLLFilter.h
 *
 *  Filter tool to provide a simple ProtoParticle selection based
 *  exclusively on the ProtoParticle combined DLL PID information
 *
 *  @author Chris Jones   Christoper.Rob.Jones@cern.ch
 *  @date   2006-05-03
 */
//-----------------------------------------------------------------------------

class ProtoParticleDLLFilter : public ProtoParticleFilterBase,
                               virtual public IProtoParticleFilter
{

public: // Core Gaudi methods

  /// Standard constructor
  ProtoParticleDLLFilter( const std::string& type,
                          const std::string& name,
                          const IInterface* parent );

  virtual ~ProtoParticleDLLFilter( ); ///< Destructor

private:

  /// Configure the cut object for the various possible DLL values
  bool tryDllTypes( const std::string & tag, 
                    ProtoParticleSelection::DLLCut * dllcut ) const;

protected:

  // Create a cut object from decoded cut options
  virtual const ProtoParticleSelection::Cut *
  createCut( const std::string & tag,
             const std::string & delim,
             const std::string & value ) const;

  // Create a DetectorRequirements object
  virtual const ProtoParticleSelection::DetectorRequirements *
  createDetReq( const std::string & tag,
                const std::string & value ) const;

};

#endif // PROTOPARTICLEFILTER_ProtoParticleDLLFilter_H
