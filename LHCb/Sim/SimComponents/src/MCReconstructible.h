
//-----------------------------------------------------------------------------
/** @file MCReconstructible.h
 *
 *  Header file for class : MCReconstructible
 *
 *  CVS Log :-
 *  $Id: MCReconstructible.h,v 1.5 2007-05-15 07:47:18 mneedham Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date 28/02/2007
 */
//-----------------------------------------------------------------------------

#ifndef SIMCOMPONENTS_MCReconstructible_H
#define SIMCOMPONENTS_MCReconstructible_H 1

// from STL
#include <string>
#include <vector>

// Gaudi
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/IIncidentSvc.h"

// base class
#include "GaudiAlg/GaudiTool.h"

// Event
#include "Event/MCTrackInfo.h"

// Interface
#include "Kernel/IMCReconstructible.h"
#include "Kernel/IMCParticleSelector.h"

#include "Event/MCTrackGeomCriteria.h"

//-----------------------------------------------------------------------------
/** @class MCReconstructible MCReconstructible.h
 *
 *  Tool to provide standardised Reconstructibility information for neutral
 *  and charged MCParticles
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   2004-04-28
 */
//-----------------------------------------------------------------------------
class MCReconstructible : public GaudiTool,
                          virtual public IMCReconstructible,
                          virtual public IIncidentListener
{

public:

  /// Standard constructor
  MCReconstructible( const std::string& type,
                     const std::string& name,
                     const IInterface* parent);

  virtual ~MCReconstructible( ); ///< Destructor

  /// Initialize
  StatusCode initialize();

  /** Implement the handle method for the Incident service.
   *  This is used to inform the tool of software incidents.
   *  @param incident The incident identifier
   */
  void handle( const Incident& incident );

public:

  /// Get the reconstruction category for the given MCParticle
  virtual IMCReconstructible::RecCategory reconstructible( const LHCb::MCParticle* mcPart ) const;

  /// Is the MCParticle in the detector acceptance?
  virtual bool inAcceptance( const LHCb::MCParticle* mcPart ) const;

private: // methods

  /// Trest acceptance for charged particles
  bool accept_charged( const LHCb::MCParticle* mcPart ) const;

  /// Trest acceptance for neutral particles
  bool accept_neutral( const LHCb::MCParticle* mcPart ) const;

  /// get the MCTrackInfo object
  inline MCTrackInfo & mcTkInfo() const
  {
    if ( NULL == m_tkInfo ) 
    { 
      m_tkInfo = new MCTrackInfo( evtSvc(), msgSvc() );
      if ( NULL == m_tkInfo ) { Exception( "Failed to load MCTrackInfo" ); }
    }
    return *m_tkInfo;
  }

private: // data

  /// Acceptance parameters (neutrals)
  double m_zECAL, m_xECALInn, m_yECALInn, m_xECALOut, m_yECALOut;

  /// Threshold for Et gammas reconstructibility
  double m_lowEt;

  /// Allow primary particles
  bool m_allowPrimary;

  /// Pointer to MCTrackInfo object
  mutable MCTrackInfo * m_tkInfo;

  /// MCParticle selector
  IMCParticleSelector * m_mcSel;

  std::vector<std::string> m_chargedLongCriteria;
  LHCb::MC::MCTrackGeomCriteria* m_chargedLong;

  std::vector<std::string> m_chargedUpstreamCriteria;
  LHCb::MC::MCTrackGeomCriteria* m_chargedUpstream;

  std::vector<std::string> m_chargedDownstreamCriteria;
  LHCb::MC::MCTrackGeomCriteria* m_chargedDownstream;

  std::vector<std::string> m_chargedTCriteria;
  LHCb::MC::MCTrackGeomCriteria* m_chargedT;

  std::vector<std::string> m_chargedVeloCriteria;
  LHCb::MC::MCTrackGeomCriteria* m_chargedVelo;

};

#endif // SIMCOMPONENTS_MCReconstructible_H
