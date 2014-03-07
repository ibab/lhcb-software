#ifndef   LLTCALO_L0CALOALG_H
#define   LLTCALO_L0CALOALG_H  1

// from Gaudi 
#include "GaudiAlg/GaudiAlgorithm.h"

// Local classes
#include "LLTTriggerCard.h"
#include "L0Candidate.h"

// forward declarations
class DeCalorimeter ;
class ICaloTriggerAdcsFromRaw ;

/** @class LLTCaloAlg LLTCaloAlg.h
 *
 *  Algorithm responsible of producing the LLT Calorimeter information.
 *
 *  @author  Patrick Robbe
 *  @date    27 September 2013
 */ 

class LLTCaloAlg : public GaudiAlgorithm {
public:  
  /// standard algorithm Constructor
  LLTCaloAlg( const std::string& name , ISvcLocator* pSvcLocator ) ;
  
  virtual ~LLTCaloAlg() ; ///< Standard destructor

  virtual StatusCode initialize(); ///< Algorithm initialization
  virtual StatusCode execute   (); ///< Algorithm execution
  virtual StatusCode finalize  (); ///< Algorithm finalization
  
protected:
  unsigned int sumEcalData( );  ///< process Ecal FE card.
  unsigned int sumHcalData( );  ///< process Hcal FE card.

private:
  // raw bank container
  std::vector< std::vector< unsigned int > > m_rawOutput ;

  unsigned int m_ECALThreshold ;  ///< Threshold to count multiplicity in ECAL
  unsigned int m_HCALThreshold ;  ///< Threshold to count multiplicity in HCAL
  
  // Local variables
  DeCalorimeter *    m_ecal               ; ///< Pointer to Ecal detector element
  DeCalorimeter *    m_hcal               ; ///< Pointer to Hcal detector element

  ICaloTriggerAdcsFromRaw * m_adcsEcal ; ///< Tool to decode trigger adcs.
  ICaloTriggerAdcsFromRaw * m_adcsHcal ; ///< Tool to decode trigger adcs.

  // Trigger cards
  std::vector< LLTTriggerCard > m_ecalFe ; ///< Ecal front-end card vector
  std::vector< LLTTriggerCard > m_hcalFe ; ///< Hcal front-end card vector

  // Name of the ADC tool
  std::string m_caloToolName ;

  double m_etScale ;
  bool   m_doubleScale ;
};
#endif //   L0CALO_LLTCALOALG_H
