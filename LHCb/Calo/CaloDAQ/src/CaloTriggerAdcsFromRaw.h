// $Id: CaloTriggerAdcsFromRaw.h,v 1.3 2006-09-26 12:42:03 odescham Exp $
#ifndef CALOTRIGGERADCSFROMRAW_H 
#define CALOTRIGGERADCSFROMRAW_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "CaloDAQ/ICaloTriggerAdcsFromRaw.h"            // Interface
#include "Event/RawEvent.h"
#include "CaloDAQ/CaloReadoutTool.h"

/** @class CaloTriggerAdcsFromRaw CaloTriggerAdcsFromRaw.h
 *  
 *
 *  @author Olivier Callot
 *  @date   2005-01-05
 */
class CaloTriggerAdcsFromRaw : public GaudiTool, virtual public ICaloTriggerAdcsFromRaw {
public: 
  /// Standard constructor
  CaloTriggerAdcsFromRaw( const std::string& type, 
                      const std::string& name,
                      const IInterface* parent);

  virtual ~CaloTriggerAdcsFromRaw( ); ///< Destructor

  virtual StatusCode initialize();

  virtual std::vector<LHCb::L0CaloAdc>& adcs( );

protected:

private:
  std::string            m_detectorName;
  bool m_packedIsDefault;  
  CaloReadoutTool*       m_roTool;
  LHCb::RawBank::BankType m_packedType;
  LHCb::RawBank::BankType m_shortType;
  std::vector<LHCb::L0CaloAdc> m_adcs;
};
#endif // CALOTRIGGERADCSFROMRAW_H
