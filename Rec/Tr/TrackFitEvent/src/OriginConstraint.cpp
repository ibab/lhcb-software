#include "Event/OriginConstraint.h"
#include "TrackKernel/TrackStateVertex.h"
#include "Event/ChiSquare.h"

namespace LHCb
{
  OriginConstraint::OriginConstraint( const Gaudi::XYZPoint& point,
				      const Gaudi::SymMatrix3x3& covariance )
    : Measurement( Measurement::Origin, LHCb::LHCbID(), 0 ),
      m_origin(point)
  {
    m_weight = covariance ;
    m_weight.InvertChol() ;
  } ;

  LHCb::ChiSquare OriginConstraint::filter( LHCb::State& state,
					    const Gaudi::TrackVector& reference) const
  {
    // std::cout << "Before filter state= " 
    // 	      << state.stateVector() << " +/- ("
    // 	      << sqrt(state.covariance()(0,0)) << ","
    // 	      << sqrt(state.covariance()(1,1)) << ","
    // 	      << sqrt(state.covariance()(2,2)) << ","
    // 	      << sqrt(state.covariance()(3,3)) << ","
    // 	      << sqrt(state.covariance()(4,4)) << ")" << std::endl ;
    // std::cout << "Reference: " << reference << std::endl ;

    // TrackStateVertex is just what we need. Unfortunately, we have a
    // small problem: we want to linearize the vertex fit with the
    // reference track, not with the prediction. In the end, I
    // modified TrackStateVertex to make that possible. However, it
    // only works for one iteration ... !
    TrackStateVertex vertex(m_origin,m_weight,true) ;
    vertex.addTrack( state, reference ) ;
    vertex.fitOneStep() ;
    // WARNING: TrackStateVertex does not cache states. so, you need
    // to compute the chi2, before updating the states.
    double chi2 = vertex.chi2()  ;
    state.stateVector() = vertex.stateVector(0) ;
    state.covariance()  = vertex.stateCovariance(0) ;
    //std::cout << "OriginConstraint: chi2 " << vertex.chi2() << std::endl ;
    // we need to set the number of dofs by hand, because
    // trackstatevertex doesn't do it right when using an initial
    // vertex
    return LHCb::ChiSquare( chi2, 2 ) ;
  }


  // LHCb::ChiSquare OriginConstraint::chi2( const LHCb::State& unbiasedstate) const
  // {
  //   LHCb::State tmp( unbiasedstate ) ;
  //   return filter( tmp ) ;
  // }


    // {
    //   LHCb::State tmpstate(state) ;
    //   tmpstate.stateVector() = reference; 
    //   TrackStateVertex tmpvertex(m_origin,m_weight,true) ;
    //   tmpvertex.addTrack( tmpstate ) ;
    //   tmpvertex.fitOneStep() ;
    //   std::cout << "OriginConstraint: chi2 of reference: " << tmpvertex.chi2() << std::endl ;
    // }
    //   std::cout << "Alternative 1: just vertex the reference, one step: " 
    // 		<< vertex.chi2() << std::endl ;
    //   vertex.fit() ;
    //   std::cout << "Alternative 2: now iterate that fit:              " << vertex.chi2() << std::endl ;
    // }
    // {
    //   LHCb::State tmpstate(state) ;
    //   TrackStateVertex vertex(m_origin,m_weight,true) ;
    //   vertex.addTrack( tmpstate ) ;
    //   vertex.fit() ;
    //   std::cout << "Alternative 3: skip the reference altogether: " 
    // 		<< vertex.chi2() << std::endl ;
    // }



    // std::cout << "After filter state= " 
    // 	      << state.stateVector() << " +/- ("
    // 	      << sqrt(state.covariance()(0,0)) << ","
    // 	      << sqrt(state.covariance()(1,1)) << ","
    // 	      << sqrt(state.covariance()(2,2)) << ","
    // 	      << sqrt(state.covariance()(3,3)) << ","
    // 	      << sqrt(state.covariance()(4,4)) << ")" << std::endl ;

    // std::cout << "OriginConstraint::filter: " << vertex.chi2() << " " << vertex.fitStatus() << std::endl ;
    // std::cout << "Vertex position: "
    // 	      << vertex.position() << std::endl
    // 	      << vertex.covMatrix() << std::endl ;

    //state.stateVector() += vertex.stateVector(0) - reference ;
    //state.covariance() = vertex.stateCovariance(0) ;
    // std::cout << "After filter, state is: "
    // 	      << state << std::endl ;

    // OK,we need to be smart now. the chi2 contribution from the track is:
    //halfDChisqDX += -ROOT::Math::Transpose(m_A) * GB * residual(pos) ;

    // but we can choose the expansion point to be the track state, in
    // which case the residual is zero. so, the only contribution is
    // from the vertex:
    // Gaudi::Vector3 origin ;
    // origin(0) = m_origin.x() ;
    // origin(1) = m_origin.y() ;
    // origin(2) = m_origin.z() ;
    // Gaudi::Vector3 vertexres ;
    // Gaudi::Vector3 
    // vertexres(0) = state.x() - m_origin.x() ;
    // vertexres(1) = state.y() - m_origin.y() ;
    // vertexres(2) = state.z() - m_origin.z() ;
    // Gaudi::Vector3 halfDChisqDX = m_weight * vertexres ;
    // // this allows us to compute the new vertex position. still need a minus sign.
    // ROOT::Math::SVector<double,3> dpos = - vertex.covariance() * halfDChisqDX ;
    // double dchisq = ROOT::Math::Dot(dpos,halfDChisqDX) ;
    // Gaudi::Vector3 position = ;
    // // now we can compute the updated momentum. I am sure here we can
    // // do something with the change in the reference. but we'll always
    // // need the matrix WBG.
    // Gaudi::TrackVector stateres ;
    // stateres(0) = state.x() - position.x() ;
    // stateres(1) = state.y() - position.y() ;
    // stateres(2) = 0 ;
    // stateres(3) = 0 ;
    // stateres(4) = 0 ;
    // Gaudi::Vector3 dmom = vertex.matrixWBG() * stateres ; 
    // Gaudi::TrackVector& X = state.stateVector() ;
    // X(2) += dmom(0) ;
    // X(3) += dmom(1) ;
    // X(3) += dmom(2) ;
    // X(0) = position(0) + ( state.z() - position(2) ) * X(2) ;
    // X(1) = position(1) + ( state.z() - position(2) ) * X(3) ;
    

}

