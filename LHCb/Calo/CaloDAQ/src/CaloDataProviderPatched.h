// $Id:
#ifndef CALODATAPROVIDERPATCHED_H 
#define CALODATAPROVIDERPATCHED_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "CaloDAQ/ICaloDataProvider.h"            // Interface
#include "CaloReadoutTool.h"

/** @class CaloDataProviderPatched CaloDataProviderPatched.h
 *  Fast access to calorimeter data from raw
 *
 * PATCHED version for DAQ test in  Bat156 (April 2007)
 *
 *  @author Olivier Deschamps
 *  @date   2007-02-27
 */
class CaloDataProviderPatched : public CaloReadoutTool, public virtual ICaloDataProvider {
public: 
  /// Standard constructor
  CaloDataProviderPatched( const std::string& type, 
                     const std::string& name,
                     const IInterface* parent);

  virtual ~CaloDataProviderPatched( ); ///< Destructor

  virtual StatusCode initialize();
  virtual int    adc(LHCb::CaloCellID id);
  virtual double digit(LHCb::CaloCellID id);
  virtual unsigned int nTell1s(){return m_tell1s;};
  virtual CaloVector<LHCb::CaloAdc>& adcs(int source=-1);
  virtual CaloVector<LHCb::CaloDigit>& digits(int source=-1);
  virtual void clear();
  virtual void cleanData(int feb);
  
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
  bool m_opg;
};
#endif // CALODATAPROVIDERPATCHED_H
