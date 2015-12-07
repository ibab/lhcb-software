// Include files
#include "RichChannelPropertiesPMT.h"

//-----------------------------------------------------------------------------
// Implementation file for class : RichChannelPropertiesPMT
//
// 2015-08-19 : Mariusz Witek
// 2015-12-07 : modified by Marcin Kucharczyk
//-----------------------------------------------------------------------------

using namespace Rich;
using namespace Rich::MC::Digi;

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( RichChannelPropertiesPMT )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
RichChannelPropertiesPMT::RichChannelPropertiesPMT( const std::string& type,
                                                    const std::string& name,
                                                    const IInterface* parent )
: ToolBase ( type, name , parent ),
  m_meanGainGen(0.),
  m_sigmaGainGen(0.),
  m_sigmaNoiseGen(0.),
  m_channelThresholdGen(0.)
{
  declareInterface<RichChannelPropertiesPMT>(this);
  declareProperty("MeanGainGen"         , m_meanGainGen      = 1.);
  declareProperty("SigmaGainGen"        , m_sigmaGainGen     = .3);
  declareProperty("SigmaNoiseGen"       , m_sigmaNoiseGen    = 0.03);
  declareProperty("ChannelThresholdGen", m_channelThresholdGen      = 0.2);
  // 
  declareProperty("SetParametersFromDB", m_setParametersFromDB  = false );
 
}
//=============================================================================
// Destructor
//=============================================================================
RichChannelPropertiesPMT::~RichChannelPropertiesPMT() {} 

//=============================================================================
// Getters
//=============================================================================
StatusCode RichChannelPropertiesPMT::ReadChannelParameters(const LHCb::RichSmartID::Vector & pixels) 
{
  
  if ( m_setParametersFromDB ) {
    setParametersFromDB(pixels);
  }
  else {
    generateParameters(pixels);
  }
  return StatusCode::SUCCESS;
} 
//=============================================================================
// Getters
//=============================================================================
double RichChannelPropertiesPMT::pixelMeanGain(const LHCb::RichSmartID id ) 
{
  return (m_tabChannelParemeters[id]).pixelMeanGain();
} 

double RichChannelPropertiesPMT::pixelSigmaGain(const LHCb::RichSmartID id ) 
{
  return (m_tabChannelParemeters[id]).pixelSigmaGain();
} 

double RichChannelPropertiesPMT::pixelSigmaNoise(const LHCb::RichSmartID id ) 
{
  return (m_tabChannelParemeters[id]).pixelSigmaNoise();
} 

double RichChannelPropertiesPMT::channelThreshold(const LHCb::RichSmartID id ) 
{
  return (m_tabChannelParemeters[id]).channelThreshold();
} 


// read or generate parametres for each channel
void RichChannelPropertiesPMT::generateParameters(const LHCb::RichSmartID::Vector & pixels)
{
  // simple generation, same parameters for all pixels 

  info() << "Generating parameters for " << pixels.size() << " pixels" << endmsg;

  for ( LHCb::RichSmartID::Vector::const_iterator intIndex = pixels.begin();
        intIndex != pixels.end();
        ++intIndex ) {
    RichChannelParamsPMT tmpParams;
    tmpParams.setPixelMeanGain(m_meanGainGen);
    tmpParams.setPixelSigmaGain(m_sigmaGainGen);
    tmpParams.setPixelSigmaNoise(m_sigmaNoiseGen);
    tmpParams.setChannelThreshold(m_channelThresholdGen);
 
    m_tabChannelParemeters[*intIndex] = tmpParams;   

  } 

}

void RichChannelPropertiesPMT::setParametersFromDB(const LHCb::RichSmartID::Vector & pixels)
{
   
  info() << "Generating parameters for " << pixels.size() << " pixels" << endmsg;

  // not implemented 
  info() << "Reading Rich channel parameters from DB not implemented yet " << endmsg;

}

//=============================================================================
