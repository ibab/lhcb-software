// $Id: $
// Include files

// local
#include "RichAlignmentEventSelection.h"

// From Gaudi
#include "GaudiKernel/SystemOfUnits.h"

// namespaces
using namespace Rich::Rec;

//-----------------------------------------------------------------------------

DECLARE_ALGORITHM_FACTORY( EventSelection );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
EventSelection::EventSelection( const std::string& name,
                                ISvcLocator* pSvcLocator)
  : RichRecTupleAlgBase ( name , pSvcLocator ),
    m_pTypes            ( 7, 0),
    m_trSelector        ( 0 ),
    m_ckAngle           ( 0 )

{
  // Maximum number of tracks
  declareProperty( "MaxRichRecTracks",    m_maxUsedTracks = 200 );
  declareProperty( "DeltaThetaRange",     m_deltaThetaRange = 0.004 );
  declareProperty( "ParticleType",        m_particleType = 2 ); // pion
  declareProperty( "RichDetector",        m_richTemp = 1 ); // default is Rich2

}

//=============================================================================
// Destructor
//=============================================================================
EventSelection::~EventSelection() {};

//=============================================================================
// Initialisation. Check parameters
//=============================================================================
StatusCode EventSelection::initialize()
{
  // Sets up various tools and services
  const StatusCode sc = RichRecTupleAlgBase::initialize();
  if ( sc.isFailure() ) { return sc; }

  acquireTool( "RichCherenkovAngle",   m_ckAngle  );
  // get track selector
  acquireTool( "TrackSelector", m_trSelector, this );

  m_pType = static_cast<Rich::ParticleIDType>(m_particleType);
  debug() << "Fixed particle type:" << m_pType << endmsg;
  m_rich = static_cast<Rich::DetectorType>(m_richTemp);
  debug() << "Detector:" << m_rich << endmsg;

  Rich::RadiatorType rad;
  if ( m_rich == Rich::Rich1 )
    rad = Rich::Rich1Gas;
  else
    rad = Rich::Rich2Gas;

  for(int pairNum=0; pairNum<=169; pairNum++)
  {
    totalPhotonsR2[pairNum]=0.0;
  }

  for(int pairNum=0; pairNum<=23; pairNum++)
  {
    totalPhotonsR1[pairNum]=0.0;
  }

  debug() << "Finished Initialization" << endmsg;
  return sc;
};



//=============================================================================
// Main execution
//=============================================================================
StatusCode EventSelection::execute() {

  debug() << "Execute" << endreq;

  // set photon counters to zero
  for (int pair=0; pair<=169; pair++)
  {
    combiCountR2[pair]=0.0;
  }

  // Check Status
  if ( !richStatus()->eventOK() ) return StatusCode::SUCCESS;

  // If any containers are empty, form them
  if ( richTracks()->empty() ) {
    if ( !trackCreator()->newTracks() ) return StatusCode::FAILURE;
  }

  if ( (int)richTracks()->size() > m_maxUsedTracks ) {
    return StatusCode::SUCCESS;
  }

  if ( richPixels()->empty() ) {
    if ( !pixelCreator()->newPixels() ) return StatusCode::FAILURE;
  }

  if ( richPhotons()->empty() ) {
    photonCreator()->reconstructPhotons();
  }

  // Iterate over segments
  for ( LHCb::RichRecSegments::const_iterator iSeg = richSegments()->begin();
        iSeg != richSegments()->end(); ++iSeg ) {
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
        if(thisCombiNr=="0000") combiCountR2[0]++;
        if(thisCombiNr=="0001") combiCountR2[1]++;
        if(thisCombiNr=="0100") combiCountR2[2]++;
        if(thisCombiNr=="0101") combiCountR2[3]++;
        if(thisCombiNr=="0102") combiCountR2[4]++;
        if(thisCombiNr=="0201") combiCountR2[5]++;
        if(thisCombiNr=="0202") combiCountR2[6]++;
        if(thisCombiNr=="0203") combiCountR2[7]++;
        if(thisCombiNr=="0302") combiCountR2[8]++;
        if(thisCombiNr=="0303") combiCountR2[9]++;
        if(thisCombiNr=="0400") combiCountR2[10]++;
        if(thisCombiNr=="0401") combiCountR2[11]++;
        if(thisCombiNr=="0404") combiCountR2[12]++;
        if(thisCombiNr=="0405") combiCountR2[13]++;
        if(thisCombiNr=="0501") combiCountR2[14]++;
        if(thisCombiNr=="0502") combiCountR2[15]++;
        if(thisCombiNr=="0505") combiCountR2[16]++;
        if(thisCombiNr=="0506") combiCountR2[17]++;
        if(thisCombiNr=="0602") combiCountR2[18]++;
        if(thisCombiNr=="0603") combiCountR2[19]++;
        if(thisCombiNr=="0606") combiCountR2[20]++;
        if(thisCombiNr=="0607") combiCountR2[21]++;
        if(thisCombiNr=="0707") combiCountR2[22]++;
        if(thisCombiNr=="0804") combiCountR2[23]++;
        if(thisCombiNr=="0805") combiCountR2[24]++;
        if(thisCombiNr=="0808") combiCountR2[25]++;
        if(thisCombiNr=="0904") combiCountR2[26]++;
        if(thisCombiNr=="0905") combiCountR2[27]++;
        if(thisCombiNr=="0906") combiCountR2[28]++;
        if(thisCombiNr=="0909") combiCountR2[29]++;
        if(thisCombiNr=="0910") combiCountR2[30]++;
        if(thisCombiNr=="1005") combiCountR2[31]++;
        if(thisCombiNr=="1006") combiCountR2[32]++;
        if(thisCombiNr=="1007") combiCountR2[33]++;
        if(thisCombiNr=="1010") combiCountR2[34]++;
        if(thisCombiNr=="1011") combiCountR2[35]++;
        if(thisCombiNr=="1106") combiCountR2[36]++;
        if(thisCombiNr=="1107") combiCountR2[37]++;
        if(thisCombiNr=="1111") combiCountR2[38]++;
        if(thisCombiNr=="1208") combiCountR2[39]++;
        if(thisCombiNr=="1209") combiCountR2[40]++;
        if(thisCombiNr=="1309") combiCountR2[41]++;
        if(thisCombiNr=="1310") combiCountR2[42]++;
        if(thisCombiNr=="1410") combiCountR2[43]++;
        if(thisCombiNr=="1411") combiCountR2[44]++;
        if(thisCombiNr=="1511") combiCountR2[45]++;
        if(thisCombiNr=="1608") combiCountR2[46]++;
        if(thisCombiNr=="1612") combiCountR2[47]++;
        if(thisCombiNr=="1613") combiCountR2[48]++;
        if(thisCombiNr=="1709") combiCountR2[49]++;
        if(thisCombiNr=="1710") combiCountR2[50]++;
        if(thisCombiNr=="1712") combiCountR2[51]++;
        if(thisCombiNr=="1713") combiCountR2[52]++;
        if(thisCombiNr=="1714") combiCountR2[53]++;
        if(thisCombiNr=="1810") combiCountR2[54]++;
        if(thisCombiNr=="1811") combiCountR2[55]++;
        if(thisCombiNr=="1813") combiCountR2[56]++;
        if(thisCombiNr=="1814") combiCountR2[57]++;
        if(thisCombiNr=="1815") combiCountR2[58]++;
        if(thisCombiNr=="1911") combiCountR2[59]++;
        if(thisCombiNr=="1914") combiCountR2[60]++;
        if(thisCombiNr=="1915") combiCountR2[61]++;
        if(thisCombiNr=="2012") combiCountR2[62]++;
        if(thisCombiNr=="2013") combiCountR2[63]++;
        if(thisCombiNr=="2016") combiCountR2[64]++;
        if(thisCombiNr=="2017") combiCountR2[65]++;
        if(thisCombiNr=="2113") combiCountR2[66]++;
        if(thisCombiNr=="2114") combiCountR2[67]++;
        if(thisCombiNr=="2117") combiCountR2[68]++;
        if(thisCombiNr=="2118") combiCountR2[69]++;
        if(thisCombiNr=="2214") combiCountR2[70]++;
        if(thisCombiNr=="2215") combiCountR2[71]++;
        if(thisCombiNr=="2218") combiCountR2[72]++;
        if(thisCombiNr=="2219") combiCountR2[73]++;
        if(thisCombiNr=="2315") combiCountR2[74]++;
        if(thisCombiNr=="2416") combiCountR2[75]++;
        if(thisCombiNr=="2417") combiCountR2[76]++;
        if(thisCombiNr=="2516") combiCountR2[77]++;
        if(thisCombiNr=="2517") combiCountR2[78]++;
        if(thisCombiNr=="2518") combiCountR2[79]++;
        if(thisCombiNr=="2617") combiCountR2[80]++;
        if(thisCombiNr=="2618") combiCountR2[81]++;
        if(thisCombiNr=="2619") combiCountR2[82]++;
        if(thisCombiNr=="2718") combiCountR2[83]++;
        if(thisCombiNr=="2719") combiCountR2[84]++;
        if(thisCombiNr=="2820") combiCountR2[85]++;
        if(thisCombiNr=="2821") combiCountR2[86]++;
        if(thisCombiNr=="2920") combiCountR2[87]++;
        if(thisCombiNr=="2921") combiCountR2[88]++;
        if(thisCombiNr=="2922") combiCountR2[89]++;
        if(thisCombiNr=="3021") combiCountR2[90]++;
        if(thisCombiNr=="3022") combiCountR2[91]++;
        if(thisCombiNr=="3023") combiCountR2[92]++;
        if(thisCombiNr=="3122") combiCountR2[93]++;
        if(thisCombiNr=="3123") combiCountR2[94]++;
        if(thisCombiNr=="3224") combiCountR2[95]++;
        if(thisCombiNr=="3320") combiCountR2[96]++;
        if(thisCombiNr=="3321") combiCountR2[97]++;
        if(thisCombiNr=="3324") combiCountR2[98]++;
        if(thisCombiNr=="3325") combiCountR2[99]++;
        if(thisCombiNr=="3421") combiCountR2[100]++;
        if(thisCombiNr=="3422") combiCountR2[101]++;
        if(thisCombiNr=="3425") combiCountR2[102]++;
        if(thisCombiNr=="3426") combiCountR2[103]++;
        if(thisCombiNr=="3522") combiCountR2[104]++;
        if(thisCombiNr=="3523") combiCountR2[105]++;
        if(thisCombiNr=="3526") combiCountR2[106]++;
        if(thisCombiNr=="3527") combiCountR2[107]++;
        if(thisCombiNr=="3624") combiCountR2[108]++;
        if(thisCombiNr=="3625") combiCountR2[109]++;
        if(thisCombiNr=="3628") combiCountR2[110]++;
        if(thisCombiNr=="3724") combiCountR2[111]++;
        if(thisCombiNr=="3725") combiCountR2[112]++;
        if(thisCombiNr=="3726") combiCountR2[113]++;
        if(thisCombiNr=="3728") combiCountR2[114]++;
        if(thisCombiNr=="3729") combiCountR2[115]++;
        if(thisCombiNr=="3825") combiCountR2[116]++;
        if(thisCombiNr=="3826") combiCountR2[117]++;
        if(thisCombiNr=="3827") combiCountR2[118]++;
        if(thisCombiNr=="3829") combiCountR2[119]++;
        if(thisCombiNr=="3830") combiCountR2[120]++;
        if(thisCombiNr=="3926") combiCountR2[121]++;
        if(thisCombiNr=="3927") combiCountR2[122]++;
        if(thisCombiNr=="3931") combiCountR2[123]++;
        if(thisCombiNr=="4028") combiCountR2[124]++;
        if(thisCombiNr=="4128") combiCountR2[125]++;
        if(thisCombiNr=="4129") combiCountR2[126]++;
        if(thisCombiNr=="4229") combiCountR2[127]++;
        if(thisCombiNr=="4230") combiCountR2[128]++;
        if(thisCombiNr=="4330") combiCountR2[129]++;
        if(thisCombiNr=="4331") combiCountR2[130]++;
        if(thisCombiNr=="4428") combiCountR2[131]++;
        if(thisCombiNr=="4432") combiCountR2[132]++;
        if(thisCombiNr=="4433") combiCountR2[133]++;
        if(thisCombiNr=="4528") combiCountR2[134]++;
        if(thisCombiNr=="4529") combiCountR2[135]++;
        if(thisCombiNr=="4532") combiCountR2[136]++;
        if(thisCombiNr=="4533") combiCountR2[137]++;
        if(thisCombiNr=="4534") combiCountR2[138]++;
        if(thisCombiNr=="4629") combiCountR2[139]++;
        if(thisCombiNr=="4630") combiCountR2[140]++;
        if(thisCombiNr=="4633") combiCountR2[141]++;
        if(thisCombiNr=="4634") combiCountR2[142]++;
        if(thisCombiNr=="4635") combiCountR2[143]++;
        if(thisCombiNr=="4731") combiCountR2[144]++;
        if(thisCombiNr=="4734") combiCountR2[145]++;
        if(thisCombiNr=="4735") combiCountR2[146]++;
        if(thisCombiNr=="4832") combiCountR2[147]++;
        if(thisCombiNr=="4932") combiCountR2[148]++;
        if(thisCombiNr=="4933") combiCountR2[149]++;
        if(thisCombiNr=="4936") combiCountR2[150]++;
        if(thisCombiNr=="4937") combiCountR2[151]++;
        if(thisCombiNr=="5033") combiCountR2[152]++;
        if(thisCombiNr=="5034") combiCountR2[153]++;
        if(thisCombiNr=="5037") combiCountR2[154]++;
        if(thisCombiNr=="5038") combiCountR2[155]++;
        if(thisCombiNr=="5134") combiCountR2[156]++;
        if(thisCombiNr=="5135") combiCountR2[157]++;
        if(thisCombiNr=="5138") combiCountR2[158]++;
        if(thisCombiNr=="5139") combiCountR2[159]++;
        if(thisCombiNr=="5236") combiCountR2[160]++;
        if(thisCombiNr=="5237") combiCountR2[161]++;
        if(thisCombiNr=="5336") combiCountR2[162]++;
        if(thisCombiNr=="5337") combiCountR2[163]++;
        if(thisCombiNr=="5338") combiCountR2[164]++;
        if(thisCombiNr=="5437") combiCountR2[165]++;
        if(thisCombiNr=="5438") combiCountR2[166]++;
        if(thisCombiNr=="5439") combiCountR2[167]++;
        if(thisCombiNr=="5538") combiCountR2[168]++;
        if(thisCombiNr=="5539") combiCountR2[169]++;
      }
    }// end of photons loop
  }// end of loop over segments

  //event selection
  bool ok=false;

  for(int mir=0; mir<=169; mir++)
  {
    if(combiCountR2[mir]>=10.0 && totalPhotonsR2[mir]<100.0)
    {
      ok=true;
    }
  }

  if(ok)
  {
    //need to add the photons in this event to the total photon arrays
    //only if the event passed!!!!
    for(int pairNum=0; pairNum<=169; pairNum++)
    {
      totalPhotonsR2[pairNum]+=combiCountR2[pairNum];
    }
  }

  setFilterPassed(ok);

  info() << "photons in 1209 = " << combiCountR2[40] << endmsg;

  if(ok)
  {
    info() << "event passed" << endmsg;
  }

  else
  {
    info() << "event didn't pass" << endmsg;
  }

  return StatusCode::SUCCESS;
};

//=============================================================================
//  Finalize
//=============================================================================
StatusCode EventSelection::finalize()
{

  //============================================================================
  //============================================================================
  // Execute base class method
  return RichRecTupleAlgBase::finalize();
}
