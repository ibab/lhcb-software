// $Id: RichSellmeirFunc.cpp,v 1.1 2003-08-06 11:08:14 jonrob Exp $

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

  // Define job option parameters
}

StatusCode RichSellmeirFunc::initialize() {

  MsgStream msg( msgSvc(), name() );
  msg << MSG::DEBUG << "Initialize" << endreq;

  // Sets up various tools and services
  if ( !RichRecToolBase::initialize() ) return StatusCode::FAILURE;

  // Retrieve particle property service
  IParticlePropertySvc * ppSvc = 0;
  if ( !serviceLocator()->service( "ParticlePropertySvc", ppSvc ) ) {
    msg << MSG::ERROR << "ParticlePropertySvc not found" << endreq;
    return StatusCode::FAILURE;
  }

  // Retrieve particle masses
  m_particleMass.push_back( ppSvc->find("e+" )->mass()/MeV );
  m_particleMass.push_back( ppSvc->find("mu+")->mass()/MeV );
  m_particleMass.push_back( ppSvc->find("pi+")->mass()/MeV );
  m_particleMass.push_back( ppSvc->find("K+" )->mass()/MeV );
  m_particleMass.push_back( ppSvc->find("p+" )->mass()/MeV );
  // cache squares of masses
  m_particleMassSq.push_back( m_particleMass[ Rich::Electron ] *
                              m_particleMass[ Rich::Electron ] );
  m_particleMassSq.push_back( m_particleMass[ Rich::Muon ] *
                              m_particleMass[ Rich::Muon ] );
  m_particleMassSq.push_back( m_particleMass[ Rich::Pion ] *
                              m_particleMass[ Rich::Pion ] );
  m_particleMassSq.push_back( m_particleMass[ Rich::Kaon ] *
                              m_particleMass[ Rich::Kaon ] );
  m_particleMassSq.push_back( m_particleMass[ Rich::Proton ] *
                              m_particleMass[ Rich::Proton ] );

  // release particle property service
  ppSvc->release();
 
  // should get from XML
  m_selF1[Rich::Aerogel] = 2.653;
  m_selF1[Rich::C4F10]   = 12305.3;
  m_selF1[Rich::CF4]     = 7626.16;
  m_selF2[Rich::Aerogel] = 13.075;
  m_selF2[Rich::C4F10]   = 0.0;
  m_selF2[Rich::CF4]     = 0.0;
  m_selE1[Rich::Aerogel] = 10.666;
  m_selE1[Rich::C4F10]   = 18.938;
  m_selE1[Rich::CF4]     = 20.003;
  m_selE2[Rich::Aerogel] = 18.125;
  m_selE2[Rich::C4F10]   = 50.0;
  m_selE2[Rich::CF4]     = 50.0;
  m_molW[Rich::Aerogel]  = 0;
  m_molW[Rich::C4F10]    = 138.0;
  m_molW[Rich::CF4]      = 88.0;
  m_rho[Rich::Aerogel]   = 0;
  m_rho[Rich::C4F10]     = 0.00964;
  m_rho[Rich::CF4]       = 0.00366;

  // Informational Printout
  // msg << MSG::DEBUG;

  return StatusCode::SUCCESS;
}

double RichSellmeirFunc::photonsInEnergyRange( RichRecSegment * segment,
                                               const Rich::ParticleIDType id,
                                               double botEn,
                                               double topEn ) {

  // Some parameters of the segment
  double momentum = segment->trackSegment().bestMomentumMag();
  double Esq = momentum * momentum + m_particleMassSq[id];
  double betaSq = ( Esq>0 ? momentum*momentum/Esq : 0 );
  double gammaSq = Esq/m_particleMassSq[id];
  double length = segment->trackSegment().pathLength();
  Rich::RadiatorType rad = segment->trackSegment().radiator();

  return ( 37.0 * length / betaSq ) * ( paraWt(rad,topEn) -
                                        paraWb(rad,botEn) -
                                        (topEn-botEn)/gammaSq );
}

double RichSellmeirFunc::paraWb ( Rich::RadiatorType rad, double botEn ) {

  double RC3,RC2;
  if ( Rich::Aerogel == rad ) {
    RC3 = 1.;
    RC2 = 1.;
  } else {
    RC3 = 3.*0.3738*m_rho[rad]/m_molW[rad];
    RC2 = 2.*0.3738*m_rho[rad]/m_molW[rad];
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
  double REP = sqrt(RE02) * RXSP;
  double REM = sqrt(RE02) * RXSM;
  double XEBEP = (RXSP - 1./RXSP) * log( (REP+botEn)/(REP-botEn) );
  double XEBEM = (RXSM - 1./RXSM) * log( (REM+botEn)/(REM-botEn) );

  // finally W parameter
  return (RC3*sqrt(RE02)/(4.*RT))*(XEBEP-XEBEM);
}

double RichSellmeirFunc::paraWt ( Rich::RadiatorType rad, double topEn ) {

  double RC3,RC2;
  if ( Rich::Aerogel == rad ) {
    RC3 = 1.;
    RC2 = 1.;
  } else {
    RC3 = 3.*0.3738*m_rho[rad]/m_molW[rad];
    RC2 = 2.*0.3738*m_rho[rad]/m_molW[rad];
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
  double REP = sqrt(RE02) * RXSP;
  double REM = sqrt(RE02) * RXSM;
  double XETEP = (RXSP - 1./RXSP) * log( (REP+topEn)/(REP-topEn) );
  double XETEM = (RXSM - 1./RXSM) * log( (REM+topEn)/(REM-topEn) );

  // finally W parameter
  return (RC3*sqrt(RE02)/(4.*RT))*(XETEP-XETEM);
}

StatusCode RichSellmeirFunc::finalize() {

  MsgStream msg( msgSvc(), name() );
  msg << MSG::DEBUG << "Finalize" << endreq;

  // release tools

  // Execute base class method
  return RichRecToolBase::finalize();
}
