// $Id: HltL0Filter.h,v 1.1 2008-05-21 12:28:34 graven Exp $
#ifndef HLTL0FILTER_H 
#define HLTL0FILTER_H 1

// Include files
// from Gaudi

#include "GaudiKernel/VectorMap.h"

#include "Event/L0DUReport.h"
#include "HltBase/HltAlgorithm.h"

/** @class HltL0Filter HltL0Filter.h
 *  
 *  functionality:
 *      give pass if the L0 has fired a given decision (controlled via options)
 *
 *  Options:
 *      L0ChannelNames: list of names of the L0 decision to accept
 *                      if no names are listed, all decision are accepted
 *
 *  @author Jose Angel Hernando Morata
 *  @date   2006-01-16
 */
class HltL0Filter : public HltAlgorithm {
public: 

  /// Standard constructor
  HltL0Filter( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~HltL0Filter( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution

private:
  typedef GaudiUtils::VectorMap<std::string,unsigned int> ChannelMap_t;
  typedef GaudiUtils::VectorMap<long, ChannelMap_t > TCKMap_t;

  ChannelMap_t getL0Map( const LHCb::L0DUChannel::Map& channels) const;

  std::string         m_l0Location;
  StringArrayProperty m_l0Channels;
  TCKMap_t            m_map;

};
#endif // HltL0Filter_H
