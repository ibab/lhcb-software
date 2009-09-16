// $Id: CaloTriggerBitsFromRaw.h,v 1.8 2009-09-16 16:02:46 odescham Exp $
#ifndef CALOTRIGGERBITSFROMRAW_H 
#define CALOTRIGGERBITSFROMRAW_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "CaloDAQ/ICaloTriggerBitsFromRaw.h"            // Interface
#include "CaloReadoutTool.h"


/** @class CaloTriggerBitsFromRaw CaloTriggerBitsFromRaw.h
 *  Decode the PRS bits. 
 *
 *  @author Olivier Callot
 *  @date   2005-01-06
 */
class CaloTriggerBitsFromRaw : public CaloReadoutTool , virtual public ICaloTriggerBitsFromRaw {
public: 
  /// Standard constructor
  CaloTriggerBitsFromRaw( const std::string& type, 
                          const std::string& name,
                          const IInterface* parent);

  virtual ~CaloTriggerBitsFromRaw( ); ///< Destructor

  virtual StatusCode initialize();
  virtual StatusCode finalize();

  virtual LHCb::Calo::FiredCells& prsCells( ); // get prs FiredCells
  virtual LHCb::Calo::FiredCells& spdCells( ); // get spd FiredCells
  virtual LHCb::Calo::PrsSpdFiredCells& prsSpdCells( ); // get all FiredCells
  virtual LHCb::Calo::PrsSpdFiredCells& prsSpdCells(int source ); // get FiredCells for a single bank
  virtual LHCb::Calo::PrsSpdFiredCells& prsSpdCells( LHCb::RawBank* bank ); // get FiredCells for a single bank
  virtual void clear();
  virtual void cleanData(int feb);
  
protected:
  bool getData( LHCb::RawBank* bank );
private:
  LHCb::Calo::PrsSpdFiredCells m_data;  
};
#endif // CALOTRIGGERBITSFROMRAW_H
