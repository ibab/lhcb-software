//*****************************************************************************
//** Local class to handle the front-end summing
//*****************************************************************************
#include <vector>

#include "CaloDet/CaloCardParams.h"

class TriggerCard {

public:

  TriggerCard( int num ) { m_number = num;};

  ~TriggerCard() {};

  void reset( ) ;

// ** Add the Et of a cell, perform the 2x2 sum on the flight. 

  void addEt( int col, int row, int digit ) ;

// ** Store the trigger bits of Prs/Spd.

  void setPrs( int col, int row ) {
    if (nRowCaloCard > row) {
      if (nColCaloCard > col) { prs[col]  [row]   += 8; }
      if (           0 < col) { prs[col-1][row]   += 4; }
    }
    if (0 < row) {
      if (nColCaloCard > col) { prs[col]  [row-1] += 2; }
      if (           0 < col) { prs[col-1][row-1] += 1; }
    }
  };
  
  void setSpd( int col, int row ) {
    if (nRowCaloCard > row) {
      if (nColCaloCard > col) { spd[col]  [row]   += 8; }
      if (           0 < col) { spd[col-1][row]   += 4; }
    }
    if (0 < row) {
      if (nColCaloCard > col) { spd[col]  [row-1] += 2; }
      if (           0 < col) { spd[col-1][row-1] += 1; }
    }
  };
  
  
  bool empty() const { return m_empty ; };
  
  int number() const { return m_number; };
  int etTot()  const { return m_etTot ; };
  int etMax()  const { return m_etMax ; };
  int colMax() const { return m_colMax; };
  int rowMax() const { return m_rowMax; };
  
  int prsMask() const { return prs[m_colMax][m_rowMax]; };
  int spdMask() const { return spd[m_colMax][m_rowMax]; };
  
  int et1()     const { return et [m_colMax  ][m_rowMax  ]; };
  int et2()     const { return et [m_colMax+1][m_rowMax  ]; };
  int et3()     const { return et [m_colMax  ][m_rowMax+1]; };
  int et4()     const { return et [m_colMax+1][m_rowMax+1]; };
  
// ** Methods for ECAL cards only: How to match the HCAL candidate

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

// ** Check if the ECAL candidate is INCLUDED in the projection of the 2x2 HCAL

  bool match_included ( int hCol, int hRow ) ;
  
// ** Methods to link Ecal cards to Hcal, and to retrieve the list
  
  void addEcalConnectedCard( int eCard ) { m_ecalCards.push_back( eCard )  ; };
  int  ecalCardNumber( int iCard ) const { return m_ecalCards[iCard]       ; };
  int  numberOfEcalCards()         const { return m_ecalCards.size()       ; };
  int  addEcalEt( int ecalEt )           { m_etMax += ecalEt; return m_etMax;};
  
private:
  int  m_number;
  int  et  [nColCaloCard+1] [nRowCaloCard+1] ;
  int  prs [nColCaloCard] [nRowCaloCard] ;
  int  spd [nColCaloCard] [nRowCaloCard] ;
  int  m_etTot ;
  bool m_empty ;
  int  m_etMax ;
  int  m_colMax;
  int  m_rowMax;
  
// ** Parameters for matching the HCAL candidate in an Ecal card

  int  m_hCard;
  int  m_mag;
  int  m_offsetCol;
  int  m_offsetRow;
    
// ** list of connected ECAL card numbers 

  std::vector<int> m_ecalCards ;

};
