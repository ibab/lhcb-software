// $Id: CaloTriggerFromRaw.h,v 1.1.1.1 2005-01-11 07:51:47 ocallot Exp $
#ifndef CALOTRIGGERFROMRAW_H 
#define CALOTRIGGERFROMRAW_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "CaloDAQ/ICaloTriggerFromRaw.h"            // Interface
#include "Event/RawEvent.h"

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
#endif // CALOTRIGGERFROMRAW_H
