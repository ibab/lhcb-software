// $Id: CaloPrsBitFromRaw.h,v 1.3 2005-09-06 14:50:01 ocallot Exp $
#ifndef CALOPRSBITFROMRAW_H 
#define CALOPRSBITFROMRAW_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "CaloDAQ/ICaloTriggerFromRaw.h"            // Interface

#include "CaloDAQ/CaloReadoutTool.h"
#include "Event/RawEvent.h"

/** @class CaloPrsBitFromRaw CaloPrsBitFromRaw.h
 *  Decode the PRS bits. 
 *
 *  @author Olivier Callot
 *  @date   2005-01-06
 */
class CaloPrsBitFromRaw : public GaudiTool, virtual public ICaloTriggerFromRaw {
public: 
  /// Standard constructor
  CaloPrsBitFromRaw( const std::string& type, 
                     const std::string& name,
                     const IInterface* parent);

  virtual ~CaloPrsBitFromRaw( ); ///< Destructor

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
  CaloReadoutTool* m_roTool;
  std::vector<CaloCellID> m_chanID;
  int m_version;
  int m_sourceID;
  int m_offset;
  int m_cardNum;
  int m_lenAdc;
  int m_lenTrig;
  std::vector<CaloCellID> m_results;
  std::vector<CaloCellID>::const_iterator m_resultsIt;
};
#endif // CALOPRSBITFROMRAW_H
