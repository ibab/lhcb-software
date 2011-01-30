// Include files 
// -------------
#include "TriangularInverter.h"
#include <vector>


//-----------------------------------------------------------------------------
// Implementation file for tools of namespace TriangularInversion
//
//  Created: 2011-01-20  Victor Coco
//-----------------------------------------------------------------------------


//=============================================================
//  Fast inversion of upper triangular matrix
//=============================================================
void TriangularInversion::invertUpperTriangular( const Gaudi::TrackMatrix& F,
			    Gaudi::TrackMatrix& Finv) {
  int N = F.kRows ; 
  for (int  ii = 0 ; ii < N ; ++ii){
    for (int  i = N - 1; i > -1 ; --i){
      if (ii == i) Finv(i,ii) = 1.;
      else Finv(i,ii) = 0.;
      for ( int j = i + 1; j < N; j++){
	Finv(i,ii) -=  F(i,j) * Finv(j,ii) ;
      }
	Finv(i,ii) /= F(i,i) ; 
    }
  } 
}


//=============================================================
//  Fast inversion of almost upper triangular matrix
//  almost means one that the element under diagonal at line "anomaly"
//  is non-null.
//=============================================================

void TriangularInversion::invertAlmostUpperTriangular( const Gaudi::TrackMatrix& F,
				  Gaudi::TrackMatrix& Finv,
				  const int& anomaly ) {
  if ( anomaly == 0 ) return;
  int N = F.kRows ; 
  for (int  ii = 0 ; ii < N ; ++ii){
    for (int  i = N - 1; i > -1 ; --i){
      if (ii == i) Finv(i,ii) = 1.;
      else Finv(i,ii) = 0.;
      for ( int j = i + 1; j < N; j++){
	Finv(i,ii) -=  F(i,j) * Finv(j,ii) ;
      }
      Finv(i,ii) /= F(i,i) ; 
      if ( i == anomaly-1 ){
	Finv(i,ii) *= F(i,i) * F(i+1,i+1) ;
	Finv(i,ii) /=( F(i,i) * F(i+1,i+1) - F(i+1,i)*F(i,i+1))  ;
	Finv(i+1,ii) -= Finv(i,ii)*F(i+1,i)/F(i+1,i+1) ;
      }
    }
  } 
}

//=============================================================
//  Compute similarity matrix of with upper triangular matrix
//=============================================================

void TriangularInversion::similarityUpperTriangular( const Gaudi::TrackMatrix& F,
							   const Gaudi::TrackMatrix& orig,
							   Gaudi::TrackSymMatrix& target ) {
  int N = F.kRows ; 
  for (int  i = 0; i < N ; ++i ){
    for (int  j = 0; j < N ; ++j ){
      target(i,j) = 0.;
      for (int  l = j ; l < N ; ++l ){
	for (int  k = i ; k < N ; ++k ){
	  target(i,j)+=F(j,l)*F(i,k)*orig(k,l);
	}
      }
    }
  }
}



//=============================================================
//  Compute similarity matrix of with almost upper triangular matrix
//  almost means one that the element under diagonal at line "anomaly"
//  is non-null.
//=============================================================

void TriangularInversion::similarityAlmostUpperTriangular( const Gaudi::TrackMatrix& F,
							   const Gaudi::TrackMatrix& orig,
							   Gaudi::TrackSymMatrix& target,
							   const int& anomaly ) {
  if ( anomaly == 0 ) return;
  int N = F.kRows ; 
  std::vector<int> indexSum(N,0.);
  for (int  i = 0; i < N ; ++i )
    indexSum[i]=i;

  indexSum[anomaly]=anomaly-1;
  for (int  i = 0; i < N ; ++i ){
    for (int  j = 0; j < N ; ++j ){
      target(i,j) = 0.;
      for (int  l = indexSum[j]; l < N ; ++l ){
	for (int  k = indexSum[i]; k < N ; ++k ){
	  target(i,j)+=F(j,l)*F(i,k)*orig(k,l);
	}
      }
    }
  }
}

