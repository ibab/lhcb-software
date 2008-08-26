
//-----------------------------------------------------------------------------
/** @file RichSellmeirFunc.cpp
 *
 *  Implementation file for tool : Rich::Rec::SellmeirFunc
 *
 *  CVS Log :-
 *  $Id: RichSellmeirFunc.cpp,v 1.23 2008-08-26 19:52:28 jonrob Exp $
 *
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/03/2002
 */
//-----------------------------------------------------------------------------

// local
#include "RichSellmeirFunc.h"

// All code is in general Rich reconstruction namespace
using namespace Rich::Rec;

//-----------------------------------------------------------------------------

DECLARE_TOOL_FACTORY( SellmeirFunc );

// Standard constructor
SellmeirFunc::SellmeirFunc ( const std::string& type,
                             const std::string& name,
                             const IInterface* parent )
  : Rich::Rec::ToolBase( type, name, parent ),
    m_Rich1DE  ( NULL ),
    m_partProp ( NULL )
{
  // interface
  declareInterface<ISellmeirFunc>(this);
}

StatusCode SellmeirFunc::initialize()
{
  // Sets up various tools and services
  StatusCode sc = Rich::Rec::ToolBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  // Get Rich1 Detector element
  m_Rich1DE = getDet<DeRich1>( DeRichLocations::Rich1 );

  // Acquire instances of tools
  acquireTool( "RichParticleProperties", m_partProp );

  // register for UMS updates
  //updMgrSvc()->registerCondition( this, m_Rich1DE, &SellmeirFunc::umsUpdate );
  // force first updates
  //sc = updMgrSvc()->update(this);
  umsUpdate();
 
  // return
  return ( sc.isSuccess() ? sc : Error ("Failed first UMS update",sc) );
}

StatusCode SellmeirFunc::umsUpdate()
{
  info() << "Sellmeir parameter update triggered" << endreq;

  // Retrieve square of particle masses
  m_particleMassSq[Rich::Electron] = m_partProp->massSq( Rich::Electron );
  m_particleMassSq[Rich::Muon]     = m_partProp->massSq( Rich::Muon );
  m_particleMassSq[Rich::Pion]     = m_partProp->massSq( Rich::Pion );
  m_particleMassSq[Rich::Kaon]     = m_partProp->massSq( Rich::Kaon );
  m_particleMassSq[Rich::Proton]   = m_partProp->massSq( Rich::Proton );

  // Load radiator parameters from XML
  m_selF1[Rich::Aerogel]    = m_Rich1DE->param<double>("SellAgelF1Param");
  m_selF1[Rich::Rich1Gas]   = m_Rich1DE->param<double>("SellC4F10F1Param");
  m_selF1[Rich::Rich2Gas]   = m_Rich1DE->param<double>("SellCF4F1Param");
  m_selF2[Rich::Aerogel]    = m_Rich1DE->param<double>("SellAgelF2Param");
  m_selF2[Rich::Rich1Gas]   = m_Rich1DE->param<double>("SellC4F10F2Param");
  m_selF2[Rich::Rich2Gas]   = m_Rich1DE->param<double>("SellCF4F2Param");
  m_selE1[Rich::Aerogel]    = m_Rich1DE->param<double>("SellAgelE1Param");
  m_selE1[Rich::Rich1Gas]   = m_Rich1DE->param<double>("SellC4F10E1Param");
  m_selE1[Rich::Rich2Gas]   = m_Rich1DE->param<double>("SellCF4E1Param");
  m_selE2[Rich::Aerogel]    = m_Rich1DE->param<double>("SellAgelE2Param");
  m_selE2[Rich::Rich1Gas]   = m_Rich1DE->param<double>("SellC4F10E2Param");
  m_selE2[Rich::Rich2Gas]   = m_Rich1DE->param<double>("SellCF4E2Param");
  m_molW[Rich::Aerogel]     = 0;
  m_molW[Rich::Rich1Gas]    = m_Rich1DE->param<double>("GasMolWeightC4F10Param");
  m_molW[Rich::Rich2Gas]    = m_Rich1DE->param<double>("GasMolWeightCF4Param");
  m_rho[Rich::Aerogel]      = 0;
  m_rho[Rich::Rich1Gas]     = m_Rich1DE->param<double>("RhoEffectiveSellC4F10Param");
  m_rho[Rich::Rich2Gas]     = m_Rich1DE->param<double>("RhoEffectiveSellCF4Param");
  const double selLorGasFac = m_Rich1DE->param<double>("SellLorGasFacParam");

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
    const double RXSP = std::sqrt( (RM/2. + RT)/RH );
    const double RXSM = std::sqrt( (RM/2. - RT)/RH );
    m_REP[rad] = std::sqrt(RE02) * RXSP;
    m_REM[rad] = std::sqrt(RE02) * RXSM;
    m_RXSPscale[rad] = (RXSP - 1./RXSP);
    m_RXSMscale[rad] = (RXSM - 1./RXSM);
    m_X[rad] = (RC3*std::sqrt(RE02)/(4.*RT));
  }

  return StatusCode::SUCCESS;
}


double SellmeirFunc::photonsInEnergyRange( const LHCb::RichRecSegment * segment,
                                           const Rich::ParticleIDType id,
                                           const double botEn,
                                           const double topEn ) const
{
  // Some parameters of the segment
  const double momentum = std::sqrt(segment->trackSegment().bestMomentum().Mag2());
  const double length   = segment->trackSegment().pathLength();
  const Rich::RadiatorType rad = segment->trackSegment().radiator();
  const double Esq      = momentum*momentum + m_particleMassSq[id];
  const double betaSq   = ( Esq>0 ? momentum*momentum/Esq : 0 );
  const double gammaSq  = Esq/m_particleMassSq[id];

  // Compute number of photons
  double nPhot = ( 37.0 / betaSq ) * ( paraW(rad,topEn) -
                                       paraW(rad,botEn) -
                                       (topEn-botEn)/gammaSq );

  // correct for wavelength independant transmission coeff. in aerogel
  if ( Rich::Aerogel == rad )
  {
    // get the radiator intersections
    const Rich::RadIntersection::Vector & radInts
      = segment->trackSegment().radIntersections();

    // normalise over each intersection
    double totPlength(0), waveIndepTrans(0);
    if ( !radInts.empty() )
    {
      // average energy for this range
      const double avEn = 0.5 * (botEn+topEn) * Gaudi::Units::eV;

      // average over all intersections
      for ( Rich::RadIntersection::Vector::const_iterator iR = radInts.begin();
            iR != radInts.end(); ++iR )
      {
        const double pLen = (*iR).pathLength();
        const double absL = (*iR).radiator()->absorption()->value(avEn);
        totPlength       += pLen;
        waveIndepTrans   += pLen * std::exp( -pLen / absL );
      }
      if ( totPlength>0 ) waveIndepTrans /= totPlength;

      // scale the expected photon yield
      nPhot *= waveIndepTrans;
    }
  }

  // return
  return length * ( nPhot < 0 ? 0 : nPhot );
}
