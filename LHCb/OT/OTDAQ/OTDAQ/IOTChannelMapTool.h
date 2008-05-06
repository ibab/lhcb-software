// $Id: IOTChannelMapTool.h,v 1.1 2008-05-06 11:45:00 wouter Exp $
#ifndef OTDAQ_IOTCHANNELMAPTOOL_H
#define OTDAQ_IOTCHANNELMAPTOOL_H 1

// Include files
// from Gaudi
#include "GaudiKernel/IAlgTool.h"

// Kernel
#include "OTDAQ/OTChannelMap.h"

// Forward declarations
class DeOTModule ;

/// Interface ID
static const InterfaceID IID_OTChannelMapTool( "IOTChannelMapTool", 1, 0 );

/** @class IOTChannelMapTool IOTChannelMapTool.h OTDAQ/IOTChannelMapTool.h
 *
 * Tool to provide conversions from straws to channels in the OTDAQ.
 *
 *  @author W. Hulsbergen
 *
 *  @date   2008-05-06
 */

class IOTChannelMapTool : virtual public IAlgTool
{
  
public: 
  /// Retrieve interface ID
  static const InterfaceID& interfaceID() { return IID_OTChannelMapTool ; }
  
  /// Retrieve straw (1-128) for given fed channel (0-127). Make sure that bank version is set!
  unsigned int straw(unsigned int station, unsigned int layer, unsigned int quadrant,
		     unsigned int module, unsigned int channel) const ;
  
  /// Retrieve channel (0-127) for given straw (1-128). Make sure that bank version is set!
  unsigned int channel(unsigned int station, unsigned int layer, unsigned int quadrant,
		       unsigned int module, unsigned int straw) const ;
  
  /// Retrieve channel (0-127) for OTChannelID
  unsigned int channel(const LHCb::OTChannelID& otid) const ;
  
  /// Retrieve the channel map. Make sure the bank version is set!
  virtual const OTDAQ::ChannelMap::Detector& getChannelMap() const = 0 ;
  
  /// Retrieve the channel map for given bank version.
  virtual const OTDAQ::ChannelMap::Detector& getChannelMap(int bankversion) const = 0 ;
  
  /// Set the bank version
  virtual void setBankVersion(int bankversion) const = 0 ;
};


// Inline functions

inline unsigned int IOTChannelMapTool::straw(unsigned int station, unsigned int layer, unsigned int quadrant,
					     unsigned int module, unsigned int channel) const 
{
  return getChannelMap().module(station,layer,quadrant,module).straw(channel) ;
}

inline unsigned int IOTChannelMapTool::channel(unsigned int station, unsigned int layer, unsigned int quadrant,
					       unsigned int module, unsigned int straw) const 
{
  return getChannelMap().module(station,layer,quadrant,module).channel(straw) ;
}

inline unsigned int IOTChannelMapTool::channel(const LHCb::OTChannelID& otid) const 
{
  return channel(otid.station(),otid.layer(),otid.quarter(),otid.module(),otid.straw()) ;
}


#endif // OTDAQ_IOTCHANNELMAPTOOL_H
