// $Id: RichG4ReconResult.h,v 1.1 2004-02-04 13:53:01 seaso Exp $
#ifndef RICHANALYSIS_RICHG4RECONRESULT_H 
#define RICHANALYSIS_RICHG4RECONRESULT_H 1

// Include files
//

/** @class RichG4ReconResult RichG4ReconResult.h RichAnalysis/RichG4ReconResult.h
 *  
 *
 *  @author Sajan EASO
 *  @date   2003-10-28
 */
class RichG4ReconResult {
public:
  /// Standard constructor
  RichG4ReconResult( ); 

  void InitReconResult();
  
  virtual ~RichG4ReconResult( ); ///< Destructor


  inline int  radiatorForCkv() 
  {return  m_radiatorForCkv;
  
  }
  
  inline double ckvAngleD1E1() 
  { return  m_ckvAngleD1E1;
  }
  inline double ckvAngleD2E1() 
  { return  m_ckvAngleD2E1;
  }
  inline double ckvAngleD3E1() 
  { return  m_ckvAngleD3E1;
  }
  
  inline double ckvAngleD3E2() 
  { return  m_ckvAngleD3E2;
  }
  inline double ckvAngleD3E4() 
  { return  m_ckvAngleD3E4;
  }

   inline double ckvAngleD1E2() 
  { return  m_ckvAngleD1E2;
  }

   inline double ckvAngleD1E4() 
  { return  m_ckvAngleD1E4;
  }

   inline double ckvAngleD1E3() 
  { return  m_ckvAngleD1E3;
  }
   inline double ckvAngleD2E3() 
  { return  m_ckvAngleD2E3;
  }
   
   inline double ckvAngleD3E3() 
  { return  m_ckvAngleD3E3;
  }

  void setradiatorForCkv(int aRadiator)
  {
    m_radiatorForCkv= aRadiator;
    
  }

  void setckvAngleD1E1( double ackvD1E1) 
  {
    m_ckvAngleD1E1= ackvD1E1;
    
  }
  void setckvAngleD2E1( double ackvD2E1) 
  {
    m_ckvAngleD2E1= ackvD2E1;
    
  }
  void setckvAngleD3E1( double ackvD3E1) 
  {
    m_ckvAngleD3E1= ackvD3E1;
    
  }
  void setckvAngleD3E2( double ackvD3E2) 
  {
    m_ckvAngleD3E2= ackvD3E2;
    
  }
  void setckvAngleD3E4( double ackvD3E4) 
  {
    m_ckvAngleD3E4= ackvD3E4;
    
  }

  void setckvAngleD1E2( double ackvD1E2) 
  {
    m_ckvAngleD1E2= ackvD1E2;
    
  }

  void setckvAngleD1E4( double ackvD1E4) 
  {
    m_ckvAngleD1E4= ackvD1E4;
    
  }

  void setckvAngleD1E3( double ackvD1E3) 
  {
    m_ckvAngleD1E3= ackvD1E3;
    
  }
  void setckvAngleD2E3( double ackvD2E3) 
  {
    m_ckvAngleD2E3= ackvD2E3;
    
  }
  void setckvAngleD3E3( double ackvD3E3) 
  {
    m_ckvAngleD3E3= ackvD3E3;
    
  }
  
protected:

private:

  int m_radiatorForCkv;  // radiator number 0,1,2.
  
  double m_ckvAngleD1E1;
  double m_ckvAngleD2E1;
  double m_ckvAngleD3E1;
  double m_ckvAngleD3E2;
  double m_ckvAngleD1E2;
  double m_ckvAngleD3E4;
  double m_ckvAngleD1E4;

  double m_ckvAngleD1E3;
  double m_ckvAngleD2E3;
  double m_ckvAngleD3E3;

  // D1,D2,D3 refer to the detection point determined from
  // pixelnum, true hit coord, global phcathode coord respectively.
  // E1, E2, E3 refer to the emission point which is the
  // true emission point, mid point of radiator, aerogel exit
  //  point respctively.
       

};
#endif // RICHANALYSIS_RICHG4RECONRESULT_H
