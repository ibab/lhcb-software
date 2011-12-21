
//-------------------------------------------------------------------------------
/** @file ChargedProtoANNPIDAlg.cpp
 *
 *  Implementation file for ANN Combined PID algorithm ChargedProtoANNPIDAlg
 *
 *  @author Chris Jones       Christopher.Rob.Jones@cern.ch
 *  @date   10/09/2010
 */
//-------------------------------------------------------------------------------

// local
#include "ChargedProtoANNPIDAlg.h"

//-----------------------------------------------------------------------------

using namespace ANNGlobalPID;

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( ChargedProtoANNPIDAlg )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
ChargedProtoANNPIDAlg::ChargedProtoANNPIDAlg( const std::string& name,
                                              ISvcLocator* pSvcLocator )
  : ChargedProtoANNPIDAlgBase ( name , pSvcLocator ),
    m_trSel                   ( NULL               ),
    m_netHelper               ( NULL               )
{
  // JOs
  declareProperty( "Configuration",     m_configFile );
  declareProperty( "TrackSelectorType", m_trSelType = "TrackSelector" );
  declareProperty( "NetworkVersion",    m_netVersion = "MC10TuneV1" );
  // turn off histos and ntuples
  setProperty( "HistoProduce",   false );
  setProperty( "NTupleProduce",  false );
  setProperty( "EvtColsProduce", false );
}

//=============================================================================
// Destructor
//=============================================================================
ChargedProtoANNPIDAlg::~ChargedProtoANNPIDAlg() { }

//=============================================================================
// Initialization
//=============================================================================
StatusCode ChargedProtoANNPIDAlg::initialize()
{
  StatusCode sc = ChargedProtoANNPIDAlgBase::initialize();
  if ( sc.isFailure() ) return sc;

#ifdef __GNUC__

  // ParamFile root
  const std::string paramEnv = "CHARGEDPROTOANNPIDPARAMROOT";
  if ( !getenv(paramEnv.c_str()) ) return Error( "$"+paramEnv+" not set" );
  const std::string paramRoot = ( std::string(getenv(paramEnv.c_str())) +
                                  "/data/" + m_netVersion + "/" );

  // configuration file path
  if ( m_configFile.empty() ) return Error( "No configuration file specified" );
  const std::string configFile = paramRoot+m_configFile;

  // Open the config file
  std::ifstream config( configFile.c_str() );
  if ( config.is_open() )
  {
    // Read the particle type
    std::string particleType;
    config >> particleType;

    // Read the track Type
    std::string trackType;
    config >> trackType;

    // Track selection cuts
    double minP(0),minPt(0),maxChiSq(0),minLikelihood(0),maxGhostProb(0);
    config >> minP;
    config >> minPt;
    config >> maxChiSq;
    config >> minLikelihood;
    config >> maxGhostProb;

    // Track Pre-Selection
    config >> m_trackPreSel;

    // Proto variable to fill
    if      ( "electron" == particleType ) { m_protoInfo = LHCb::ProtoParticle::ProbNNe; }
    else if ( "muon"     == particleType ) { m_protoInfo = LHCb::ProtoParticle::ProbNNmu; }
    else if ( "pion"     == particleType ) { m_protoInfo = LHCb::ProtoParticle::ProbNNpi; }
    else if ( "kaon"     == particleType ) { m_protoInfo = LHCb::ProtoParticle::ProbNNk; }
    else if ( "proton"   == particleType ) { m_protoInfo = LHCb::ProtoParticle::ProbNNp; }
    else if ( "ghost"    == particleType ) { m_protoInfo = LHCb::ProtoParticle::ProbNNghost; }
    else    { return Error( "Unknown particle type " + particleType ); }

    // Read the network type
    std::string annType;
    config >> annType;

    // Read network parameters file name
    std::string paramFileName;
    config >> paramFileName;
    paramFileName = paramRoot+paramFileName;
    std::ifstream ftest(paramFileName.c_str());
    if ( !ftest.is_open() ) return Error( "Network parameters file '" +
                                          paramFileName + "' cannot be opened" );
    ftest.close();

    // Read the list of inputs
    std::string input;
    StringInputs inputs;
    while ( config >> input )
    {
      if ( !input.empty() )
      {
        if ( input.find("#") == std::string::npos )
        {
          inputs.push_back(input);
        }
      }
    }

    // Load the network   
    if ( "NeuroBayes" == annType )
    {
      // FPE Guard for NB call
      FPE::Guard guard(true);
      m_netHelper = new NeuroBayesANN(paramFileName,variableIDs(inputs),this);
    }
    else
    {
      return Error( "Unknown ANN type '"+annType+"'" );
    }

    // Set options for owned TrackSelector
    const std::string trSelName = "TrackSelector";
    DoubleProperty       pProp    ( "MinPCut",    minP  );
    DoubleProperty      ptProp    ( "MinPtCut",   minPt );
    DoubleProperty      chiProp   ( "MaxChi2Cut", maxChiSq );
    DoubleProperty      likProp   ( "MinLikelihoodCut", minLikelihood );
    DoubleProperty      ghostProp ( "MaxGhostProbCut", maxGhostProb );
    StringArrayProperty tkProp    ( "TrackTypes", boost::assign::list_of(trackType) );
    IJobOptionsSvc * joSvc = svc<IJobOptionsSvc>("JobOptionsSvc");
    sc = sc && joSvc->addPropertyToCatalogue( name()+"."+trSelName, pProp     );
    sc = sc && joSvc->addPropertyToCatalogue( name()+"."+trSelName, ptProp    );
    sc = sc && joSvc->addPropertyToCatalogue( name()+"."+trSelName, chiProp   );
    sc = sc && joSvc->addPropertyToCatalogue( name()+"."+trSelName, likProp   );
    sc = sc && joSvc->addPropertyToCatalogue( name()+"."+trSelName, ghostProp );
    sc = sc && joSvc->addPropertyToCatalogue( name()+"."+trSelName, tkProp    );
    sc = sc && release(joSvc);
    if ( sc.isFailure() )
    { return Error( "Problems setting TrackSelector Properties" ); }

    // get an instance of the track selector
    m_trSel = tool<ITrackSelector>( m_trSelType, trSelName, this );

    // print a summary of the configuration
    if ( msgLevel(MSG::DEBUG) )
      debug() << "Particle type    = " << particleType << endmsg
              << "Track Selection  = " << trackType << " " << m_trackPreSel << endmsg
              << "Network type     = " << annType << endmsg
              << "ParamFile        = " << paramFileName << endmsg
              << "ANN inputs (" << inputs.size() << ")  = " << inputs
              << endmsg;
    
  }
  else
  {
    return Error( "Failed to open configuration file '"+paramRoot+m_configFile+"'" );
  }

  // Close the config file
  config.close();

#endif

  // return
  return sc;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode ChargedProtoANNPIDAlg::execute()
{

#ifdef __GNUC__

  // Load the charged ProtoParticles
  LHCb::ProtoParticles * protos = get<LHCb::ProtoParticles>( m_protoPath );

  // Loop over ProtoParticles
  for ( LHCb::ProtoParticles::iterator iP = protos->begin();
        iP != protos->end(); ++iP )
  {
    LHCb::ProtoParticle * proto = *iP;

    // Select Tracks
    if ( !proto->track() )
    { return Error( "Charged ProtoParticle has NULL Track pointer" ); }
    if ( !m_trSel->accept(*(proto->track())) ) continue;

    // Clear current ANN PID information
    if ( proto->hasInfo(m_protoInfo) )
    {
      std::ostringstream mess;
      mess << "ProtoParticle already has '" << m_protoInfo << "' information -> Replacing.";
      Warning( mess.str(), StatusCode::SUCCESS, 1 ).ignore();
      proto->eraseInfo(m_protoInfo);
    }

    // Track Pre-selection
    if ( !trackPreSel(proto) ) continue;

    // get the ANN output for this proto
    const double nnOut = m_netHelper->getOutput( proto );
    if ( msgLevel(MSG::VERBOSE) )
      verbose() << " -> ANN value = " << nnOut << endmsg;

    // add to protoparticle
    proto->addInfo( m_protoInfo, nnOut );

  } // loop over protos

#endif

  return StatusCode::SUCCESS;
}

//=============================================================================
// Run track preselection
//=============================================================================
bool
ChargedProtoANNPIDAlg::trackPreSel( const LHCb::ProtoParticle * proto ) const
{
  bool OK = true;
  if ( !m_trackPreSel.empty() && "TrackPreSelNone" != m_trackPreSel )
  {
    if      ( "TrackPreSelIsLooseMuon" == m_trackPreSel )
    {
      OK = ( getInput(proto,"MuonIsLooseMuon") > 0.5 );
    }
    else if ( "TrackPreSelIsMuon"      == m_trackPreSel )
    {
      OK = ( getInput(proto,"MuonIsMuon") > 0.5 );
    }
    else if ( "TrackPreSelHasRICHInfo" == m_trackPreSel )
    {
      OK = hasRichInfo(proto);
    }
    else if ( "TrackPreSelHasECALInfo" == m_trackPreSel )
    {
      OK = hasEcalInfo(proto);
    }
    else if ( "TrackPreSelHasECALorRICHInfo" == m_trackPreSel )
    {
      OK = ( hasRichInfo(proto) || hasEcalInfo(proto) );
    }
    else if ( "TrackPreSelHasECALandRICHInfo" == m_trackPreSel )
    {
      OK = ( hasRichInfo(proto) && hasEcalInfo(proto) );
    }
    else
    {
      OK = false;
      Exception( "Unknown Track pre-selection '" + m_trackPreSel + "'" );
    }
  }
  return OK;
}
//=============================================================================

//=============================================================================
// Get ANN output for NeuroBayes network helper
//=============================================================================
#ifdef __GNUC__
double
ChargedProtoANNPIDAlg::NeuroBayesANN::getOutput( const LHCb::ProtoParticle * proto ) const
{
  // Fill the array of network inputs
  unsigned int input = 0;
  for ( ChargedProtoANNPIDAlgBase::IntInputs::const_iterator iIn = m_inputs.begin();
        iIn != m_inputs.end(); ++iIn, ++input )
  {
    m_inArray[input] = static_cast<float>(m_parent->getInput(proto,*iIn));
  }
 
  // FPE Guard for NB call
  FPE::Guard guard(true);

  // get the NN output
  //m_parent->verbose() << "Calling NB" << endmsg;
  const double nnOut = m_expert->nb_expert(m_inArray);

  // return final output, rescaled to the range 0 to 1
  return ( 1.0 + nnOut ) * 0.5;
}
#endif

//=============================================================================
//  Finalize
//=============================================================================
StatusCode ChargedProtoANNPIDAlg::finalize()
{
  // Clean Up
  delete m_netHelper;
  m_netHelper = NULL;
  // return
  return ChargedProtoANNPIDAlgBase::finalize();
}

//=============================================================================
