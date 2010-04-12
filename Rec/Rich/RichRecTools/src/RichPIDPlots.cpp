// $Id: RichPIDPlots.cpp,v 1.11 2010-02-11 20:05:28 jonrob Exp $
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
  declareProperty ( "ExtraHistos",  m_extraHistos = false );
  declareProperty ( "DllCut",       m_dllCut      = 0.0   );
  declareProperty ( "DllPlotRange", m_dllRange    = 100   );
  // turn off histo printing by default
  setProperty ( "HistoPrint", false );
  // Default number of bins
  setProperty ( "NBins1DHistos", 50 );
  setProperty ( "NBins2DHistos", 20 );
}

PIDPlots::~PIDPlots() {}

StatusCode PIDPlots::initialize()
{
  // Initialize base class
  const StatusCode sc = HistoToolBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  // Warn if extra histos are enabled
  if ( m_extraHistos ) debug() << "Extra histograms are enabled" << endmsg;

  return sc;
}

//=============================================================================

void PIDPlots::plots( const LHCb::ProtoParticle * proto,
                      const Rich::ParticleIDType hypo,
                      const Configuration & config ) const
{
  if ( !proto ) 
  { Warning( "Null ProtoParticle pointer passed !" ).ignore(); return; }

  // Get the Track pointer
  const LHCb::Track * track = proto->track();
  if ( !track ) 
  { Warning( "ProtoParticle has null Track pointer !" ).ignore(); return; }

  // track selection
  if ( !selected(track,config) ) return;

  // Fill RichPID plots
  plots( proto->richPID(), hypo, config );

  // ProtoParticle level plots

  // Extra plots
  if ( m_extraHistos )
  {
    
    // track momentum
    const double pTot = trackP ( track );
    const double pT   = trackPt( track );
    
    // Efficiency to have a RichPID data object associated (~ RICH Acceptance)
    const double accEff( proto->richPID() ? 100.0 : 0.0 );
    
    // Eff v P and Pt plots
    std::string title = "Eff. RICH acceptance Versus P (MeV/c)";
    richProfile1D( HID(title,hypo), title,
                   config.minP, config.maxP, nBins1D() )->fill(pTot,accEff);
    title = "Eff. RICH acceptance Versus Pt (MeV/c)";
    richProfile1D( HID(title,hypo), title,
                   config.minPt, config.maxPt, nBins1D() )->fill(pT,accEff);
    title = "Eff. RICH acceptance Versus P,Pt (MeV/c)";
    richProfile2D( HID(title,hypo), title,
                   config.minP,  config.maxP,  nBins2D(),
                   config.minPt, config.maxPt, nBins2D() )->fill(pTot,pT,accEff);
    
  }
  
}

void PIDPlots::plots( const LHCb::RichPID * pid,
                      const Rich::ParticleIDType hypo,
                      const Configuration & config ) const
{
  // Only proceed further with valid RichPID data objects
  if ( !pid ) { return; }

  // track selection
  if ( !selected(pid->track(),config) ) return;

  // Fill 'track' plots
  plots( pid->track(), hypo, config );

  // Now, plots that require RichPID info ...

  // Extra plots
  if ( m_extraHistos )
  {

    // track momentum
    const double pTot = trackP ( pid->track() );
    const double pT   = trackPt( pid->track() );

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
        std::string title = "RichDLL("+DllDiff+")";
        const double dll = pid->particleDeltaLL(first) - pid->particleDeltaLL(last);
        richHisto1D( HID(title,hypo), title,
                     -m_dllRange, m_dllRange, nBins1D() )->fill(dll);

        // Efficiency
        const double eff( dll>m_dllCut ? 100.0 : 0.0 );

        // Efficiency plots
        title = "Eff. RichDLL("+DllDiff+")>0 Versus P (MeV/c)";
        richProfile1D( HID(title,hypo), title,
                       config.minP, config.maxP, nBins1D() )->fill(pTot,eff);
        title = "Eff. RichDLL("+DllDiff+")>0 Versus Pt (MeV/c)";
        richProfile1D( HID(title,hypo), title,
                       config.minPt, config.maxPt, nBins1D() )->fill(pT,eff);
        title = "Eff. RichDLL("+DllDiff+")>0 Versus P,Pt (MeV/c)";
        richProfile2D( HID(title,hypo), title,
                       config.minP,  config.maxP,  nBins2D(),
                       config.minPt, config.maxPt, nBins2D() )->fill(pTot,pT,eff);

        // # Sigma distributions
        title = "# Sigma("+DllDiff+")";
        const double nsigma = pid->nSigmaSeparation(first,last);
        richHisto1D( HID(title,hypo), title,
                     -30, 30, nBins1D() )->fill(nsigma);

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

  // Track momentum in GeV/c
  const double tkPtot = trackP( track );

  // Momentum spectra
  richHisto1D( HID("Ptot",hypo), "Ptot",
               config.minP, config.maxP, nBins1D() )->fill(tkPtot);
}
