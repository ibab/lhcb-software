// $Id: RichSellmeirFunc.cpp,v 1.3 2003-10-13 16:32:34 jonrob Exp $

// from Gaudi
#include "GaudiKernel/ToolFactory.h"
#include "GaudiKernel/ParticleProperty.h"
#include "GaudiKernel/IParticlePropertySvc.h"

// local
#include "RichSellmeirFunc.h"

// CLHEP
#include "CLHEP/Units/PhysicalConstants.h"

// Rich Kernel
#include "RichKernel/MessageSvcStl.h"


//-----------------------------------------------------------------------------
// Implementation file for class : RichSellmeirFunc
//
// 15/03/2002 : Chris Jones   Christopher.Rob.Jones@cern.ch
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

  // Aerogel specific parameters... SHould be in XML
  declareProperty( "WaveIndpTrans", m_waveIndepTrans = 0.78 ); 

}

StatusCode RichSellmeirFunc::initialize() {

  MsgStream msg( msgSvc(), name() );
  msg << MSG::DEBUG << "Initialize" << endreq;

  // Sets up various tools and services
  if ( !RichRecToolBase::initialize() ) return StatusCode::FAILURE;

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
  SmartDataPtr<IDetectorElement> Rich1DE( detSvc(), "/dd/Structure/LHCb/Rich1" );

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
    Rich::RadiatorType rad = (Rich::RadiatorType)iRad;
    double RC3,RC2;
    if ( Rich::Aerogel == rad ) {
      RC3 = 1.;
      RC2 = 1.;
    } else {
      RC3 = 3 * selLorGasFac * m_rho[rad] / m_molW[rad];
      RC2 = 2 * selLorGasFac * m_rho[rad] / m_molW[rad];
    }
    double RF = m_selF1[rad] + m_selF2[rad];
    double RE02 = ( m_selF1[rad]*m_selE2[rad]*m_selE2[rad] +
                    m_selF2[rad]*m_selE1[rad]*m_selE1[rad] ) / RF;
    double RE = ( m_selE2[rad]*m_selE2[rad] +
                  m_selE1[rad]*m_selE1[rad] ) / RF;
    double RG = ( m_selE1[rad]*m_selE1[rad] *
                  m_selE2[rad]*m_selE2[rad] ) / (RF*RE02);
    double RH = RE02/RF;
    double RM = RE + RC2;
    double RS = RG + RC2;
    double RT = sqrt( 0.25*RM*RM - RH*RS );
    double RXSP = sqrt( (RM/2. + RT)/RH );
    double RXSM = sqrt( (RM/2. - RT)/RH );
    m_REP[rad] = sqrt(RE02) * RXSP;
    m_REM[rad] = sqrt(RE02) * RXSM;
    m_RXSPscale[rad] = (RXSP - 1./RXSP);
    m_RXSMscale[rad] = (RXSM - 1./RXSM);
    m_X[rad] = (RC3*sqrt(RE02)/(4.*RT));
  }

  // Informational Printout
  msg << MSG::DEBUG
      << " Using XML version" << endreq;

  return StatusCode::SUCCESS;
}

double RichSellmeirFunc::photonsInEnergyRange( RichRecSegment * segment,
                                               const Rich::ParticleIDType id,
                                               double botEn,
                                               double topEn ) {

  // Some parameters of the segment
  double momentum = segment->trackSegment().bestMomentumMag();
  double Esq = momentum*momentum + m_particleMassSq[id];
  double betaSq = ( Esq>0 ? momentum*momentum/Esq : 0 );
  double gammaSq = Esq/m_particleMassSq[id];
  double length = segment->trackSegment().pathLength();
  Rich::RadiatorType rad = segment->trackSegment().radiator();

  double nPhot = ( 37.0 * length / betaSq ) * ( paraW(rad,topEn) -
                                                paraW(rad,botEn) -
                                                (topEn-botEn)/gammaSq );
  if ( Rich::Aerogel == rad ) nPhot *= m_waveIndepTrans;
  return nPhot;
}

double RichSellmeirFunc::paraW ( Rich::RadiatorType rad, double energy ) {

  double X = m_RXSPscale[rad] * log( (m_REP[rad]+energy)/(m_REP[rad]-energy) );
  double Y = m_RXSMscale[rad] * log( (m_REM[rad]+energy)/(m_REM[rad]-energy) );

  return m_X[rad] * (X-Y);
}

StatusCode RichSellmeirFunc::finalize() {

  MsgStream msg( msgSvc(), name() );
  msg << MSG::DEBUG << "Finalize" << endreq;

  // Execute base class method
  return RichRecToolBase::finalize();
}
