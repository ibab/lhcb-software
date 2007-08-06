// $Id: CaloTriggerAdcsFromRaw.h,v 1.5 2007-08-06 21:31:48 odescham Exp $
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

protected:
  bool getData ( LHCb::RawBank* bank );
private:
  std::vector<LHCb::L0CaloAdc> m_data;
  std::vector<LHCb::L0CaloAdc> m_pinData;
  unsigned int m_pinArea;
};
#endif // CALOTRIGGERADCSFROMRAW_H
