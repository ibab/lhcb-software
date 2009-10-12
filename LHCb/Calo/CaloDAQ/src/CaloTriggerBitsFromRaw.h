// $Id: CaloTriggerBitsFromRaw.h,v 1.9 2009-10-12 16:03:54 odescham Exp $
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

  const LHCb::Calo::FiredCells& prsCells( ); // get prs FiredCells
  const LHCb::Calo::FiredCells& spdCells( ); // get spd FiredCells
  const  LHCb::Calo::PrsSpdFiredCells& prsSpdCells( ); // get all FiredCells
  const  LHCb::Calo::PrsSpdFiredCells& prsSpdCells(int source ); // get FiredCells for a single bank
  const  LHCb::Calo::PrsSpdFiredCells& prsSpdCells( LHCb::RawBank* bank ); // get FiredCells for a single bank
  void clear();
  void cleanData(int feb);
  
protected:
  bool getData( LHCb::RawBank* bank );
private:
  LHCb::Calo::PrsSpdFiredCells m_data;  
};
#endif // CALOTRIGGERBITSFROMRAW_H
