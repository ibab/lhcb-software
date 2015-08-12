
//-----------------------------------------------------------------------------
/** @file RichPIDPlots.cpp
 *
 *  Implementation file for RICH tool : Rich::Rec::PIDPlots
 *
 *  @author Chris Jones     Christopher.Rob.Jones@cern.ch
 *  @date   2008-04-14
 */
//-----------------------------------------------------------------------------

// local
#include "RichPIDPlots.h"

//-----------------------------------------------------------------------------

using namespace Rich::Rec;

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( PIDPlots )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
PIDPlots::PIDPlots( const std::string& type,
                    const std::string& name,
                    const IInterface* parent )
  : HistoToolBase ( type, name, parent )
{
  // interface
  declareInterface<Rich::Rec::IPIDPlots>(this);
  // JOs
  declareProperty ( "DllCut",       m_dllCut      = 0.0   );
  declareProperty ( "DllPlotRange", m_dllRange    = 100   );
  // turn off histo printing by default
  setProperty ( "HistoPrint", false );
  // Default number of bins
  setProperty ( "NBins1DHistos", 50 );
  setProperty ( "NBins2DHistos", 20 );
}

PIDPlots::~PIDPlots() {}

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
  if ( config.expertPlots )
  {
    
    // track momentum
    const double pTot = trackP ( track );
    const double pT   = trackPt( track );
    
    // Efficiency to have a RichPID data object associated (~ RICH Acceptance)
    const double accEff( proto->richPID() ? 100.0 : 0.0 );
    
    // Eff v P and Pt plots
    std::string title = "Eff. RICH acceptance V P";
    richProfile1D( HID(title,hypo), title,
                   config.minP, config.maxP, nBins1D(),
                   "Track Momentum (MeV/c)", 
                   "Selection Efficiency (%)" )->fill(pTot,accEff);
    title = "Eff. RICH acceptance Versus Pt";
    richProfile1D( HID(title,hypo), title,
                   config.minPt, config.maxPt, nBins1D(),
                   "Track Transverse Momentum (MeV/c)",
                   "Selection Efficiency (%)" )->fill(pT,accEff);
    title = "Eff. RICH acceptance Versus P,Pt (MeV)";
    richProfile2D( HID(title,hypo), title,
                   config.minP,  config.maxP,  nBins2D(),
                   config.minPt, config.maxPt, nBins2D(),
                   "Track Momentum (MeV/c)", 
                   "Track Transverse Momentum (MeV/c)",
                   "Selection Efficiency (%)" )->fill(pTot,pT,accEff);
 
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
  //if ( msgLevel(MSG::DEBUG) )
  //  debug() << "RichPID " << *pid << endmsg;

  // Extra plots
  if ( config.expertPlots )
  {

    // track momentum
    const double pTot = trackP ( pid->track() );
    const double pT   = trackPt( pid->track() );

    // Heavy or light ?
    const bool heavy = ( (int)hypo > (int)(Rich::Pion) );

    // stream for titles
    std::ostringstream title;

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
        // For given PID type
        title.str("");
        title << "RichDLL(" << DllDiff << ")";
        const double dll = pid->particleDeltaLL(first) - pid->particleDeltaLL(last);
        richHisto1D( HID(title.str(),hypo), title.str(),
                     -m_dllRange, m_dllRange, nBins1D() )->fill(dll);
        // inclusive plot
        richHisto1D( HID("All/"+title.str()), title.str(),
                     -m_dllRange, m_dllRange, nBins1D() )->fill(dll);

        // Efficiency
        const double eff( dll>m_dllCut ? 100.0 : 0.0 );

        // Efficiency plots
        title.str("");
        title << "Eff. RichDLL(" << DllDiff << ")>" << m_dllCut << " Versus P (MeV)";
        richProfile1D( HID(title.str(),hypo), title.str(),
                       config.minP, config.maxP, nBins1D(),
                       "Track Momentum (MeV/c)",
                       "PID Efficiency (%)" )->fill(pTot,eff);
        title.str("");
        title << "Eff. RichDLL(" << DllDiff << ")>" << m_dllCut << " Versus Pt (MeV)";
        richProfile1D( HID(title.str(),hypo), title.str(),
                       config.minPt, config.maxPt, nBins1D(),
                       "Track Transverse Momentum (MeV/c)",
                       "PID Efficiency (%)" )->fill(pT,eff);
        title.str("");
        title << "Eff. RichDLL(" << DllDiff << ")>" << m_dllCut << " Versus P,Pt (MeV)";
        richProfile2D( HID(title.str(),hypo), title.str(),
                       config.minP,  config.maxP,  nBins2D(),
                       config.minPt, config.maxPt, nBins2D(),
                       "Track Momentum (MeV/c)", 
                       "Track Transverse Momentum (MeV/c)",
                       "PID Efficiency (%)" )->fill(pTot,pT,eff);

        // # Sigma distributions
        title.str("");
        title << "# Sigma(" << DllDiff << ")";
        const double nsigma = pid->nSigmaSeparation(first,last);
        richHisto1D( HID(title.str(),hypo), title.str(),
                     -30, 30, nBins1D() )->fill(nsigma);
        title.str("");
        title << "# Sigma(" << DllDiff << ") Versus P (MeV)";
        richProfile1D( HID(title.str(),hypo), title.str(),
                       config.minP, config.maxP, nBins1D(),
                       "Track Momentum (MeV/c)",
                       title.str() )->fill(pTot,nsigma);

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
  richHisto1D( HID("Ptot",hypo), "Track Momentum",
               config.minP, config.maxP, nBins1D(),
               "Track Momentum (MeV/c)" )->fill(tkPtot);
}
