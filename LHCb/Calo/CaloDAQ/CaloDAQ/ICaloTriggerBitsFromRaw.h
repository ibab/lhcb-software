// $Id: ICaloTriggerBitsFromRaw.h,v 1.3 2007-02-22 23:39:52 odescham Exp $
#ifndef CALODAQ_ICALOTRIGGERBITSFROMRAW_H 
#define CALODAQ_ICALOTRIGGERBITSFROMRAW_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/IAlgTool.h"
#include "Kernel/CaloCellID.h"
#include "Event/RawEvent.h"

static const InterfaceID IID_ICaloTriggerBitsFromRaw ( "ICaloTriggerBitsFromRaw", 1, 0 );

/** @class ICaloTriggerBitsFromRaw ICaloTriggerBitsFromRaw.h CaloDAQ/ICaloTriggerBitsFromRaw.h
 *  Interface to the Prs/SPD bit decoding
 *
 *  @author Olivier Callot
 *  @date   2005-11-07
 */
namespace LHCb{
  namespace Calo{
  typedef std::vector<LHCb::CaloCellID> FiredCells;
  typedef std::pair<FiredCells,FiredCells> PrsSpdFiredCells;
  }
}

class ICaloTriggerBitsFromRaw : virtual public IAlgTool {
public: 

  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_ICaloTriggerBitsFromRaw; }
  virtual LHCb::Calo::FiredCells& prsCells( ) = 0;
  virtual LHCb::Calo::FiredCells& spdCells( ) = 0;
  virtual LHCb::Calo::PrsSpdFiredCells& prsSpdCells( ) = 0;
  virtual LHCb::Calo::PrsSpdFiredCells& prsSpdCells(int source ) = 0;
  virtual LHCb::Calo::PrsSpdFiredCells& prsSpdCells(LHCb::RawBank* bank ) = 0;
  //
  virtual StatusCode  getCaloBanks()=0;
  virtual void setBanks(const std::vector<LHCb::RawBank*>* bank ) = 0;

protected:

private:

};
#endif // CALODAQ_ICALOTRIGGERBITSFROMRAW_H
