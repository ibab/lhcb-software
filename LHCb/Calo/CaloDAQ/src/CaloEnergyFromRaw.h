// $Id: CaloEnergyFromRaw.h,v 1.5 2005-12-19 19:29:14 ocallot Exp $
#ifndef CALOENERGYFROMRAW_H 
#define CALOENERGYFROMRAW_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "CaloDAQ/ICaloEnergyFromRaw.h"            // Interface
#include "CaloDet/DeCalorimeter.h"
#include "CaloDAQ/CaloReadoutTool.h"

/** @class CaloEnergyFromRaw CaloEnergyFromRaw.h
 *  Decode the calorimeter energies, accodrding to version
 *
 *  @author Olivier Callot
 *  @date   2005-01-10
 */
class CaloEnergyFromRaw : public GaudiTool, virtual public ICaloEnergyFromRaw {
public: 
  /// Standard constructor
  CaloEnergyFromRaw( const std::string& type, 
                     const std::string& name,
                     const IInterface* parent);

  virtual ~CaloEnergyFromRaw( ); ///< Destructor

  virtual StatusCode initialize();

  virtual std::vector<LHCb::CaloAdc>& adcs( );
  
  virtual std::vector<LHCb::CaloDigit>& digits( );

protected:

private:
  std::string      m_detectorName;
  DeCalorimeter*   m_calo;
  CaloReadoutTool* m_roTool;
  double   m_pedShift;
  LHCb::RawBank::BankType m_packedType;
  LHCb::RawBank::BankType m_shortType;

  std::vector<LHCb::CaloAdc>    m_adcs;
  std::vector<LHCb::CaloDigit>  m_digits;
};
#endif // CALOENERGYFROMRAW_H
