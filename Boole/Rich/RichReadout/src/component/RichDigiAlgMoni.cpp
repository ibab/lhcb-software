// Units
#include "GaudiKernel/SystemOfUnits.h"

// From GaudiKernel
#include "GaudiKernel/SmartDataPtr.h"

// From PartProp
#include "Kernel/IParticlePropertySvc.h"
#include "Kernel/ParticleProperty.h"

// local
#include "RichDigiAlgMoni.h"

//-----------------------------------------------------------------------------
// Implementation file for class : RichDigiAlgMoni
//
// 2003-09-08 : Chris Jones   (Christopher.Rob.Jones@cern.ch)
//-----------------------------------------------------------------------------

using namespace Rich::MC::Digi;

DECLARE_ALGORITHM_FACTORY( AlgMoni )

// Standard constructor, initializes variables
AlgMoni::AlgMoni( const std::string& name,
                  ISvcLocator* pSvcLocator )
  : HistoAlgBase     ( name, pSvcLocator ),
    m_smartIDTool    ( NULL ),
    m_mcTool         ( NULL ),
    m_richSys        ( NULL ),
    m_ID             ( 0    ),
    m_maxID          ( 500  )
{
  // Declare job options
  declareProperty( "InputMCRichDigits", m_mcdigitTES = LHCb::MCRichDigitLocation::Default   );
  declareProperty( "InputMCRichDeposits", m_mcdepTES = LHCb::MCRichDepositLocation::Default );
  declareProperty( "InputMCRichHits", m_mchitTES     = LHCb::MCRichHitLocation::Default     );
  declareProperty( "MaxHPDHistos", m_maxID );
}

// Destructor
AlgMoni::~AlgMoni() {}

// Initialisation
StatusCode AlgMoni::initialize()
{
  // Initialize base class
  const StatusCode sc = HistoAlgBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  // get tools
  acquireTool( "RichSmartIDTool", m_smartIDTool, 0, true );
  acquireTool( "RichMCTruthTool", m_mcTool,      0, true );

  // RichDet
  m_richSys = getDet<DeRichSystem>( DeRichLocations::RichSystem );

  // Retrieve particle property service
  LHCb::IParticlePropertySvc * ppSvc = svc<LHCb::IParticlePropertySvc>( "LHCb::ParticlePropertySvc", true );

  // Retrieve particle masses
  m_particleMass[Rich::Unknown]  = 0;
  m_particleMass[Rich::Electron] = ppSvc->find("e+" )->mass()/Gaudi::Units::MeV;
  m_particleMass[Rich::Muon]     = ppSvc->find("mu+")->mass()/Gaudi::Units::MeV;
  m_particleMass[Rich::Pion]     = ppSvc->find("pi+")->mass()/Gaudi::Units::MeV;
  m_particleMass[Rich::Kaon]     = ppSvc->find("K+" )->mass()/Gaudi::Units::MeV;
  m_particleMass[Rich::Proton]   = ppSvc->find("p+" )->mass()/Gaudi::Units::MeV;

  // release particle property service
  release(ppSvc);

  return sc;
}

// Main execution
StatusCode AlgMoni::execute()
{
    if ( msgLevel(MSG::DEBUG) ) debug() << "Execute" << endmsg;

  // Maps for number of pe's
  PhotMap ckPhotMapHit;
  PhotMap ckPhotMapDig;
  PhotMap ckPhotMapDep;

  // Locate MCRichDigits
  // ================================================================================
  LHCb::MCRichDigits * mcRichDigits = get<LHCb::MCRichDigits>( m_mcdigitTES );
    if ( msgLevel(MSG::DEBUG) )
      debug() << "Successfully located " << mcRichDigits->size()
              << " MCRichDigits at " << m_mcdigitTES << endmsg;

  // PD occupancy
  PDMulti pdMult;

  // Initialise mult counts
  std::vector<unsigned int> digMult(Rich::NRiches,0);
  std::vector<unsigned int> hpdCKMult(Rich::NRiches,0);

  Rich::Map<const LHCb::RichSmartID,bool> hasHPDQuartzCKBkg;

  // Histogramming
  PD_GLOBAL_POSITIONS;
  const double maxMult[] = { 5000, 2000 };
  const Rich::HistoID hid;

  // Loop over all MCRichDigits
  for ( LHCb::MCRichDigits::const_iterator iMcDigit = mcRichDigits->begin();
        iMcDigit != mcRichDigits->end(); ++iMcDigit )
  {

    // Smart ID
    const LHCb::RichSmartID id = (*iMcDigit)->key();

    // position for this ID
    Gaudi::XYZPoint point;
    const StatusCode sc = m_smartIDTool->globalPosition( id, point );
    if ( sc.isFailure() ) { continue; }

    // RICH
    const Rich::DetectorType rich = id.rich();

    // increment digit count
    ++digMult[rich];

    // increment PD multiplicity count
    ++pdMult[id.pdID()];
    if ( (*iMcDigit)->history().hpdQuartzCK() )
    {
      ++hpdCKMult[rich];
      hasHPDQuartzCKBkg[id.pdID()] = true;
    }

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
    const LHCb::MCRichDigitHit::Vector & mcHits = (*iMcDigit)->hits();
    if ( mcHits.empty() )
    {
      warning() << "MCRichDigit " << (int)(*iMcDigit)->key()
                << " has no MCRichHits..." << endmsg;
    }

    // hit mult per digit
    plot1D( mcHits.size(), hid(rich,"hitsPerDig"), "Hit mult. per Digit", -0.5,10.5,11 );

    bool thisDigCounted = false;
    for ( LHCb::MCRichDigitHit::Vector::const_iterator iHit = mcHits.begin();
          iHit != mcHits.end(); ++iHit )
    {
      const LHCb::MCRichHit * hit = (*iHit).mcRichHit();

      // Compare digit/hit err for signal hits only
      if ( !hit->isBackground() )
      {

        plot1D( point.x() - hit->entry().x(), hid(rich,"digErrX"), "digit-hit diff X", -10, 10 );
        plot1D( point.y() - hit->entry().y(), hid(rich,"digErrY"), "digit-hit diff Y", -10, 10 );
        plot1D( point.z() - hit->entry().z(), hid(rich,"digErrZ"), "digit-hit diff Z", -10, 10 );
        plot1D( sqrt((point-hit->entry()).Mag2()), hid(rich,"digErrR"), "digit-hit diff R", -10, 10 );

        // Get MCRichOptical Photon
        const LHCb::MCRichOpticalPhoton * mcPhot = m_mcTool->mcOpticalPhoton(hit);
        if ( mcPhot )
        {
          // Compare position on HPD entrance window
          const Gaudi::XYZPoint & mcPoint = mcPhot->pdIncidencePoint();
          plot1D( point.X()-mcPoint.X(), hid(rich,"pdImpX"),
                  "dX on HPD entry window : CK signal only", -30, 30 );
          plot1D( point.Y()-mcPoint.Y(), hid(rich,"pdImpY"),
                  "dY on HPD entry window : CK signal only", -30, 30 );
          plot1D( point.Z()-mcPoint.Z(), hid(rich,"pdImpZ"),
                  "dZ on HPD entry window : CK signal only", -30, 30 );
          plot1D( sqrt((point-mcPoint).Mag2()), hid(rich,"pdImpR"),
                  "dR on HPD entry window : CK signal only",  0, 10 );
        }

      }

      // Count beta=1 PEs
      countNPE( ckPhotMapDig, hit );

      // count digits from charged tracks
      if ( !thisDigCounted && hit->chargedTrack() )
      {
        thisDigCounted = true;
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

  // Fill HPD occupancy plots
  for ( PDMulti::const_iterator iOcc = pdMult.begin();
        iOcc != pdMult.end(); ++iOcc )
  {
    plot1D( (*iOcc).second, hid(((*iOcc).first).rich(),"digPerPD"), "Digits per HPD (nDigits>0)",
            -0.5, 200.5, 201 );
    std::ostringstream HPD;
    HPD << (*iOcc).first;
    plot1D( (*iOcc).second,
            hid(((*iOcc).first).rich(),"hpdOcc/"+(std::string)m_richSys->hardwareID((*iOcc).first)),
            HPD.str() + " Digits (nDigits>0)",
            -0.5, 200.5, 201 );
    if ( hasHPDQuartzCKBkg[(*iOcc).first] )
    {
      plot1D( (*iOcc).second,
              hid(((*iOcc).first).rich(),"hpdOccWithHPDQCK/"+(std::string)m_richSys->hardwareID((*iOcc).first)),
              HPD.str() + " Digits (nDigits>0 and nHPDQCK>0)",
              -0.5, 200.5, 201 );
    }
  }

  // Locate MCRichDeposits
  // ================================================================================
  SmartDataPtr<LHCb::MCRichDeposits> deps( eventSvc(), m_mcdepTES );
  if ( !deps )
  {
    if ( msgLevel(MSG::DEBUG) )
      debug() << "Cannot locate MCRichDeposits at " << m_mcdepTES << endmsg;
  }
  else
  {
    if ( msgLevel(MSG::DEBUG) )
      debug() << "Successfully located " << deps->size()
              << " MCRichDeposits at " << m_mcdepTES << endmsg;

    std::vector<unsigned int> nChargedTracks(Rich::NRiches,0);

    // map of backgrounds in each HPD
    PartMap pmap1, pmap2, pmap3;

    // Loop over deposits
    for ( LHCb::MCRichDeposits::const_iterator iDep = deps->begin();
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

      // study hpd quartz window backgrounds
      if ( (*iDep)->history().hpdQuartzCK() )
      {
      // key for this hit HPD and MCParticle pair
      const PartKey key( (*iDep)->smartID().pdID(), (*iDep)->parentHit()->mcParticle() );
        // add pointer to deposit to vector for this key
        pmap1[key].push_back( *iDep );
        if ( msgLevel(MSG::VERBOSE) )
        {
          const Gaudi::XYZPoint locpoint =
            m_smartIDTool->globalToPDPanel( (*iDep)->parentHit()->entry() );
          verbose() << (*iDep)->smartID() << " glo=" << (*iDep)->parentHit()->entry()
                    << " loc=" << locpoint << " mcp=" << (*iDep)->parentHit()->mcParticle() << endmsg;
        }
      }

      // study gas quartz window backgrounds
      if ( (*iDep)->history().gasQuartzCK() ) 
      { 
        // key for this hit HPD and MCParticle pair
        const PartKey key( (*iDep)->smartID().pdID(), (*iDep)->parentHit()->mcParticle() );
        pmap2[key].push_back(*iDep); 
      }

      // HPD Reflections
      if ( (*iDep)->history().hpdReflection() ) 
      {
        // key for this hit HPD 
        const PartKey key( (*iDep)->smartID().pdID(), NULL );
        pmap3[key].push_back(*iDep);
      }

    } // MCRichDeposits loop

    // fill background plots
    fillHPDPlots( pmap1, "HPDQuartzBkg", "HPD Quartz Window CK" );
    fillHPDPlots( pmap2, "GasQuartzBkg", "Gas Quartz Window CK" );
    fillHPDPlots( pmap3, "HPDReflectionBkg", "HPD Internal reflections" );

  } // MCRichDeposits exist

  // Locate MCRichHits
  // ================================================================================
  SmartDataPtr<LHCb::MCRichHits> hits( eventSvc(), m_mchitTES );
  if ( !hits ) {
    if ( msgLevel(MSG::DEBUG) )
      debug() << "Cannot locate MCRichHits at " << m_mchitTES << endmsg;
  } else {

    // Hit mult counters
    std::vector<unsigned int> hitMult(Rich::NRiches,0);
    std::vector<unsigned int> hpdQCKMult(Rich::NRiches,0);

    // Loop over hits
    for ( LHCb::MCRichHits::const_iterator iHit = hits->begin();
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

void AlgMoni::fillHPDPlots( const PartMap & pmap,
                            const std::string & plotsDir,
                            const std::string & plotsName )
{
  // histogramming
  PD_LOCAL_POSITIONS_X;
  PD_LOCAL_POSITIONS_Y;
  const Rich::HistoID hid;

  // count for each RICH
  std::vector<unsigned int> nHPDs(Rich::NRiches,0);

  if ( msgLevel(MSG::DEBUG) )
    debug() << "Filling plots for " << plotsName << endmsg;

  // plots for each HPD
  for ( PartMap::const_iterator iP = pmap.begin(); iP != pmap.end(); ++iP )
  {
    // SmartID for this HPD
    const LHCb::RichSmartID hpdID = (*iP).first.first;
    // increment count for RICH
    ++nHPDs[hpdID.rich()];
    // Centre point of HPD in local coords
    Gaudi::XYZPoint hpdGlo;
    const StatusCode sc = m_smartIDTool->pdPosition(hpdID,hpdGlo);
    if ( sc.isFailure() ) continue;
    const Gaudi::XYZPoint hpdLoc = m_smartIDTool->globalToPDPanel(hpdGlo);
    // create histo title
    std::ostringstream HPD;
    HPD << plotsName << " " << hpdID << " " << hpdGlo;
    // histo ID
    std::string Hid = plotsDir+"/"+boost::lexical_cast<std::string>( m_ID++ );
    // loop over deposits
    Rich::Map<LHCb::RichSmartID,bool> uniqueIDs;
    if ( msgLevel(MSG::DEBUG) )
      debug() << "Found " << (*iP).second.size() << " Deposits for " << Hid << " " << HPD.str() << endmsg;
    for ( std::vector<const LHCb::MCRichDeposit*>::const_iterator iD = (*iP).second.begin();
          iD != (*iP).second.end(); ++iD )
    {
      uniqueIDs[(*iD)->smartID()] = true;
      // hit point on silicon in local coords
      const Gaudi::XYZVector hitP
        = m_smartIDTool->globalToPDPanel((*iD)->parentHit()->entry()) - hpdLoc;
      // fill single event HPD plot
      if ( m_ID < m_maxID )
      {
        plot2D( hitP.X(), hitP.Y(), Hid, HPD.str(), 
                -8*Gaudi::Units::mm, 8*Gaudi::Units::mm, -8*Gaudi::Units::mm, 8*Gaudi::Units::mm, 32, 32 );
      }
      // Intergrated plot
      plot2D( hitP.X(), hitP.Y(), plotsDir+"/"+"intBkg", "Integrated "+plotsName, 
              -8*Gaudi::Units::mm, 8*Gaudi::Units::mm, -8*Gaudi::Units::mm, 8*Gaudi::Units::mm, 32, 32 );         
    }
    // plot number of hits
    plot1D( (*iP).second.size(), plotsDir+"/"+hid(hpdID.rich(),"/nHitsPerHPD").id(),
            "# hits per HPD from "+plotsName, 0.5, 200.5, 100 );
    // plot number of digits
    plot1D( uniqueIDs.size(), plotsDir+"/"+hid(hpdID.rich(),"/nDigsPerHPD").id(),
            "# digits per HPD from "+plotsName, 0.5, 200.5, 100 );
    // position of HPD
    plot2D( hpdLoc.X(), hpdLoc.Y(),
            plotsDir+"/"+hid(hpdID.rich(),"hitHPDs").id(), "HPD Hit Map",
            xMinPDLoc[hpdID.rich()],xMaxPDLoc[hpdID.rich()],
            yMinPDLoc[hpdID.rich()],yMaxPDLoc[hpdID.rich()] );
  }

  // number of affected HPD plots
  plot1D( nHPDs[Rich::Rich1], plotsDir+"/"+hid(Rich::Rich1,"/nHPDsPerEv").id(),
          "# RICH1 HPDs per event with "+plotsName, -0.5, 20.5, 21 );
  plot1D( nHPDs[Rich::Rich2], plotsDir+"/"+hid(Rich::Rich2,"/nHPDsPerEv").id(),
          "# RICH2 HPDs per event with "+plotsName, -0.5, 20.5, 21 );
}

void AlgMoni::countNPE( PhotMap & photMap,
                        const LHCb::MCRichHit * hit )
{

  // MCParticle momentum
  const double tkPtot = momentum( hit->mcParticle() );

  // Parent particle hypothesis
  const Rich::ParticleIDType mcid = m_mcTool->mcParticleType( hit->mcParticle() );

  // Which radiator
  const Rich::RadiatorType rad = (Rich::RadiatorType)( hit->radiator() );

  // Increment PES count for high beta tracks
  if ( tkPtot > 1*Gaudi::Units::GeV &&
       mcid != Rich::Unknown &&
       mcid != Rich::Electron &&
       hit->isSignal() &&
       mcBeta( hit->mcParticle() ) > 0.99 ) 
  {
    PhotPair pairC( hit->mcParticle(), rad );
    ++photMap[ pairC ];
  }
  
}
