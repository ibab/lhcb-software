// $Id:
#ifndef CALOL0DATAPROVIDER_H 
#define CALOL0DATAPROVIDER_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "CaloDAQ/ICaloL0DataProvider.h"            // Interface
#include "CaloReadoutTool.h"

/** @class CaloL0DataProvider CaloL0DataProvider.h
 *  Fast access to calorimeter L0 data from raw
 *
 *  @author Olivier Deschamps
 *  @date   2007-12-03
 */
class CaloL0DataProvider : public CaloReadoutTool, virtual public ICaloL0DataProvider {

public: 
  /// Standard constructor
  CaloL0DataProvider( const std::string& type, 
                     const std::string& name,
                     const IInterface* parent);

  virtual ~CaloL0DataProvider( ); ///< Destructor

  virtual StatusCode initialize();
  virtual StatusCode finalize();
   void clear();
   void cleanData(int feb);
  int    l0Adc(LHCb::CaloCellID id,int def=0);
  unsigned int nTell1s(){return m_tell1s;};
  const CaloVector<LHCb::L0CaloAdc>& l0Adcs(int source=-1,bool clean=true);
  const CaloVector<LHCb::L0CaloAdc>& l0Adcs(std::vector<int> sources,bool clean=true);
  



protected:
  bool decodeCell(LHCb::CaloCellID id);
  bool decodeTell1(int tell1);
  bool decodeBank(LHCb::RawBank* bank);
  bool decodePrsTriggerBank(LHCb::RawBank* bank);
private:
  CaloVector<LHCb::L0CaloAdc>    m_adcs;
  unsigned int m_tell1s;
};
#endif // CALOL0DATAPROVIDER_H
