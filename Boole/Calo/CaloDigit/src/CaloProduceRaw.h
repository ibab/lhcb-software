// $Id: CaloProduceRaw.h,v 1.2 2003-06-23 11:43:04 ocallot Exp $
#ifndef CALOPRODUCERAW_H 
#define CALOPRODUCERAW_H 1

// Include files

// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/Algorithm.h"


/** @class CaloProduceRaw CaloProduceRaw.h
 *  
 *
 *  @author Olivier Callot
 *  @date   2003-03-21
 */
class CaloProduceRaw : public Algorithm {
public:
  /// Standard constructor
  CaloProduceRaw( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~CaloProduceRaw( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:
 
  void buildEnergyBuffer( std::string location );

  void createBankHeader( int classID, unsigned int sourceID )  {
    if ( m_buffers.size() <= sourceID ) return;
    m_startOfBank[sourceID] = m_buffers[sourceID].size();
    m_buffers[sourceID].push_back( 0 );
    m_buffers[sourceID].push_back( m_magicWord );
    m_buffers[sourceID].push_back( (sourceID<<16) + classID );
  }
  
  void adjustBankHeader( unsigned int sourceID ) {
    if ( m_buffers.size() <= sourceID ) return;
    int l = m_buffers[sourceID].size() - m_startOfBank[sourceID];
    if ( 0 < l ) {
      m_buffers[sourceID][ m_startOfBank[sourceID] ] = l;
    }
  }

  int bufferNumber( const CaloCellID& ID )  {
    int det  = ID.calo();
    int area = ID.area();
    int row  = ID.row();
    int col  = ID.col();
    int bNum = 0;

    if ( 1 >= det ) { 
      bNum = 1 + area;
      if ( 0 == area && 34 > row ) bNum = 0;  //== Two L1 boards
      if ( 31 < col ) bNum += 4;
    } else if ( 2 == det ) {
      bNum = 8 + 2*area;
      if ( 0 == area && 33 < row ) bNum += 1; //== Two L1 boards
      if ( 1 == area && 31 < row ) bNum += 1; //== Two L1 boards
      if ( 31 < col ) bNum += 5;
    } else if ( 3 == det ) {
      bNum = 18 + area;
      if ( 15 < col ) bNum += 2;
    }
    return bNum;
  }
  
  


private:
  std::vector< std::vector<int> > m_buffers;
  std::vector<int> m_startOfBank;

  int m_caloEnergyID;
  int m_caloTriggerID;
  int m_prsSpdTriggerID;
  int m_magicWord;

  double m_nEvents;
  std::vector<double> m_size;

};
#endif // CALOPRODUCERAW_H
