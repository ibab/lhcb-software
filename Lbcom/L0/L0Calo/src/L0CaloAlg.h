#ifndef   L0CALO_L0CALOALG_H
#define   L0CALO_L0CALOALG_H  1
// $Id: L0CaloAlg.h,v 1.10 2003-12-15 09:18:14 ocallot Exp $

// from STL 
#include <cmath>
#include <string>
#include <vector>

// from Gaudi 
#include "GaudiKernel/Algorithm.h"

// from Calo
#include "CaloDet/DeCalorimeter.h"

// from DAQEvent
#include "Event/RawEvent.h"
#include "Event/L1Event.h"

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
   *  @param card Integer card number
   *  @param ID ClaoCellID location of the candidate 
   */
  void        setCandidate( int et, CaloCellID ID ) ;

  /** Save the candidate in the output container as L0CaloCandidate
   *  @param type : Type of candidate, from L0::L0Type
   *  @param L0Calo : Vector of candidates, to which the current object
   *                  is added after being properly formatted.
   */
  void        saveCandidate( int type, L0CaloCandidates* L0Calo ) ;
  
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

class L0CaloAlg : public Algorithm {

public:

/// standard algorithm Constructor

  L0CaloAlg(const std::string& name, ISvcLocator* pSvcLocator);


  virtual ~L0CaloAlg();       ///< Standard destructor

  StatusCode initialize();    ///< Algorithm initialization
  StatusCode execute   ();    ///< Algorithm execution
  StatusCode finalize  ();    ///< Algorithm finalization
  
protected:

private:   

  void sumEcalData( MsgStream& , RawEvent& );  ///< process Ecal FE card.
  void sumHcalData( MsgStream& , RawEvent& );  ///< process Hcal FE card.
  void addPrsData(  MsgStream& , RawEvent& );  ///< process the Prs information
  void addSpdData(  MsgStream& , RawEvent& );  ///< Produce the Spd data

  void saveCandidate( int, L0Candidate&, int ); ///< Save in L1 and RAW buffers
  
/// Name in the Transient store of the output container.

  std::string m_nameOfOutputDataContainer ;

/// Name of Detector in Detector Transient Store  e.g. /dd/structure/LHCb/

  std::string m_nameOfGeometryRoot ; 

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

// Trigger cards

  std::vector<TriggerCard> ecalFe  ; ///< Ecal front-end card vector
  std::vector<TriggerCard> hcalFe  ; ///< Hcal front-end card vector

  // Validation
  int         m_nbValidation;

  std::vector<l1_int> m_l1Output;   ///< L1 output bank
  std::vector<raw_int> m_rawOutput; ///< RAW output bank

  int m_l1ElectronThr;
  int m_l1PhotonThr;
  int m_l1HadronThr;
  int m_l1Pi0LocalThr;
  int m_l1Pi0GlobalThr;

  bool m_storeFlag;

  double m_totL1Size;
  double m_totRawSize;
  int m_nbEvents;
  
};


#endif //   L0CALO_L0CALOALG_H
