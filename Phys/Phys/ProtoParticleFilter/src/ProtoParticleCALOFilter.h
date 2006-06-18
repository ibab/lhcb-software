
//-----------------------------------------------------------------------------
/** @file ProtoParticleCALOFilter.h
 *
 * Header file for algorithm ProtoParticleCALOFilter
 *
 * CVS Log :-
 * $Id: ProtoParticleCALOFilter.h,v 1.1.1.1 2006-06-18 14:23:45 jonrob Exp $
 *
 * @author Chris Jones   Christopher.Rob.Jones@cern.ch
 * @date 2006-05-03
 */
//-----------------------------------------------------------------------------

#ifndef PROTOPARTICLEFILTER_ProtoParticleCALOFilter_H
#define PROTOPARTICLEFILTER_ProtoParticleCALOFilter_H 1

// base class
#include "ChargedProtoParticleDLLFilter.h"

//-----------------------------------------------------------------------------
/** @class ProtoParticleCALOFilter ProtoParticleCALOFilter.h
 *
 *  Filter tool to extend the simple DLL filter ProtoParticleDLLFilter
 *  with CALO system specific cuts and detector requirements.
 *
 *  @author Chris Jones   Christoper.Rob.Jones@cern.ch
 *  @date   2006-05-03
 */
//-----------------------------------------------------------------------------

class ProtoParticleCALOFilter : public ChargedProtoParticleDLLFilter
{

public: // Core Gaudi methods

  /// Standard constructor
  ProtoParticleCALOFilter( const std::string& type,
                           const std::string& name,
                           const IInterface* parent );

  virtual ~ProtoParticleCALOFilter( ); ///< Destructor

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

#endif // PROTOPARTICLEFILTER_ProtoParticleCALOFilter_H
