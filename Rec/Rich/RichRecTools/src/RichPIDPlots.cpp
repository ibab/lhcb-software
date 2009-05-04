// $Id: RichPIDPlots.cpp,v 1.10 2009-05-04 13:54:44 jonrob Exp $
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
  declareProperty( "HistoBins",    m_bins = 50 );
  declareProperty( "ExtraHistos",  m_extraHistos = false );
  declareProperty( "DllCut",       m_dllCut = 0.0 );
  declareProperty( "DllPlotRange", m_dllRange = 100 );
  // turn off histo printing by default
  setProperty("HistoPrint",false);
}

PIDPlots::~PIDPlots() {}

StatusCode PIDPlots::initialize()
{
  // Initialize base class
  const StatusCode sc = HistoToolBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  // Warn if extra histos are enabled
  //if ( m_extraHistos ) 
  //  Warning( "Extra histograms are enabled", StatusCode::SUCCESS );

  return sc;
}

//=============================================================================

void PIDPlots::plots( const LHCb::ProtoParticle * proto,
                      const Rich::ParticleIDType hypo,
                      const Configuration & config ) const
{
  if ( !proto ) { Warning( "Null ProtoParticle pointer passed !" ); return; }

  // track selection
  if ( !selected( proto->track(), config ) ) return;
  
  // Fill 'track' plots
  plots( proto->track(), hypo, config );

  // Fill additional ProtoParticle specific plots here ;)

}

void PIDPlots::plots( const LHCb::RichPID * pid,
                      const Rich::ParticleIDType hypo,
                      const Configuration & config ) const
{
  if ( !pid ) { Warning( "Null RichPID pointer passed !" ); return; }

  // track selection
  if ( !selected(pid->track(),config) ) return;

  // Fill 'track' plots
  plots( pid->track(), hypo, config );

  // Now, plots that require RichPID info ...

  // Extra plots (disabled by default)
  if ( m_extraHistos )
  {

    // track momentum
    const double pTot = trackP( pid->track() );
    // hypo as string
    const std::string shypo = Rich::text(hypo);

    // Heavy or light ?
    const bool heavy = ( (int)hypo > (int)(Rich::Pion) );

    // Loop over all combinations of PID pairs
    for ( Rich::Particles::const_reverse_iterator i = Rich::particles().rbegin();
          i != Rich::particles().rend(); ++i )
    {
      Rich::Particles::const_reverse_iterator j = i; ++j;
      for ( ; j != Rich::particles().rend(); ++j )
      {
       
        const Rich::ParticleIDType first = ( heavy ? *i : *j );
        const Rich::ParticleIDType last  = ( heavy ? *j : *i );
        const std::string DllDiff = Rich::text(first) + "-" + Rich::text(last);

        // Dll(X-Y) distributions
        std::string title = "RichDLL("+DllDiff+") : "+shypo;
        const double dll = pid->particleDeltaLL(first) - pid->particleDeltaLL(last);
        plot1D( dll,
                hPath(hypo)+title, title,
               -m_dllRange, m_dllRange, m_bins );

        // Efficiency plots
        title = "Eff. RichDLL("+DllDiff+")>0 Versus Ptot : "+shypo;
        profile1D( pTot, double(dll>m_dllCut),
                   hPath(hypo)+title, title, 
                   config.minP, config.maxP, m_bins );
        
        // # Sigma distributions
        title = "# Sigma("+DllDiff+") : "+shypo;
        const double nsigma = pid->nSigmaSeparation(first,last);
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

  // hypo as string
  const std::string shypo = Rich::text(hypo);

  // Momentum spectra
  plot1D( tkPtot, hPath(hypo)+"Ptot",
          "Ptot : "+shypo,
          config.minP, config.maxP, m_bins );
}
