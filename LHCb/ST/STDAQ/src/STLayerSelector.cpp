// $Id: STLayerSelector.cpp,v 1.0 2012-05-12 15:16:34 elsasser Exp $

// Include files
// local
#include "STLayerSelector.h"
// kernel
#include "Kernel/STChannelID.h"

// ====================================================================
// ====================================================================

DECLARE_TOOL_FACTORY( STLayerSelector )

// ====================================================================
// ====================================================================
STLayerSelector::STLayerSelector
( const std::string& type,
  const std::string& name,
  const IInterface* parent )
  : GaudiTool ( type, name, parent)
{
  declareInterface<ISTChannelIDSelector> (this);
  
  declareProperty( "DetType", m_detType, "TT");
  declareProperty( "IgnoredLayers",m_ignoredLayers=std::vector<std::string>(0));
  // TT layer mapping
  m_layerMap.insert(std::pair<std::string, unsigned int>("TTaX",9));
  m_layerMap.insert(std::pair<std::string, unsigned int>("TTaU",10));
  m_layerMap.insert(std::pair<std::string, unsigned int>("TTbV",17));
  m_layerMap.insert(std::pair<std::string, unsigned int>("TTbX",18));

  // IT layer mapping
  m_layerMap.insert(std::pair<std::string, unsigned int>("T1X1",9));
  m_layerMap.insert(std::pair<std::string, unsigned int>("T1U", 10));
  m_layerMap.insert(std::pair<std::string, unsigned int>("T1V", 11));
  m_layerMap.insert(std::pair<std::string, unsigned int>("T1X2",12));

  m_layerMap.insert(std::pair<std::string, unsigned int>("T2X1",17));
  m_layerMap.insert(std::pair<std::string, unsigned int>("T2U", 18));
  m_layerMap.insert(std::pair<std::string, unsigned int>("T2V", 19));
  m_layerMap.insert(std::pair<std::string, unsigned int>("T2X2",20));

  m_layerMap.insert(std::pair<std::string, unsigned int>("T3X1",25));
  m_layerMap.insert(std::pair<std::string, unsigned int>("T3U", 26));
  m_layerMap.insert(std::pair<std::string, unsigned int>("T3V", 27));
  m_layerMap.insert(std::pair<std::string, unsigned int>("T3X2",28));
  
}

// ====================================================================

// ====================================================================
// ====================================================================
STLayerSelector::~STLayerSelector(){}
// ====================================================================

// ====================================================================
StatusCode STLayerSelector::initialize ()
{
  // initalize the base class
  StatusCode sc = GaudiTool::initialize();
  if( sc.isFailure() )
  {
    return Error("Could not initialize the base class GaudiTool",sc);
  }
  return StatusCode::SUCCESS;
}
// ====================================================================

// ====================================================================
// ====================================================================
bool STLayerSelector::select
(const LHCb::STChannelID& id ) const 
{
  return (*this)(id);
}
// ====================================================================

// ====================================================================
// ====================================================================
bool STLayerSelector::operator() ( const LHCb::STChannelID& id) const
{
  
  
  // Checks detector
  if ((m_detType == "TT" && id.isIT()) || (m_detType == "IT" && id.isTT()))
  {
    if( msgLevel(MSG::DEBUG) ) debug() << "Excluded layers are in " << m_detType << ". Cluster is in other detector." <<endmsg;
    
    return false;
  }

  // Checks layer
  std::vector<std::string>::const_iterator it = m_ignoredLayers.begin();
  for (;it!=m_ignoredLayers.end();it++){
    if ( (int)((*it).find(m_detType)) == -1){ // Checks if detector and layer agree
      continue;
    } 
    std::map<std::string,unsigned int>::const_iterator jt = m_layerMap.begin();
    for(;jt!=m_layerMap.end();jt++){ 
      unsigned int layerNumb  =  (*jt).second; 
      std::string  layerName  =  (*jt).first;
      if( (id.uniqueLayer() == layerNumb) && (layerName==(*it)) ){
        if( msgLevel(MSG::DEBUG) ) debug() << "Cluster is in "<< m_detType <<" layer "<< (*it) <<" and will be removed!" << endmsg;
        return true;
      }
    }
    
    if( msgLevel(MSG::DEBUG) ) debug() << "Cluster will not be removed!"<<endmsg;
  }
  
    

  return false;
}
// ====================================================================
