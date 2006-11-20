
//-----------------------------------------------------------------------------
/** @file ProtoParticleRICHFilter.h
 *
 * Header file for algorithm ProtoParticleRICHFilter
 *
 * CVS Log :-
 * $Id: ProtoParticleRICHFilter.h,v 1.1 2006-11-20 15:59:49 jonrob Exp $
 *
 * @author Chris Jones   Christopher.Rob.Jones@cern.ch
 * @date 2006-05-03
 */
//-----------------------------------------------------------------------------

#ifndef PROTOPARTICLEFILTER_ProtoParticleRICHFilter_H
#define PROTOPARTICLEFILTER_ProtoParticleRICHFilter_H 1

// base classyy
#include "ChargedProtoParticleDLLFilter.h"

// Interfaces
#include "Kernel/IProtoParticleFilter.h"

//-----------------------------------------------------------------------------
/** @class ProtoParticleRICHFilter ProtoParticleRICHFilter.h
 *
 *  Tool which extends ProtoParticleDLLFilter to add charged track selection
 *  criteria
 *
 *  @author Chris Jones   Christoper.Rob.Jones@cern.ch
 *  @date   2006-05-03
 */
//-----------------------------------------------------------------------------

class ProtoParticleRICHFilter : public ChargedProtoParticleDLLFilter,
                                virtual public IProtoParticleFilter
{

public: // Core Gaudi methods

  /// Standard constructor
  ProtoParticleRICHFilter( const std::string& type,
                           const std::string& name,
                           const IInterface* parent );

  virtual ~ProtoParticleRICHFilter( ); ///< Destructor

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

};

#endif // PROTOPARTICLEFILTER_ProtoParticleRICHFilter_H
