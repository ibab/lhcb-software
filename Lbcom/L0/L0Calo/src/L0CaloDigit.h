// $Id: L0CaloDigit.h,v 1.1 2002-01-30 15:58:20 ocallot Exp $
#ifndef L0CALODIGIT_H 
#define L0CALODIGIT_H 1

// Include files
// from STL
#include <string>
#include <vector>

// from Gaudi
#include "GaudiKernel/Algorithm.h"

// from Calo
#include "CaloDet/DeCalorimeter.h"

/** @class L0CaloDigit L0CaloDigit.h
 *  Digitizes the data for the L0 Calo trigger. This should be part of the 
 *  Calorimeter digitization, as it should start from the ADC, not from the 
 *  digits... Later ?
 *
 *  @author Olivier Callot
 *  @date   23/08/2001
 */
class L0CaloDigit : public Algorithm {
public:
  /// Standard constructor
  L0CaloDigit( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~L0CaloDigit( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

private:
  std::string m_nameOfEcalDataContainer   ;  ///< ECAL input container
  std::string m_nameOfHcalDataContainer   ;  ///< HCAL input container  
  std::string m_nameOfPrsDataContainer    ;  ///< Prs input container  
  std::string m_nameOfSpdDataContainer    ;  ///< Spd input container  
  std::string m_nameOfOutputRoot          ;  ///< Output directory
  std::string m_nameOfGeometryRoot        ; ///< Name of the detector root.

  double      m_etScale            ; ///< Energy per bin in Et for trigger
  double      m_prsThreshold       ; ///< Energy threshold for Preshower
  double      m_gainCorrEcal[3]    ; ///< Gain correction in Ecal, per area
  double      m_gainCorrHcal[2]    ; ///< Gain correction in Hcal, per area

  DeCalorimeter* m_ecal            ; ///< Pointer to Ecal detector element
  DeCalorimeter* m_hcal            ; ///< Pointer to Hcal detector element

};
#endif // L0CALODIGIT_H
