#ifndef   L0CALO_L0CALOALG_H
#define   L0CALO_L0CALOALG_H  1
// $Id: L0CaloAlg.h,v 1.13 2005-11-10 16:45:38 ocallot Exp $

// from Gaudi 
#include "GaudiAlg/GaudiAlgorithm.h"

// from Calo
#include "CaloDet/DeCalorimeter.h"

// from CaloDAQ
#include "CaloDAQ/ICaloTriggerAdcsFromRaw.h"
#include "CaloDAQ/ICaloTriggerBitsFromRaw.h"

// from DAQEvent
#include "Event/RawEvent.h"

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
  L0Candidate( DeCalorimeter* det, double scale ) : m_ID() {
    m_det     = det ;
    m_et      = 0   ;
    m_etScale = scale ;
    m_tol     = 0   ;
  };

  ~L0Candidate()                           { };
  
  /** update the information on a candidate
   *  @param et Integer transverse energy
   *  @param ID CaloCellID location of the candidate 
   */
  void        setCandidate( int et, CaloCellID ID ) ;

  /** Save the candidate in the output container as L0CaloCandidate
   *  @param type : Type of candidate, from L0::L0Type
   *  @param L0Calo : Container of candidates, to which the current object
   *                  is added after being properly formatted.
   */
  void  saveCandidate( int type, L0CaloCandidates* L0Calo ) {
    if ( 0 < m_et ) {
      L0CaloCandidate* temp = new L0CaloCandidate ( type,
                                                    m_ID,
                                                    m_et,
                                                    m_et * m_etScale,
                                                    m_center,
                                                    m_tol );
      L0Calo->add( temp );
    }
  }
  int         et( )        const { return m_et     ; };
  HepPoint3D  center( )    const { return m_center ; };
  double      tolerance( ) const { return m_tol    ; };
  CaloCellID  ID( )        const { return m_ID     ; };

 private:
  int            m_et     ;
  double         m_etScale;
  CaloCellID     m_ID     ;
  DeCalorimeter* m_det    ;
  HepPoint3D     m_center ;
  double         m_tol    ;
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

  void saveInRawBuffer( int, L0Candidate& ); ///< Save in RAW buffers
  
private:

  std::string m_nameOfOutputDataContainer ;   ///< of the output container.
  std::string m_nameOfGeometryRoot ;          ///< Name of Detector  e.g. /dd/structure/LHCb/
  double      m_etScale;

// Pi0 trigger parameters

  int         m_pi0Strategy        ; ///< Defines how Pi0 are defined.
  double      m_minEtPi0Local      ; ///< Min cluster Et for local pi0
  double      m_minEtGammaGlobal   ; ///< Min cluster Et for gamma Global pi0
  double      m_minPi0Mass         ; ///< Min mass for global-II method
  double      m_minPi0Mass2        ; ///< Squared version of m_minPi0Mass
  double      m_maxPi0Mass         ; ///< Max mass for global-II method
  double      m_maxPi0Mass2        ; ///< Squared version of m_maxPi0Mass

// Local variables

  DeCalorimeter* m_ecal            ; ///< Pointer to Ecal detector element
  DeCalorimeter* m_hcal            ; ///< Pointer to Hcal detector element
  int         m_validPrs[16]       ; ///< Array for PRS validation: 1 or 2 bits
  double      m_gainCorrEcal[3]    ; ///< Gain correction in Ecal, per area
  double      m_gainCorrHcal[2]    ; ///< Gain correction in Hcal, per area

  int         m_spdMult            ; ///< Multiplicity of Spd

  ICaloTriggerAdcsFromRaw* m_adcsEcal; ///< Tool to decode trigger adcs.
  ICaloTriggerAdcsFromRaw* m_adcsHcal; ///< Tool to decode trigger adcs.
  ICaloTriggerBitsFromRaw* m_bitsFromRaw; ///< Tool to decode trigger bits.
  
// Trigger cards

  std::vector<TriggerCard> ecalFe  ; ///< Ecal front-end card vector
  std::vector<TriggerCard> hcalFe  ; ///< Hcal front-end card vector

  // Validation
  int         m_nbValidation;

  std::vector<raw_int> m_rawOutput; ///< RAW output bank

  bool m_storeFlag;

  double m_totRawSize;
  int m_nbEvents;  
};
#endif //   L0CALO_L0CALOALG_H
