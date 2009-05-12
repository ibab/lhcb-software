#include "DecayTreeFitter/VtxFitParams.h"

namespace LHCb
{

  VtxFitParams::VtxFitParams( double charge,
			      const Gaudi::XYZPoint& pos,
			      const Gaudi::LorentzVector& p4,
			      const Gaudi::SymMatrix7x7& cov7)
    : 
    m_q(charge),
    m_pos(pos),
    m_p4(p4),
    m_posCovMatrix(cov7.Sub<Gaudi::SymMatrix3x3>(0,0)),
    m_momCovMatrix(cov7.Sub<Gaudi::SymMatrix4x4>(3,3)),
    m_momPosCovMatrix(cov7.Sub<Gaudi::Matrix4x3>(3,0))
  {
  }
 
  VtxFitParams::VtxFitParams( double charge,
			      const Gaudi::XYZPoint& pos,
			      const Gaudi::LorentzVector& p4,
			      double decaylength,
			      const Gaudi::SymMatrix8x8& cov8) 
    :
    m_q(charge),
    m_pos(pos),
    m_p4(p4),
    m_len(decaylength),
    m_posCovMatrix(cov8.Sub<Gaudi::SymMatrix3x3>(0,0)),
    m_momCovMatrix(cov8.Sub<Gaudi::SymMatrix4x4>(3,3)),
    m_lenCovMatrix(cov8.Sub<Gaudi::SymMatrix1x1>(7,7)),
    m_momPosCovMatrix(cov8.Sub<Gaudi::Matrix4x3>(3,0)),
    m_lenPosCovMatrix(cov8.Sub<Gaudi::Matrix1x3>(7,0)),
    m_lenMomCovMatrix(cov8.Sub<Gaudi::Matrix1x4>(7,3))
  {
  }
  
  Gaudi::SymMatrix8x8
  fillCov8(const Gaudi::SymMatrix3x3& posCovMatrix,
	   const Gaudi::SymMatrix4x4& momCovMatrix,
	   const Gaudi::SymMatrix1x1& lenCovMatrix,
	   const Gaudi::Matrix4x3&    momPosCovMatrix,
	   const Gaudi::Matrix1x3&    lenPosCovMatrix,
	   const Gaudi::Matrix1x4&    lenMomCovMatrix)
  {
    Gaudi::SymMatrix8x8 cov ;
    cov.Place_at(posCovMatrix,0,0);
    cov.Place_at(momCovMatrix,3,3);
    cov.Place_at(lenCovMatrix,7,7);
    for(int imom =0; imom<4; ++imom)
      for(int ipos=0 ; ipos<3; ++ipos)
	cov(imom+3,ipos) = momPosCovMatrix(imom,ipos);
    for(int imom =0; imom<4; ++imom)
      for(int ilen =0; ilen<1; ++ilen)
	cov(ilen+7,imom+3) = lenMomCovMatrix(ilen,imom);
    for(int ipos=0 ; ipos<3; ++ipos)
      for(int ilen =0; ilen<1; ++ilen)
	cov(ilen+7,ipos) = lenPosCovMatrix(ilen,ipos);
    return cov ;
  }

  Gaudi::SymMatrix4x4
  fillCov4(const Gaudi::SymMatrix4x4& momCovMatrix,
	   const Gaudi::SymMatrix1x1& lenCovMatrix,
	   const Gaudi::Matrix1x4&    lenMomCovMatrix)
  {
    // construct the (p3,len) cov matrix
    Gaudi::SymMatrix4x4 cov ;
    cov.Place_at(momCovMatrix.Sub<Gaudi::SymMatrix3x3>(0,0),0,0);
    cov.Place_at(lenCovMatrix,3,3);
    for(int imom =0; imom<3; ++imom)
      for(int ilen =0; ilen<1; ++ilen)
	cov(ilen+3,imom) = lenMomCovMatrix(ilen,imom);
    return cov ;
  }
  
  Gaudi::SymMatrix5x5
  fillCov5(const Gaudi::SymMatrix4x4& momCovMatrix,
	   const Gaudi::SymMatrix1x1& lenCovMatrix,
	   const Gaudi::Matrix1x4&    lenMomCovMatrix)
  {
    // construct the (p4,len) cov matrix
    Gaudi::SymMatrix5x5 cov ;
    cov.Place_at(momCovMatrix,0,0);
    cov.Place_at(lenCovMatrix,4,4);
    for(int imom =0; imom<4; ++imom)
      for(int ilen =0; ilen<1; ++ilen)
	cov(ilen+4,imom) = lenMomCovMatrix(ilen,imom);
    return cov ;
  }
  
  Gaudi::SymMatrix8x8
  VtxFitParams::cov8() const
  {
    return fillCov8( m_posCovMatrix,m_momCovMatrix,m_lenCovMatrix,
		     m_momPosCovMatrix,m_lenPosCovMatrix,m_lenMomCovMatrix) ;
  }

  Gaudi::Vector8
  VtxFitParams::par8() const
  {
    Gaudi::Vector8 rc ;
    rc(0) = m_pos.X() ;
    rc(1) = m_pos.Y() ;
    rc(2) = m_pos.Z() ;
    rc(3) = m_p4.Px() ;
    rc(4) = m_p4.Py() ;
    rc(5) = m_p4.Pz() ;
    rc(6) = m_p4.E() ;
    rc(7) = m_len ;
    return rc ;
  }

  double
  VtxFitParams::setMass( double fixedmass )
  {
    // applies a mass constrained. bail out if error on mass is very
    // small, because then the mass constrained was already applied.
    double chisq = 0 ;
    
    double px = m_p4.Px() ;
    double py = m_p4.Py() ;
    double pz = m_p4.Pz() ;
    double E  = m_p4.E() ;
    double m2 = E*E - (px*px + py*py + pz*pz) ;
    double m  = std::sqrt(m2) ;
   
    Gaudi::Matrix1x4 mjacobian ;
    mjacobian(0,0) = - px/m ;
    mjacobian(0,1) = - py/m ;
    mjacobian(0,2) = - pz/m ;
    mjacobian(0,3) =   E/m ;
    double mcov = ROOT::Math::Similarity( mjacobian, m_momCovMatrix )(0,0) ;
    
    if( mcov < 1e-3 ) { // bah
      std::cout << "Mass constraint already applied ... bailing out " << std::sqrt(mcov) << std::endl ;
    } else {
      
      // the trick is to rotate first everything from (p3,E) to
      // (p3,m2). we then filter the mass constrained. after that we
      // rotate back.
      
      // first transform to the (px,py,pz,m2) basis. since the jacobian is
      // rather empty, this could be optimized, another time.
      ROOT::Math::SMatrix<double,4,4> p4jacobian ; // = dQnew/dQold
      for(size_t col=0; col<3; ++col) p4jacobian(col,col) = 1 ;
      p4jacobian(3,0) = -2*px ;
      p4jacobian(3,1) = -2*py ;
      p4jacobian(3,2) = -2*pz ;
      p4jacobian(3,3) = 2*E ;

      //std::cout << "cov8 before mass constrained: " << std::endl << cov8() << std::endl ;
      
      // now fill the 8x8 cov matrix using the transformed momentum.
      Gaudi::SymMatrix8x8 rotatedCov8 = 
	fillCov8( m_posCovMatrix,
		  ROOT::Math::Similarity(p4jacobian,m_momCovMatrix),
		  m_lenCovMatrix,
		  p4jacobian*m_momPosCovMatrix,
		  m_lenPosCovMatrix,
		  m_lenMomCovMatrix*ROOT::Math::Transpose(p4jacobian)) ;
      
      // and fill the parameter vector
      Gaudi::Vector8 rotatedPar8 = par8() ;
      rotatedPar8(6) = m2 ;
      
      // apply the constraint, which is trivial now
      double residual  = fixedmass*fixedmass - m2;
      double residualV = rotatedCov8(6,6) ;
      chisq = residual* residual/residualV ;
      
      Gaudi::SymMatrix8x8 rotatedCov8New = rotatedCov8 ;
      for(int irow=0; irow<8; ++irow) {
	rotatedPar8(irow) += residual/residualV * rotatedCov8(irow,6) ;
	for(int icol=0; icol<=irow; ++icol)
	  rotatedCov8New(irow,icol) -= rotatedCov8(irow,6) * 1/residualV * rotatedCov8(6,icol)  ;
      }
      rotatedCov8 = rotatedCov8New ;

      //std::cout << "rot cov8 after mass constrained: " << std::endl << rotatedCov8 << std::endl ;
      
      // extract result, rotate back
      px = rotatedPar8(3) ;
      py = rotatedPar8(4) ;
      pz = rotatedPar8(5) ;
      m2 = rotatedPar8(6) ;
      E = std::sqrt( px*px+py*py+pz*pz + m2) ;
      
      p4jacobian  = ROOT::Math::SMatrix<double,4,4>() ; 
      p4jacobian(0,0) = p4jacobian(1,1) = p4jacobian(2,2) = 1 ;
      p4jacobian(3,0) = px/E ;
      p4jacobian(3,1) = py/E ;
      p4jacobian(3,2) = pz/E ;
      p4jacobian(3,3) = 1/(2*E) ;
      
      m_posCovMatrix    = rotatedCov8.Sub<Gaudi::SymMatrix3x3>(0,0) ;
      m_momCovMatrix    = ROOT::Math::Similarity( p4jacobian, rotatedCov8.Sub<Gaudi::SymMatrix4x4>(3,3)) ;
      m_lenCovMatrix    = rotatedCov8.Sub<Gaudi::SymMatrix1x1>(7,7) ;
      m_momPosCovMatrix =  p4jacobian * rotatedCov8.Sub<Gaudi::Matrix4x3>(3,0) ;
      m_lenPosCovMatrix = rotatedCov8.Sub<Gaudi::Matrix1x3>(7,0) ;
      m_lenMomCovMatrix = rotatedCov8.Sub<Gaudi::Matrix1x4>(7,3) * ROOT::Math::Transpose( p4jacobian ) ;

      //std::cout << "cov8 after mass constrained: " << std::endl << cov8() << std::endl ;
      
      m_pos.SetX( rotatedPar8(0) ) ;
      m_pos.SetY( rotatedPar8(1) ) ;
      m_pos.SetZ( rotatedPar8(2) ) ;
      m_p4.SetPx( px ) ;
      m_p4.SetPy( py ) ;
      m_p4.SetPz( pz ) ;
      m_p4.SetE( E ) ;
      m_len = rotatedPar8( 7 ) ;
    }
    
    return chisq ;
  }

  VtxDoubleErr VtxFitParams::properDecayTime( double fixedmass ) const 
  {
    VtxFitParams copy = *this ;
    copy.setMass(fixedmass) ;
    return copy.properDecayTime() ; 
  }

  template<class T> T correlationMatrix( const T& m )
  {
    T rc ;
    for( int row=0; row<T::kRows; ++row)
      for( int col=0; col<=row; ++col)
	rc(row,col) = m(row,col)/std::sqrt(m(row,row)*m(col,col)) ;
    return rc ;
  }

  VtxDoubleErr VtxFitParams::properDecayTime() const 
  {
    // tau = len * mass / | p3 |
    double px  = m_p4.Px() ;
    double py  = m_p4.Py() ;
    double pz  = m_p4.Pz() ;
    double E   = m_p4.E() ;
    double p2  = px*px+py*py+pz*pz ;
    double m2  = E*E - p2 ;
    double m   = std::sqrt( m2 ) ;
    double p   = std::sqrt(p2) ;
    double len = m_len ;
    double tau = len * m / p ;
    
    Gaudi::Matrix1x5 jacobian ;
    jacobian(0,0) = -px * tau * (1/m2 + 1/p2) ;
    jacobian(0,1) = -py * tau * (1/m2 + 1/p2) ;
    jacobian(0,2) = -pz * tau * (1/m2 + 1/p2) ;
    jacobian(0,3) =   E * tau * (1/m2) ;
    jacobian(0,4) = m / p ;

    Gaudi::SymMatrix5x5 cov5 = fillCov5(m_momCovMatrix, m_lenCovMatrix,m_lenMomCovMatrix) ;
    return VtxDoubleErr( tau, ROOT::Math::Similarity( jacobian, cov5 )(0,0) ) ;

  }
}
