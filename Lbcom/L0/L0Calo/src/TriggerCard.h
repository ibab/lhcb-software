// $Header: /afs/cern.ch/project/cvs/reps/lhcb/L0/L0Calo/src/TriggerCard.h,v 1.5 2006-01-26 16:52:13 ocallot Exp $

#include <vector>

#include "CaloDet/CaloCardParams.h"
#include "CaloDet/DeCalorimeter.h"

/** @class TriggerCard TriggerCard.h
 * 
 *  This class represents one FE card, from the trigger view. It is used
 *  to compute and store the 2x2 sums, to store Prs and Spd bits, and also to
 *  hold the connection between ECAL and HCAL for the trigger
 *
 *  @author Olivier Callot
 *  @date   20 march 2001
 */

class TriggerCard {

public:

  TriggerCard( int num, DeCalorimeter* det ) { 
    m_number  = num;
    m_detElem = det;
    reset();
  };

  ~TriggerCard() {};

/** Resets the event part. 
 *  This functions resets the dynamic information (event data) while 
 *  keeping the structural information (ECAL-HCAL links)
 */
  void reset( ) ;
  void setValidationNumber( int num ) { m_validationNumber = num; }

/// Add the Et of a cell, perform the 2x2 sum on the flight. 
  void addEt( int col, int row, int digit ) ;

/// Store the trigger bits of Prs.
  void setPrs( int col, int row ) ;
  
/// Store the trigger bits of Spd.
  void setSpd( int col, int row ) ;
  
  bool empty() const { return m_empty ; };  ///< return if the card is empty
  
  int number() const { return m_number; };  ///< card number
  int validationNumber() const { return m_validationNumber; };  ///< card num
  int etTot()  const { return m_etTot ; };  ///< total Et in the cards
  int etMax()  const { return m_etMax ; };  ///< Et of the 2x2 candidate
  int colMax() const { return m_colMax; };  ///< column of the candidate
  int rowMax() const { return m_rowMax; };  ///< row of the candidate
  
/// returns the cel ID at the maximum Et.
  LHCb::CaloCellID cellIdMax() const { 
    return m_detElem->cardCellID( m_number, m_rowMax, m_colMax ); 
  };
  
  int prsMask() const { return prs[m_colMax][m_rowMax]; }; ///< Prs mask
  int spdMask() const { return spd[m_colMax][m_rowMax]; }; ///< Spd mask 
  
  int et1()     const { return et [m_colMax  ][m_rowMax  ]; }; ///< Et bot.lef
  int et2()     const { return et [m_colMax+1][m_rowMax  ]; }; ///< Et bot.rig
  int et3()     const { return et [m_colMax  ][m_rowMax+1]; }; ///< Et top.lef
  int et4()     const { return et [m_colMax+1][m_rowMax+1]; }; ///< Et top.rig
  
/// Method for ECAL cards only: Set the Hcal card which is connected.

  void setHcalParams( int hCard, int mag, int offsetCol, int offsetRow ) {
    m_hCard     = hCard;
    m_mag       = mag;
    m_offsetCol = offsetCol;
    m_offsetRow = offsetRow;
  };

  int hcalCard( )        const { return m_hCard     ; };
  int hcalMag( )         const { return m_mag       ; };
  int hcalOffsetCol( )   const { return m_offsetCol ; };
  int hcalOffsetRow( )   const { return m_offsetRow ; };

/// Check if the ECAL candidate is INCLUDED in the projection of the 2x2 HCAL
  bool match_included ( int hCol, int hRow ) ;
  
/// Methods to link Ecal cards to Hcal
  void addEcalConnectedCard( int eCard ) { m_ecalCards.push_back( eCard )  ; };

/// get the iCard-th connected card.
  int  ecalCardNumber( int iCard ) const { return m_ecalCards[iCard]       ; };
  int  numberOfEcalCards()         const { return m_ecalCards.size()       ; };
  int  addEcalEt( int ecalEt )           { m_etMax += ecalEt; return m_etMax;};
  
private:
  int  m_number;
  DeCalorimeter* m_detElem;
  int  m_validationNumber;
  int  et  [nColCaloCard+1] [nRowCaloCard+1] ;
  int  prs [nColCaloCard] [nRowCaloCard] ;
  int  spd [nColCaloCard] [nRowCaloCard] ;
  int  m_etTot ;
  bool m_empty ;
  int  m_etMax ;
  int  m_colMax;
  int  m_rowMax;
  
// Parameters for matching the HCAL candidate in an Ecal card

  int  m_hCard;
  int  m_mag;
  int  m_offsetCol;
  int  m_offsetRow;
    
// list of connected ECAL card numbers 

  std::vector<int> m_ecalCards ;

};
