// $Id: CaloSpdBitFromRaw.h,v 1.1.1.1 2005-01-11 07:51:47 ocallot Exp $
#ifndef CALOSPDBITFROMRAW_H 
#define CALOSPDBITFROMRAW_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "CaloDAQ/ICaloTriggerFromRaw.h"            // Interface

#include "Event/RawEvent.h"

/** @class CaloSpdBitFromRaw CaloSpdBitFromRaw.h
 *  
 *
 *  @author Olivier Callot
 *  @date   2005-01-06
 */
class CaloSpdBitFromRaw : public GaudiTool, virtual public ICaloTriggerFromRaw {
public: 
  /// Standard constructor
  CaloSpdBitFromRaw( const std::string& type, 
                     const std::string& name,
                     const IInterface* parent);

  virtual ~CaloSpdBitFromRaw( ); ///< Destructor

  virtual void prepare( int type );

  virtual StatusCode nextCell( CaloCellID& id, int& adc );

protected:

private:
  std::vector<RawBank> m_banks;
  std::vector<RawBank>::const_iterator m_itB;
  int m_dataSize;
  int m_lastData;
  int m_lastID;
  raw_int* m_data;
};
#endif // CALOSPDBITFROMRAW_H
