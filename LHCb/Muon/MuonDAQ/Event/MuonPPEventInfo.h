// $Id: MuonPPEventInfo.h,v 1.1 2008-04-02 13:25:52 asatta Exp $
#ifndef EVENT_MUONPPEVENTINFO_H 
#define EVENT_MUONPPEVENTINFO_H 1

// Include files

/** @class MuonPPEventInfo MuonPPEventInfo.h Event/MuonPPEventInfo.h
 *  
 *
 *  @author 
 *  @date   2008-01-07
 */
class MuonPPEventInfo {
public: 
  /// Standard constructor
  MuonPPEventInfo( ){
    for (int i=0;i<8;i++)
    {      
      m_data[i]=0;
    }
    
  }; 

  virtual ~MuonPPEventInfo( ){}; ///< Destructor
  void setWord(unsigned int i, unsigned int word)
  {
    m_data[i]=word;
    
  };
  
  unsigned int getBCN()
  {
    return m_data[0]&0x0FFF;
    
  };
  
  unsigned int getDetID()
  {
    return m_data[0]&0xF000;
    
  }
   unsigned int getL0EvtID()
  {
    return m_data[1];
    
  };
  

protected:

private:
  unsigned int m_data[8];
  
};
#endif // EVENT_MUONPPEVENTINFO_H
