// $Id: RichPIDPlots.cpp,v 1.4 2008-05-09 13:56:26 jonrob Exp $
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
  declareProperty( "ExtraHistos", m_extraHistos = false );
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

void PIDPlots::plots( const LHCb::RichPID * pid,
                      const Rich::ParticleIDType hypo,
                      const Configuration & config ) const
{
  if ( !pid ) { Warning( "Null RichPID pointer passed !" ); return; }

  // track selection
  if ( !selected(pid->track(),config) ) return;

  // Fill 'track' plots
  this -> plots( pid->track(), hypo, config );

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
                hPath(hypo)+title, title,
               -100, 100, m_bins );
        
        // # Sigma distributions
        title = "nSigma("+Rich::text(*i)+"-"+Rich::text(*j)+")";
        const double nsigma = pid->nSigmaSeparation(*i,*j);
        plot1D( nsigma,
                hPath(hypo)+title, title,
                -30, 30, m_bins );

      }
    }

  }

}

void PIDPlots::plots( const LHCb::Track * track, 
                      const Rich::ParticleIDType hypo,
                      const Configuration & config ) const
{
  if ( !track ) { Warning( "Null Track pointer passed !" ); return; }

  // track selection
  if ( !selected(track,config) ) return;
  
  // Rich Histo ID
  const RichHistoID hid;

  // Number of each type of hypothesis
  //plot1D( (int)hypo, "nPIDsPerHypo", "# PIDs per hypothesis", -1.5, 5.5, 7 );

  // Track momentum in GeV/c
  const double tkPtot = trackP( track );

  // Momentum spectra
  plot1D( tkPtot, hPath(hypo)+"Ptot",
          "Ptot : "+Rich::text(hypo),
          config.minP, config.maxP, m_bins );

}
