// $Id: CaloPrsBitFromRaw.h,v 1.2 2005-05-09 06:38:53 ocallot Exp $
#ifndef CALOPRSBITFROMRAW_H 
#define CALOPRSBITFROMRAW_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "CaloDAQ/ICaloTriggerFromRaw.h"            // Interface

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
};
#endif // CALOPRSBITFROMRAW_H
