
//-----------------------------------------------------------------------------
/** @file ProtoParticleMUONFilter.h
 *
 * Header file for algorithm ProtoParticleMUONFilter
 *
 * CVS Log :-
 * $Id: ProtoParticleMUONFilter.h,v 1.3 2009-07-20 16:43:18 jonrob Exp $
 *
 * @author Chris Jones   Christopher.Rob.Jones@cern.ch
 * @date 2006-05-03
 */
//-----------------------------------------------------------------------------

#ifndef PROTOPARTICLEFILTER_ProtoParticleMUONFilter_H
#define PROTOPARTICLEFILTER_ProtoParticleMUONFilter_H 1

// base class
#include "ProtoParticleCALOFilter.h"

// Boost
#include "boost/lexical_cast.hpp"

//-----------------------------------------------------------------------------
/** @class ProtoParticleMUONFilter ProtoParticleMUONFilter.h
 *
 *  Filter tool to extend the simple DLL filter ProtoParticleDLLFilter
 *  with MUON system specific cuts and detector requirements.
 *
 *  @author Chris Jones   Christoper.Rob.Jones@cern.ch
 *  @date   2006-05-03
 */
//-----------------------------------------------------------------------------

class ProtoParticleMUONFilter : public ProtoParticleCALOFilter
{

public: // Core Gaudi methods

  /// Standard constructor
  ProtoParticleMUONFilter( const std::string& type,
                           const std::string& name,
                           const IInterface* parent );

  virtual ~ProtoParticleMUONFilter( ); ///< Destructor

protected:

  // Create a cut object from decoded cut options
  virtual const ProtoParticleSelection::Cut *
  createCut( const std::string & tag,
             const std::string & delim,
             const std::string & value ) const;

};

#endif // PROTOPARTICLEFILTER_ProtoParticleMUONFilter_H
