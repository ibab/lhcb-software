#ifndef PRPLANEHYBRIDCOUNTER_H 
#define PRPLANEHYBRIDCOUNTER_H 1

// Include files

/** @class PrPlaneHybridCounter PrPlaneHybridCounter.h
 *  Class to count how many different planes are in a List and how many planes with a single hit fired. It also check if the content of hits is distributed in the SciFi in a proper way 1 hit per station X ( and UV ).
 *
 *  @author renato quagliani
 *  @date   2015-07-13
 */
class PrPlaneHybridCounter {
public: 
  /// Standard constructor
  PrPlaneHybridCounter( ):
    m_nbFiredLayers(0),
    m_nbFiredLayers_singleHit(0),
    m_planeList({{0,0,0,0,0,0,0,0,0,0,0,0}}),
    m_nbT1X(0),
    m_nbT1X_singleHit(0),
    m_nbT2X(0),
    m_nbT2X_singleHit(0),
    m_nbT3X(0),
    m_nbT3X_singleHit(0),
    m_nbT1UV(0),
    m_nbT1UV_singleHit(0),
    m_nbT2UV(0),
    m_nbT2UV_singleHit(0),
    m_nbT3UV(0),
    m_nbT3UV_singleHit(0),
    m_nbHits(0)
  { };
  
  /** Set values: given a range of iterators it fills the information for the hit content
   *  @brief Set values for a given range of iterators
   *  @param itBeg First iterator, begin of range
   *  @param itEnd Last iterator, end of range
   *  @param fill Should the values be reset?
   */
  void set( PrHits::const_iterator itBeg, PrHits::const_iterator itEnd, const bool fill = true){
    if(fill){
      m_nbUsed=0;
      m_nbHits=0;
      m_nbFiredLayers = 0;
      m_nbFiredLayers_singleHit = 0;
      m_planeList = {{0,0,0,0,0,0,0,0,0,0,0,0}};
      //X layer T1
      m_nbT1X=0;
      m_nbT1X_singleHit=0;
      //X layer T2
      m_nbT2X=0;
      m_nbT2X_singleHit=0;
      //X layer T3
      m_nbT3X=0;
      m_nbT3X_singleHit=0;
      //UV Layer T1
      m_nbT1UV=0;
      m_nbT1UV_singleHit=0;
      //UV Layer T2
      m_nbT2UV=0;
      m_nbT2UV_singleHit=0;
      //UV Layer T3
      m_nbT3UV=0;
      m_nbT3UV_singleHit=0;
      m_nbT1_singleHit = 0;
      m_nbT2_singleHit = 0;
      m_nbT3_singleHit = 0;
    }
    PrHit *hit = nullptr;
    for( PrHits::const_iterator itH = itBeg; itEnd != itH; ++itH) {
      m_nbHits++;
      hit = (*itH);
      if(hit->isUsed()) m_nbUsed++;
      unsigned int plane = hit->planeCode();
      if( m_planeList[plane] >0 ){m_planeList[plane]++;}
      //if no hits in the plane, increase the counters anyway even if the plane
      if( 0 == m_planeList[ plane ] ){
        m_planeList[plane]++;
        m_nbFiredLayers_singleHit++;
        m_nbFiredLayers++;
        if( isT1(hit) ){ // increase conters of T1
          m_nbT1++;                                                                                                     
          m_nbT1_singleHit++;
          if( hit->isX() ){
            m_nbT1X++;
            m_nbT1X_singleHit++;
          }else{
            m_nbT1UV++;
            m_nbT1UV_singleHit++;
          }
        }else if( isT2(hit) ){
          m_nbT2++;
          m_nbT2_singleHit++;
          if( (hit)->isX()){
            m_nbT2X++;
            m_nbT2X_singleHit++;
          }else{
            m_nbT2UV++;
            m_nbT2UV_singleHit++;
          }
        }else if(isT3(hit)){
          m_nbT3++;
          m_nbT3_singleHit++;
          if( (hit)->isX()){
            m_nbT3X++;
            m_nbT3X_singleHit++;
          }else{
            m_nbT3UV++;
            m_nbT3UV_singleHit++;
          }
        }
      }
      
      //  if( isT1(hit) &&  hit->isX()){
      //     m_nbT1X++  ; 
      //     m_nbT1X_singleHit++;  
      //   }
      //   if(isT1(hit) && !hit->isX()){
      //     m_nbT1UV++ ; 
      //     m_nbT1UV_singleHit++; 
      //   }
      //   if( isT2(hit) &&  hit->isX()){ 
      //     m_nbT2X++  ; 
      //     m_nbT2X_singleHit++;  
      //   }
      //   if( isT2(hit) && !hit->isX()){ 
      //     m_nbT2UV++ ;
      //     m_nbT2UV_singleHit++; 
      //   }
      //   if( isT3(hit) &&  hit->isX()){ 
      //     m_nbT3X++  ; 
      //     m_nbT3X_singleHit++;  
      //   }
      //   if( isT3(hit) && !hit->isX()){
      //     m_nbT3UV++ ; m_nbT3UV_singleHit++; 
      //   }
      //   if(isT1(hit)){ 
      //     m_nbT1++; m_nbT1_singleHit++;  }
      //   if(isT2(hit)){ 
      //     m_nbT2++; m_nbT2_singleHit++;  }
      //   if(isT3(hit)){ 
      //     m_nbT3++; m_nbT3_singleHit++;  }
      // }
      if(2 == m_planeList[plane]){
        m_nbFiredLayers_singleHit --;
        if( isT1(hit) ){
          m_nbT1_singleHit--;
          if( hit->isX() ){ 
            m_nbT1X_singleHit--;
          }else{  
            m_nbT1UV_singleHit--;}
        }else if( isT2(hit)){
          m_nbT2_singleHit--;
          if(hit->isX()){
            m_nbT2X_singleHit--;
          }else{
            m_nbT2UV_singleHit--;
          }
        }else if( isT3(hit)){
          m_nbT3_singleHit--;
          if(hit->isX()){
            m_nbT3X_singleHit--;
          }else{
            m_nbT3UV_singleHit--;
          }
        }
      }
      // m_nbFiredLayers_singleHit--;
      //   if( isT1(hit) && hit->isX()) { m_nbT1X_singleHit--;}
      //   if( isT1(hit) && !hit->isX()){ m_nbT1UV_singleHit--;}
      //   if( isT2(hit) && hit->isX() ) { m_nbT2X_singleHit--; }
      //   if( isT2(hit)&& !hit->isX()){ m_nbT2UV_singleHit--;}
      //   if( isT3(hit) && hit->isX()) { m_nbT3X_singleHit--; }
      //   if( isT3(hit) && !hit->isX()){ m_nbT3UV_singleHit--;}
      //   if(isT1(hit)){ m_nbT1_singleHit--; }
      //   if(isT2(hit)){ m_nbT2_singleHit--; }
      //   if(isT3(hit)){ m_nbT3_singleHit--; }
      // }
    }//end loop hits
  }
  
  int addHit( const PrHit* hit){
    //if(hit->isUsed()) return m_nbFiredLayers;
    unsigned int plane = hit->planeCode();
    m_nbHits++;
    
    if( 0 == m_planeList[plane]++){
       m_nbFiredLayers_singleHit ++;
       m_nbFiredLayers ++;
       if( isT1(hit) && hit->isX()) { m_nbT1X++; m_nbT1X_singleHit++;}
       if( isT1(hit) && !hit->isX()){ m_nbT1UV++ ;m_nbT1UV_singleHit++;}
       if( isT2(hit) && hit->isX()) { m_nbT2X++; m_nbT2X_singleHit++;}
       if( isT2(hit) && !hit->isX()){ m_nbT2UV++ ;m_nbT2UV_singleHit++;}
       if( isT3(hit) && hit->isX()) { m_nbT3X++; m_nbT3X_singleHit++;}
       if( isT3(hit) && !hit->isX()){ m_nbT3UV++ ;m_nbT3UV_singleHit++;}
       if( isT1(hit) ){ m_nbT1++; m_nbT1_singleHit++; }
       if( isT2(hit) ){ m_nbT2++; m_nbT2_singleHit++; }
       if( isT3(hit) ){ m_nbT3++; m_nbT3_singleHit++; }
    }else{
      if( 2 == m_planeList[plane]){ //if you are adding a hit where already present one singlecounting decreased
        m_nbFiredLayers_singleHit --;
        if( isT1(hit) && hit->isX()) { m_nbT1X_singleHit--;}
        if( isT1(hit) && !hit->isX()){ m_nbT1UV_singleHit--;}
        if( isT2(hit) && hit->isX()) { m_nbT2X_singleHit--;}
        if( isT2(hit) && !hit->isX()){ m_nbT2UV_singleHit--;}
        if( isT3(hit) && hit->isX()) {  m_nbT3X_singleHit--;}
        if( isT3(hit) && !hit->isX()){ m_nbT3UV_singleHit--;}
        if(isT1(hit)){ m_nbT1_singleHit--; }
        if(isT2(hit)){ m_nbT2_singleHit--; }
        if(isT3(hit)){ m_nbT3_singleHit--; }
      }
    }
    return m_nbFiredLayers;
  }
  
  int removeHit( const PrHit* hit){
    //if(hit->isUsed() ) return m_nbFiredLayers;
    m_nbHits--;
    unsigned int plane = hit->planeCode();
    unsigned int NumberInLayerAfterRemove = m_planeList[plane] -1;
    m_planeList[plane]--;
    if( 0 == NumberInLayerAfterRemove){ //you remain with 0 hits in that plane decrease everything
      m_nbFiredLayers_singleHit --;
      m_nbFiredLayers --;
      if( isT1(hit) && hit->isX()) {m_nbT1X--; m_nbT1X_singleHit--;}
      if( isT1(hit) && !hit->isX()){ m_nbT1UV-- ;m_nbT1UV_singleHit--;}
      if( isT2(hit) && hit->isX()) { m_nbT2X--; m_nbT2X_singleHit--; }
      if( isT2(hit) && !hit->isX()){ m_nbT2UV-- ;m_nbT2UV_singleHit--;}
      if( isT3(hit) && hit->isX()) { m_nbT3X--; m_nbT3X_singleHit--; }
      if( isT3(hit) && !hit->isX()){ m_nbT3UV-- ;m_nbT3UV_singleHit--;}
      if( isT1(hit)){ m_nbT1--; m_nbT1_singleHit--; }
      if(isT2(hit)){ m_nbT2--; m_nbT2_singleHit--; }
      if(isT3(hit)){ m_nbT3--; m_nbT2_singleHit--; }
    }
    if( 1 == NumberInLayerAfterRemove) //you remain with just one hit in that layer after removing it
    {
      m_nbFiredLayers_singleHit++;
      if( isT1(hit) && !hit->isX()){ m_nbT1UV_singleHit++;}
      if( isT1(hit) && hit->isX()) { m_nbT1X_singleHit++;  }
      if( isT2(hit) && hit->isX()) { m_nbT2X_singleHit++; }
      if( isT2(hit) && !hit->isX()){ m_nbT2UV_singleHit++;}
      if( isT3(hit) && hit->isX()) { m_nbT3X_singleHit++; }
      if( isT3(hit) && !hit->isX()){ m_nbT3UV_singleHit++;}
      if( isT1(hit)){ m_nbT1_singleHit++; }
      if( isT2(hit)){ m_nbT2_singleHit++; }
      if( isT3(hit)){ m_nbT3_singleHit++; }
    }
    return m_nbFiredLayers;
  }
  unsigned int nbDifferent() const{ return m_nbFiredLayers;}
  unsigned int nbSingle() const{ return m_nbFiredLayers_singleHit;}
  unsigned int nbDifferentX() const { return m_nbT1X + m_nbT2X + m_nbT3X; }
  unsigned int nbSingleX() const{ return (m_nbT1X_singleHit + m_nbT2X_singleHit + m_nbT3X_singleHit);}
  unsigned int nbDifferentUV() const{ return (m_nbT1UV + m_nbT2UV + m_nbT3UV);}
  unsigned int nbSingleUV() const{return (m_nbT1UV_singleHit + m_nbT2UV_singleHit + m_nbT3UV_singleHit);}
  unsigned int nbHits() const{return m_nbHits;}
  
  
  unsigned int nbDifferentUVIter(PrHits::iterator itBeg, PrHits::iterator itEnd) {
    unsigned int m_uvlist[12];
    unsigned int nDiff = 0;
    std::fill( m_uvlist,m_uvlist+12,0);
    for( PrHits::iterator it = itBeg; itEnd!=it ; ++it){
      if((*it)->isX()) continue;
      if( m_uvlist[ (*it)->planeCode() ] == 0){
        m_uvlist[(*it)->planeCode()]++; nDiff++;
      }
    }
    return nDiff;
  }
  
  //  bool isOKHole() const{
    
  //}
  
  bool isOK2() const{
    return ( isOK() && isOKUV() && (m_nbT1X+m_nbT1UV>=3) && (m_nbT2X+m_nbT2UV>=3) && (m_nbT3X+m_nbT3UV>=3));
  }
  
  bool isOKUV() const{
    //at least 1 UV hit in each station & at least one station with 2 UV hits
    return ( (m_nbT1UV>=1 && m_nbT2UV>=1 && m_nbT3UV>=1) && 
             ( m_nbT1UV>1 || m_nbT2UV>1 || m_nbT3UV>1) );
    return false;
  }
  bool isOKX() const{
    return ( (m_nbT1X>=1 && m_nbT2X>=1 && m_nbT3X>=1) && 
             (m_nbT1X>1 || m_nbT2X>1 || m_nbT3X>1));
    return false;
  }
  bool isOK() const{ 
    return ( isOKX() && isOKUV() );
    return false;
  }
  unsigned int nbInPlane( const int plane) const{return m_planeList[plane];}
  //Checkers for the Hits
  bool isT1(const PrHit *hit)const{
    if( hit->planeCode()>-1 && hit->planeCode() <4){
      return true;
    }
    return false;
  }
  bool isT2(const PrHit *hit)const{
    if(hit->planeCode() >3 && hit->planeCode() <8){
      return true;
    }
    return false;
  }
  
  bool isT3(const PrHit *hit)const{
    if( hit->planeCode() >7 && hit->planeCode() < 12){
      return true;
    }
    return false;
  }

  ~PrPlaneHybridCounter( ) {} ///< Destructor
protected:
  
private:
  unsigned int m_nbUsed;
  unsigned int m_nbFiredLayers; 
  
  unsigned int m_nbFiredLayers_singleHit;
  std::array<unsigned int,12> m_planeList;
  unsigned int m_nbT1X;
  unsigned int m_nbT1X_singleHit;
  
  unsigned int m_nbT2X;
  unsigned int m_nbT2X_singleHit;
  
  unsigned int m_nbT3X;
  unsigned int m_nbT3X_singleHit;

  unsigned int m_nbT1UV;
  unsigned int m_nbT1UV_singleHit;
  
  unsigned int m_nbT2UV;
  unsigned int m_nbT2UV_singleHit;
  
  unsigned int m_nbT3UV;
  unsigned int m_nbT3UV_singleHit;

  unsigned int m_nbT1;
  unsigned int m_nbT1_singleHit;

  unsigned int m_nbT2;
  unsigned int m_nbT2_singleHit;
  
  unsigned int m_nbT3;
  unsigned int m_nbT3_singleHit;
  
  unsigned int m_nbHits;
  
};
#endif // PRPLANEHYBRIDCOUNTER_H
