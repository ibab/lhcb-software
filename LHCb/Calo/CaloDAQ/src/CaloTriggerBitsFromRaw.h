// $Id: CaloTriggerBitsFromRaw.h,v 1.5 2007-02-22 23:39:52 odescham Exp $
#ifndef CALOTRIGGERBITSFROMRAW_H 
#define CALOTRIGGERBITSFROMRAW_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "CaloDAQ/ICaloTriggerBitsFromRaw.h"            // Interface
#include "CaloDAQ/CaloReadoutTool.h"


/** @class CaloTriggerBitsFromRaw CaloTriggerBitsFromRaw.h
 *  Decode the PRS bits. 
 *
 *  @author Olivier Callot
 *  @date   2005-01-06
 */
class CaloTriggerBitsFromRaw : public CaloReadoutTool, virtual public ICaloTriggerBitsFromRaw {
public: 
  /// Standard constructor
  CaloTriggerBitsFromRaw( const std::string& type, 
                          const std::string& name,
                          const IInterface* parent);

  virtual ~CaloTriggerBitsFromRaw( ); ///< Destructor

  virtual StatusCode initialize();

  virtual LHCb::Calo::FiredCells& prsCells( ); // get prs FiredCells
  virtual LHCb::Calo::FiredCells& spdCells( ); // get spd FiredCells
  virtual LHCb::Calo::PrsSpdFiredCells& prsSpdCells( ); // get all FiredCells
  virtual LHCb::Calo::PrsSpdFiredCells& prsSpdCells(int source ); // get FiredCells for a single bank
  virtual LHCb::Calo::PrsSpdFiredCells& prsSpdCells( LHCb::RawBank* bank ); // get FiredCells for a single bank
  // Useful method  to setup m_banks externally only once
  // Avoid call to getCaloBanksFromRaw() at each call of adc(bank)
  virtual StatusCode getCaloBanks(){
    m_getRaw = false;
    return getCaloBanksFromRaw();
  };
  virtual void setBanks(const std::vector<LHCb::RawBank*>* bank ){
    m_getRaw = false;
    m_banks = bank;
  };
  
protected:
  StatusCode getData( LHCb::RawBank* bank );
private:
  LHCb::Calo::PrsSpdFiredCells m_data;  
};
#endif // CALOTRIGGERBITSFROMRAW_H
