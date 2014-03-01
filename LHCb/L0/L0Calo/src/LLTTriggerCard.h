// $Header: /afs/cern.ch/project/cvs/reps/lhcb/L0/L0Calo/src/LLTTriggerCard.h,v 1.5 2006-01-26 16:52:13 ocallot Exp $

#include <vector>

#include "CaloDet/CaloCardParams.h"
#include "CaloDet/DeCalorimeter.h"

/** @class LLTTriggerCard LLTTriggerCard.h
 * 
 *  This class represents one FE card, from the trigger view. It is used
 *  to compute and store the 2x2 sums, to store Prs and Spd bits, and also to
 *  hold the connection between ECAL and HCAL for the trigger
 *
 *  @author Olivier Callot
 *  @date   20 march 2001
 */

class LLTTriggerCard {

public:

  LLTTriggerCard( int num, DeCalorimeter* det ) { 
    m_number  = num;
    m_detElem = det;
    reset();
  };

  ~LLTTriggerCard() {};

  /** Resets the event part. 
   *  This functions resets the dynamic information (event data) while 
   *  keeping the structural information (ECAL-HCAL links)
   */
  void reset( ) ;
  
  /// Add the Et of a cell, perform the 2x2 sum on the flight. 
  void addEt( int col, int row, int digit , int mult_thresh ) ;
  
  bool empty() const { return m_empty ; };  ///< return if the card is empty
  
  int number() const { return m_number; };  ///< card number
  int etTot()  const { return m_etTot ; };  ///< total Et in the cards
  int etMax()  const { return m_etMax ; };  ///< Et of the 2x2 candidate
  int multiplicity() const { return m_multiplicity ;} ; ///< multiplicity of the card
  int colMax() const { return m_colMax; };  ///< column of the candidate
  int rowMax() const { return m_rowMax; };  ///< row of the candidate
  
  /// returns the cel ID at the maximum Et.
  LHCb::CaloCellID cellIdMax() const { 
    return m_detElem->cardCellID( m_number, m_rowMax, m_colMax ); 
  };

private:
  int  m_number;
  DeCalorimeter* m_detElem;
  int  et  [nColCaloCard+1] [nRowCaloCard+1] ;
  int  m_etTot ;
  int  m_multiplicity ;
  bool m_empty ;
  int  m_etMax ;
  int  m_colMax;
  int  m_rowMax;
};
