// $Id: RichDigiAlgMoni.cpp,v 1.1 2003-09-20 15:12:52 jonesc Exp $

// local
#include "RichDigiAlgMoni.h"

//-----------------------------------------------------------------------------
// Implementation file for class : RichDigiAlgMoni
//
// 2003-09-08 : Chris Jones   (Christopher.Rob.Jones@cern.ch)
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
static const  AlgFactory<RichDigiAlgMoni>          s_factory ;
const        IAlgFactory& RichDigiAlgMoniFactory = s_factory ;

// Standard constructor, initializes variables
RichDigiAlgMoni::RichDigiAlgMoni( const std::string& name,
                                  ISvcLocator* pSvcLocator)
  : Algorithm ( name, pSvcLocator ),
    m_mapmtDet(0),
    m_sicbDet (0),
    m_detInt  (0) {

  // Declare job options
  declareProperty( "InputRichDigits", m_digitTES = RichDigitLocation::Default );
  declareProperty( "InputMCRichDigits", m_mcdigitTES = MCRichDigitLocation::Default );
  declareProperty( "InputMCRichDeposits", m_mcdepTES = MCRichDepositLocation::Default );
  declareProperty( "InputMCRichHits", m_mchitTES = MCRichHitLocation::Default );
  declareProperty( "HistoPath", m_histPth = "RICH/DIGI/ALGQC" );
  declareProperty( "DetMode", m_detMode = "HPDSICB" );

}

// Destructor
RichDigiAlgMoni::~RichDigiAlgMoni() {};

// Initialisation
StatusCode RichDigiAlgMoni::initialize() {

  MsgStream msg(msgSvc(), name());
  msg << MSG::DEBUG << "Initialize" << endreq;

  if ( "HPDSICB" == m_detMode ) {
    // Use the temporary SICB compatible tool for SICB data
    if ( !toolSvc()->retrieveTool( "PixelFinder", m_sicbDet ) ) {
      msg << MSG::ERROR << "Unable to create PixelFinder tool" << endreq;
      return StatusCode::FAILURE;
    }
  } else if ( "MaPMTSICB" == m_detMode ) {
    // Use the temporary tool for MaPMTs from SICB data
    if ( !toolSvc()->retrieveTool( "MaPMTDetTool", m_mapmtDet ) ) {
      msg << MSG::ERROR << "Unable to create MaPMTDetTool" << endreq;
      return StatusCode::FAILURE;
    }
  } else if ( "GAUSS" == m_detMode ) {
    // The main tool. For Gauss data
    if ( !toolSvc()->retrieveTool("RichDetInterface" , m_detInt ) ) {
      msg << MSG::ERROR << "Unable to create RichDetInterface" << endreq;
      return StatusCode::FAILURE;
    }
  } else {
    msg << MSG::ERROR << "Unknown detector mode " << m_detMode << endreq;
    return StatusCode::FAILURE;
  }

  // Book histograms
  if ( !bookHistograms() ) return StatusCode::FAILURE;

  msg << MSG::DEBUG
      << " Histogram location   = " << m_histPth << endreq
      << " Detector Mode        = " << m_detMode << endreq;

  return StatusCode::SUCCESS;
};

// Main execution
StatusCode RichDigiAlgMoni::execute() {

  MsgStream msg( msgSvc(), name() );
  msg << MSG::DEBUG << "Execute" << endreq;

  // Initialise mult counts
  double digMult[Rich::NRiches];
  double hitMult[Rich::NRiches];
  hitMult[Rich::Rich1] = 0;
  hitMult[Rich::Rich2] = 0;
  digMult[Rich::Rich1] = 0;
  digMult[Rich::Rich2] = 0;

  // Locate MCRichDigits
  SmartDataPtr<MCRichDigits> mcRichDigits( eventSvc(), m_mcdigitTES );
  if ( !mcRichDigits ) {
    msg << MSG::DEBUG << "Cannot locate MCRichDigits at " << m_mcdigitTES << endreq;
  } else {
    msg << MSG::DEBUG << "Successfully located " << mcRichDigits->size()
        << " MCRichDigits at " << m_mcdigitTES << endreq;

    // Loop over all MCRichDigits
    for ( MCRichDigits::const_iterator iMcDigit = mcRichDigits->begin();
          iMcDigit != mcRichDigits->end(); ++iMcDigit ) {

      RichSmartID id = (*iMcDigit)->key();

      // increment digit count
      ++digMult[id.rich()];

      HepPoint3D point;
      if ( !getPosition( id, point ) ) {
        msg << MSG::WARNING << "Position conversion error : ID = " << id << endreq;
      }

      m_pdDigsXGlobal[id.rich()]->fill( point.x() );
      m_pdDigsYGlobal[id.rich()]->fill( point.y() );
      m_pdDigsZGlobal[id.rich()]->fill( point.z() );
      m_pdDigsXYGlobal[id.rich()]->fill( point.x(), point.y() );
      m_pdDigsXZGlobal[id.rich()]->fill( point.z(), point.x() );
      m_pdDigsYZGlobal[id.rich()]->fill( point.z(), point.y() );
      m_pdCloseUpXZ[id.rich()]->fill( point.z(), point.x() );
      m_pdCloseUpYZ[id.rich()]->fill( point.z(), point.y() );

      // loop over all hits associated to the digit
      SmartRefVector<MCRichHit>& mcHits = (*iMcDigit)->hits();
      for ( SmartRefVector<MCRichHit>::const_iterator iHit = mcHits.begin();
            iHit != mcHits.end(); ++iHit ) {
        // Compare digit/hit
        m_digiErrX[id.rich()]->fill( point.x() - (*iHit)->entry().x() );
        m_digiErrY[id.rich()]->fill( point.y() - (*iHit)->entry().y() );
        m_digiErrZ[id.rich()]->fill( point.z() - (*iHit)->entry().z() );
        m_digiErrR[id.rich()]->fill( point.distance( (*iHit)->entry() ) );
      }

    } // MCRichDigits Loop

    // Fill multiplicity plots
    m_digitMult[Rich::Rich1]->fill( digMult[Rich::Rich1] );
    m_digitMult[Rich::Rich2]->fill( digMult[Rich::Rich2] );

  } // MCRichDigits exist

  // Locate MCRichDeposits
  SmartDataPtr<MCRichDeposits> deps( eventSvc(), m_mcdepTES );
  if ( !deps ) {
    msg << MSG::DEBUG << "Cannot locate MCRichDeposits at "
        << MCRichDepositLocation::Default << endreq;
  } else {
    msg << MSG::DEBUG << "Successfully located " << deps->size()
        << " MCRichDeposits at " << MCRichDepositLocation::Default << endreq;

    // Loop over deposits
    for ( MCRichDeposits::const_iterator iDep = deps->begin();
          iDep != deps->end(); ++iDep ) {

      // Which RICH ?
      int rich = (*iDep)->parentHit()->rich();
      // TOF for this deposit
      m_tofDep[rich]->fill( (*iDep)->time() );
      // Deposit energy
      m_depEnDep[rich]->fill( (*iDep)->energy() );

    } // MCRichDeposits loop

  } // MCRichDeposits exist

  // Locate MCRichHits
  SmartDataPtr<MCRichHits> hits( eventSvc(), m_mchitTES );
  if ( !hits ) {
    msg << MSG::DEBUG << "Cannot locate MCRichHits at "
        << MCRichDepositLocation::Default << endreq;
  } else {

    // Loop over hits
    for ( MCRichHits::const_iterator iHit = hits->begin();
          iHit != hits->end(); ++iHit ) {

      // Which RICH ?
      int rich = (*iHit)->rich();
      // TOF for this hit
      m_tofHit[rich]->fill( (*iHit)->timeOfFlight() );
      // hit energy
      m_depEnHit[rich]->fill( (*iHit)->energy() );

      // increment hit count
      ++hitMult[rich];

      HepPoint3D & point = (*iHit)->entry();
      m_mcHitsXGlobal[rich]->fill( point.x() );
      m_mcHitsYGlobal[rich]->fill( point.y() );
      m_mcHitsZGlobal[rich]->fill( point.z() );
      m_mcHitsXYGlobal[rich]->fill( point.x(), point.y() );
      m_mcHitsXZGlobal[rich]->fill( point.z(), point.x() );
      m_mcHitsYZGlobal[rich]->fill( point.z(), point.y() );
      m_mcCloseUpXZ[rich]->fill( point.z(), point.x() );
      m_mcCloseUpYZ[rich]->fill( point.z(), point.y() );

    }

    // Fill multiplicity plots
    m_hitMult[Rich::Rich1]->fill( hitMult[Rich::Rich1] );
    m_hitMult[Rich::Rich2]->fill( hitMult[Rich::Rich2] );

  } // MCRichHits exist

  return StatusCode::SUCCESS;
};

bool RichDigiAlgMoni::getPosition( const RichSmartID & id, HepPoint3D & position ) {

  if ( "HPDSICB" == m_detMode ) {
    position = 10.0 * (m_sicbDet->globalPosition(id));
    return true;
  } else if ( "MaPMTSICB" == m_detMode ) {
    return m_mapmtDet->cdfDetectionPoint( id, position );
  } else if ( "GAUSS" == m_detMode ) {
    return m_detInt->globalPosition( id, position );
  }

  return false;
}

//  Finalize
StatusCode RichDigiAlgMoni::finalize() {

  MsgStream msg(msgSvc(), name());
  msg << MSG::DEBUG << "Finalize" << endreq;

  // release tools
  if (m_sicbDet) { toolSvc()->releaseTool(m_sicbDet); m_sicbDet=0; }
  if (m_mapmtDet) { toolSvc()->releaseTool(m_mapmtDet); m_mapmtDet=0; }
  if (m_detInt) { toolSvc()->releaseTool(m_detInt); m_detInt=0; }

  return StatusCode::SUCCESS;
}


StatusCode RichDigiAlgMoni::bookHistograms() {

  std::string title;
  int nBins = 100;
  int id;

  // defines for various parameters
  RICH_HISTO_OFFSET;
  RICH_NAMES;
  PD_GLOBAL_POSITIONS;
  double maxMult[] = { 5000, 2000 };

  for ( int iRich = 0; iRich<Rich::NRiches; ++iRich ) { // loop over rich detectors

    title = rich[iRich]+" MCRichDigit Occupancy";
    id = (1+iRich)*richOffset + 1;
    m_digitMult[iRich] = histoSvc()->book(m_histPth, id, title, nBins, 0, maxMult[iRich] );

    title = rich[iRich]+" MCRichHit Occupancy";
    id = (1+iRich)*richOffset + 2;
    m_hitMult[iRich] = histoSvc()->book(m_histPth, id, title, nBins, 0, maxMult[iRich] );

    title = rich[iRich]+" digits x global";
    id = (1+iRich)*richOffset + 11;
    m_pdDigsXGlobal[iRich] = histoSvc()->book(m_histPth,id,title,
                                              nBins,xMinPDGlo[iRich],xMaxPDGlo[iRich]);

    title = rich[iRich]+" digits y global";
    id = (1+iRich)*richOffset + 12;
    m_pdDigsYGlobal[iRich] = histoSvc()->book(m_histPth,id,title,
                                              nBins,yMinPDGlo[iRich],yMaxPDGlo[iRich]);

    title = rich[iRich]+" digits z global";
    id = (1+iRich)*richOffset + 13;
    m_pdDigsZGlobal[iRich] = histoSvc()->book(m_histPth,id,title,
                                              nBins,zMinPDGlo[iRich],zMaxPDGlo[iRich]);

    title = rich[iRich]+" digits yVx global";
    id = (1+iRich)*richOffset + 14;
    m_pdDigsXYGlobal[iRich] = histoSvc()->book(m_histPth,id,title,
                                               nBins,xMinPDGlo[iRich],xMaxPDGlo[iRich],
                                               nBins,yMinPDGlo[iRich],yMaxPDGlo[iRich]);

    title = rich[iRich]+" digits xVz global";
    id = (1+iRich)*richOffset + 15;
    m_pdDigsXZGlobal[iRich] = histoSvc()->book(m_histPth,id,title,
                                               nBins,zMinPDGlo[iRich],zMaxPDGlo[iRich],
                                               nBins,xMinPDGlo[iRich],xMaxPDGlo[iRich]);

    title = rich[iRich]+" digits yVz global";
    id = (1+iRich)*richOffset + 16;
    m_pdDigsYZGlobal[iRich] = histoSvc()->book(m_histPth,id,title,
                                               nBins,zMinPDGlo[iRich],zMaxPDGlo[iRich],
                                               nBins,yMinPDGlo[iRich],yMaxPDGlo[iRich]);

    title = rich[iRich]+" mchits x global";
    id = (1+iRich)*richOffset + 21;
    m_mcHitsXGlobal[iRich] = histoSvc()->book(m_histPth,id,title,
                                              nBins,xMinPDGlo[iRich],xMaxPDGlo[iRich]);

    title = rich[iRich]+" mchits y global";
    id = (1+iRich)*richOffset + 22;
    m_mcHitsYGlobal[iRich] = histoSvc()->book(m_histPth,id,title,
                                              nBins,yMinPDGlo[iRich],yMaxPDGlo[iRich]);

    title = rich[iRich]+" mchits z global";
    id = (1+iRich)*richOffset + 23;
    m_mcHitsZGlobal[iRich] = histoSvc()->book(m_histPth,id,title,
                                              nBins,zMinPDGlo[iRich],zMaxPDGlo[iRich]);

    title = rich[iRich]+" mchits yVx global";
    id = (1+iRich)*richOffset+24;
    m_mcHitsXYGlobal[iRich] = histoSvc()->book(m_histPth,id,title,
                                               nBins,xMinPDGlo[iRich],xMaxPDGlo[iRich],
                                               nBins,yMinPDGlo[iRich],yMaxPDGlo[iRich]);

    title = rich[iRich]+" mchits xVz global";
    id = (1+iRich)*richOffset+25;
    m_mcHitsXZGlobal[iRich] = histoSvc()->book(m_histPth,id,title,
                                               nBins,zMinPDGlo[iRich],zMaxPDGlo[iRich],
                                               nBins,xMinPDGlo[iRich],xMaxPDGlo[iRich]);

    title = rich[iRich]+" mchits yVz global";
    id = (1+iRich)*richOffset+26;
    m_mcHitsYZGlobal[iRich] = histoSvc()->book(m_histPth,id,title,
                                               nBins,zMinPDGlo[iRich],zMaxPDGlo[iRich],
                                               nBins,yMinPDGlo[iRich],yMaxPDGlo[iRich]);

    title = rich[iRich]+" digit-hit diff X";
    id = (1+iRich)*richOffset+31;
    m_digiErrX[iRich] = histoSvc()->book(m_histPth,id,title,nBins,-5,5);

    title = rich[iRich]+" digit-hit diff Y";
    id = (1+iRich)*richOffset+32;
    m_digiErrY[iRich] = histoSvc()->book(m_histPth,id,title,nBins,-5,5);

    title = rich[iRich]+" digit-hit diff Z";
    id = (1+iRich)*richOffset+33;
    m_digiErrZ[iRich] = histoSvc()->book(m_histPth,id,title,nBins,-5,5);

    title = rich[iRich]+" digit-hit seperation";
    id = (1+iRich)*richOffset+34;
    m_digiErrR[iRich] = histoSvc()->book(m_histPth,id,title,nBins,0,5);

    title = rich[iRich] + " MCRichDeposit TOF";
    id = richOffset*(iRich+1) + 41;
    m_tofDep[iRich] = histoSvc()->book(m_histPth,id,title,nBins,-0,50);

    title = rich[iRich] + " MCRichDeposit Energy";
    id = richOffset*(iRich+1) + 42;
    m_depEnDep[iRich] = histoSvc()->book(m_histPth,id,title,nBins,0,100);

    title = rich[iRich] + " MCRichHit TOF";
    id = richOffset*(iRich+1) + 43;
    m_tofHit[iRich] = histoSvc()->book(m_histPth,id,title,nBins,-100,100);

    title = rich[iRich] + " MCRichHit Energy";
    id = richOffset*(iRich+1) + 44;
    m_depEnHit[iRich] = histoSvc()->book(m_histPth,id,title,nBins,0,100);

  }

  // PD Close Up plots - Digits
  title = rich[Rich::Rich1]+" PD closeUp xVz RichDigit";
  id = richOffset+51;
  m_pdCloseUpXZ[Rich::Rich1] = histoSvc()->book(m_histPth,id,title,
                                                nBins, 1485, 1560, nBins, -50, 50 );
  title = rich[Rich::Rich1]+" PD closeUp yVz RichDigit";
  id = richOffset+52;
  m_pdCloseUpYZ[Rich::Rich1] = histoSvc()->book(m_histPth,id,title,
                                                nBins, 1485, 1560, nBins, 920, 1020 );
  title = rich[Rich::Rich2]+" PD closeUp xVz RichDigit";
  id = 2*richOffset+51;
  m_pdCloseUpXZ[Rich::Rich2] = histoSvc()->book(m_histPth,id,title,
                                                nBins, 10720, 10800, nBins, 3800, 4000 );
  title = rich[Rich::Rich2]+" PD closeUp yVz RichDigit";
  id = 2*richOffset+52;
  m_pdCloseUpYZ[Rich::Rich2] = histoSvc()->book(m_histPth,id,title,
                                                nBins, 10720, 10800, nBins, -70, 30 );

  // PD Close Up plots - MCHits
  title = rich[Rich::Rich1]+" PD closeUp xVz MCRichHit";
  id = richOffset+61;
  m_mcCloseUpXZ[Rich::Rich1] = histoSvc()->book(m_histPth,id,title,
                                                nBins, 1485, 1560, nBins, -50, 50 );
  title = rich[Rich::Rich1]+" PD closeUp yVz MCRichHit";
  id = richOffset+62;
  m_mcCloseUpYZ[Rich::Rich1] = histoSvc()->book(m_histPth,id,title,
                                                nBins, 1485, 1560, nBins, 920, 1020 );
  title = rich[Rich::Rich2]+" PD closeUp xVz MCRichHit";
  id = 2*richOffset+61;
  m_mcCloseUpXZ[Rich::Rich2] = histoSvc()->book(m_histPth,id,title,
                                                nBins, 10720, 10800, nBins, 3800, 4000 );
  title = rich[Rich::Rich2]+" PD closeUp yVz MCRichHit";
  id = 2*richOffset+62;
  m_mcCloseUpYZ[Rich::Rich2] = histoSvc()->book(m_histPth,id,title,
                                                nBins, 10720, 10800, nBins, -70, 30 );

  return StatusCode::SUCCESS;
}
