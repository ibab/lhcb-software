
//-----------------------------------------------------------------------------
/** @file RichSellmeirFunc.cpp
 *
 *  Implementation file for tool : RichSellmeirFunc
 *
 *  CVS Log :-
 *  $Id: RichSellmeirFunc.cpp,v 1.11 2005-02-17 09:56:20 jonrob Exp $
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

// CLHEP
#include "CLHEP/Units/PhysicalConstants.h"

//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
static const  ToolFactory<RichSellmeirFunc>          s_factory ;
const        IToolFactory& RichSellmeirFuncFactory = s_factory ;

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
  IRichParticleProperties * partProp;
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
  m_selF1[Rich::Aerogel] = Rich1DE->userParameterAsDouble("SellAgelF1Param");
  m_selF1[Rich::C4F10]   = Rich1DE->userParameterAsDouble("SellC4F10F1Param");
  m_selF1[Rich::CF4]     = Rich1DE->userParameterAsDouble("SellCF4F1Param");
  m_selF2[Rich::Aerogel] = Rich1DE->userParameterAsDouble("SellAgelF2Param");
  m_selF2[Rich::C4F10]   = Rich1DE->userParameterAsDouble("SellC4F10F2Param");
  m_selF2[Rich::CF4]     = Rich1DE->userParameterAsDouble("SellCF4F2Param");
  m_selE1[Rich::Aerogel] = Rich1DE->userParameterAsDouble("SellAgelE1Param");
  m_selE1[Rich::C4F10]   = Rich1DE->userParameterAsDouble("SellC4F10E1Param");
  m_selE1[Rich::CF4]     = Rich1DE->userParameterAsDouble("SellCF4E1Param");
  m_selE2[Rich::Aerogel] = Rich1DE->userParameterAsDouble("SellAgelE2Param");
  m_selE2[Rich::C4F10]   = Rich1DE->userParameterAsDouble("SellC4F10E2Param");
  m_selE2[Rich::CF4]     = Rich1DE->userParameterAsDouble("SellCF4E2Param");
  m_molW[Rich::Aerogel]  = 0;
  m_molW[Rich::C4F10]    = Rich1DE->userParameterAsDouble("GasMolWeightC4F10Param");
  m_molW[Rich::CF4]      = Rich1DE->userParameterAsDouble("GasMolWeightCF4Param");
  m_rho[Rich::Aerogel]   = 0;
  m_rho[Rich::C4F10]     = Rich1DE->userParameterAsDouble("RhoEffectiveSellC4F10Param");
  m_rho[Rich::CF4]       = Rich1DE->userParameterAsDouble("RhoEffectiveSellCF4Param");
  double selLorGasFac    = Rich1DE->userParameterAsDouble("SellLorGasFacParam");

  // Initialise the calculations and cache as much as possible for efficiency
  for ( int iRad = 0; iRad < Rich::NRadiatorTypes; ++iRad ) {
    const Rich::RadiatorType rad = static_cast<Rich::RadiatorType>(iRad);
    double RC3,RC2;
    if ( Rich::Aerogel == rad ) {
      RC3 = 1.;
      RC2 = 1.;
    } else {
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
  const double momentum = segment->trackSegment().bestMomentum().mag();
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
