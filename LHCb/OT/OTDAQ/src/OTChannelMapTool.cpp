// Include files
// from STD
#include <sstream>

#include "GaudiAlg/GaudiTool.h"
#include "OTDAQ/IOTChannelMapTool.h"

#include "OTDet/DeOTModule.h"
#include "OTDet/DeOTDetector.h"
#include "Event/OTBankVersion.h"

/** @class OTChannelMapTool OTChannelMapTool.cpp
 *
 *
 *  @author Wouter Hulsbergen
 *
 *  @date   2008-03-06
 */

class OTChannelMapTool : public GaudiTool,
			 virtual public IOTChannelMapTool
{
  
public:
  /// Standard constructor
  OTChannelMapTool( const std::string& type, const std::string& name, const IInterface* parent);
  
  /// Destructor
  ~OTChannelMapTool( ) ; ///< Destructor
  
  /// initialization
  StatusCode initialize();
  
  /// get the channel map
  const OTDAQ::ChannelMap::Detector& getChannelMap() const { return m_channelmap ; }
  const OTDAQ::ChannelMap::Detector& getChannelMap(int bankversion) const { 
    setBankVersion(bankversion) ;
    return m_channelmap ; 
  }
  
  void setBankVersion( int bankversion ) const ;
  
private:
  void updateChannelMap() const ;
  static OTDAQ::EModuleLayout computeLayout( const DeOTModule& mod ) ;
  static unsigned char computeNumStrawPerMono ( const OTDAQ::EModuleLayout& layout ) ;
  static unsigned char computeStrawV3  ( const OTDAQ::EModuleLayout& layout, unsigned char channel ) ;
  static unsigned char computeStrawDC06( unsigned char channel ) ;
private:
  // data
  mutable OTDAQ::ChannelMap::Detector m_channelmap ; 
  mutable int m_currentBankVersion ;
};


//=============================================================================
// Declaration in the Tool Factory
//=============================================================================
DECLARE_TOOL_FACTORY( OTChannelMapTool )
  

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
OTChannelMapTool::OTChannelMapTool( const std::string& type,
				    const std::string& name,
				    const IInterface* parent )
  : GaudiTool ( type, name , parent ),
    m_currentBankVersion(OTBankVersion::DEFAULT)
{
  declareInterface<IOTChannelMapTool>(this);
}
//=============================================================================
// Destructor
//=============================================================================
OTChannelMapTool::~OTChannelMapTool() {}

//=============================================================================

//=============================================================================
// Initialisation. Check parameters
//=============================================================================
StatusCode OTChannelMapTool::initialize()
{
  
  if ( msgLevel( MSG::DEBUG ) ) debug()<<"initializing OTChannelMapTool"<<endmsg;
  
  StatusCode sc = GaudiTool::initialize();
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm
  
  updateChannelMap() ;
  
  return StatusCode::SUCCESS;
}


void OTChannelMapTool::setBankVersion( int bankversion ) const
{ 
  if( m_currentBankVersion != bankversion ) {
    m_currentBankVersion = bankversion ;
    updateChannelMap() ;
  }
}

OTDAQ::EModuleLayout OTChannelMapTool::computeLayout( const DeOTModule& mod )
{
  //int isOddLayer = mod.layerID() & 0x1;
  //int isPositiveY = mod.quarterID() / 0x2 ;
  //int isShortModule = mod.nChannels() <= 64 ;
  
  int rc(0) ;
  if( mod.quarterID() <=1 )                        rc += 1 ; // negative y
  if( mod.layerID()%2 ==0 )                        rc += 2 ; // even layer
  if( mod.nChannels() <= 2*OTDAQ::NumChanPerOtis ) rc += 4 ; // short module
  return OTDAQ::EModuleLayout(rc) ;
}

unsigned char OTChannelMapTool::computeNumStrawPerMono( const OTDAQ::EModuleLayout& layout )
{
  using namespace OTDAQ ;
  unsigned char numStrawPerMono(0) ;
  switch( layout ) {
  case OddLayerPosY:
  case OddLayerNegY:
  case EvenLayerPosY:
  case EvenLayerNegY:
    numStrawPerMono = 2*NumChanPerOtis ;
    break ;
  case OddLayerPosYShort:
  case OddLayerNegYShort:
  case EvenLayerPosYShort:
  case EvenLayerNegYShort:
  case NumDefaultModuleLayouts:
    numStrawPerMono = NumChanPerOtis ;
    break ;
  }
  return numStrawPerMono ;
}

unsigned char OTChannelMapTool::computeStrawV3( const OTDAQ::EModuleLayout& layout, unsigned char channel )
{
  using namespace OTDAQ ;
  // See Antonio's web page
  // http://www.nikhef.nl/pub/experiments/bfys/lhcb/outerTracker/NamingConventions/FE-Channels/index.html
  
  // now calculate the mono-layer (0-1) and the straw-in-mono-layer
  // (1-64) corresponding to the most 'regular' conversion: otis 0 and
  // 1 are the first monolayer and channel numbering for those is
  // already in positive x. otis 2 and 3 are the second mono layer,
  // but the numbering is inverted in x. 

  // We first create the map as if this was a long module. For short
  // modules, we'll swap things around afterwards. The main motivation
  // for this is that this routine also needs to do something quasi
  // sensible for non-existing straws.

  // This is the corresponding formula:
  unsigned char NumStrawPerMono = 2*NumChanPerOtis ;
  unsigned char otis           = channel/32 ;
  unsigned char tmpMono        = otis >= 2 ? 1 : 0 ;
  unsigned char tmpStrawInMono = tmpMono ? 2*NumStrawPerMono - channel : channel + 1 ;
  
  // depending on the type of module, we now correct this 'default'
  // numbering with rotation or mirror operations
  unsigned char rc(1) ;
  switch ( layout ) {
    case EvenLayerNegY:
    case EvenLayerNegYShort:
    case NumDefaultModuleLayouts:
      // All "oprations" are w.r.t. "regular"
      // "regular" i.e. bottom half even layer
      rc = tmpMono*NumStrawPerMono + tmpStrawInMono ;
      break ;
    case EvenLayerPosY:
    case EvenLayerPosYShort:
      // mirrored in X
      rc = (1 + tmpMono)*NumStrawPerMono - tmpStrawInMono + 1 ;
      break;
    case OddLayerNegY:
    case OddLayerNegYShort: 
      // rotated around Y
      rc = (2 - tmpMono)*NumStrawPerMono - tmpStrawInMono + 1 ;
      break ;
    case OddLayerPosY:
    case OddLayerPosYShort:
      // mirrored in X and rotated around Y ( monolayers are swapped )
      rc = (1 - tmpMono)*NumStrawPerMono + tmpStrawInMono ;
      break ;
  }

  // now treat the short modules. otis 0 and 3 must be in the realm
  // straws [1,64]. otis 1 and 2 will be moved to nonexisting straws
  // [65,128]. this is really ugly, but it works, somehow.
  switch( layout ) {
  case OddLayerPosYShort:
  case EvenLayerPosYShort:
  case OddLayerNegYShort: 
  case EvenLayerNegYShort: 
    rc = (rc-1)%NumChanPerOtis + ((rc-1)/(2*NumChanPerOtis)) * NumChanPerOtis  // this moves it to the (1-64) range
      +  ((otis==1 || otis==2) ? 2*NumChanPerOtis :0)                          // add 64 for the non-existing channels
      + 1;                                                                     // add the straw offset
    break ;
  default: ;
  }
  
  return rc ;
}

unsigned char OTChannelMapTool::computeStrawDC06( unsigned char channel )
{ 
  using namespace OTDAQ ;
  unsigned int otis = channel/NumChanPerOtis ;
  unsigned int channelInStraw = channel%NumChanPerOtis ;
  return otis/2 ? NumChanPerOtis * (6-otis) - channelInStraw : NumChanPerOtis * otis + channelInStraw + 1 ;
}

void OTChannelMapTool::updateChannelMap() const
{
  using namespace OTDAQ ;
  std::ostringstream mess;
  mess << "Updating OT channel map for bank version " << m_currentBankVersion ;
  Warning( mess.str(), StatusCode::SUCCESS, 0 ).ignore();

  // first sort the modules by type ('layout') and assign that type to the module in the channel map
  const DeOTDetector* otdet = getDet<DeOTDetector>(DeOTDetectorLocation::Default );
  typedef std::map< OTDAQ::EModuleLayout, std::vector<const DeOTModule*> > LayoutMap ;
  LayoutMap layoutmap ;
  for(DeOTDetector::Modules::const_iterator imod = otdet->modules().begin() ;
      imod != otdet->modules().end() ; ++imod) {
    OTDAQ::EModuleLayout thislayout = computeLayout(**imod) ;
    m_channelmap.setModuleLayout((*imod)->elementID(),thislayout) ;
    layoutmap[ thislayout ].push_back( *imod ) ;
  }
  
  // now fill the decoding table for each layout
  for( LayoutMap::const_iterator it = layoutmap.begin(); it != layoutmap.end(); ++it) {
    if ( msgLevel( MSG::DEBUG ) ) debug() << "Number of modules of type " << it->first 
                                          << " is " << it->second.size() << endmsg ;

    // create a new module
    OTDAQ::ChannelMap::Module module ;

    // set the number of channels
    module.m_nstraws = 2*computeNumStrawPerMono(it->first) ;
    
    // what happens next depends on the bank version. eventually, we
    // can also read a file that corrects the channel map
     switch( m_currentBankVersion ) {
       case OTBankVersion::DC06:
         for ( unsigned ichan = 0; ichan < OTDAQ::ChannelMap::Module::NumChannels; ++ichan ) 
           module.m_channelToStraw[ichan] = computeStrawDC06(ichan) ;
         break;
         // Note: SIM and v3 currently (22/07/2008) uses same decoding.
         //       If SIM changes w.r.t. to the real decoding then we'll need
         //       to change it here.
       case OTBankVersion::SIM:
       case OTBankVersion::v3:
         for ( unsigned ichan = 0; ichan < OTDAQ::ChannelMap::Module::NumChannels; ++ichan ) 
           module.m_channelToStraw[ichan] =computeStrawV3( it->first, ichan ) ;
     }
     
    // if( m_currentBankVersion == OTBankVersion::DC06 ) {
    //       for(unsigned int ichan = 0; ichan < OTDAQ::ChannelMap::Module::NumChannels; ++ichan) 
    // 	module.m_channelToStraw[ichan] = computeStrawDC06(ichan) ;
    //     } else {
    //       for(unsigned int ichan = 0; ichan < OTDAQ::ChannelMap::Module::NumChannels; ++ichan)
    //         module.m_channelToStraw[ichan] =computeStrawV3( it->first, ichan ) ; 
    //     }
    
    // finally, fill the inverse table
    module.fillStrawToChannelMap() ;
    
    // finally set the module
    m_channelmap.m_modules[it->first] = module ;
  }

  // now you can set specific things for special modules (e.g. from a file). the recipee is:
  // 1. fill a new ModuleLayout (don't forget to fill inverse as well with 'fillStrawToChannelMap')
  // 2. add it to the list of modulelayouts:
  //    m_channelmap.m_modules.push_back(newmodulelayout)
  // 3. set the pointer: 
  //    m_channelmap.setModuleLayout(moduleID,m_channelmap.m_modules.size()-1) ;
  //
  
}
