#ifndef   L0CALO_L0CALOALG_H
#define   L0CALO_L0CALOALG_H  1
// $Header: /afs/cern.ch/project/cvs/reps/lhcb/L0/L0Calo/src/L0CaloAlg.h,v 1.4 2001-05-10 14:44:33 ocallot Exp $

// from STL 
#include <cmath>
#include <string>
#include <vector>

// from Gaudi 
#include "GaudiKernel/Algorithm.h"

// from Calo
#include "CaloDet/DeCalorimeter.h"

// Local classes
#include "TriggerCard.h"

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

  void sumEcalData( MsgStream& );  ///< Sum the candidate on Ecal FE card.
  void sumHcalData( MsgStream& );  ///< Sum the candidate on Hcal FE card.
  void addPrsData(  MsgStream& );  ///< Produce the Prs information 
  void addSpdData(  MsgStream& );  ///< Produce the Spd data


/// Name in the Transient store of the containers of CaloDigit* objects  

  std::string m_nameOfEcalDataContainer   ;  
  std::string m_nameOfHcalDataContainer   ;  
  std::string m_nameOfPrsDataContainer    ;  
  std::string m_nameOfSpdDataContainer    ;  
  std::string m_nameOfOutputDataContainer ;  
  std::string m_nameOfOutputDirectory     ;  

/// Name of Detector in Detector Transient Store  e.g. /dd/structure/LHCb/

  std::string m_nameOfGeometryRoot ; 

// Parameter for conversion

  double      m_etScale            ; ///< Energy per bin in Et for trigger
  double      m_prsThreshold       ; ///< Energy threshold for Preshower

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

// Trigger cards

  std::vector<TriggerCard> ecalFe  ; ///< Ecal front-end card vector
  std::vector<TriggerCard> hcalFe  ; ///< Hcal front-end card vector
};

/** @class L0Candidate
 *
 *  This is a local class for temporary candidates, ID and Et, with saturation.
 *  This is internal to the algorithm, and is a termporary storage of the 
 *  candidates, during th eprocess of selecting the highest.
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
  };

  ~L0Candidate()                           { };
  
  /** update the information on a candidate
   *  @param et Integer transverse energy
   *  @param card Integer card number
   *  @param ID ClaoCellID location of the candidate 
   */
  void        setCandidate( int et, int card, CaloCellID ID ) ;

  /** Save the candidate in the output container as L0CaloCandidate
   *  @param type : Type of candidate, from L0::L0Type
   *  @param L0Calo : Vector of candidates, to which the current object
   *                  is added after being properly formatted.
   */
  void        saveCandidate( int type, L0CaloCandidateVector* L0Calo ) ;
  
  int         et( )        const { return m_et     ; };
  HepPoint3D  center( )    const { return m_center ; };
  double      tolerance( ) const { return m_tol    ; };
  CaloCellID  ID( )        const { return m_ID     ; };
  int         card( )      const { return m_card   ; };

 private:
  int            m_et     ;
  int            m_card   ;
  double         m_etScale;
  CaloCellID     m_ID     ;
  DeCalorimeter* m_det    ;
  HepPoint3D     m_center ;
  double         m_tol    ;
};

#endif //   L0CALO_L0CALOALG_H
