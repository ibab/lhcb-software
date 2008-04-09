#ifndef MUONDAQ_MUONODEDATA_H 
#define MUONDAQ_MUONODEDATA_H 1

/** @class MuonODEData MuonODEData.h Event/MuonODEData.h
 *  
 *
 *  @author Alessia Satta
 *  @date   2007-12-05
 */


class MuonODEData {
public:
  MuonODEData(){};
  virtual ~MuonODEData(){};
  void resetODEData(){
    for(int i=0;i<35;i++){
      
      m_data[i]=0;
    }
    
  };
  void setData(unsigned int words[35]){
    for(int i=0;i<35;i++){
      
      m_data[i]=words[i];
    }
  };
  unsigned int getWord(int index){
    return m_data[index];
  };
  unsigned int getTDC(int index,int ch)
  {
    //int shift_left=(ch)*4;    
    int shift_rigth=32-(ch+1)*4;
    return ( m_data[index]<<shift_rigth)>>28;
    
  
  }
  unsigned int getTDC(int ch)
  {
    //int shift_left=(ch)*4;    
    int index=ch/8+1;
    int relative_ch=ch%8;	
    return getTDC(index,relative_ch);
  }
  

protected:
private:
  
  unsigned int m_data[35];
};
#endif // MUONDAQ_MUONODEDATA_H
