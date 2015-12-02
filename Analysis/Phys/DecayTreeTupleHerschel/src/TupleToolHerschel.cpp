// Include files

// from Gaudi
#include "GaudiKernel/ToolFactory.h"
#include "Event/ODIN.h"
#include "Event/HCDigit.h"
// from LHCb
#include "DetDesc/Condition.h"

// local
#include "TupleToolHerschel.h"

DECLARE_TOOL_FACTORY( TupleToolHerschel )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
TupleToolHerschel::TupleToolHerschel( const std::string& type,
                                              const std::string& name,
                                              const IInterface* parent )
  : TupleToolBase ( type, name, parent ),
    m_cond(nullptr)
{
  declareInterface<IEventTupleTool>(this);
  // Digits location
  declareProperty("DigitsLocation", m_DigitsLocation = "Raw/HC/CorrectedDigits");
  // Mappings (to be retrieved from conditions database).
  declareProperty("CrateB", m_crateB = 0);
  declareProperty("CrateF", m_crateF = 1);

  declareProperty("MasksB0", m_masksB0 = {false, false, false, false});
  declareProperty("MasksB1", m_masksB1 = {false, false, false, false});
  declareProperty("MasksB2", m_masksB2 = {false, false, false, false});
  declareProperty("MasksF1", m_masksF1 = {false, false, false, false});
  declareProperty("MasksF2", m_masksF2 = {false, false, false, false});

  declareProperty("ChannelsB0", m_channelsB0);
  declareProperty("ChannelsB1", m_channelsB1);
  declareProperty("ChannelsB2", m_channelsB2);
  declareProperty("ChannelsF1", m_channelsF1);
  declareProperty("ChannelsF2", m_channelsF2);

  declareProperty("SpareChannelsB0", m_sparesB0);
  declareProperty("SpareChannelsB1", m_sparesB1);
  declareProperty("SpareChannelsB2", m_sparesB2);
  declareProperty("SpareChannelsF1", m_sparesF1);
  declareProperty("SpareChannelsF2", m_sparesF2);
}

StatusCode TupleToolHerschel::initialize() {
  StatusCode sc = TupleToolBase::initialize();
  if ( sc.isFailure() ) return sc;
  // Set up the mapping
  m_channels.resize(5);
  m_masked.resize(5);
  // Check if the mapping is available in the conditions database.
  const std::string location = "Conditions/ReadoutConf/HC/Mapping";
  if (existDet<Condition>(location)) {
    registerCondition(location, m_cond, &TupleToolHerschel::cacheMapping);
    // First update.
    sc = updMgrSvc()->update(this);
    if (sc.isFailure()) {
      return Error("Cannot update mapping.", StatusCode::FAILURE);
    }
  } else {
    warning() << "Cannot find " << location << " in database" << endmsg;
    mapChannels(m_channelsB0, m_sparesB0, m_masksB0, 0, true);
    mapChannels(m_channelsB1, m_sparesB1, m_masksB1, 1, true);
    mapChannels(m_channelsB2, m_sparesB2, m_masksB2, 2, true);
    mapChannels(m_channelsF1, m_sparesF1, m_masksF1, 1, false);
    mapChannels(m_channelsF2, m_sparesF2, m_masksF2, 2, false);
  }
  printMapping();

  return sc ;
}

//=============================================================================
// Update the channel map using the conditions database.
//=============================================================================
StatusCode TupleToolHerschel::cacheMapping() {

  m_crateB = m_cond->param<int>("CrateB");
  m_crateF = m_cond->param<int>("CrateF");

  m_channelsB0 = m_cond->paramVect<int>("ChannelsB0");
  m_channelsB1 = m_cond->paramVect<int>("ChannelsB1");
  m_channelsB2 = m_cond->paramVect<int>("ChannelsB2");
  m_channelsF1 = m_cond->paramVect<int>("ChannelsF1");
  m_channelsF2 = m_cond->paramVect<int>("ChannelsF2");

  m_sparesB0 = m_cond->paramVect<int>("ReferenceChannelsB0");
  m_sparesB1 = m_cond->paramVect<int>("ReferenceChannelsB1");
  m_sparesB2 = m_cond->paramVect<int>("ReferenceChannelsB2");
  m_sparesF1 = m_cond->paramVect<int>("ReferenceChannelsF1");
  m_sparesF2 = m_cond->paramVect<int>("ReferenceChannelsF2");

  m_masksB0 = m_cond->paramVect<int>("MasksB0");
  m_masksB1 = m_cond->paramVect<int>("MasksB1");
  m_masksB2 = m_cond->paramVect<int>("MasksB2");
  m_masksF1 = m_cond->paramVect<int>("MasksF1");
  m_masksF2 = m_cond->paramVect<int>("MasksF2");

  mapChannels(m_channelsB0, m_sparesB0, m_masksB0, 0, true);
  mapChannels(m_channelsB1, m_sparesB1, m_masksB1, 1, true);
  mapChannels(m_channelsB2, m_sparesB2, m_masksB2, 2, true);
  mapChannels(m_channelsF1, m_sparesF1, m_masksF1, 1, false);
  mapChannels(m_channelsF2, m_sparesF2, m_masksF2, 2, false);
  return StatusCode::SUCCESS;
}

//=============================================================================
// Setup the channel map for a given station.
//=============================================================================
bool TupleToolHerschel::mapChannels(const std::vector<int>& channels,
                                    const std::vector<int>& refs,
                                    const std::vector<int>& masks,
                                    const unsigned int station,
                                    const bool bwd) {

  const unsigned int offset = bwd ? 0 : 2;
  // Check if the input is valid.
  if (channels.size() != 4 || refs.size() != 4 || masks.size() != 4) {
    std::string s = bwd ? "B" : "F";
    s += std::to_string(station);
    warning() << "Invalid channel map for station " << s
              << ". Masking all quadrants." << endmsg;
    m_channels[station + offset].assign(4, 0);
    m_masked[station + offset].assign(4, true);
    return false;
  }

  const unsigned int crate = bwd ? m_crateB : m_crateF;
  m_channels[station + offset].resize(4);
  m_masked[station + offset].resize(4);
  for (unsigned int i = 0; i < 4; ++i) {
    if (channels[i] < 0) {
      std::string s = bwd ? "B" : "F";
      s += std::to_string(station);
      warning() << "Invalid channel number " << channels[i]
                << ". Masking quadrant " << s << i << endmsg;
      m_masked[station + offset][i] = true;
      continue;
    }
    m_channels[station + offset][i] = (crate << 6) | channels[i];
    m_masked[station + offset][i] = masks[i] != 0;
  }
  return true;
}

//=============================================================================
// Print the channel map for information
//=============================================================================
void TupleToolHerschel::printMapping() const {

  info() << " Quadrant   ID   Ref. ID    Comment" << endmsg;
  const std::vector<std::string> stations = {"B0", "B1", "B2", "F1", "F2"};
  const unsigned int nStations = stations.size();
  for (unsigned int i = 0; i < nStations; ++i) {
    for (unsigned int j = 0; j < 4; ++j) {
      const std::string comment = m_masked[i][j] ? "Masked" : "";
      info() << format("   %2s/%1d", stations[i].c_str(), j)
             << format("   %4d", m_channels[i][j])
             << format(" %-10s", comment.c_str()) << endmsg;
    }
  }

}

//=============================================================================
// Destructor
//=============================================================================
TupleToolHerschel::~TupleToolHerschel() {}
//=============================================================================

//=============================================================================
StatusCode TupleToolHerschel::fill( Tuples::Tuple& tup )
{
  const std::string prefix = fullName();
 
  // Get ODIN
  const LHCb::ODIN* odin = getIfExists<LHCb::ODIN>(evtSvc(),LHCb::ODINLocation::Default);
  if ( !odin ) { odin = getIfExists<LHCb::ODIN>(evtSvc(),LHCb::ODINLocation::Default,false); }
  if ( !odin ) {
    // should always be available ...
    return Error( "Cannot load the ODIN data object", StatusCode::SUCCESS );
  }

  // Get Herschel digits.
//  LHCb::HCDigits* digits = getIfExists<LHCb::HCDigits>(LHCb::HCDigitLocation::Default);
  LHCb::HCDigits* digits = getIfExists<LHCb::HCDigits>(m_DigitsLocation);
  warning() << "No default HCDigits location has yet been defined -- beware" << endmsg;
  if ( !digits ) {
    // should always be available ...
    return Error( "Cannot load the HCDigits data object", StatusCode::SUCCESS );
  }
  
  // Fill the Herschel information
  bool test = true;
  const std::vector<std::string> stations = {"B0", "B1", "B2", "F1", "F2"};
  const unsigned int nStations = 5;
  for (unsigned int i = 0; i < nStations; ++i) {
    for (unsigned int j = 0; j < 4; ++j) {
      // Skip masked channels.
      if (m_masked[i][j]) continue;
      const std::string ch = stations[i] + std::to_string(j);
      LHCb::HCCellID id(m_channels[i][j]);
      const LHCb::HCDigit* digit = digits->object(id);
      if (!digit) {
        warning() << "Cannot retrieve digit for " << ch << endmsg;
        continue;
      }
      unsigned int adc = digit->adc();
      test &= tup->column(ch, adc);
     }
  }

  // Fill ADC vectors
  std::vector<double> adcB,adcF;
  for (const LHCb::HCDigit* dig : *digits) {
    const int cell = int(dig->cellID().cellID());
    const int crate = cell >> 6;
    if (crate == 0) {
      adcB.push_back((double)dig->adc());
    } else if (crate == 1) {
      adcF.push_back((double) dig->adc());
    } else {
      info() << "Crate = " << crate << ". We have a problem." << endmsg;
    }
  }
  
  // Fill tuple 
  test &= tup->farray( prefix+"adc_B", adcB.begin(), adcB.end(),"nchB",1000);
  test &= tup->farray( prefix+"adc_F", adcF.begin(), adcF.end(),"nchF",1000);

  return StatusCode(test);
}

