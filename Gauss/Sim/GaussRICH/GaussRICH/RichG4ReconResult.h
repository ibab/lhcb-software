// $Id: RichG4ReconResult.h,v 1.2 2004-02-10 14:24:15 jonesc Exp $
#ifndef GAUSSRICH_RICHG4RECONRESULT_H
#define GAUSSRICH_RICHG4RECONRESULT_H 1

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

  inline int  radiatorForCkv() const { return  m_radiatorForCkv;}

  inline double ckvAngleD1E1() const { return  m_ckvAngleD1E1; }

  inline double ckvAngleD2E1() const { return  m_ckvAngleD2E1; }

  inline double ckvAngleD3E1() const { return  m_ckvAngleD3E1; }

  inline double ckvAngleD3E2() const { return  m_ckvAngleD3E2; }

  inline double ckvAngleD3E4() const { return  m_ckvAngleD3E4; }

  inline double ckvAngleD1E2() const { return  m_ckvAngleD1E2; }

  inline double ckvAngleD1E4() const { return  m_ckvAngleD1E4; }

  inline double ckvAngleD1E3() const { return  m_ckvAngleD1E3; }

  inline double ckvAngleD2E3() const { return  m_ckvAngleD2E3; }

  inline double ckvAngleD3E3() const { return  m_ckvAngleD3E3; }

  inline double ckvAngleD4E1() const { return  m_ckvAngleD4E1; }
  inline double ckvAngleD4E3() const { return  m_ckvAngleD4E3; }

  inline double ckvAngleD4E2() const { return  m_ckvAngleD4E2; }
  inline double ckvAngleD4E4() const { return  m_ckvAngleD4E4; }


  inline double ckvAngleD5E4() const  {  return m_ckvAngleD5E4;}
  inline double ckvAngleD5E1() const  {  return m_ckvAngleD5E1;}
  inline double ckvAngleD6E1() const  {  return m_ckvAngleD6E1;}
  inline double ckvAngleD7E1() const  {  return m_ckvAngleD7E1;}
  
  inline double ckvAngleD5E3() const  {  return m_ckvAngleD5E3;}
  inline double ckvAngleD6E3() const  {  return m_ckvAngleD6E3;}
  inline double ckvAngleD7E3() const  {  return m_ckvAngleD7E3;}


  

  void setradiatorForCkv( const int aRadiator)
  {
    m_radiatorForCkv= aRadiator;
  }

  void setckvAngleD1E1( const double ackvD1E1)
  {
    m_ckvAngleD1E1= ackvD1E1;
  }

  void setckvAngleD2E1( const double ackvD2E1)
  {
    m_ckvAngleD2E1= ackvD2E1;
  }

  void setckvAngleD3E1( const double ackvD3E1)
  {
    m_ckvAngleD3E1= ackvD3E1;
  }

  void setckvAngleD3E2( const double ackvD3E2)
  {
    m_ckvAngleD3E2= ackvD3E2;
  }

  void setckvAngleD3E4( const double ackvD3E4)
  {
    m_ckvAngleD3E4= ackvD3E4;
  }

  void setckvAngleD1E2( const double ackvD1E2)
  {
    m_ckvAngleD1E2= ackvD1E2;
  }

  void setckvAngleD1E4( const double ackvD1E4)
  {
    m_ckvAngleD1E4= ackvD1E4;
  }

  void setckvAngleD1E3( const double ackvD1E3)
  {
    m_ckvAngleD1E3= ackvD1E3;
  }

  void setckvAngleD2E3( const double ackvD2E3)
  {
    m_ckvAngleD2E3= ackvD2E3;
  }

  void setckvAngleD3E3( const double ackvD3E3)
  {
    m_ckvAngleD3E3= ackvD3E3;
  }

  void setckvAngleD4E1( const double ackvD4E1)
  {
    m_ckvAngleD4E1= ackvD4E1;
  }
  void setckvAngleD4E3( const double ackvD4E3)
  {
    m_ckvAngleD4E3= ackvD4E3;
  }

  void setckvAngleD4E2( const double ackvD4E2)
  {
    m_ckvAngleD4E2= ackvD4E2;
  }
  void setckvAngleD4E4( const double ackvD4E4)
  {
    m_ckvAngleD4E4= ackvD4E4;
  }

  void setckvAngleD5E4(const double ackvD5E4) 
  {m_ckvAngleD5E4=ackvD5E4;}
  void setckvAngleD5E1(const double ackvD5E1) 
  {m_ckvAngleD5E1=ackvD5E1;}
  void setckvAngleD6E1(const double ackvD6E1) 
  {m_ckvAngleD6E1=ackvD6E1;}
  void setckvAngleD7E1(const double ackvD7E1) 
  {m_ckvAngleD7E1=ackvD7E1;}
    
  void setckvAngleD5E3(const double ackvD5E3) 
  {m_ckvAngleD5E3=ackvD5E3;}
  void setckvAngleD6E3(const double ackvD6E3) 
  {m_ckvAngleD6E3=ackvD6E3;}
  void setckvAngleD7E3(const double ackvD7E3) 
  {m_ckvAngleD7E3=ackvD7E3;}


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

  double m_ckvAngleD4E1;
  double m_ckvAngleD4E3;
  double m_ckvAngleD4E2;
  double m_ckvAngleD4E4;

  double  m_ckvAngleD5E4;
  double  m_ckvAngleD5E1;
  double  m_ckvAngleD6E1;
  double  m_ckvAngleD7E1;
  
  double  m_ckvAngleD5E3;
  double  m_ckvAngleD6E3;
  double  m_ckvAngleD7E3;

  // D1,D2,D3,D4 refer to the detection point determined from
  // pixelnum, true hit coord, global phcathode QwExt coord respectively.
  // D5 D6 D7 refer to the hit coord from pixel num, true hit and ph cat hit all with
  // the refraction correction applied.
  // E1, E2, E3 refer to the emission point which is the
  // true emission point, mid point of radiator, aerogel exit
  //  point respctively.


};
#endif // GAUSSRICH_RICHG4RECONRESULT_H
