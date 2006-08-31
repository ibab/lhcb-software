
//-----------------------------------------------------------------------------
/** @file RichSellmeirFunc.cpp
 *
 *  Implementation file for tool : RichSellmeirFunc
 *
 *  CVS Log :-
 *  $Id: RichSellmeirFunc.cpp,v 1.16 2006-08-31 13:38:25 cattanem Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */
//-----------------------------------------------------------------------------

// from Gaudi
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/ParticleProperty.h"
#include "GaudiKernel/IParticlePropertySvc.h"

// local
#include "RichSellmeirFunc.h"

// namespaces
using namespace LHCb;

//-----------------------------------------------------------------------------

DECLARE_TOOL_FACTORY( RichSellmeirFunc );

// Standard constructor
RichSellmeirFunc::RichSellmeirFunc ( const std::string& type,
                                     const std::string& name,
                                     const IInterface* parent )
  : RichRecToolBase( type, name, parent ) {

  declareInterface<IRichSellmeirFunc>(this);

  // Aerogel specific parameters... Should be in XML
  declareProperty( "WaveIndpTrans", m_waveIndepTrans = 0.78 );

}

StatusCode RichSellmeirFunc::initialize() {

  // Sets up various tools and services
  const StatusCode sc = RichRecToolBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  // Acquire instances of tools
  const IRichParticleProperties * partProp;
  acquireTool( "RichParticleProperties", partProp );

  // Retrieve square of particle masses
  m_particleMassSq[Rich::Electron] = partProp->massSq( Rich::Electron );
  m_particleMassSq[Rich::Muon]     = partProp->massSq( Rich::Muon );
  m_particleMassSq[Rich::Pion]     = partProp->massSq( Rich::Pion );
  m_particleMassSq[Rich::Kaon]     = partProp->massSq( Rich::Kaon );
  m_particleMassSq[Rich::Proton]   = partProp->massSq( Rich::Proton );

  // release particle properties tool
  releaseTool( partProp );

  // Get Rich1 Detector element
  DeRich1 * Rich1DE = getDet<DeRich1>( DeRichLocation::Rich1 );

  // Load radiator parameters from XML
  m_selF1[Rich::Aerogel]  = Rich1DE->param<double>("SellAgelF1Param");
  m_selF1[Rich::Rich1Gas] = Rich1DE->param<double>("SellC4F10F1Param");
  m_selF1[Rich::Rich2Gas] = Rich1DE->param<double>("SellCF4F1Param");
  m_selF2[Rich::Aerogel]  = Rich1DE->param<double>("SellAgelF2Param");
  m_selF2[Rich::Rich1Gas] = Rich1DE->param<double>("SellC4F10F2Param");
  m_selF2[Rich::Rich2Gas] = Rich1DE->param<double>("SellCF4F2Param");
  m_selE1[Rich::Aerogel]  = Rich1DE->param<double>("SellAgelE1Param");
  m_selE1[Rich::Rich1Gas] = Rich1DE->param<double>("SellC4F10E1Param");
  m_selE1[Rich::Rich2Gas] = Rich1DE->param<double>("SellCF4E1Param");
  m_selE2[Rich::Aerogel]  = Rich1DE->param<double>("SellAgelE2Param");
  m_selE2[Rich::Rich1Gas] = Rich1DE->param<double>("SellC4F10E2Param");
  m_selE2[Rich::Rich2Gas] = Rich1DE->param<double>("SellCF4E2Param");
  m_molW[Rich::Aerogel]   = 0;
  m_molW[Rich::Rich1Gas]  = Rich1DE->param<double>("GasMolWeightC4F10Param");
  m_molW[Rich::Rich2Gas]  = Rich1DE->param<double>("GasMolWeightCF4Param");
  m_rho[Rich::Aerogel]    = 0;
  m_rho[Rich::Rich1Gas]   = Rich1DE->param<double>("RhoEffectiveSellC4F10Param");
  m_rho[Rich::Rich2Gas]   = Rich1DE->param<double>("RhoEffectiveSellCF4Param");
  const double selLorGasFac = Rich1DE->param<double>("SellLorGasFacParam");

  // Initialise the calculations and cache as much as possible for efficiency
  for ( int iRad = 0; iRad < Rich::NRadiatorTypes; ++iRad )
  {
    const Rich::RadiatorType rad = static_cast<Rich::RadiatorType>(iRad);
    double RC3,RC2;
    if ( Rich::Aerogel == rad )
    {
      RC3 = 1.;
      RC2 = 1.;
    }
    else
    {
      RC3 = 3. * selLorGasFac * m_rho[rad] / m_molW[rad];
      RC2 = 2. * selLorGasFac * m_rho[rad] / m_molW[rad];
    }
    const double RF = m_selF1[rad] + m_selF2[rad];
    const double RE02 = ( m_selF1[rad]*m_selE2[rad]*m_selE2[rad] +
                          m_selF2[rad]*m_selE1[rad]*m_selE1[rad] ) / RF;
    const double RE = ( m_selE2[rad]*m_selE2[rad] +
                        m_selE1[rad]*m_selE1[rad] ) / RF;
    const double RG = ( m_selE1[rad]*m_selE1[rad] *
                        m_selE2[rad]*m_selE2[rad] ) / (RF*RE02);
    const double RH = RE02/RF;
    const double RM = RE + RC2;
    const double RS = RG + RC2;
    const double RT = sqrt( 0.25*RM*RM - RH*RS );
    const double RXSP = sqrt( (RM/2. + RT)/RH );
    const double RXSM = sqrt( (RM/2. - RT)/RH );
    m_REP[rad] = sqrt(RE02) * RXSP;
    m_REM[rad] = sqrt(RE02) * RXSM;
    m_RXSPscale[rad] = (RXSP - 1./RXSP);
    m_RXSMscale[rad] = (RXSM - 1./RXSM);
    m_X[rad] = (RC3*sqrt(RE02)/(4.*RT));
  }

  return sc;
}

StatusCode RichSellmeirFunc::finalize()
{
  // Execute base class method
  return RichRecToolBase::finalize();
}

double RichSellmeirFunc::photonsInEnergyRange( RichRecSegment * segment,
                                               const Rich::ParticleIDType id,
                                               const double botEn,
                                               const double topEn ) const
{

  // Some parameters of the segment
  const double momentum = sqrt(segment->trackSegment().bestMomentum().Mag2());
  const double Esq      = momentum*momentum + m_particleMassSq[id];
  const double betaSq   = ( Esq>0 ? momentum*momentum/Esq : 0 );
  const double gammaSq  = Esq/m_particleMassSq[id];
  const double length   = segment->trackSegment().pathLength();
  const Rich::RadiatorType rad = segment->trackSegment().radiator();

  // Compute number of photons
  double nPhot = ( 37.0 * length / betaSq ) * ( paraW(rad,topEn) -
                                                paraW(rad,botEn) -
                                                (topEn-botEn)/gammaSq );

  // correct for wavelength independant transmission coeff. in aerogel
  if ( Rich::Aerogel == rad ) nPhot *= m_waveIndepTrans;

  return ( nPhot < 0 ? 0 : nPhot );
}

double RichSellmeirFunc::paraW ( const Rich::RadiatorType rad,
                                 const double energy ) const
{
  const double X = m_RXSPscale[rad] * log( (m_REP[rad]+energy)/(m_REP[rad]-energy) );
  const double Y = m_RXSMscale[rad] * log( (m_REM[rad]+energy)/(m_REM[rad]-energy) );
  return m_X[rad] * (X-Y);
}
