// $Id: CaloEnergyFromRaw.h,v 1.4 2005-11-10 16:43:22 ocallot Exp $
#ifndef CALOENERGYFROMRAW_H 
#define CALOENERGYFROMRAW_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "CaloDAQ/ICaloEnergyFromRaw.h"            // Interface
#include "CaloDet/DeCalorimeter.h"
#include "Event/RawEvent.h"
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

  virtual std::vector<CaloAdc>& adcs( );
  
  virtual std::vector<CaloDigit>& digits( );

protected:

private:
  std::string      m_detectorName;
  DeCalorimeter*   m_calo;
  CaloReadoutTool* m_roTool;
  double   m_pedShift;
  int      m_packedType;
  int      m_shortType;

  std::vector<CaloAdc>    m_adcs;
  std::vector<CaloDigit>  m_digits;
};
#endif // CALOENERGYFROMRAW_H
