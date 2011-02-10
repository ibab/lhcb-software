// $Id: $
// Include files

// local
#include "RichAlignmentEventSelection.h"

// From Gaudi
#include "GaudiKernel/SystemOfUnits.h"

// namespaces
using namespace Rich::Rec;

//-----------------------------------------------------------------------------

DECLARE_ALGORITHM_FACTORY( EventSelection )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
EventSelection::EventSelection( const std::string& name,
                                ISvcLocator* pSvcLocator)
  : RichRecTupleAlgBase ( name , pSvcLocator ),
    m_rich              ( Rich::InvalidDetector ),
    m_pType             ( Rich::Unknown ),
    m_pTypes            ( 7, 0 ),
    m_trSelector        ( 0 ),
    m_ckAngle           ( 0 )

{
  // Maximum number of tracks
  declareProperty( "MaxRichRecTracks",    m_maxUsedTracks = 200 );
  declareProperty( "DeltaThetaRange",     m_deltaThetaRange = 0.004 );
  declareProperty( "ParticleType",        m_particleType = 2 ); // pion
  declareProperty( "RichDetector",        m_richTemp = 1 ); // default is Rich2
  // initialise
  {
    for (unsigned int pairNum=0; pairNum<=nRich2Comb-1; ++pairNum)
    {
      m_totalPhotonsR2[pairNum]=0.0;
      m_combiCountR2[pairNum]=0.0;
    }
  }
  {
    for (unsigned int pairNum=0; pairNum<=nRich1Comb-1; ++pairNum)
    {
      m_totalPhotonsR1[pairNum]=0.0;
      m_combiCountR1[pairNum]=0.0;
    }
  }
}

//=============================================================================
// Destructor
//=============================================================================
EventSelection::~EventSelection() {}

//=============================================================================
// Initialisation. Check parameters
//=============================================================================
StatusCode EventSelection::initialize()
{
  // Sets up various tools and services
  const StatusCode sc = Rich::Rec::TupleAlgBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  acquireTool( "RichCherenkovAngle",   m_ckAngle  );
  // get track selector
  acquireTool( "TrackSelector", m_trSelector, this );

  m_pType = static_cast<Rich::ParticleIDType>(m_particleType);
  debug() << "Fixed particle type:" << m_pType << endmsg;
  m_rich = static_cast<Rich::DetectorType>(m_richTemp);
  debug() << "Detector:" << m_rich << endmsg;

  debug() << "Finished Initialization" << endmsg;
  return sc;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode EventSelection::execute() 
{

  debug() << "Execute" << endreq;

  // set photon counters to zero
  for (unsigned int pair=0; pair<=nRich2Comb-1; ++pair)
  {
    m_combiCountR2[pair]=0.0;
  }

  // Check Status
  if ( !richStatus()->eventOK() ) return StatusCode::SUCCESS;

  // If any containers are empty, form them
  if ( richTracks()->empty() ) 
  {
    if ( !trackCreator()->newTracks() ) return StatusCode::FAILURE;
  }

  if ( (int)richTracks()->size() > m_maxUsedTracks )
  {
    return StatusCode::SUCCESS;
  }

  if ( richPixels()->empty() ) 
  {
    if ( !pixelCreator()->newPixels() ) return StatusCode::FAILURE;
  }

  if ( richPhotons()->empty() )
  {
    photonCreator()->reconstructPhotons();
  }

  // Iterate over segments
  for ( LHCb::RichRecSegments::const_iterator iSeg = richSegments()->begin();
        iSeg != richSegments()->end(); ++iSeg ) 
  {
    LHCb::RichRecSegment* segment = *iSeg;

    const Rich::DetectorType rich = segment->trackSegment().rich();

    // Radiator info
    const Rich::RadiatorType rad = segment->trackSegment().radiator();

    if (rad == Rich::Aerogel || rich != m_rich) continue;

    // track selection
    if ( !m_trSelector->trackSelected(segment->richRecTrack()) ) continue;

    double thetaExpected(0.0);

    thetaExpected =  m_ckAngle->avgCherenkovTheta( segment, m_pType);

    if ( msgLevel(MSG::DEBUG) )
      debug() << " Found " << segment->richRecPhotons().size() << " in this segment" << endreq;

    for ( LHCb::RichRecSegment::Photons::const_iterator iPhot = segment->richRecPhotons().begin();
          iPhot != segment->richRecPhotons().end(); ++iPhot )
    {
      LHCb::RichRecPhoton* photon = *iPhot;

      // get the geometrical photon
      const LHCb::RichGeomPhoton & gPhoton = photon->geomPhoton();

      // Cherenkov angles
      const double thetaRec = gPhoton.CherenkovTheta();
      if ( !gPhoton.primaryMirror() || !gPhoton.secondaryMirror() )
      {
        Error( "Mirror information not set in photon !" );
        continue;
      }
      const MirrorNumber sphMirNum  = gPhoton.primaryMirror()->mirrorNumber();
      const MirrorNumber flatMirNum = gPhoton.secondaryMirror()->mirrorNumber();

      double delTheta = thetaRec - thetaExpected;
      if ( fabs(delTheta) > m_deltaThetaRange ) continue;

      const bool unAmbiguousPhoton = photon->geomPhoton().unambiguousPhoton();

      if ( !unAmbiguousPhoton )
      {
        continue;
      }

      std::string thisCombiNr( "" ); // only the 4-digit combination number (string)

      if ( sphMirNum > 9 )
        thisCombiNr +=       boost::lexical_cast<std::string>( sphMirNum );
      else
        thisCombiNr += "0" + boost::lexical_cast<std::string>( sphMirNum );
      if ( flatMirNum > 9 )
        thisCombiNr +=       boost::lexical_cast<std::string>( flatMirNum );
      else
        thisCombiNr += "0" + boost::lexical_cast<std::string>( flatMirNum );

      // count the photons in each of the mirror combinations
      if ( rich == Rich::Rich2 )
      {
        if(thisCombiNr=="0000") m_combiCountR2[0]++;
        if(thisCombiNr=="0001") m_combiCountR2[1]++;
        if(thisCombiNr=="0100") m_combiCountR2[2]++;
        if(thisCombiNr=="0101") m_combiCountR2[3]++;
        if(thisCombiNr=="0102") m_combiCountR2[4]++;
        if(thisCombiNr=="0201") m_combiCountR2[5]++;
        if(thisCombiNr=="0202") m_combiCountR2[6]++;
        if(thisCombiNr=="0203") m_combiCountR2[7]++;
        if(thisCombiNr=="0302") m_combiCountR2[8]++;
        if(thisCombiNr=="0303") m_combiCountR2[9]++;
        if(thisCombiNr=="0400") m_combiCountR2[10]++;
        if(thisCombiNr=="0401") m_combiCountR2[11]++;
        if(thisCombiNr=="0404") m_combiCountR2[12]++;
        if(thisCombiNr=="0405") m_combiCountR2[13]++;
        if(thisCombiNr=="0501") m_combiCountR2[14]++;
        if(thisCombiNr=="0502") m_combiCountR2[15]++;
        if(thisCombiNr=="0505") m_combiCountR2[16]++;
        if(thisCombiNr=="0506") m_combiCountR2[17]++;
        if(thisCombiNr=="0602") m_combiCountR2[18]++;
        if(thisCombiNr=="0603") m_combiCountR2[19]++;
        if(thisCombiNr=="0606") m_combiCountR2[20]++;
        if(thisCombiNr=="0607") m_combiCountR2[21]++;
        if(thisCombiNr=="0707") m_combiCountR2[22]++;
        if(thisCombiNr=="0804") m_combiCountR2[23]++;
        if(thisCombiNr=="0805") m_combiCountR2[24]++;
        if(thisCombiNr=="0808") m_combiCountR2[25]++;
        if(thisCombiNr=="0904") m_combiCountR2[26]++;
        if(thisCombiNr=="0905") m_combiCountR2[27]++;
        if(thisCombiNr=="0906") m_combiCountR2[28]++;
        if(thisCombiNr=="0909") m_combiCountR2[29]++;
        if(thisCombiNr=="0910") m_combiCountR2[30]++;
        if(thisCombiNr=="1005") m_combiCountR2[31]++;
        if(thisCombiNr=="1006") m_combiCountR2[32]++;
        if(thisCombiNr=="1007") m_combiCountR2[33]++;
        if(thisCombiNr=="1010") m_combiCountR2[34]++;
        if(thisCombiNr=="1011") m_combiCountR2[35]++;
        if(thisCombiNr=="1106") m_combiCountR2[36]++;
        if(thisCombiNr=="1107") m_combiCountR2[37]++;
        if(thisCombiNr=="1111") m_combiCountR2[38]++;
        if(thisCombiNr=="1208") m_combiCountR2[39]++;
        if(thisCombiNr=="1209") m_combiCountR2[40]++;
        if(thisCombiNr=="1309") m_combiCountR2[41]++;
        if(thisCombiNr=="1310") m_combiCountR2[42]++;
        if(thisCombiNr=="1410") m_combiCountR2[43]++;
        if(thisCombiNr=="1411") m_combiCountR2[44]++;
        if(thisCombiNr=="1511") m_combiCountR2[45]++;
        if(thisCombiNr=="1608") m_combiCountR2[46]++;
        if(thisCombiNr=="1612") m_combiCountR2[47]++;
        if(thisCombiNr=="1613") m_combiCountR2[48]++;
        if(thisCombiNr=="1709") m_combiCountR2[49]++;
        if(thisCombiNr=="1710") m_combiCountR2[50]++;
        if(thisCombiNr=="1712") m_combiCountR2[51]++;
        if(thisCombiNr=="1713") m_combiCountR2[52]++;
        if(thisCombiNr=="1714") m_combiCountR2[53]++;
        if(thisCombiNr=="1810") m_combiCountR2[54]++;
        if(thisCombiNr=="1811") m_combiCountR2[55]++;
        if(thisCombiNr=="1813") m_combiCountR2[56]++;
        if(thisCombiNr=="1814") m_combiCountR2[57]++;
        if(thisCombiNr=="1815") m_combiCountR2[58]++;
        if(thisCombiNr=="1911") m_combiCountR2[59]++;
        if(thisCombiNr=="1914") m_combiCountR2[60]++;
        if(thisCombiNr=="1915") m_combiCountR2[61]++;
        if(thisCombiNr=="2012") m_combiCountR2[62]++;
        if(thisCombiNr=="2013") m_combiCountR2[63]++;
        if(thisCombiNr=="2016") m_combiCountR2[64]++;
        if(thisCombiNr=="2017") m_combiCountR2[65]++;
        if(thisCombiNr=="2113") m_combiCountR2[66]++;
        if(thisCombiNr=="2114") m_combiCountR2[67]++;
        if(thisCombiNr=="2117") m_combiCountR2[68]++;
        if(thisCombiNr=="2118") m_combiCountR2[69]++;
        if(thisCombiNr=="2214") m_combiCountR2[70]++;
        if(thisCombiNr=="2215") m_combiCountR2[71]++;
        if(thisCombiNr=="2218") m_combiCountR2[72]++;
        if(thisCombiNr=="2219") m_combiCountR2[73]++;
        if(thisCombiNr=="2315") m_combiCountR2[74]++;
        if(thisCombiNr=="2416") m_combiCountR2[75]++;
        if(thisCombiNr=="2417") m_combiCountR2[76]++;
        if(thisCombiNr=="2516") m_combiCountR2[77]++;
        if(thisCombiNr=="2517") m_combiCountR2[78]++;
        if(thisCombiNr=="2518") m_combiCountR2[79]++;
        if(thisCombiNr=="2617") m_combiCountR2[80]++;
        if(thisCombiNr=="2618") m_combiCountR2[81]++;
        if(thisCombiNr=="2619") m_combiCountR2[82]++;
        if(thisCombiNr=="2718") m_combiCountR2[83]++;
        if(thisCombiNr=="2719") m_combiCountR2[84]++;
        if(thisCombiNr=="2820") m_combiCountR2[85]++;
        if(thisCombiNr=="2821") m_combiCountR2[86]++;
        if(thisCombiNr=="2920") m_combiCountR2[87]++;
        if(thisCombiNr=="2921") m_combiCountR2[88]++;
        if(thisCombiNr=="2922") m_combiCountR2[89]++;
        if(thisCombiNr=="3021") m_combiCountR2[90]++;
        if(thisCombiNr=="3022") m_combiCountR2[91]++;
        if(thisCombiNr=="3023") m_combiCountR2[92]++;
        if(thisCombiNr=="3122") m_combiCountR2[93]++;
        if(thisCombiNr=="3123") m_combiCountR2[94]++;
        if(thisCombiNr=="3224") m_combiCountR2[95]++;
        if(thisCombiNr=="3320") m_combiCountR2[96]++;
        if(thisCombiNr=="3321") m_combiCountR2[97]++;
        if(thisCombiNr=="3324") m_combiCountR2[98]++;
        if(thisCombiNr=="3325") m_combiCountR2[99]++;
        if(thisCombiNr=="3421") m_combiCountR2[100]++;
        if(thisCombiNr=="3422") m_combiCountR2[101]++;
        if(thisCombiNr=="3425") m_combiCountR2[102]++;
        if(thisCombiNr=="3426") m_combiCountR2[103]++;
        if(thisCombiNr=="3522") m_combiCountR2[104]++;
        if(thisCombiNr=="3523") m_combiCountR2[105]++;
        if(thisCombiNr=="3526") m_combiCountR2[106]++;
        if(thisCombiNr=="3527") m_combiCountR2[107]++;
        if(thisCombiNr=="3624") m_combiCountR2[108]++;
        if(thisCombiNr=="3625") m_combiCountR2[109]++;
        if(thisCombiNr=="3628") m_combiCountR2[110]++;
        if(thisCombiNr=="3724") m_combiCountR2[111]++;
        if(thisCombiNr=="3725") m_combiCountR2[112]++;
        if(thisCombiNr=="3726") m_combiCountR2[113]++;
        if(thisCombiNr=="3728") m_combiCountR2[114]++;
        if(thisCombiNr=="3729") m_combiCountR2[115]++;
        if(thisCombiNr=="3825") m_combiCountR2[116]++;
        if(thisCombiNr=="3826") m_combiCountR2[117]++;
        if(thisCombiNr=="3827") m_combiCountR2[118]++;
        if(thisCombiNr=="3829") m_combiCountR2[119]++;
        if(thisCombiNr=="3830") m_combiCountR2[120]++;
        if(thisCombiNr=="3926") m_combiCountR2[121]++;
        if(thisCombiNr=="3927") m_combiCountR2[122]++;
        if(thisCombiNr=="3931") m_combiCountR2[123]++;
        if(thisCombiNr=="4028") m_combiCountR2[124]++;
        if(thisCombiNr=="4128") m_combiCountR2[125]++;
        if(thisCombiNr=="4129") m_combiCountR2[126]++;
        if(thisCombiNr=="4229") m_combiCountR2[127]++;
        if(thisCombiNr=="4230") m_combiCountR2[128]++;
        if(thisCombiNr=="4330") m_combiCountR2[129]++;
        if(thisCombiNr=="4331") m_combiCountR2[130]++;
        if(thisCombiNr=="4428") m_combiCountR2[131]++;
        if(thisCombiNr=="4432") m_combiCountR2[132]++;
        if(thisCombiNr=="4433") m_combiCountR2[133]++;
        if(thisCombiNr=="4528") m_combiCountR2[134]++;
        if(thisCombiNr=="4529") m_combiCountR2[135]++;
        if(thisCombiNr=="4532") m_combiCountR2[136]++;
        if(thisCombiNr=="4533") m_combiCountR2[137]++;
        if(thisCombiNr=="4534") m_combiCountR2[138]++;
        if(thisCombiNr=="4629") m_combiCountR2[139]++;
        if(thisCombiNr=="4630") m_combiCountR2[140]++;
        if(thisCombiNr=="4633") m_combiCountR2[141]++;
        if(thisCombiNr=="4634") m_combiCountR2[142]++;
        if(thisCombiNr=="4635") m_combiCountR2[143]++;
        if(thisCombiNr=="4731") m_combiCountR2[144]++;
        if(thisCombiNr=="4734") m_combiCountR2[145]++;
        if(thisCombiNr=="4735") m_combiCountR2[146]++;
        if(thisCombiNr=="4832") m_combiCountR2[147]++;
        if(thisCombiNr=="4932") m_combiCountR2[148]++;
        if(thisCombiNr=="4933") m_combiCountR2[149]++;
        if(thisCombiNr=="4936") m_combiCountR2[150]++;
        if(thisCombiNr=="4937") m_combiCountR2[151]++;
        if(thisCombiNr=="5033") m_combiCountR2[152]++;
        if(thisCombiNr=="5034") m_combiCountR2[153]++;
        if(thisCombiNr=="5037") m_combiCountR2[154]++;
        if(thisCombiNr=="5038") m_combiCountR2[155]++;
        if(thisCombiNr=="5134") m_combiCountR2[156]++;
        if(thisCombiNr=="5135") m_combiCountR2[157]++;
        if(thisCombiNr=="5138") m_combiCountR2[158]++;
        if(thisCombiNr=="5139") m_combiCountR2[159]++;
        if(thisCombiNr=="5236") m_combiCountR2[160]++;
        if(thisCombiNr=="5237") m_combiCountR2[161]++;
        if(thisCombiNr=="5336") m_combiCountR2[162]++;
        if(thisCombiNr=="5337") m_combiCountR2[163]++;
        if(thisCombiNr=="5338") m_combiCountR2[164]++;
        if(thisCombiNr=="5437") m_combiCountR2[165]++;
        if(thisCombiNr=="5438") m_combiCountR2[166]++;
        if(thisCombiNr=="5439") m_combiCountR2[167]++;
        if(thisCombiNr=="5538") m_combiCountR2[168]++;
        if(thisCombiNr=="5539") m_combiCountR2[169]++;
      }
    }// end of photons loop
  }// end of loop over segments

  //event selection
  bool ok=false;

  for(unsigned int mir=0; mir<=nRich2Comb-1; ++mir)
  {
    if(m_combiCountR2[mir]>=10.0 && m_totalPhotonsR2[mir]<100.0)
    {
      ok=true;
    }
  }

  if(ok)
  {
    //need to add the photons in this event to the total photon arrays
    //only if the event passed!!!!
    for(unsigned int pairNum=0; pairNum<=nRich2Comb-1; ++pairNum)
    {
      m_totalPhotonsR2[pairNum]+=m_combiCountR2[pairNum];
    }
  }

  setFilterPassed(ok);

  info() << "photons in 1209 = " << m_combiCountR2[40] << endmsg;

  if(ok)
  {
    info() << "event passed" << endmsg;
  }

  else
  {
    info() << "event didn't pass" << endmsg;
  }

  return StatusCode::SUCCESS;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode EventSelection::finalize()
{

  //============================================================================
  //============================================================================
  // Execute base class method
  return Rich::Rec::TupleAlgBase::finalize();
}
