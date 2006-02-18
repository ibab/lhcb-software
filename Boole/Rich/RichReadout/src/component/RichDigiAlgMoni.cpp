// $Id: RichDigiAlgMoni.cpp,v 1.6 2006-02-18 16:04:06 jonrob Exp $

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
  : RichHistoAlgBase ( name, pSvcLocator ),
    m_smartIDTool    ( 0 ),
    m_mcTool         ( 0 ),
    m_ID             ( 0 ),
    m_maxID          ( 100 )
{
  // Declare job options
  declareProperty( "InputMCRichDigits", m_mcdigitTES = MCRichDigitLocation::Default   );
  declareProperty( "InputMCRichDeposits", m_mcdepTES = MCRichDepositLocation::Default );
  declareProperty( "InputMCRichHits", m_mchitTES     = MCRichHitLocation::Default     );
  declareProperty( "MaxHPDQuartzHistos", m_maxID );
}

// Destructor
RichDigiAlgMoni::~RichDigiAlgMoni() {};

// Initialisation
StatusCode RichDigiAlgMoni::initialize()
{
  // Initialize base class
  const StatusCode sc = RichHistoAlgBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  // get tools
  acquireTool( "RichSmartIDTool", m_smartIDTool, 0, true );
  acquireTool( "RichMCTruthTool", m_mcTool,      0, true );

  // Retrieve particle property service
  IParticlePropertySvc * ppSvc = svc<IParticlePropertySvc>( "ParticlePropertySvc", true );

  // Retrieve particle masses
  m_particleMass[Rich::Unknown]  = 0;
  m_particleMass[Rich::Electron] = ppSvc->find("e+" )->mass()/MeV;
  m_particleMass[Rich::Muon]     = ppSvc->find("mu+")->mass()/MeV;
  m_particleMass[Rich::Pion]     = ppSvc->find("pi+")->mass()/MeV;
  m_particleMass[Rich::Kaon]     = ppSvc->find("K+" )->mass()/MeV;
  m_particleMass[Rich::Proton]   = ppSvc->find("p+" )->mass()/MeV;

  // release particle property service
  release(ppSvc);

  return sc;
}

// Main execution
StatusCode RichDigiAlgMoni::execute()
{
  debug() << "Execute" << endreq;

  // Maps for number of pe's
  PhotMap ckPhotMapHit;
  PhotMap ckPhotMapDig;
  PhotMap ckPhotMapDep;

  // Locate MCRichDigits
  // ================================================================================
  MCRichDigits * mcRichDigits = get<MCRichDigits>( m_mcdigitTES );
  debug() << "Successfully located " << mcRichDigits->size()
          << " MCRichDigits at " << m_mcdigitTES << endreq;

  int nChargedTracks[Rich::NRiches];
  nChargedTracks[Rich::Rich1] = 0;
  nChargedTracks[Rich::Rich2] = 0;

  // PD occupancy
  PDMulti pdMult;

  // Initialise mult counts
  std::vector<unsigned int> digMult(Rich::NRiches,0);
  std::vector<unsigned int> hpdCKMult(Rich::NRiches,0);

  // Histogramming
  PD_GLOBAL_POSITIONS;
  const double maxMult[] = { 5000, 2000 };
  const RichHistoID hid;

  // Loop over all MCRichDigits
  for ( MCRichDigits::const_iterator iMcDigit = mcRichDigits->begin();
        iMcDigit != mcRichDigits->end(); ++iMcDigit )
  {

    // Smart ID
    const RichSmartID id = (*iMcDigit)->key();
    // position for this ID
    const Gaudi::XYZPoint point = m_smartIDTool->globalPosition( id );
    // RICH
    const Rich::DetectorType rich = id.rich();

    // increment digit count
    ++digMult[id.rich()];

    // increment PD multiplicity count
    ++pdMult[id.hpdID()];
    if ( (*iMcDigit)->history().hpdQuartzCK() ) { ++hpdCKMult[id.rich()]; }

    // Position plots
    plot1D( point.x(), hid(rich,"digitXglo"), "digits x global", xMinPDGlo[rich], xMaxPDGlo[rich] );
    plot1D( point.x(), hid(rich,"digitYglo"), "digits y global", yMinPDGlo[rich], yMaxPDGlo[rich] );
    plot1D( point.x(), hid(rich,"digitZglo"), "digits z global", zMinPDGlo[rich], zMaxPDGlo[rich] );
    plot2D( point.x(), point.y(), hid(rich,"digitXYglo"), "digits yVx global",
            xMinPDGlo[rich], xMaxPDGlo[rich], yMinPDGlo[rich], yMaxPDGlo[rich] );
    plot2D( point.z(), point.x(), hid(rich,"digitZXglo"), "digits xVz global",
            zMinPDGlo[rich], zMaxPDGlo[rich], xMinPDGlo[rich], xMaxPDGlo[rich] );
    plot2D( point.z(), point.y(), hid(rich,"digitZYglo"), "digits yVz global",
            zMinPDGlo[rich], zMaxPDGlo[rich], yMinPDGlo[rich], yMaxPDGlo[rich] );

    // loop over all hits associated to the digit
    const SmartRefVector<MCRichHit>& mcHits = (*iMcDigit)->hits();
    if ( mcHits.empty() )
    {
      warning() << "MCRichDigit " << (int)(*iMcDigit)->key()
                << " has no MCRichHits..." << endreq;
    }

    // hit mult per digit
    plot1D( mcHits.size(), hid(rich,"hitsPerDig"), "Hit mult. per Digit", -0.5,10.5,11 );

    bool thisDigCounted = false;
    for ( SmartRefVector<MCRichHit>::const_iterator iHit = mcHits.begin();
          iHit != mcHits.end(); ++iHit )
    {

      // Compare digit/hit err for signal hits only
      if ( !(*iHit)->isBackground() )
      {
        plot1D( point.x() - (*iHit)->entry().x(), hid(rich,"digErrX"), "digit-hit diff X", -10, 10 );
        plot1D( point.y() - (*iHit)->entry().y(), hid(rich,"digErrY"), "digit-hit diff Y", -10, 10 );
        plot1D( point.z() - (*iHit)->entry().z(), hid(rich,"digErrZ"), "digit-hit diff Z", -10, 10 );
        plot1D( sqrt((point-(*iHit)->entry()).Mag2()), hid(rich,"digErrR"), "digit-hit diff R", -10, 10 );
      }

      // Count beta=1 PEs
      countNPE( ckPhotMapDig, *iHit );

      // count digits from charged tracks
      if ( !thisDigCounted && (*iHit)->chargedTrack() )
      {
        thisDigCounted = true;
        ++nChargedTracks[rich];
      }

    } // end hits loop

  } // MCRichDigits Loop

  // Fill multiplicity plots

  plot1D( digMult[Rich::Rich1], hid(Rich::Rich1,"digitMult"),
          "Overall Digit RICH1 Occupancy", 0, maxMult[Rich::Rich1] );
  plot1D( digMult[Rich::Rich2], hid(Rich::Rich2,"digitMult"),
          "Overall Digit RICH2 Occupancy", 0, maxMult[Rich::Rich2] );

  plot1D( hpdCKMult[Rich::Rich1], hid(Rich::Rich1,"hpdQCKDigMult"),
          "HPD Quartz CK Digit RICH1 Occupancy", 0, maxMult[Rich::Rich1] );
  plot1D( hpdCKMult[Rich::Rich2], hid(Rich::Rich2,"hpdQCKDigMult"),
          "HPD Quartz CK Digit RICH2 Occupancy", 0, maxMult[Rich::Rich2] );

  // Fill PD occupancy plots
  for ( PDMulti::const_iterator iOcc = pdMult.begin();
        iOcc != pdMult.end(); ++iOcc )
  {
    plot1D( (*iOcc).second, hid(((*iOcc).first).rich(),"digPerPD"), "Digits per PD",-0.5,100.5,101 );
  }

  // Locate MCRichDeposits
  // ================================================================================
  SmartDataPtr<MCRichDeposits> deps( eventSvc(), m_mcdepTES );
  if ( !deps )
  {
    debug() << "Cannot locate MCRichDeposits at " << m_mcdepTES << endreq;
  }
  else
  {
    debug() << "Successfully located " << deps->size()
            << " MCRichDeposits at " << m_mcdepTES << endreq;

    std::vector<unsigned int> nChargedTracks(Rich::NRiches,0);

    // map of backgrounds in each HPD
    PartMap pmap;

    // Loop over deposits
    for ( MCRichDeposits::const_iterator iDep = deps->begin();
          iDep != deps->end(); ++iDep )
    {

      // Which RICH ?
      const Rich::DetectorType rich = (*iDep)->parentHit()->rich();

      // TOF for this deposit
      //m_tofDep[rich]->fill( (*iDep)->time() );

      // Deposit energy
      //m_depEnDep[rich]->fill( (*iDep)->energy() );

      // Count beta=1 PEs
      countNPE( ckPhotMapDep, (*iDep)->parentHit() );

      // count hits from charged tracks
      if ( (*iDep)->parentHit()->chargedTrack() ) ++nChargedTracks[rich];

      // study quartz window backgrounds
      if ( (*iDep)->history().hpdQuartzCK() )
      {

        // key for this hits HPD and MCParticle pair
        PartKey key( (*iDep)->smartID().hpdID(), (*iDep)->parentHit()->mcParticle() );
        // add poitner to deposit to vector for this key
        pmap[key].push_back( *iDep );

        if ( msgLevel(MSG::VERBOSE) )
        {
          const Gaudi::XYZPoint locpoint =
            m_smartIDTool->globalToPDPanel( (*iDep)->parentHit()->entry() );
          verbose() << (*iDep)->smartID() << " glo=" << (*iDep)->parentHit()->entry()
                    << " loc=" << locpoint << " mcp=" << (*iDep)->parentHit()->mcParticle() << endreq;
        }
      }

    } // MCRichDeposits loop

    // fill background plots
    fillHPDPlots( pmap, "QuartzBkg" );

  } // MCRichDeposits exist

  // Locate MCRichHits
  // ================================================================================
  SmartDataPtr<MCRichHits> hits( eventSvc(), m_mchitTES );
  if ( !hits ) {
    debug() << "Cannot locate MCRichHits at " << m_mchitTES << endreq;
  } else {

    // Hit mult counters
    std::vector<unsigned int> hitMult(Rich::NRiches,0);
    std::vector<unsigned int> hpdQCKMult(Rich::NRiches,0);

    // Loop over hits
    for ( MCRichHits::const_iterator iHit = hits->begin();
          iHit != hits->end(); ++iHit )
    {

      // Which RICH ?
      const Rich::DetectorType rich = (*iHit)->rich();

      if ( trueCKHit(*iHit) )
      {
        plot1D( (*iHit)->timeOfFlight(), hid(rich,"tofHitSignal"), "Signal MCRichHit TOF", -100, 100 );
        plot1D( (*iHit)->energy(), hid(rich,"enHitSignal"), "Signal MCRichHit Energy", 0, 100 );
      }
      else
      {
        plot1D( (*iHit)->timeOfFlight(), hid(rich,"tofHitBkgrd"), "Background MCRichHit TOF", -100, 100 );
        plot1D( (*iHit)->energy(), hid(rich,"enHitBkgrd"), "Background MCRichHit Energy", 0, 100 );
      }

      // increment hit count
      ++hitMult[rich];
      if ( (*iHit)->hpdQuartzCK() ) { ++hpdQCKMult[rich]; }

      // Increment PES count for high beta tracks
      countNPE( ckPhotMapHit, *iHit );

      // Plot hit positions
      const Gaudi::XYZPoint & point = (*iHit)->entry();
      plot1D( point.x(), hid(rich,"hitXGlo"), "mchits x global", xMinPDGlo[rich], xMaxPDGlo[rich] );
      plot1D( point.y(), hid(rich,"hitYGlo"), "mchits y global", yMinPDGlo[rich], yMaxPDGlo[rich] );
      plot1D( point.z(), hid(rich,"hitZGlo"), "mchits z global", zMinPDGlo[rich], zMaxPDGlo[rich] );
      plot2D( point.x(), point.y(), hid(rich,"hitXYglo"), "mchits yVx global",
              xMinPDGlo[rich], xMaxPDGlo[rich], yMinPDGlo[rich], yMaxPDGlo[rich] );
      plot2D( point.z(), point.x(), hid(rich,"hitZXglo"), "mchits xVz global",
              zMinPDGlo[rich], zMaxPDGlo[rich], xMinPDGlo[rich], xMaxPDGlo[rich] );
      plot2D( point.z(), point.y(), hid(rich,"hitZYglo"), "mchits yVz global",
              zMinPDGlo[rich], zMaxPDGlo[rich], yMinPDGlo[rich], yMaxPDGlo[rich] );

    }

    // Fill multiplicity plots

    plot1D( hitMult[Rich::Rich1], hid(Rich::Rich1,"hitMult"),
            "Overall MCRichHit RICH1 Occupancy", 0, maxMult[Rich::Rich1] );
    plot1D( hitMult[Rich::Rich2], hid(Rich::Rich2,"hitMult"),
            "Overall MCRichHit RICH2 Occupancy", 0, maxMult[Rich::Rich2] );

    plot1D( hpdQCKMult[Rich::Rich1], hid(Rich::Rich1,"hpdQCKHitMult"),
            "HPD Quartz CK MCRichHit RICH1 Occupancy", 0, maxMult[Rich::Rich1] );
    plot1D( hpdQCKMult[Rich::Rich2], hid(Rich::Rich2,"hpdQCKHitMult"),
            "HPD Quartz CK MCRichHit RICH2 Occupancy", 0, maxMult[Rich::Rich2] );

  } // MCRichHits exist

  // Loop over counted PES for MCRichHits
  {for ( PhotMap::iterator iPhot = ckPhotMapHit.begin();
         iPhot != ckPhotMapHit.end(); ++iPhot )
    {
      plot1D( (*iPhot).second, hid( ((*iPhot).first).second, "npesHit"),
              "# MCRichHit p.e.s : beta=1", -0.5, 100.5, 101 );
    }}

  // Loop over counted PES for MCRichDeposits
  {for ( PhotMap::iterator iPhot = ckPhotMapDep.begin();
         iPhot != ckPhotMapDep.end(); ++iPhot )
    {
      plot1D( (*iPhot).second, hid( ((*iPhot).first).second, "npesDep"),
              "# MCRichDeposit p.e.s : beta=1", -0.5, 100.5, 101 );
    }}

  // Loop over counted PES for MCRichDigits
  {for ( PhotMap::iterator iPhot = ckPhotMapDig.begin();
         iPhot != ckPhotMapDig.end(); ++iPhot ) {

      const double digCount = (*iPhot).second;
      plot1D( digCount, hid(((*iPhot).first).second,"npesDig"),
              "# MCRichDigit p.e.s : beta=1", -0.5, 100.5, 101 );

      // locate the entry for the MCRichDeps
      const double depCount = ckPhotMapDep[ (*iPhot).first ];

      const double frac = ( depCount != 0 ? digCount/depCount : 0 );
      plot1D( frac, hid(((*iPhot).first).second,"npesRetained"),
              "Retained p.e.s : beta=1", 0, 1 );

    }}

  return StatusCode::SUCCESS;
}

void RichDigiAlgMoni::fillHPDPlots( const PartMap & pmap, const std::string & plotsTitle )
{
  // count for each RICH
  std::vector<unsigned int> nHPDs(Rich::NRiches,0);

  // plots for each HPD
  for ( PartMap::const_iterator iP = pmap.begin(); iP != pmap.end(); ++iP )
  {
    // SmartID for this HPD
    const RichSmartID hpdID = (*iP).first.first;
    // increment count for RICH
    ++nHPDs[hpdID.rich()];
    // plot number of hits
    plot1D( (*iP).second.size(), plotsTitle+"/nHitsPerHPD", 
            "# HPD Quartz window hits per HPD", 0.5, 200.5, 100 );
    if ( m_ID < m_maxID )
    {
      // Centre point of HPD in local coords
      const Gaudi::XYZPoint hpdGlo = m_smartIDTool->hpdPosition(hpdID);
      const Gaudi::XYZPoint hpdLoc = m_smartIDTool->globalToPDPanel(hpdGlo);
      // create histo title
      std::ostringstream HPD;
      HPD << hpdID << " " << hpdGlo;
      // histo ID
      std::string hid = plotsTitle+"/"+boost::lexical_cast<std::string>( m_ID++ );
      // loop over deposits
      for ( std::vector<const LHCb::MCRichDeposit*>::const_iterator iD = (*iP).second.begin();
            iD != (*iP).second.end(); ++iD )
      {
        // reference to hit point on silicon in local coords
        const Gaudi::XYZVector hitP
          = m_smartIDTool->globalToPDPanel((*iD)->parentHit()->entry()) - hpdLoc;
        // fill plot
        plot2D( hitP.X(), hitP.Y(), hid, HPD.str(), -8*mm, 8*mm, -8*mm, 8*mm, 32, 32 );
      }
    }
  }

  // number of affected HPD plots
  plot1D( nHPDs[Rich::Rich1], plotsTitle+"/nHPDsRich1",
          "# RICH1 HPDs per event with HPD Quartz Window CK", -0.5, 20.5, 21 );
  plot1D( nHPDs[Rich::Rich2], plotsTitle+"/nHPDsRich2",
          "# RICH2 HPDs per event with HPD Quartz Window CK", -0.5, 20.5, 21 );
}

void RichDigiAlgMoni::countNPE( PhotMap & photMap,
                                const MCRichHit * hit )
{

  // MCParticle momentum
  const double tkPtot = momentum( hit->mcParticle() );

  // Parent particle hypothesis
  const Rich::ParticleIDType mcid = m_mcTool->mcParticleType( hit->mcParticle() );

  // Which radiator
  const Rich::RadiatorType rad = (Rich::RadiatorType)( hit->radiator() );

  // Increment PES count for high beta tracks
  if ( tkPtot > 1*GeV &&
       mcid != Rich::Unknown &&
       mcid != Rich::Electron &&
       mcBeta( hit->mcParticle() ) > 0.99 &&
       !hit->scatteredPhoton() &&
       !hit->chargedTrack()
       && ( rad == Rich::Aerogel ||
            rad == Rich::Rich1Gas ||
            rad == Rich::Rich2Gas ) ) {
    PhotPair pairC( hit->mcParticle(), rad );
    ++photMap[ pairC ];
  }

}

//  Finalize
StatusCode RichDigiAlgMoni::finalize()
{
  // finalize base class
  return RichHistoAlgBase::finalize();
}

