// $Id: RichPIDPlots.cpp,v 1.1 2008-04-29 12:52:30 jonrob Exp $
// Include files

// from Gaudi
#include "GaudiKernel/ToolFactory.h"

// local
#include "RichPIDPlots.h"

//-----------------------------------------------------------------------------

using namespace Rich::Rec;

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( PIDPlots );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
PIDPlots::PIDPlots( const std::string& type,
                    const std::string& name,
                    const IInterface* parent )
  : HistoToolBase ( type, name, parent ),
    m_trSelector  ( NULL )
{
  // interface
  declareInterface<Rich::Rec::IPIDPlots>(this);
  // JOs
  declareProperty( "HistoBins",   m_bins = 50 );
  declareProperty( "ExtraHistos", m_extraHistos = true );
}

PIDPlots::~PIDPlots() {}

StatusCode PIDPlots::initialize()
{
  // Initialize base class
  const StatusCode sc = HistoToolBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  // Warn if extra histos are enabled
  if ( m_extraHistos ) Warning( "Extra histograms are enabled", StatusCode::SUCCESS );

  return sc;
}

//=============================================================================

void PIDPlots::setTrackSelector( const Rich::Rec::ITrackSelector * selector ) const
{
  m_trSelector = selector;
}

void PIDPlots::plots( const LHCb::RichPID * pid,
                      const Rich::ParticleIDType reco_hypo,
                      const Rich::ParticleIDType true_hypo ) const
{
  if ( !pid ) { Warning( "Null RichPID pointer passed !" ); return; }

  // Fill 'track' plots
  this -> plots( pid->track(), reco_hypo, true_hypo );

  // Now, plots that require RichPID info ...

  // Extra plots (disabled by default)
  if ( m_extraHistos )
  {
    // Loop over all combinations of PID pairs
    for ( Rich::Particles::const_iterator i = Rich::particles().begin();
          i != Rich::particles().end(); ++i )
    {
      Rich::Particles::const_iterator j = i; ++j;
      for ( ; j != Rich::particles().end(); ++j )
      {
        
        // Dll(X-Y) distributions
        std::string title = "DLL("+Rich::text(*i)+"-"+Rich::text(*j)+")";
        const double dll = pid->particleDeltaLL(*i) - pid->particleDeltaLL(*j);
        plot1D( dll,
                hPath(reco_hypo,true_hypo)+title, title,
               -100, 100, m_bins );
        
        // # Sigma distributions
        title = "nSigma("+Rich::text(*i)+"-"+Rich::text(*j)+")";
        const double nsigma = pid->nSigmaSeparation(*i,*j);
        plot1D( nsigma,
                hPath(reco_hypo,true_hypo)+title, title,
                -30, 30, m_bins );

      }
    }

  }

}

void PIDPlots::plots( const LHCb::Track * track, 
                      const Rich::ParticleIDType reco_hypo,
                      const Rich::ParticleIDType true_hypo ) const
{
  if ( !track ) { Warning( "Null Track pointer passed !" ); return; }

  // Rich Histo ID
  const RichHistoID hid;

  // Number of each type of hypothesis
  plot1D( (int)reco_hypo, "nPIDsPerRecoType", "# PIDs per reconstructed hypothesis", -1.5, 5.5, 7 );
  plot1D( (int)true_hypo, "nPIDsPerTrueType", "# PIDs per 'true' hypothesis",        -1.5, 5.5, 7 );

  // Fill performance table
  plot2D( (int)reco_hypo, (int)true_hypo, "pidTable", "PID Performance Table", -1.5, 6.5, -1.5, 6.5, 7, 7 );

  // Track momentum in GeV/c
  const double tkPtot = trackP( track );

  // Momentum spectra
  plot1D( tkPtot, hPath(reco_hypo,true_hypo)+"Ptot",
          "Ptot : 'True'="+Rich::text(true_hypo)+ " Reco="+Rich::text(reco_hypo),
          ( m_trSelector ? m_trSelector->minPCut() :   2*Gaudi::Units::GeV ),
          ( m_trSelector ? m_trSelector->maxPCut() : 100*Gaudi::Units::GeV ),
          m_bins );

}
