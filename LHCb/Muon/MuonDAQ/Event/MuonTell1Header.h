// $Id: MuonTell1Header.h,v 1.1 2008-06-30 11:43:46 asatta Exp $
#ifndef EVENT_MUONTELL1HEADER_H 
#define EVENT_MUONTELL1HEADER_H 1

// Include files
#include <ostream>
/** @class MuonTell1Header MuonTell1Header.h Event/MuonTell1Header.h
 *  
 *
 *  @author 
 *  @date   2008-01-07
 */
class MuonTell1Header {
public: 
  /// Standard constructor
  MuonTell1Header( ){
      m_data=0;
    
  }; 
  MuonTell1Header(unsigned int data ){
      m_data=data;    
  }; 

  virtual ~MuonTell1Header( ){}; ///< Destructor
  void setWord(unsigned int word)
  {
    m_data=word;
    
  };
  const unsigned int getBCN() const 
  {
    //first 8 bits
    return m_data&0x00FF;
    
  };
  



   const unsigned int getError() const 
  {
    //secodn 8 bits
    return (m_data&0xFF00)>>8;
    
  }

    const unsigned int getEventError() const 
  {
    //PP_ODE_err bit 0 provided by synch link

    return ((m_data>>8)&(unsigned int)1);
    
  }
     const unsigned int getSYNCHDataErrorInODE() const 
  {
    //PP_ODE_err bit 5
    return (m_data>>13)&((unsigned int)1);
    
  }
  
     const unsigned int getSYNCHEventCntErrorInODE() const 
  {
    //PP_ODE_err bit 7
    return (m_data>>15)&((unsigned int)1);
    
  }
  
     const unsigned int getSYNCHBCNCntErrorInODE() const 
  {
    //PP_ODE_err bit 6
    return (m_data>>14)&((unsigned int)1);
    
  }
  
  const unsigned int getOROfODEErrors() const 
  {
    //PP_ODE_err bits 1-4 
    return ((m_data>>9)&0x000F);
    
  }
  
  std::ostream& fillStream(std::ostream& s) const
  {
    s << "{ ";
    s << "Muon Tell1 Header : "  
      <<" BCN "<<getBCN()
      <<" Tell1 error "<< getEventError()
      <<" OR of ODE errors "<<getOROfODEErrors()
      <<" SYNCH data consistency "<<getSYNCHDataErrorInODE()
      <<" SYNCH BC  consistency "<< getSYNCHBCNCntErrorInODE()
      <<" SYNCH Evt  consistency "<<  getSYNCHEventCntErrorInODE()
      ;
    
    return s << " }";
  }
  
protected:

private:
  unsigned int m_data;


  
};
  inline std::ostream& operator<< (std::ostream& str, 
                                   const MuonTell1Header& obj)
  {
    return obj.fillStream(str);
  }
  


#endif // EVENT_MUONTELL1HEADER_H
