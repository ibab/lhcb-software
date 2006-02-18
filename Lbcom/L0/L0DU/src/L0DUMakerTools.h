// $Id: L0DUMakerTools.h,v 1.1 2006-02-18 11:29:37 ocallot Exp $
// ============================================================================
// CVS tag $Name: not supported by cvs2svn $
// ============================================================================
// $Log: not supported by cvs2svn $
//
//
// ============================================================================
#ifndef L0DU_L0DUMAKERTOOLS_H 
#define L0DU_L0DUMAKERTOOLS_H 1

// from STL
#include <string>
#include <utility>
#include <vector>
// from GaudiKernel
#include "GaudiAlg/GaudiTool.h"
// Local
#include "Event/L0DUChannel.h"

static const InterfaceID IID_L0DUMakerTools( "L0DUMakerTools", 1, 0 );
class L0DUMakerTools : public GaudiTool {

 public:
  L0DUMakerTools ( const std::string& type   , 
                   const std::string& name   ,
                   const IInterface*  parent ) ;

  virtual ~L0DUMakerTools () ;

  static const InterfaceID& interfaceID() { return IID_L0DUMakerTools; }


  StatusCode makeData(std::string DataName ,
                      LHCb::L0DUElementaryData::Map& Datas);
  
  StatusCode makeCondition(std::string ConditionName,
                           LHCb::L0DUElementaryData::Map& DataContainer, 
                           LHCb::L0DUElementaryCondition::Map& Conditions);

  StatusCode makeChannel (const std::string Channelname,
                          LHCb::L0DUElementaryCondition::Map&  ConditionsContainer,
                          LHCb::L0DUChannel::Map& Channels);
  
  
private:

  std::string m_operator;
  std::vector<std::string> m_operands;

  std::string m_data;
  std::string m_comparator;
  double m_threshold;
  
  std::vector<std::string> m_conditions;
  unsigned int m_dsrate ;
};

// ============================================================================
// The END 
// ============================================================================
#endif // L0DU_L0DUMAKERTOOLS_H 
