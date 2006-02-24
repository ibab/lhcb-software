// $Id: CaloTriggerBitsFromRaw.h,v 1.3 2006-02-24 07:35:01 ocallot Exp $
#ifndef CALOTRIGGERBITSFROMRAW_H 
#define CALOTRIGGERBITSFROMRAW_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "CaloDAQ/ICaloTriggerBitsFromRaw.h"            // Interface

#include "CaloDAQ/CaloReadoutTool.h"
#include "Event/RawEvent.h"

/** @class CaloTriggerBitsFromRaw CaloTriggerBitsFromRaw.h
 *  Decode the PRS bits. 
 *
 *  @author Olivier Callot
 *  @date   2005-01-06
 */
class CaloTriggerBitsFromRaw : public GaudiTool, virtual public ICaloTriggerBitsFromRaw {
public: 
  /// Standard constructor
  CaloTriggerBitsFromRaw( const std::string& type, 
                          const std::string& name,
                          const IInterface* parent);

  virtual ~CaloTriggerBitsFromRaw( ); ///< Destructor

  virtual StatusCode initialize();

  virtual std::vector<LHCb::CaloCellID>& firedCells( bool isPrs );
  
protected:

private:
  CaloReadoutTool* m_roTool;
  std::vector<LHCb::CaloCellID> m_prsCells;
  std::vector<LHCb::CaloCellID> m_spdCells;
};
#endif // CALOTRIGGERBITSFROMRAW_H
