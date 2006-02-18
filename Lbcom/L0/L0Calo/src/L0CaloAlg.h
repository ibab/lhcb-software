#ifndef   L0CALO_L0CALOALG_H
#define   L0CALO_L0CALOALG_H  1
// $Id: L0CaloAlg.h,v 1.16 2006-02-18 11:20:41 ocallot Exp $

// from Gaudi 
#include "GaudiAlg/GaudiAlgorithm.h"
#include "Kernel/Point3DTypes.h"
// from Calo
#include "CaloDet/DeCalorimeter.h"

// from CaloDAQ
#include "CaloDAQ/ICaloTriggerAdcsFromRaw.h"
#include "CaloDAQ/ICaloTriggerBitsFromRaw.h"

// from DAQEvent
#include "Event/RawEvent.h"
#include "Event/L0ProcessorData.h"

// Local classes
#include "TriggerCard.h"

/** @class L0Candidate
 *
 *  This is a local class for temporary candidates, ID and Et, with saturation.
 *  This is internal to the algorithm, and is a termporary storage of the 
 *  candidates, during the process of selecting the highest.
 */

class L0Candidate {
public:
  /** creator
   * @param det : The detector element pointer
   * @param scale : et scale
   */
  L0Candidate( DeCalorimeter* det ) : m_ID() {
    m_det     = det ;
    m_et      = 0   ;
  };

  ~L0Candidate()                           { };
  
  /** update the information on a candidate
   *  @param et Integer transverse energy
   *  @param ID LHCb::CaloCellID location of the candidate 
   */
  void        setCandidate( int et, LHCb::CaloCellID ID ) ;

  /** Save the candidate in the output container as L0ProcessorData
   *  @param type : Type of candidate, from L0DUFiber
   *  @param L0Calo : Container of candidates, to which the current object
   *                  is added after being properly formatted.
   */
  void  saveCandidate( L0DUBase::Fiber::Type type, LHCb::L0ProcessorDatas* L0Calo ) {
    unsigned int word = ( 0x10000 | 
                          (m_ID.all() << L0DUBase::Calo::Address::Shift) | 
                          (m_et << L0DUBase::Calo::Et::Shift)      );  // According to EDMS 525625      
    LHCb::L0ProcessorData* temp = new LHCb::L0ProcessorData ( type, word );
    L0Calo->add( temp );
  }
  int              et( )        const { return m_et     ; };
  LHCb::CaloCellID ID( )        const { return m_ID     ; };

 private:
  int              m_et     ;
  LHCb::CaloCellID m_ID     ;
  DeCalorimeter*   m_det    ;
};


/** @class L0CaloAlg L0CaloAlg.h
 *
 *  Algorithm responsible of produciong the L0 Calorimeter information.
 *
 *  @author  Olivier Callot
 *  @date    4 October 2000
 */ 

class L0CaloAlg : public GaudiAlgorithm {

public:

/// standard algorithm Constructor

  L0CaloAlg(const std::string& name, ISvcLocator* pSvcLocator);


  virtual ~L0CaloAlg();       ///< Standard destructor

  StatusCode initialize();    ///< Algorithm initialization
  StatusCode execute   ();    ///< Algorithm execution
  StatusCode finalize  ();    ///< Algorithm finalization
  
protected:

  void sumEcalData( );  ///< process Ecal FE card.
  void sumHcalData( );  ///< process Hcal FE card.
  void addPrsData(  );  ///< process the Prs information
  void addSpdData(  );  ///< Produce the Spd data

  void saveInRawEvent( int, L0Candidate& ); ///< Save in Raw Event.
  
private:

  std::string m_nameOfOutputDataContainer ;   ///< of the output container.
  double      m_etScale;

// Local variables

  DeCalorimeter* m_ecal            ; ///< Pointer to Ecal detector element
  DeCalorimeter* m_hcal            ; ///< Pointer to Hcal detector element
  int         m_validPrs[16]       ; ///< Array for PRS validation: 1 or 2 bits
  int         m_spdMult            ; ///< Multiplicity of Spd

  ICaloTriggerAdcsFromRaw* m_adcsEcal; ///< Tool to decode trigger adcs.
  ICaloTriggerAdcsFromRaw* m_adcsHcal; ///< Tool to decode trigger adcs.
  ICaloTriggerBitsFromRaw* m_bitsFromRaw; ///< Tool to decode trigger bits.
  
// Trigger cards

  std::vector<TriggerCard> ecalFe  ; ///< Ecal front-end card vector
  std::vector<TriggerCard> hcalFe  ; ///< Hcal front-end card vector

  // Validation
  int         m_nbValidation;

  std::vector<unsigned int> m_rawOutput; ///< RAW output bank

  bool m_storeFlag;

  double m_totRawSize;
  int m_nbEvents;  
};
#endif //   L0CALO_L0CALOALG_H
