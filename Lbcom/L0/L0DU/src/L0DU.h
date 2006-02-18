// $Id: L0DU.h,v 1.1 2006-02-18 11:29:37 ocallot Exp $
#ifndef L0DU_H 
#define L0DU_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiAlgorithm.h"
//Event
#include "Event/RawEvent.h"
#include "Event/L0DUReport.h"
// Local
#include "L0DUMakerTools.h"
class IL0Candidate;

/** @class L0DU L0DU.h
 *  L0DU report
 *  
 *
 *  @author Olivier Deschamps
 *  @date   2005-01-28
 */



class L0DU : public GaudiAlgorithm {
public:
  /// Standard constructor
  L0DU( const std::string& name, ISvcLocator* pSvcLocator );
  virtual ~L0DU( ); ///< Destructor
  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

private:
  StatusCode dataProcessing();
  
  void predefinedData(std::string name);

  void dataSetup();

  StatusCode setDefault();

  StatusCode setDefaultCondition( std::string name , 
                            std::string data , 
                            std::string comparator , 
                            double threshold);

  StatusCode setDefaultChannel(std::string name , 
                         std::vector<std::string> conditions,
                         std::string addCondition, 
                         unsigned int acceptRate);

  void printOut();
  
  // I/O Locations  
  std::string m_CaloDataLocation;
  std::string m_MuonDataLocation;
  std::string m_PileUpDataLocation;
  std::string m_L0DUReportLocation;
  std::string m_L0DUChannelLocation;
  // Configuration 
  std::vector<std::string> m_data;
  std::vector<std::string> m_conditions;
  std::vector<std::string> m_channels;
  LHCb::L0DUChannels* m_channelsOnTES;
  
  // parameters
  bool m_ADCconvert;
  bool m_reconfigure;
  bool m_reparametrize; 
  bool m_writeOnTES;
  bool m_fillRaw;

  unsigned int m_rawSrcID;
  unsigned int m_rawVsn ;  
  LHCb::RawBank::BankType m_rawBankType;
  

  // Containers
  LHCb::L0DUChannel::Map             m_channelsCont;
  LHCb::L0DUElementaryData::Map      m_dataCont;
  LHCb::L0DUElementaryCondition::Map m_conditionsCont;
  std::map<int,std::string>   m_conditionsMap;
  std::map<int,std::string>   m_channelsMap;

  // L0 Candidates
  IL0Candidate* m_CaloCandidate;
  IL0Candidate* m_MuonCandidate;
  IL0Candidate* m_PileUpCandidate;
    
};
#endif // L0DU_H
