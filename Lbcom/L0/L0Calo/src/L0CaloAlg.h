#ifndef   L0CALOALG_H
#define   L0CALOALG_H  1

///
/// from STL 
#include <cmath>
#include <string>
#include <vector>

///
/// from Gaudi 
#include "GaudiKernel/Algorithm.h"

/// from Calo

#include "CaloDet/DeCalorimeter.h"

// Local classes

#include "TriggerCard.h"

//
//  Class L0CaloAlg : a (sub)Algorithm responsible 
//                    for the L0 Calorimeter trigger 
//
//   Author: Olivier Callot
//   Date:   4 October 2000
// 

class L0CaloAlg : public Algorithm {

public:

// ** Constructor of this form must be provided

  L0CaloAlg(const std::string& name, ISvcLocator* pSvcLocator);

// ** Standard Destructor

  virtual ~L0CaloAlg();

// ** Three mandatory member functions of any algorithm

  StatusCode initialize();
  StatusCode execute   ();
  StatusCode finalize  ();
  
protected:

private:   

// ** Methods to handle part of the job, to make the code more readable
  
  void sumEcalData( MsgStream );
  void sumHcalData( MsgStream );
  void addPrsData( MsgStream );
  void addSpdData( MsgStream );


// ** name in the Transient store of the containers of CaloDigit* objects  

  std::string m_nameOfEcalDataContainer   ;  
  std::string m_nameOfHcalDataContainer   ;  
  std::string m_nameOfPrsDataContainer    ;  
  std::string m_nameOfSpdDataContainer    ;  
  std::string m_nameOfOutputDataContainer ;  
  std::string m_nameOfOutputDirectory     ;  

// ** name of Detector in Detector Transient Store  e.g. /dd/structure/LHCb/

  std::string m_nameOfGeometryRoot ; 

// ** Parameter for conversion

  double      m_etScale            ; // Energy per bin in Et for trigger
  double      m_prsThreshold       ; // Energy threshold for Preshower

// ** Pi0 trigger parameters

  int         m_pi0Strategy        ; // Defines how Pi0 are defined.
  double      m_minEtPi0Local      ; // Min cluster Et for local pi0
  double      m_minEtGammaGlobal   ; // Min cluster Et for gamma Global pi0
  double      m_minPi0Mass         ; // Min mass gor global-II method
  double      m_minPi0Mass2        ; // Squared version
  double      m_maxPi0Mass         ; // Max mass gor global-II method
  double      m_maxPi0Mass2        ; // Squared version

// ** Local variables

  DeCalorimeter* m_ecal            ;
  DeCalorimeter* m_hcal            ;
  int         m_validPrs[16]       ; // Array for PRS validation: 1 or 2 bits
  double      m_gainCorrEcal[3]    ; // Gain correction
  double      m_gainCorrHcal[2]    ;

// ** Trigger cards

  std::vector<TriggerCard> ecalFe  ;
  std::vector<TriggerCard> hcalFe  ;
};

//-----------------------------------------------------------------------
// ** Local class For temporary candidates, ID and Et, with saturation
//-----------------------------------------------------------------------

class L0Candidate {
public:
  L0Candidate( DeCalorimeter* det ) : m_ID() {
    m_det = det ;
    m_et  = 0   ;
  };

  ~L0Candidate()                           { };
  
  int        et( )                   const { return m_et;      };
  void       setCandidate( int et, int card, CaloCellID ID ) { 
    m_ID     = ID;
    m_card   = card;
    if ( 255 >= et ) { m_et = et; } else { m_et = 255; }
    m_center = m_det->cellCenter( ID ); 
    m_tol    = m_det->cellSize( ID ) * .5;
    m_center.setX( m_center.x() + m_tol );
    m_center.setY( m_center.y() + m_tol );
  };
  
  HepPoint3D  center( )             const { return m_center ; };
  double      tolerance( )          const { return m_tol    ; };
  CaloCellID  ID( )                 const { return m_ID     ; };
  int         card( )               const { return m_card   ; };

 private:
  int            m_et     ;
  int            m_card   ;
  CaloCellID     m_ID     ;
  DeCalorimeter* m_det    ;
  HepPoint3D     m_center ;
  double         m_tol    ;
};

#endif //   L0CALOALG_H
