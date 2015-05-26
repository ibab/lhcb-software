#include "nio.h"

int proto::getrest(char * buf, int n, int *r ){
  
  int opos=m_bufopos;
  
  int re;


  while (opos < n){
    if (m_pos>=m_inl) {
      if ((re = reget())<=0) return opos;
      
    }
    if (m_pos<0) {
      if ((re = reget())<=0) return opos;
      
    }
    buf[opos] = m_buf[m_pos];
    
    opos++;m_pos++;
    *r=opos;
  }
  
  return opos; 
  
}

int proto::getline(char * buf,int n){
  
  
  int opos=m_bufopos;
  
  while (opos < n){
    // printf("%d %d %d %d\n",m_pos,m_inl,m_bufopos,n);
    if (m_pos>=m_inl){
      int reg = reget();
      if (reg==0) return 0;
      if (reg==-1) return -1;
      if (reg==-2) {
	m_bufopos = opos;
	return -2;
      }
    } 
    //    printf("%d %d %d %d\n",m_pos,m_inl,m_bufopos,n);
    if (m_pos<0){
      int reg = reget();
      if (reg==0) return 0;
      if (reg==-1) return -1;
      if (reg==-2){
	m_bufopos = opos;
	return -2;
      }
    }  
    // printf("%d %d %d %d\n",m_pos,m_inl,m_bufopos,n);
    
    // printf(" %d %d %d:%c \n",m_pos,opos,m_buf[m_pos],m_buf[m_pos]);
    
    if (m_buf[m_pos]!='\n'){
      buf[opos] = m_buf[m_pos];
    }
    else{
      buf[opos]='\0';
      m_pos++;
      return opos+1;
    }
    
    m_pos++;
    
    if (m_pos>=m_inl) m_pos =-1;
    
    opos++;
  }
  m_bufopos = 0;
  return opos;
  
}

int proto::getnum(char * buf,int n, int *r){
  
  
  int opos=m_bufopos;
  
  while (opos < n){
    *r = opos;
    // printf("%d %d %d %d\n",m_pos,m_inl,m_bufopos,n);
    if (m_pos>=m_inl){
      int reg = reget();
      if (reg==0) return 0;
      if (reg==-1) return -1;
      if (reg==-2) {
	m_bufopos = opos;
	return -2;
      }
    } 
    //    printf("%d %d %d %d\n",m_pos,m_inl,m_bufopos,n);
    if (m_pos<0){
      int reg = reget();
      if (reg==0) return 0;
      if (reg==-1) return -1;
      if (reg==-2){
	m_bufopos = opos;
	return -2;
      }
    }  
    //    printf("%d %d %d %d\n",m_pos,m_inl,m_bufopos,n);
    
    //    printf(" %d %d %d:%c \n",m_pos,opos,m_buf[m_pos],m_buf[m_pos]);
    
    buf[opos] = m_buf[m_pos];
  
    //if (m_buf[m_pos]!='\n'){
    //  buf[opos] = m_buf[m_pos];
    //}
    //else{
    //  buf[opos]='\0';
    //  m_pos++;
    //  return opos+1;
    //}
    
    m_pos++;
    
    if (m_pos>=m_inl) m_pos =-1;
    
    opos++;
  }
  
  m_bufopos = 0;
  *r = opos;
  return opos;
  
}
