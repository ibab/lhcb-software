#ifndef PATPV_PATVERTEXSTATE_H 
#define PATPV_PATVERTEXSTATE_H 1

// Include files
// -------------

// From TrgEvent
#include "Event/Track.h"

/** @class PatVertexState PatVertexState.h
 *  
 *
 *  @author Eduardo Rodrigues
 *  @date   2004-02-18
 */ 

// simple representation of track for vertexing
class localTrack {
 public:
  localTrack() {
    pTr3d = 0;
    vertexNb = 0;
    x  = 0.;
    y  = 0.;
    z  = 0.;
    tx = 0.;
    ty = 0.;
    errX2 = 0.;
    errY2 = 0.;
    backward = false;
  }

  ~localTrack() {}

  LHCb::Track* pTr3d;         // pointer to Track3D
  unsigned int    vertexNb;      // index of PV the track belongs to
  double x;
  double y;
  double z;
  double tx;
  double ty;
  double errX2;
  double errY2;
  bool backward;

};



class PatVertexState {
public:
  // Constructor
  PatVertexState()
  {
    m_vertexPosition.SetX( 0. );
    m_vertexPosition.SetY( 0. );
    m_vertexPosition.SetZ( 0. );

    m_xSum     = 0.0;
    m_xSumSl   = 0.0;
    m_xSumX    = 0.0;
    m_xSumSlX  = 0.0;
    m_xSumSlZ  = 0.0;
    m_xSumSl2  = 0.0;
    m_xSumSl2Z = 0.0;
    m_ySum     = 0.0;
    m_ySumSl   = 0.0;
    m_ySumY    = 0.0;
    m_ySumSlY  = 0.0;
    m_ySumSlZ  = 0.0;
    m_ySumSl2  = 0.0;
    m_ySumSl2Z = 0.0;

    m_vertexContent.reserve( 200 );

    m_nbTracks = 0;
    m_nbBackTracks = 0;

    m_isUseful = true;
  }
  
  // Destructor
  ~PatVertexState() {};

  void addTrackInTrgVertex( localTrack *lotra ) 
  {

    double x = lotra->x;
    double y = lotra->y;
    double z = lotra->z;
    
    double slX = lotra->tx;
    double slY = lotra->ty;
    
    double errX2 = lotra->errX2;
    double errY2 = lotra->errY2;

    m_xSum     += ( 1. / errX2 );
    m_xSumSl   += ( slX / errX2 );
    m_xSumX    += ( x / errX2 );
    m_xSumSlX  += ( x * slX / errX2 );
    m_xSumSlZ  += ( z * slX / errX2 );
    m_xSumSl2  += ( slX * slX / errX2 );
    m_xSumSl2Z += ( slX * slX * z / errX2 );
    
    m_ySum     += ( 1. / errY2 );
    m_ySumSl   += ( slY / errY2 );
    m_ySumY    += ( y / errY2 );
    m_ySumSlY  += ( y * slY / errY2 );
    m_ySumSlZ  += ( z * slY / errY2 );
    m_ySumSl2  += ( slY * slY / errY2 );
    m_ySumSl2Z += ( slY * slY * z / errY2 );

    lotra->vertexNb =  m_vertexID ;
    m_vertexContent.push_back( lotra );
    m_nbTracks ++;
    if ( lotra->backward ) m_nbBackTracks++;
  }
  
  void computeTrgVertex()
  {
    double num, denom;
    
    num = 
      m_xSumSl * ( m_xSumX - m_xSumSlZ ) / m_xSum +
      m_ySumSl * ( m_ySumY - m_ySumSlZ ) / m_ySum -
      ( m_xSumSlX + m_ySumSlY ) + m_xSumSl2Z + m_ySumSl2Z;
    denom = m_xSumSl2 + m_ySumSl2 
      - m_xSumSl * m_xSumSl / m_xSum
      - m_ySumSl * m_ySumSl / m_ySum;
    
    m_vertexPosition.SetZ( num / denom) ;
    m_vertexPosition.SetX( ( m_xSumSl * m_vertexPosition.z() + 
                             m_xSumX - m_xSumSlZ ) / m_xSum) ;
    m_vertexPosition.SetY( ( m_ySumSl * m_vertexPosition.z() + 
                             m_ySumY - m_ySumSlZ ) / m_ySum) ; 

    updateErrorMatrix();
  }

  double computeChi2( double & chi2Max, localTrack* & trackMaxChi2 )
  {
    double chi2 = 0.0;
    double maxChi2 = 0.0;
    
    localTrack* maxTrack = 0;
    
    std::vector<localTrack*>::iterator it;
    
    for( it = m_vertexContent.begin() ; it != m_vertexContent.end() ; it ++ ) {
      localTrack* lotra = (*it);

      if( lotra->vertexNb != m_vertexID ) continue;  
     
      double chi2Temp = chi2Contribution( lotra );

      chi2 += chi2Temp;
      if( chi2Temp > maxChi2 ) {
        maxChi2 = chi2Temp;
        maxTrack = lotra;
      } 
    }
    
    double nDOF = 2. * m_nbTracks - 3.;
    
    chi2Max = maxChi2 / nDOF;
    trackMaxChi2 = maxTrack;

    m_chi2 = chi2 / nDOF;
    
    return( m_chi2 );
  }

  double chi2() { return m_chi2; }
  
  double chi2Contribution( localTrack *lotra ) 
  {
    if( lotra->vertexNb != m_vertexID ) return( - 1 );
    
    double tempX = m_vertexPosition.x() - 
      ( lotra->x + lotra->tx * ( m_vertexPosition.z() - lotra->z ) );
    double tempY = m_vertexPosition.y() - 
      ( lotra->y + lotra->ty * ( m_vertexPosition.z() - lotra->z ) );

    return( tempX * tempX / lotra->errX2 + 
            tempY * tempY / lotra->errY2 );
  }

  double chi2ContributionNormalized( localTrack *lotra ) 
  {
    double chi2Temp = chi2Contribution( lotra );
    double nDOF = 2. * m_nbTracks - 3.;
    return( chi2Temp / nDOF );
  }
  
  
  int nbTracksInTrgVertex() { return m_nbTracks; }
  int nbBackTracksInTrgVertex() { return m_nbBackTracks; }

  void rejectTrack( localTrack* lotra ) 
  {
    lotra->vertexNb = 0 ; // no more included in the vertex search
    
    double x = lotra->x;
    double y = lotra->y;
    double z = lotra->z;
    
    double slX = lotra->tx;
    double slY = lotra->ty;
    
    double errX2 = lotra->errX2;
    double errY2 = lotra->errY2;

    m_xSum     -= ( 1. / errX2 );
    m_xSumSl   -= ( slX / errX2 );
    m_xSumX    -= ( x / errX2 );
    m_xSumSlX  -= ( x * slX / errX2 );
    m_xSumSlZ  -= ( z * slX / errX2 );
    m_xSumSl2  -= ( slX * slX / errX2 );
    m_xSumSl2Z -= ( slX * slX * z / errX2 );
    
    m_ySum     -= ( 1. / errY2 );
    m_ySumSl   -= ( slY / errY2 );
    m_ySumY    -= ( y / errY2 );
    m_ySumSlY  -= ( y * slY / errY2 );
    m_ySumSlZ  -= ( z * slY / errY2 );
    m_ySumSl2  -= ( slY * slY / errY2 );
    m_ySumSl2Z -= ( slY * slY * z / errY2 );

    m_nbTracks --;
    if (lotra->backward) m_nbBackTracks--;
    
  }

  void updateErrorMatrix() {
    double det = m_xSum * m_ySum * ( m_xSumSl2 + m_ySumSl2 )
      - m_xSum * m_ySumSl * m_ySumSl - m_ySum * m_xSumSl * m_xSumSl;
    
    m_errorMatrix( 0, 0 ) = 
      ( m_ySum * ( m_xSumSl2 + m_ySumSl2 ) - m_ySumSl * m_ySumSl ) / det;
    m_errorMatrix( 1, 1 ) = 
      ( m_xSum * ( m_xSumSl2 + m_ySumSl2 ) - m_xSumSl * m_xSumSl ) / det;
    m_errorMatrix( 2, 2 ) = m_xSum * m_ySum / det;
    m_errorMatrix( 0, 1 ) = m_xSumSl * m_ySumSl / det;
    m_errorMatrix( 0, 2 ) = m_ySum * m_xSumSl / det;
    m_errorMatrix( 1, 2 ) = m_xSum * m_ySumSl / det;
  }
  
  Gaudi::SymMatrix3x3* getErrorMatrix() { return &m_errorMatrix; }

  double getEstimatedErrorX2() { return m_errorMatrix( 0, 0 ); }
  double getEstimatedErrorY2() { return m_errorMatrix( 1, 1 ); }
  double getEstimatedErrorZ2() { return m_errorMatrix( 2, 2 ); }

  std::vector<localTrack*>* trgVertexContent(){ return &m_vertexContent; }

  bool isUseful() 
  { return( m_isUseful ); }
  
  void setUsefulness( bool usefulness ) { m_isUseful = usefulness; }

  int nbTracksUsedForVertexing() 
  {
    int res = 0;
    std::vector<localTrack*>::iterator it;
    for( it  = m_vertexContent.begin() ;
         it != m_vertexContent.end() ; it ++ ) {
      if( m_vertexID == (*it)->vertexNb ) res ++;
    }
    return( res );  
  }

  unsigned int vertexID() { return( m_vertexID ); }

  void setVertexID( unsigned int id ) { m_vertexID = id; }

  Gaudi::XYZPoint position() { return m_vertexPosition; }

  class sortTrgVertexByPosition {
  public: 
    bool operator() ( PatVertexState* first, PatVertexState* second ) const {
      return first->position().z() < second->position().z();
    }
  };
  
  class sortTrgVertexByTrackNum {
  public:
    bool operator() ( PatVertexState* first, PatVertexState* second ) const {
      return first->nbTracksInTrgVertex() > second->nbTracksInTrgVertex();
    }
  };

private:
  Gaudi::SymMatrix3x3 m_errorMatrix;

  Gaudi::XYZPoint m_vertexPosition;

  double m_xSum, m_xSumSl, m_xSumX, m_xSumSlX, m_xSumSlZ, m_xSumSl2, m_xSumSl2Z;
  double m_ySum, m_ySumSl, m_ySumY, m_ySumSlY, m_ySumSlZ, m_ySumSl2, m_ySumSl2Z;

  std::vector<localTrack*> m_vertexContent;

  int m_nbTracks;  
  int m_nbBackTracks;  

  bool m_isUseful;

  unsigned int m_vertexID;

  double m_chi2;
};

typedef std::vector<PatVertexState*> PatVertexStates;

#endif // PATPV_PATVERTEXSTATE_H
