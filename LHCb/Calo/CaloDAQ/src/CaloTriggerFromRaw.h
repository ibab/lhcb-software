// $Id: CaloTriggerFromRaw.h,v 1.3 2005-09-06 14:50:01 ocallot Exp $
#ifndef CALOTRIGGERFROMRAW_H 
#define CALOTRIGGERFROMRAW_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "CaloDAQ/ICaloTriggerFromRaw.h"            // Interface
#include "Event/RawEvent.h"
#include "CaloDAQ/CaloReadoutTool.h"

/** @class CaloTriggerFromRaw CaloTriggerFromRaw.h
 *  
 *
 *  @author Olivier Callot
 *  @date   2005-01-05
 */
class CaloTriggerFromRaw : public GaudiTool, virtual public ICaloTriggerFromRaw {
public: 
  /// Standard constructor
  CaloTriggerFromRaw( const std::string& type, 
                      const std::string& name,
                      const IInterface* parent);

  virtual ~CaloTriggerFromRaw( ); ///< Destructor

  virtual StatusCode initialize();

  virtual void prepare( int type );
  
  virtual StatusCode nextCell( CaloCellID& id, int& adc );

protected:

private:
  const std::vector<RawBank>* m_banks;
  std::vector<RawBank>::const_iterator m_itB;
  int m_dataSize;
  int m_lastData;
  int m_lastID;
  raw_int* m_data;
  CaloReadoutTool* m_ecalRoTool;
  CaloReadoutTool* m_hcalRoTool;
  CaloReadoutTool* m_prsRoTool;
  CaloReadoutTool* m_roTool;
  std::vector<CaloCellID> m_chanID;
  int m_version;
  int m_sourceID;
  int m_offset;
  int m_bitNum;
  int m_cardNum;
  int m_lenAdc;
  int m_lenTrig;
  int m_pattern;
  std::vector<std::pair<CaloCellID, int> > m_results;
  std::vector<std::pair<CaloCellID, int> >::const_iterator m_resultsIt;
};
#endif // CALOTRIGGERFROMRAW_H
