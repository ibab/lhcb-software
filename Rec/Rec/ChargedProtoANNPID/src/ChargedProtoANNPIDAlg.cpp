
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

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
ChargedProtoANNPIDAlg::ChargedProtoANNPIDAlg( const std::string& name,
                                              ISvcLocator* pSvcLocator )
  : ChargedProtoANNPIDAlgBase ( name , pSvcLocator ),
    m_netHelper               ( NULL               ),
    m_tkType                  ( LHCb::Track::TypeUnknown )
{
  // JOs
  declareProperty( "Configuration",       m_configFile );
  declareProperty( "NetworkVersion",      m_netVersion = "MC12TuneV2"  );
  declareProperty( "SuppressANNPrintout", m_suppressANNPrintout = true );
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

  // ParamFile root
  const std::string paramEnv = "CHARGEDPROTOANNPIDROOT";
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
    if      ( "Long"       == trackType ) { m_tkType = LHCb::Track::Long; }
    else if ( "Downstream" == trackType ) { m_tkType = LHCb::Track::Downstream; }
    else if ( "Upstream"   == trackType ) { m_tkType = LHCb::Track::Upstream; }
    else
    {
      return Error( "Unsupported track type '" + trackType + "'" );
    }

    // Track selection cuts file name
    std::string cutsFile;
    config >> cutsFile;

    // Check if this string ends with .txt ?
    if ( cutsFile.find(".txt") != std::string::npos )
    {

      // New style cuts file, so read from file
      cutsFile = paramRoot+cutsFile;
      std::ifstream cuts(cutsFile.c_str());
      if ( !cuts.is_open() ) return Error( "Track Selection cuts file '" +
                                           cutsFile + "' cannot be opened" );

      // Read the cuts and create a cut object for each
      std::string cut;
      while ( std::getline(cuts,cut) )
      {
        // Skip empty lines or comments
        if ( !cut.empty() && cut.find("#") == std::string::npos )
        {
          // try and make a cut for this string
          m_cuts.push_back( new Cut(cut,this) );
          if ( !m_cuts.back()->isOK() )
          { return Error( "Failed to decode selection cut '" + cut + "'" ); }
        }
      }

      // close the file
      cuts.close();

    }
    else
    {

      // Old style, so last line and the next few are cut values themselves
      std::string minP,minPt,maxChiSq,minLikeli,maxGhostProb,trackPreSel;
      minP = cutsFile;
      config >> minPt;
      config >> maxChiSq;
      config >> minLikeli;
      config >> maxGhostProb;
      config >> trackPreSel;

      // Create cut objects
      bool ok = true;
      m_cuts.push_back( new Cut( "TrackP                > "+minP,         this ) );
      ok &= m_cuts.back()->isOK();
      m_cuts.push_back( new Cut( "TrackPt               > "+minPt,        this ) );
      ok &= m_cuts.back()->isOK();
      m_cuts.push_back( new Cut( "TrackChi2PerDof       < "+maxChiSq,     this ) );
      ok &= m_cuts.back()->isOK();
      m_cuts.push_back( new Cut( "TrackLikelihood       > "+minLikeli,    this ) );
      ok &= m_cuts.back()->isOK();
      m_cuts.push_back( new Cut( "TrackGhostProbability < "+maxGhostProb, this ) );
      ok &= m_cuts.back()->isOK();
      if ( trackPreSel == "TrackPreSelIsMuon" )
      {
        m_cuts.push_back( new Cut( "MuonIsMuon > 0.5", this ) );
        ok &= m_cuts.back()->isOK();
      }
      if ( !ok ) { return Error( "Failed to decode old style track cuts" ); }

    }

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
    std::ifstream netparamtest(paramFileName.c_str());
    if ( !netparamtest.is_open() ) return Error( "Network parameters file '" +
                                                 paramFileName + "' cannot be opened" );
    netparamtest.close();

    // Read the list of inputs
    std::string input;
    StringInputs inputs;
    while ( config >> input )
    {
      // Skip empty lines and comments
      if ( !input.empty() && input.find("#") == std::string::npos )
      {
        inputs.push_back(input);
      }
    }

    // Load the network helper object
    if ( "TMVA" == annType )
    {
      // First see if we have a built in C++ implementation for this case
      m_netHelper = new TMVAImpANN( m_netVersion, particleType, trackType,
                                    inputs, this );
      if ( !m_netHelper->isOK() )
      {
        // No, so try again with a TMVA Reader
        warning() << "Compiled TMVA implementation not available for "
                  << m_netVersion << " " << particleType << " " << trackType
                  << " -> Reverting to XML Reader"
                  << endmsg;
        delete m_netHelper;
        m_netHelper = new TMVAReaderANN( paramFileName, inputs, this );
      }
    }
#ifdef _ENABLE_NEUROBAYES
    else if ( "NeuroBayes" == annType )
    {
      debug() << "Using NeuroBayes Expert implementation" << endmsg;
      // FPE Guard for NB call
      FPE::Guard guard(true);
      m_netHelper = new NeuroBayesANN( paramFileName, inputs, this,
                                       m_suppressANNPrintout );
    }
#endif
    else
    {
      return Error( "Unknown ANN type '"+annType+"'" );
    }

    // print a summary of the configuration
    const std::string sF = "ANNPID : Tune=%-13s TrackType=%-12s Particle=%-12s";
    info() << boost::format(sF) % m_netVersion % trackType % particleType << endmsg;
    if ( msgLevel(MSG::DEBUG) )
      debug() << "Network type     = " << annType << endmsg
              << "ConfigFile       = " << configFile << endmsg
              << "ParamFile        = " << paramFileName << endmsg
              << "ANN inputs (" << inputs.size() << ")  = " << inputs << endmsg
              << "Preselection Cuts (" << m_cuts.size() << ") = " << m_cuts
              << endmsg;

  }
  else
  {
    return Error( "Failed to open configuration file '"+configFile+"'" );
  }

  // Close the config file
  config.close();

  // return
  return sc;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode ChargedProtoANNPIDAlg::execute()
{

  // Load the charged ProtoParticles
  LHCb::ProtoParticles * protos = getIfExists<LHCb::ProtoParticles>( m_protoPath );
  if ( !protos ) return StatusCode::SUCCESS;

  // Loop over ProtoParticles
  for ( LHCb::ProtoParticles::iterator iP = protos->begin();
        iP != protos->end(); ++iP )
  {
    LHCb::ProtoParticle * proto = *iP;

    // Select Tracks
    if ( !proto->track() )
    { return Error( "Charged ProtoParticle has NULL Track pointer" ); }
    if ( !proto->track()->checkType(m_tkType) ) continue;

    // Clear current ANN PID information
    if ( proto->hasInfo(m_protoInfo) )
    {
//       std::ostringstream mess;
//       mess << "ProtoParticle already has '" << m_protoInfo
//            << "' information -> Replacing.";
//       Warning( mess.str(), StatusCode::SUCCESS, 1 ).ignore();
      proto->eraseInfo(m_protoInfo);
    }

    // ANN Track Selection.
    bool ok = true;
    for ( Cut::ConstVector::const_iterator iC = m_cuts.begin();
          iC != m_cuts.end(); ++iC )
    {
      if ( !(*iC)->isSatisfied(proto) ) { ok = false; break; }
    }
    if ( !ok ) continue;

    // get the ANN output for this proto
    const double nnOut = m_netHelper->getOutput( proto );
    if ( msgLevel(MSG::VERBOSE) )
      verbose() << " -> ANN value = " << nnOut << endmsg;

    // add to protoparticle
    proto->addInfo( m_protoInfo, nnOut );

  } // loop over protos

  return StatusCode::SUCCESS;
}

//=============================================================================
// Cut constructor
//=============================================================================
ChargedProtoANNPIDAlg::Cut::Cut( const std::string& desc,
                                 const ChargedProtoANNPIDAlgBase * parent )
  : m_desc(desc),
    m_OK(false),
    m_variable(NULL),
    m_cut(0),
    m_delim(UNDEFINED)
{
  // Cuts must have a precise form. Either
  //    variable > value
  // or
  //    variable < value

  // Parse the cut string
  boost::regex re("\\s+");
  boost::sregex_token_iterator i( desc.begin(), desc.end(), re, -1 );
  boost::sregex_token_iterator j;
  std::vector<std::string> matches;
  while ( i != j ) { matches.push_back( *i++ ); }
  if ( matches.size() == 3 )
  {
    // Get the variable from its name
    m_variable = parent->getInput( matches[0] );

    // Delimitor
    m_OK = setDelim( matches[1] );

    // The cut value
    m_cut = boost::lexical_cast<double>( matches[2] );
  }

  // Remove spaces from the cached description string
  boost::erase_all( m_desc, " " );
}

#ifdef _ENABLE_NEUROBAYES
//=============================================================================
// Get ANN output for NeuroBayes network helper
//=============================================================================
double
ChargedProtoANNPIDAlg::NeuroBayesANN::getOutput( const LHCb::ProtoParticle * proto )
  const
{
  // Fill the array of network inputs
  unsigned int input = 0;
  for ( Inputs::const_iterator iIn = m_inputs.begin();
        iIn != m_inputs.end(); ++iIn, ++input )
  {
    m_inArray[input] = static_cast<float>( (*iIn)->value(proto) );
  }

  // FPE Guard for NB call
  FPE::Guard guard(true);

  // NeuroBayes seems to sporadically send mysterious std messages which we
  // cannot control... So forcibly intercept them all here and send to /dev/null
  int original_stdout(0), original_stderr(0);
  if ( m_suppressPrintout )
  {
    original_stdout = dup(fileno(stdout));
    fflush(stdout);
    freopen("/dev/null","w",stdout);
    original_stderr = dup(fileno(stderr));
    fflush(stderr);
    freopen("/dev/null","w",stderr);
  }

  // get the NN output, rescaled to the range 0 to 1
  const double nnOut = 0.5 * ( 1.0 + (double)m_expert->nb_expert(m_inArray) );

  // put std back to normal
  if ( m_suppressPrintout )
  {
    fflush(stdout);
    dup2(original_stdout,fileno(stdout));
    close(original_stdout);
    fflush(stderr);
    dup2(original_stderr,fileno(stderr));
    close(original_stderr);
  }

  // return final output
  return nnOut;
}
#endif

//=============================================================================
// Get ANN output for TMVA Reader helper
//=============================================================================
double
ChargedProtoANNPIDAlg::TMVAReaderANN::getOutput( const LHCb::ProtoParticle * proto )
  const
{
  // Fill the array of network inputs
  unsigned int input = 0;
  for ( Inputs::const_iterator iIn = m_inputs.begin();
        iIn != m_inputs.end(); ++iIn, ++input )
  {
    m_inArray[input] = static_cast<float>( (*iIn)->value(proto) );
  }

  // get the output
  double mvaOut = m_reader->EvaluateMVA("PID");

  // Final sanity check
  if      ( mvaOut > 1.0 ) { mvaOut = 1.0; }
  else if ( mvaOut < 0.0 ) { mvaOut = 0.0; }

  // return
  return mvaOut;
}

//=============================================================================
// Get ANN output for TMVA Imp helper
//=============================================================================
double
ChargedProtoANNPIDAlg::TMVAImpANN::getOutput( const LHCb::ProtoParticle * proto )
  const
{
  // Fill the array of network inputs
  unsigned int input = 0;
  for ( Inputs::const_iterator iIn = m_inputs.begin();
        iIn != m_inputs.end(); ++iIn, ++input )
  {
    m_vars[input] = (*iIn)->value(proto);
  }

  // get the output
  double mvaOut = m_reader->GetMvaValue(m_vars);

  // Final sanity check
  if      ( mvaOut > 1.0 ) { mvaOut = 1.0; }
  else if ( mvaOut < 0.0 ) { mvaOut = 0.0; }

  // return
  return mvaOut;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode ChargedProtoANNPIDAlg::finalize()
{
  // Clean Up
  delete m_netHelper;
  m_netHelper = NULL;
  for ( Cut::ConstVector::const_iterator iC = m_cuts.begin();
        iC != m_cuts.end(); ++iC ) { delete *iC; }
  m_cuts.clear();
  // return
  return ChargedProtoANNPIDAlgBase::finalize();
}

//=============================================================================

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( ChargedProtoANNPIDAlg )

//=============================================================================
