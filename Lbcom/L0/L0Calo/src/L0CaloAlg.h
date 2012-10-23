#ifndef   L0CALO_L0CALOALG_H
#define   L0CALO_L0CALOALG_H  1

// from Gaudi 
#include "L0Base/L0AlgBase.h"

// Local classes
#include "TriggerCard.h"
#include "L0Candidate.h"

// From Calo
#include "CaloDAQ/ICaloTriggerBitsFromRaw.h"

// forward declarations
class DeCalorimeter ;
class ICaloTriggerAdcsFromRaw ;
class L0CaloCandidatesFromRawBank ;

/** @class L0CaloAlg L0CaloAlg.h
 *
 *  Algorithm responsible of producing the L0 Calorimeter information.
 *
 *  @author  Olivier Callot
 *  @author  Marie-Helene Schune
 *  @author  Patrick Robbe
 *  @date    4 October 2000
 */ 

class L0CaloAlg : public L0AlgBase {
public:  
  /// standard algorithm Constructor
  L0CaloAlg( const std::string& name , ISvcLocator* pSvcLocator ) ;
  
  virtual ~L0CaloAlg() ; ///< Standard destructor

  virtual StatusCode initialize(); ///< Algorithm initialization
  virtual StatusCode execute   (); ///< Algorithm execution
  virtual StatusCode finalize  (); ///< Algorithm finalization

  static int  s_ecalLUT[ 14 ][ 2  ] ; /// Temporary wait for CondDB
  static int  s_hcalLUT[ 4  ][ 16 ] ; /// Temporary wait for CondDB
  
protected:
  void sumEcalData( );  ///< process Ecal FE card.
  void sumHcalData( );  ///< process Hcal FE card.
  void addPrsData(  );  ///< process the Prs information
  void addSpdData(  );  ///< Produce the Spd data
  
  /** Save a L0Candidate in a RAW bank using the format
   *  defined in EDMS 845277
   *  @param[in] io    0 for input, 1 for output
   *  @param[in] slave 0 for all types except for hadron types
   *                   where 0 means master, 1 slave1 and 2 slave2
   *  @param[in] mask  0 for unmasked channels, 1 for masked channels
   *  @param[in] type  type of candidate (of type L0DUBase::CaloType)
   *  @param[in] cand  L0Candidate to save in raw bank
   *  @param[in] bank  bank number (0 for Hadron, 1 for ECAL+SPD)
   */
  void saveInRawEvent( int io , int slave , int mask , int type , 
                       L0Candidate & cand, unsigned int bank ) ;

  /** Save the SumEt in a RAW bank using the format
   *  defined in EDMS 845277
   *  @param[in] io    0 for input, 1 for output
   *  @param[in] slave 0 for all types except for hadron types
   *                   where 0 means master, 1 slave1 and 2 slave2
   *  @param[in] mask  0 for unmasked channels, 1 for masked channels
   *  @param[in] type  type of candidate (of type L0DUBase::CaloType)
   *  @param[in] sumEt sumEt value to save in raw bank
   *  @param[in] bank  bank number (0 for Hadron, 1 for ECAL+SPD)
   */
  void saveInRawEvent( int io , int slave , int mask , int type , 
                       int sumEt , unsigned int bank ) ;

  /** Save the SPD multiplicity in a RAW bank using the format
   *  defined in EDMS 845277
   *  @param[in] io    0 for input, 1 for output
   *  @param[in] slave 0 for all types except for hadron types
   *                   where 0 means master, 1 slave1 and 2 slave2
   *  @param[in] mask  0 for unmasked channels, 1 for masked channels
   *  @param[in] type  type of candidate (of type L0DUBase::CaloType)
   *  @param[in] cardNumber SPD Multiplicity board number
   *  @param[in] spdMult SPD Multiplicity value to save in raw bank
   *  @param[in] bank  bank number (0 for Hadron, 1 for ECAL+SPD)
   */  
  void saveInRawEvent( int io , int slave , int mask , int type , 
                       int cardNumber , int spdMult ,  unsigned int bank ) ;

private:
  bool        m_usePsSpd                  ;   ///< use Ps/Spd in validation
  bool        m_addEcalToHcal             ;   ///< Use ECAL in Validation

  bool        m_usePsSpdOpts              ;   ///< use Ps/Spd in validation (by options)
  bool        m_addEcalToHcalOpts         ;   ///< Use ECAL in Validation (by options)

  bool        m_createHCALLut             ;   ///< Create HCAL LUT Files for validation boards (Set by options)
  bool        m_newElectron               ;   ///< Use new electron definition in Inner ECAL

  int         m_hcalThreshold             ;   ///< Zero suppression threshold for hadron
  int         m_ecalThreshold             ;   ///< Zero suppression threshold for electromagnetic
  
  // Local variables
  DeCalorimeter *    m_ecal               ; ///< Pointer to Ecal detector element
  DeCalorimeter *    m_hcal               ; ///< Pointer to Hcal detector element
  DeCalorimeter *    m_prs                ; ///< Pointer to Prs detector element
  int                m_validPrs[16]       ; ///< Array for PRS validation: 1 or 2 bits
  int                m_validPrsInner[16]  ; ///< Array for PRS validation in the outer part.

  ICaloTriggerAdcsFromRaw * m_adcsEcal ; ///< Tool to decode trigger adcs.
  ICaloTriggerAdcsFromRaw * m_adcsHcal ; ///< Tool to decode trigger adcs.
  ICaloTriggerBitsFromRaw * m_bitsFromRaw ; ///< Tool to decode trigger bits.

  LHCb::Calo::PrsSpdFiredCells m_PrsSpdIds  ; ///< Fired cells
  L0CaloCandidatesFromRawBank * m_bankToTES ; ///< Tool to decode the raw banks

  // Trigger cards
  std::vector< TriggerCard > m_ecalFe ; ///< Ecal front-end card vector
  std::vector< TriggerCard > m_hcalFe ; ///< Hcal front-end card vector

  std::vector<std::vector<unsigned int> > m_rawOutput ; ///< RAW output bank

  double m_totRawSize;
  int    m_nbEvents;  
  
  int m_bankVersion ; ///< Version of the bank to simulate
  int m_nbValidation ; ///< Number of validation boards

  std::vector< int > m_spdMult            ; ///< Multiplicity of Spd

  Condition * m_l0Cond ; ///< Pointer to the L0 conditions 

  /// Call back function to check condition database content
  StatusCode updateL0Calibration() ;
  
  /// Utility function to create HCAL LUT for validation from geometry
  void createHCALLut( ) ;
};

namespace L0Calo {
  inline bool zeroEt( const L0Candidate& C ) {
    if ( C.et() == 0 ) return true ;
    return false ;
  }
  
  inline bool compareL0Candidate( const L0Candidate& A , 
                                  const L0Candidate& B ) {
    if ( B.et() > A.et() ) return true ;
    if ( B.et() < A.et() ) return false ;
    LHCb::CaloCellID caloCellA , caloCellB ;

    caloCellA = A.ID() ;
    caloCellB = B.ID() ;
    
    int cardA = A.det() -> cardNumber( caloCellA ) ;
    int cardB = B.det() -> cardNumber( caloCellB ) ;

    int cardSlotA = A.det() -> cardSlot( cardA ) ;
    int cardSlotB = B.det() -> cardSlot( cardB ) ;

    int crateNumberA = A.det() -> cardCrate( cardA ) ;
    int crateNumberB = B.det() -> cardCrate( cardB ) ;

    if ( 2 == caloCellA.calo() ) {  // ECAL
      int valBoardExtA( 0 ), valBoardExtB( 0 ) ;
      if ( cardSlotA >= 8 ) valBoardExtA = 1 ;
      if ( cardSlotB >= 8 ) valBoardExtB = 1 ;
      
      int inputNumberA = 
        L0CaloAlg::s_ecalLUT[ crateNumberA - 8 ][ valBoardExtA ] ;
      int inputNumberB = 
        L0CaloAlg::s_ecalLUT[ crateNumberB - 8 ][ valBoardExtB ] ;

      if ( inputNumberB <= inputNumberA ) return true ; 
    } else { // HCAL
      int inputNumberA = 
        L0CaloAlg::s_hcalLUT[ crateNumberA - 22 ][ cardSlotA ] ;
      int inputNumberB =
        L0CaloAlg::s_hcalLUT[ crateNumberB - 22 ][ cardSlotB ] ;      

      if ( inputNumberB <= inputNumberA ) return true ;
    }    

    return false ;
  }
}
#endif //   L0CALO_L0CALOALG_H
