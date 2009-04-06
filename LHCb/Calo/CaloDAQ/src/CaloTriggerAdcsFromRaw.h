// $Id: CaloTriggerAdcsFromRaw.h,v 1.7 2009-04-06 15:45:03 odescham Exp $
#ifndef CALOTRIGGERADCSFROMRAW_H 
#define CALOTRIGGERADCSFROMRAW_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "CaloDAQ/ICaloTriggerAdcsFromRaw.h"            // Interface
#include "CaloReadoutTool.h"

/** @class CaloTriggerAdcsFromRaw CaloTriggerAdcsFromRaw.h
 *  
 *
 *  @author Olivier Callot
 *  @date   2005-01-05
 */
class CaloTriggerAdcsFromRaw : public CaloReadoutTool, virtual public ICaloTriggerAdcsFromRaw {
public: 
  /// Standard constructor
  CaloTriggerAdcsFromRaw( const std::string& type, 
                      const std::string& name,
                      const IInterface* parent);

  virtual ~CaloTriggerAdcsFromRaw( ); ///< Destructor

  virtual StatusCode initialize();

  virtual std::vector<LHCb::L0CaloAdc>& adcs( );
  virtual std::vector<LHCb::L0CaloAdc>& adcs( int source );
  virtual std::vector<LHCb::L0CaloAdc>& adcs( LHCb::RawBank* bank);
  virtual std::vector<LHCb::L0CaloAdc>& pinAdcs( );
  virtual void clear();
  virtual void cleanData(int feb);

protected:
  bool getData ( LHCb::RawBank* bank );
private:
  std::vector<LHCb::L0CaloAdc> m_data;
  std::vector<LHCb::L0CaloAdc> m_pinData;
};
#endif // CALOTRIGGERADCSFROMRAW_H
