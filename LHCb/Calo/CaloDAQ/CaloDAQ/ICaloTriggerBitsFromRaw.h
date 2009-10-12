// $Id: ICaloTriggerBitsFromRaw.h,v 1.5 2009-10-12 16:03:53 odescham Exp $
#ifndef CALODAQ_ICALOTRIGGERBITSFROMRAW_H 
#define CALODAQ_ICALOTRIGGERBITSFROMRAW_H 1

// Include files
// from STL
#include <string>

#include "ICaloReadoutTool.h"
#include "Kernel/CaloCellID.h"

static const InterfaceID IID_ICaloTriggerBitsFromRaw ( "ICaloTriggerBitsFromRaw", 3, 0 );

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

class ICaloTriggerBitsFromRaw : virtual public ICaloReadoutTool {
public: 

  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_ICaloTriggerBitsFromRaw; }
  virtual const LHCb::Calo::FiredCells& prsCells( ) = 0;
  virtual const LHCb::Calo::FiredCells& spdCells( ) = 0;
  virtual const LHCb::Calo::PrsSpdFiredCells& prsSpdCells( ) = 0;
  virtual const LHCb::Calo::PrsSpdFiredCells& prsSpdCells(int source ) = 0;
  virtual const LHCb::Calo::PrsSpdFiredCells& prsSpdCells(LHCb::RawBank* bank ) = 0;

protected:

private:

};
#endif // CALODAQ_ICALOTRIGGERBITSFROMRAW_H
