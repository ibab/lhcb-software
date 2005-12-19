// $Id: CaloReadoutTool.h,v 1.3 2005-12-19 19:29:14 ocallot Exp $
#ifndef CALODAQ_CALOREADOUTTOOL_H 
#define CALODAQ_CALOREADOUTTOOL_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "CaloDet/DeCalorimeter.h"

static const InterfaceID IID_CaloReadoutTool ( "CaloReadoutTool", 1, 0 );

/** @class CaloFECard
 *  Auxilliary class to hold the properties of a FE card
 *  @author Olivier Callot
 *  @date   2005-08-25
 */
class CaloFECard {
public:
  CaloFECard( int num, int crate, int slot ) {
    m_number = num;
    m_crate  = crate;
    m_slot   = slot; 
    m_code   = m_crate * 16 + m_slot;
    m_ids.reserve( 64 );
  }

  ~CaloFECard() {};
  
  void addID( LHCb::CaloCellID id )    { m_ids.push_back( id ); }
  std::vector<LHCb::CaloCellID>& ids() { return m_ids; }
  int  number()    const { return m_number; }
  int  crate()     const { return m_crate; }
  int  slot()      const { return m_slot; }
  int  code()      const { return m_code; }

private:
  int m_number;
  std::vector<LHCb::CaloCellID> m_ids;
  int m_crate;
  int m_slot;
  int m_code;
};

/** @class CaloTell1
 *  Auxilliary class to hold the properties of a Tell1
 *  @author Olivier Callot
 *  @date   2005-08-25
 */
class CaloTell1 {
public:
  CaloTell1( int num ) {
    m_number = num;
    m_feCards.reserve( 24 );
  }
  ~CaloTell1() {};

  int number()      const { return m_number;  }
  
  void addFeCard( int num )   { m_feCards.push_back( num ); }

  std::vector<int>& feCards()  { return m_feCards; }

private:
  int m_number;
  std::vector<int> m_feCards;
};

/** @class CaloReadoutTool CaloReadoutTool.h CaloDAQ/CaloReadoutTool.h
 *  Readout tool for the calorimeter, defines the FE Cards and TELL1 structure
 *
 *  @author Olivier Callot
 *  @date   2005-08-25
 */
class CaloReadoutTool : public GaudiTool {
public: 

  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_CaloReadoutTool; }

  /// Standard constructor
  CaloReadoutTool( const std::string& type, 
                   const std::string& name,
                   const IInterface* parent);

  virtual ~CaloReadoutTool( ); ///< Destructor

  virtual StatusCode initialize();

  int nbFECards()    const { return m_cards.size(); }
  
  std::vector<LHCb::CaloCellID>& cellInFECard( int num ) { return m_cards[num].ids(); }

  int cardCode( int num ) { return m_cards[num].code(); }

  int nbTell1()    const { return m_tell1.size(); }
  
  std::vector<int>& feCardsInTell1( int num )  {
    return m_tell1[num].feCards();
  }

protected:

private:
  std::string    m_detectorName;
  bool           m_checkingCards;
  DeCalorimeter* m_calo;
  std::vector<CaloFECard> m_cards;
  std::vector<CaloTell1>  m_tell1;
};
#endif // CALODAQ_CALOREADOUTTOOL_H
