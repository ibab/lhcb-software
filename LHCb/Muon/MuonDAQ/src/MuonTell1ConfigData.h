// $Id: MuonTell1ConfigData.h,v 1.1 2008-04-02 11:53:40 asatta Exp $
#ifndef MUONTELL1CONFIGDATA_H 
#define MUONTELL1CONFIGDATA_H 1

// Include files

/** @class MuonTell1ConfigData MuonTell1ConfigData.h
 *  
 *
 *  @author 
 *  @date   2008-03-01
 */
class MuonTell1ConfigData {
public: 
  /// Standard constructor
  MuonTell1ConfigData( ){
    for (int i=0;i<24;i++)
    {
      
      pad_offset[i]=0;
      hit_offset[i]=0;
      padTU[i]=0;
    }
    
}; 

  virtual ~MuonTell1ConfigData( ){}; ///< Destructor
  void setPadOffset(unsigned int* pad_off);
  

  void setHitOffset(unsigned int* Hit_off);
  
  void setPadTU(unsigned int* pad_TU);
  void setPadOffset(unsigned int ch,unsigned int pad_off){
    pad_offset[ch]=pad_off;};
  void setHitOffset(unsigned int ch,unsigned int Hit_off){
    hit_offset[ch]=Hit_off;};
  
  void setPadTU(unsigned int ch,unsigned int pad_TU){
    padTU[ch]=pad_TU;};
  unsigned int getPadOffset(int ch){
    return pad_offset[ch];    
  };
   unsigned int getHitOffset(int ch){
    return hit_offset[ch];    
  };
   unsigned int getPadTU(int ch){
    return padTU[ch];    
  };
  
  std::ostream& fillStream(std::ostream& s) const;

protected:

private:
  
  unsigned int pad_offset[24];
  unsigned int padTU[24];
  unsigned int hit_offset[24];
  
};

void MuonTell1ConfigData::setPadOffset(unsigned int * pad_off){
for(int i=0;i<24;i++)pad_offset[i]=*(pad_off+i);  
}
void MuonTell1ConfigData::setHitOffset(unsigned int * pad_off){
for(int i=0;i<24;i++)hit_offset[i]=*(pad_off+i);  
}
void MuonTell1ConfigData::setPadTU(unsigned int * pad_off){
for(int i=0;i<24;i++)padTU[i]=*(pad_off+i);  
}
std::ostream & MuonTell1ConfigData::fillStream(std::ostream &s)const
{  
  s<<"#  The TU type of each optical link. "<<
    "(0->Type1a, 1->Type1b,2->Type2,3->Type3,4->Type4a,"<<
    "5->Type4b,6->Type5,7->Type6)";
  s<<"\n";
std::string sep[6];
sep[0]=",";
sep[1]=",";
sep[2]=",";
sep[3]=",";
sep[4]=",";
sep[5]="";
  s<<" _L0050:___    ["; 
  for(int i=0;i<6;i++){    
    s.width(4);  
    s<<padTU[i]<<sep[i%6];
  }
  s<<"]    --PP0, optical link 0-5 ";
  s<<"\n";
  s<<" _L0051:___    ["; 
  for(int i=6;i<12;i++){    
    s.width(4);  
    s<<padTU[i]<<sep[i%6];
  }
  s<<"]    --PP1, optical link 0-5 ";
  s<<"\n";
  s<<" _L0052:___    ["; 
  for(int i=12;i<18;i++){    
    s.width(4);  
    s<<padTU[i]<<sep[i%6];
  }
  s<<"]    --PP2, optical link 0-5 ";
  s<<"\n";
  s<<" _L0053:___    ["; 
  for(int i=18;i<24;i++){    
    s.width(4);  
    s<<padTU[i]<<sep[i%6];
  }
  s<<"]    --PP3, optical link 0-5 ";
  s<<"\n";
  s<<"#  The PAD process offset address of each optical link";
  s<<"\n";




  s<<" _L0054:___    ["; 
  for(int i=0;i<6;i++){    
    s.width(8);  
    s<<pad_offset[i]<<sep[i%6];
  }
  s<<"]    --PP0, optical link 0-5 ";
  s<<"\n";
  s<<" _L0055:___    ["; 
  for(int i=6;i<12;i++){    
    s.width(8);  
    s<<pad_offset[i]<<sep[i%6];
  }
  s<<"]    --PP1, optical link 0-5 ";
  s<<"\n";
  s<<" _L0056:___    ["; 
  for(int i=12;i<18;i++){    
    s.width(8);  
    s<<pad_offset[i]<<sep[i%6];
  }
  s<<"]    --PP2, optical link 0-5 ";
  s<<"\n";
  s<<" _L0057:___    ["; 
  for(int i=18;i<24;i++){    
    s.width(8);  
    s<<pad_offset[i]<<sep[i%6];
  }
  s<<"]    --PP3, optical link 0-5 ";
  s<<"\n";
    
 





  s<<" #  The HIT process offset address of each optical link ";
  //s.newLine();
  s<<"\n";
  
  s<<" _L0058:___    ["; 
  for(int i=0;i<6;i++){    
    s.width(8);  
    s<<hit_offset[i]<<sep[i%6];
  }
  s<<"]    --PP0, optical link 0-5 ";
  s<<"\n";
  s<<" _L0059:___    ["; 
  for(int i=6;i<12;i++){    
    s.width(8);  
    s<<hit_offset[i]<<sep[i%6];
  }
  s<<"]    --PP1, optical link 0-5 ";
  s<<"\n";
  s<<" _L0060:___    ["; 
  for(int i=12;i<18;i++){    
    s.width(8);  
    s<<hit_offset[i]<<sep[i%6];
  }
  s<<"]    --PP2, optical link 0-5 ";
  s<<"\n";
  s<<" _L0061:___    ["; 
  for(int i=18;i<24;i++){    
    s.width(8);  
    s<<hit_offset[i]<<sep[i%6];
  }
  s<<"]    --PP3, optical link 0-5 ";
  s<<"\n";
    
  
  return s ;
}

inline std::ostream& operator<< (std::ostream& str, const MuonTell1ConfigData& obj) {
  return obj.fillStream(str);   }
#endif // MUONTELL1CONFIGDATA_H
