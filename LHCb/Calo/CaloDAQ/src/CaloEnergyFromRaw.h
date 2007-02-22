// $Id: CaloEnergyFromRaw.h,v 1.7 2007-02-22 23:39:52 odescham Exp $
#ifndef CALOENERGYFROMRAW_H 
#define CALOENERGYFROMRAW_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "CaloDAQ/ICaloEnergyFromRaw.h"            // Interface
#include "CaloDAQ/CaloReadoutTool.h"

/** @class CaloEnergyFromRaw CaloEnergyFromRaw.h
 *  Decode the calorimeter energies, accodrding to version
 *
 *  @author Olivier Callot
 *  @date   2005-01-10
 */
class CaloEnergyFromRaw : public CaloReadoutTool, virtual public ICaloEnergyFromRaw {
public: 
  /// Standard constructor
  CaloEnergyFromRaw( const std::string& type, 
                     const std::string& name,
                     const IInterface* parent);

  virtual ~CaloEnergyFromRaw( ); ///< Destructor

  virtual StatusCode initialize();

  virtual std::vector<LHCb::CaloAdc>& adcs( );// decode all banks
  virtual std::vector<LHCb::CaloAdc>& adcs( LHCb::RawBank* bank ); // decode a single bank
  virtual std::vector<LHCb::CaloAdc>& adcs( int source ); // decode a single Tell1 bank
  virtual std::vector<LHCb::CaloAdc>& pinAdcs( );
  //
  virtual std::vector<LHCb::CaloDigit>& digits( ); // decode all banks
  virtual std::vector<LHCb::CaloDigit>& digits( LHCb::RawBank* bank );// decode a single bank
  virtual std::vector<LHCb::CaloDigit>& digits(  int source );// decode a single Tell1 bank
  
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
  virtual StatusCode getData ( LHCb::RawBank* bank );
  StatusCode getDigits ();

private:
  double   m_pedShift;
  unsigned int      m_pinArea;
  std::vector<LHCb::CaloAdc>    m_pinData;
  std::vector<LHCb::CaloAdc>    m_data;
  std::vector<LHCb::CaloDigit>  m_digits;
};
#endif // CALOENERGYFROMRAW_H
