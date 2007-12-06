// $Id:
#ifndef CALODATAPROVIDER_H 
#define CALODATAPROVIDER_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "CaloDAQ/ICaloDataProvider.h"            // Interface
#include "CaloReadoutTool.h"

/** @class CaloDataProvider CaloDataProvider.h
 *  Fast access to calorimeter data from raw
 *
 *  @author Olivier Deschamps
 *  @date   2007-02-27
 */
class CaloDataProvider : public CaloReadoutTool, virtual public ICaloDataProvider {

public: 
  /// Standard constructor
  CaloDataProvider( const std::string& type, 
                     const std::string& name,
                     const IInterface* parent);

  virtual ~CaloDataProvider( ); ///< Destructor

  virtual StatusCode initialize();
  virtual void clear();
  virtual void cleanData(int feb);
  virtual int    adc(LHCb::CaloCellID id);
  virtual double digit(LHCb::CaloCellID id);
  virtual unsigned int nTell1s(){return m_tell1s;};
  virtual CaloVector<LHCb::CaloAdc>& adcs(int source=-1);
  virtual CaloVector<LHCb::CaloDigit>& digits(int source=-1); 
  



protected:
  bool decodeCell(LHCb::CaloCellID id);
  bool decodeTell1(int tell1);
  bool decodeBank(LHCb::RawBank* bank);
  bool decodePrsTriggerBank(LHCb::RawBank* bank);
private:
  double   m_pedShift;
  CaloVector<LHCb::CaloAdc>    m_adcs;
  CaloVector<LHCb::CaloDigit> m_digits;
  unsigned int m_tell1s;
};
#endif // CALODATAPROVIDER_H
