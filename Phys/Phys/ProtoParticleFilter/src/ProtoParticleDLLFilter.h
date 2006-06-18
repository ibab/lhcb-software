
//-----------------------------------------------------------------------------
/** @file ProtoParticleDLLFilter.h
 *
 * Header file for algorithm ProtoParticleDLLFilter
 *
 * CVS Log :-
 * $Id: ProtoParticleDLLFilter.h,v 1.1.1.1 2006-06-18 14:23:44 jonrob Exp $
 *
 * @author Chris Jones   Christopher.Rob.Jones@cern.ch
 * @date 2006-05-03
 */
//-----------------------------------------------------------------------------

#ifndef PROTOPARTICLEFILTER_ProtoParticleDLLFilter_H
#define PROTOPARTICLEFILTER_ProtoParticleDLLFilter_H 1

// base class
#include "Kernel/ProtoParticleFilterBase.h"

// Interfaces
#include "Kernel/IProtoParticleFilter.h"

// Boost
#include "boost/lexical_cast.hpp"

//-----------------------------------------------------------------------------
/** @class ProtoParticleDLLFilter ProtoParticleDLLFilter.h
 *
 *  Filter tool to provide a simple ProtoParticle selection based
 *  exclusively on the ProtoParticle delta Log-Likelihood information
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

  /// Initialisation
  virtual StatusCode initialize();

  /// Finalisation
  virtual StatusCode finalize();

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
