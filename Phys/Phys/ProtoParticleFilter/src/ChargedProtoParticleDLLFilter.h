
//-----------------------------------------------------------------------------
/** @file ChargedProtoParticleDLLFilter.h
 *
 * Header file for algorithm ChargedProtoParticleDLLFilter
 *
 * CVS Log :-
 * $Id: ChargedProtoParticleDLLFilter.h,v 1.2 2006-11-20 15:59:49 jonrob Exp $
 *
 * @author Chris Jones   Christopher.Rob.Jones@cern.ch
 * @date 2006-05-03
 */
//-----------------------------------------------------------------------------

#ifndef PROTOPARTICLEFILTER_ChargedProtoParticleDLLFilter_H
#define PROTOPARTICLEFILTER_ChargedProtoParticleDLLFilter_H 1

// base classyy
#include "ProtoParticleDLLFilter.h"

// Interfaces
#include "Kernel/IProtoParticleFilter.h"
#include "TrackInterfaces/ITrackSelector.h"

//-----------------------------------------------------------------------------
/** @class ChargedProtoParticleDLLFilter ChargedProtoParticleDLLFilter.h
 *
 *  Tool which extends ProtoParticleDLLFilter to add charged track selection
 *  criteria
 *
 *  @author Chris Jones   Christoper.Rob.Jones@cern.ch
 *  @date   2006-05-03
 */
//-----------------------------------------------------------------------------

class ChargedProtoParticleDLLFilter : public ProtoParticleDLLFilter,
                                      virtual public IProtoParticleFilter
{

public: // Core Gaudi methods

  /// Standard constructor
  ChargedProtoParticleDLLFilter( const std::string& type,
                                 const std::string& name,
                                 const IInterface* parent );

  virtual ~ChargedProtoParticleDLLFilter( ); ///< Destructor

  /// Initialisation
  virtual StatusCode initialize();

  /// Finalisation
  virtual StatusCode finalize();

public: // tool interface methods

  /// Test if filter is satisfied.
  virtual bool isSatisfied( const LHCb::ProtoParticle* const & proto ) const;

protected:

  // Create a cut object from decoded cut options
  virtual const ProtoParticleSelection::Cut *
  createCut( const std::string & tag,
             const std::string & delim,
             const std::string & value ) const;

private:

  /// Track selector tool
  ITrackSelector * m_trSel;

};

#endif // PROTOPARTICLEFILTER_ChargedProtoParticleDLLFilter_H
