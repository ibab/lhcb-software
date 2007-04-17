// $Id:
#ifndef CALODATAPROVIDERPATCHED_H 
#define CALODATAPROVIDERPATCHED_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "CaloDAQ/ICaloDataProvider.h"            // Interface
#include "CaloDAQ/CaloReadoutTool.h"

/** @class CaloDataProviderPatched CaloDataProviderPatched.h
 *  Fast access to calorimeter data from raw
 *
 * PATCHED version for DAQ test in  Bat156 (April 2007)
 *
 *  @author Olivier Deschamps
 *  @date   2007-02-27
 */
class CaloDataProviderPatched : public CaloReadoutTool, virtual public ICaloDataProvider {
public: 
  /// Standard constructor
  CaloDataProviderPatched( const std::string& type, 
                     const std::string& name,
                     const IInterface* parent);

  virtual ~CaloDataProviderPatched( ); ///< Destructor

  virtual StatusCode initialize();
  virtual StatusCode setBank();
  virtual void clear();
  virtual int    adc(LHCb::CaloCellID id);
  virtual double digit(LHCb::CaloCellID id);
  virtual unsigned int nTell1s(){return m_tell1s;};
  virtual CaloVector<LHCb::CaloAdc>& adcs(int source=-1);
  virtual CaloVector<LHCb::CaloDigit>& digits(int source=-1);
  virtual std::string rawRoot();
  
protected:
  StatusCode decodeCell(LHCb::CaloCellID id);
  StatusCode decodeTell1(int tell1);
  StatusCode decodeBank(LHCb::RawBank* bank);
  StatusCode decodePrsTriggerBank(LHCb::RawBank* bank);
private:
  double   m_pedShift;
  CaloVector<LHCb::CaloAdc>    m_adcs;
  CaloVector<LHCb::CaloDigit> m_digits;
  unsigned int m_tell1s;
  bool m_opg;
};
#endif // CALODATAPROVIDERPATCHED_H
