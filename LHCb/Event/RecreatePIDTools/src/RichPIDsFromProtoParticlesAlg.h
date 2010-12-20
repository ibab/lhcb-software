
//-----------------------------------------------------------------------------
/** @file RichPIDsFromProtoParticlesAlg.h
 *
 * Header file for algorithm RichPIDsFromProtoParticlesAlg
 *
 * @author Chris Jones   Christopher.Rob.Jones@cern.ch
 * @date 29/03/2006
 */
//-----------------------------------------------------------------------------

#ifndef GLOBALRECO_RICHPIDSFROMPROTOPARTICLESALG_H 
#define GLOBALRECO_RICHPIDSFROMPROTOPARTICLESALG_H 1

// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"

// Event
#include "Event/ProtoParticle.h"
#include "Event/RichPID.h"

//-----------------------------------------------------------------------------
/** @class RichPIDsFromProtoParticlesAlg RichPIDsFromProtoParticlesAlg.h
 *  
 *  Simple algorithm to recreate RichPID data objects from the data
 *  stored in the ProtoParticles
 *m_richPIDloc 
 *  @author Chris Jones
 *  @date   2006-08-01
 */
//-----------------------------------------------------------------------------

class RichPIDsFromProtoParticlesAlg : public GaudiAlgorithm 
{

public: 

  /// Standard constructor
  RichPIDsFromProtoParticlesAlg( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~RichPIDsFromProtoParticlesAlg( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution

private:

  /// Input location of ProtoParticles in TES
  std::string m_protoPloc;

  /// Output location of RichPIDs in TES
  std::string m_richPIDloc;

};

#endif // GLOBALRECO_RICHPIDSFROMPROTOPARTICLESALG_H
