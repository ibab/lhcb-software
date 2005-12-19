// $Id: CaloTriggerBitsFromRaw.h,v 1.2 2005-12-19 19:29:14 ocallot Exp $
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

  bool eventHasChanged();

private:
  int      m_runNum;
  int      m_evtNum;
  CaloReadoutTool* m_roTool;
  std::vector<LHCb::CaloCellID> m_prsCells;
  std::vector<LHCb::CaloCellID> m_spdCells;
};
#endif // CALOTRIGGERBITSFROMRAW_H
