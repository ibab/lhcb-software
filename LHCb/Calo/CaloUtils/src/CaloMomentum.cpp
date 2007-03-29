#include "CaloUtils/CaloMomentum.h"


//-----------------------------------------------------------------------------
// Implementation file for class : CaloMomentum
//
// 2006-08-08 : Olivier Deschamps
//-----------------------------------------------------------------------------

// -- Special constructors
//------------------------

// From CaloPosition assume origin is (0,0,0)
LHCb::CaloMomentum::CaloMomentum(const LHCb::CaloPosition* calo )
  : m_caloPositions(), m_point(), m_pointCovMatrix(),
    m_momentum(), m_momCovMatrix(),m_momPointCovMatrix(),
    m_status(LHCb::CaloMomentum::OK),m_flag(LHCb::CaloMomentum::Empty)
{
  this->addCaloPosition(calo);
}
    
// From CaloPosition + reference point (null covariance)
LHCb::CaloMomentum::CaloMomentum(const LHCb::CaloPosition* calo,
                                 Point point)
  : m_caloPositions(), m_point( point ), m_pointCovMatrix(), 
    m_momentum(), m_momCovMatrix(),m_momPointCovMatrix(),
    m_status(LHCb::CaloMomentum::OK),m_flag(LHCb::CaloMomentum::Empty)
{
  this->addCaloPosition(calo);
      this->addToFlag( LHCb::CaloMomentum::NewReferencePoint);
}
    
// From CaloPosition + reference point + covariance 
LHCb::CaloMomentum::CaloMomentum(const LHCb::CaloPosition* calo,
                                 const Point point, const PointCovariance cov)
  : m_caloPositions(), m_point( point ), m_pointCovMatrix( cov ), 
    m_momentum(), m_momCovMatrix(),m_momPointCovMatrix(),
    m_status(LHCb::CaloMomentum::OK),m_flag(LHCb::CaloMomentum::Empty)
{
  this->addCaloPosition(calo);
  this->addToFlag( LHCb::CaloMomentum::NewReferencePoint);
  this->addToFlag( LHCb::CaloMomentum::NewPointCovariance);
}

    
    
// From ProtoParticle
LHCb::CaloMomentum::CaloMomentum(const LHCb::ProtoParticle* proto)
  :  m_caloPositions(), m_point(), m_pointCovMatrix(),
     m_momentum(), m_momCovMatrix(),m_momPointCovMatrix(),
    m_status(LHCb::CaloMomentum::OK),m_flag(LHCb::CaloMomentum::Empty)
{
  this->addCaloPosition(proto);
}

// From ProtoParticle +  XYZPoint
LHCb::CaloMomentum::CaloMomentum(const LHCb::ProtoParticle* proto,
                                 const Point point)
  : m_caloPositions(), m_point( point ), m_pointCovMatrix(),
    m_momentum(), m_momCovMatrix(),m_momPointCovMatrix(),
    m_status(LHCb::CaloMomentum::OK),m_flag(LHCb::CaloMomentum::Empty)
{
  this->addCaloPosition(proto);
  this->addToFlag( LHCb::CaloMomentum::NewReferencePoint);
}


// From (ProtoParticle, XYZPoint, SymMatrix3x3)
LHCb::CaloMomentum::CaloMomentum(const LHCb::ProtoParticle* proto,
                                 const Point point, const PointCovariance cov)
  : m_caloPositions( ),
    m_point( point ), m_pointCovMatrix( cov ),
    m_momentum(), m_momCovMatrix(),m_momPointCovMatrix(),
    m_status(LHCb::CaloMomentum::OK),m_flag(LHCb::CaloMomentum::Empty)
{
  this->addCaloPosition(proto);
  this->addToFlag( LHCb::CaloMomentum::NewReferencePoint);
  this->addToFlag( LHCb::CaloMomentum::NewPointCovariance);
}


// From CaloHypo

LHCb::CaloMomentum::CaloMomentum(const LHCb::CaloHypo* hypo)
  :  m_caloPositions(),
     m_point(), m_pointCovMatrix(),
     m_momentum(),m_momCovMatrix(),m_momPointCovMatrix(),
     m_status(LHCb::CaloMomentum::OK),m_flag(LHCb::CaloMomentum::Empty)
{
  this->addCaloPosition(hypo);
}
    
// From (CaloHypo, XYZPoint)

LHCb::CaloMomentum::CaloMomentum(const LHCb::CaloHypo* hypo,
                                 const Point point)
  :  m_caloPositions(),
     m_point(point), m_pointCovMatrix(),
     m_momentum(),m_momCovMatrix(),m_momPointCovMatrix(),
     m_status(LHCb::CaloMomentum::OK),m_flag(LHCb::CaloMomentum::Empty)
{
  this->addCaloPosition(hypo);
  this->addToFlag( LHCb::CaloMomentum::NewReferencePoint);
}

// From (CaloHypo,XYZPoint, SymMatrix3x3)

LHCb::CaloMomentum::CaloMomentum(const LHCb::CaloHypo* hypo,
                                 const Point point, const PointCovariance cov)
  :  m_caloPositions(),
     m_point(point), m_pointCovMatrix(cov),
     m_momentum(),m_momCovMatrix(),m_momPointCovMatrix(),
     m_status(LHCb::CaloMomentum::OK),m_flag(LHCb::CaloMomentum::Empty)
{
  this->addCaloPosition(hypo);
  this->addToFlag( LHCb::CaloMomentum::NewReferencePoint);
  this->addToFlag( LHCb::CaloMomentum::NewPointCovariance);
}

    
// From CaloCluster

LHCb::CaloMomentum::CaloMomentum(const LHCb::CaloCluster* cluster)
  :  m_caloPositions(),
     m_point(), m_pointCovMatrix(),
     m_momentum(),
     m_momCovMatrix(),m_momPointCovMatrix(),
     m_status(LHCb::CaloMomentum::OK),m_flag(LHCb::CaloMomentum::Empty)
{
  m_caloPositions.push_back( cluster->position() );
}

//-- SetCaloPosition methods
//--------------------------
void LHCb::CaloMomentum::addCaloPosition(const LHCb::CaloPosition* calopos)
{
  m_flag |= LHCb::CaloMomentum::FromCaloPos;
  if( 0 == calopos){
    m_status |= LHCb::CaloMomentum::NullCaloPos;
    return;
  }
  m_caloPositions.push_back( *calopos );
}


void LHCb::CaloMomentum::addCaloPosition(const LHCb::CaloCluster* cluster)
{
  m_flag |= LHCb::CaloMomentum::FromCaloCluster;
  if( 0 == cluster){
    m_status |= LHCb::CaloMomentum::NullCaloCluster;
    return;
  }
  m_caloPositions.push_back( cluster->position() );
}

void LHCb::CaloMomentum::addCaloPosition(const LHCb::CaloHypo* hypo)
{
  m_flag |= LHCb::CaloMomentum::FromCaloHypo;

  if( 0 == hypo){
    m_status |= LHCb::CaloMomentum::NullCaloHypo;
    return;
  }

  // Check if the hypo has referenced hypos 
  const SmartRefVector<LHCb::CaloHypo>& hypos = hypo->hypos();

  if( hypos.empty()){
    // if no : photon, photonFromMerged, ...
    const LHCb::CaloPosition* calopos = hypo->position();
    this->addCaloPosition(calopos);
  }
  else{
    // if yes : mergedPi0
    for(SmartRefVector<LHCb::CaloHypo>::const_iterator ihypo = hypos.begin() ; ihypo != hypos.end(); ++ihypo){
      m_flag |= LHCb::CaloMomentum::FromCaloHypo;
      const LHCb::CaloPosition* calopos = (*ihypo)->position();
      this->addCaloPosition(calopos);
    }
  }
}

void LHCb::CaloMomentum::addCaloPosition(const  LHCb::ProtoParticle* proto)
{
  const SmartRefVector<LHCb::CaloHypo>& hypos = proto->calo();


  m_flag |= LHCb::CaloMomentum::FromProtoPart;

  if( 0 == proto ){
    m_status |= LHCb::CaloMomentum::NullProtoPart;
    return;    
  }
  if( 0 != proto->charge() ){
    m_status |= LHCb::CaloMomentum::ChargedProtoPart;
    return;    
  }


  if( hypos.empty()){
    m_status |= LHCb::CaloMomentum::EmptyHypoRef;
  }
  else if( 1 == hypos.size()){
    // Each ProtoPart derive from a single CaloHypo so far
    SmartRefVector<LHCb::CaloHypo>::const_iterator ihypo = hypos.begin() ;
    this->addCaloPosition( *ihypo );
  }
  else{    
    // If not the case the special configuration should be implemented here
    m_status |= LHCb::CaloMomentum::MultipleHypoRef; 
  }
}


//--- evaluate method
//-------------------
void LHCb::CaloMomentum::evaluate(int param){


  if( LHCb::CaloMomentum::OK != m_status)return; 
  
    
  // adapted from PhotonParam.h (thanks to Vanya Belyaev)


  double px = 0.;
  double py = 0.;
  double pz = 0.;
  double e  = 0.;

  
  LHCb::CaloMomentum::MomCovariance      C11      ;   // 4-moment 4x4 covariance matrix
  LHCb::CaloMomentum::PointCovariance    C22  = m_pointCovMatrix; // 3-point  3x3 covariance matrix
  LHCb::CaloMomentum::MomPointCovariance C12      ;   // momentum-point 4x3 off-diagonal block

  for(LHCb::CaloMomentum::CaloPositions::const_iterator icalo = m_caloPositions.begin();
      icalo != m_caloPositions.end(); ++icalo){

    LHCb::CaloPosition calo = *icalo;
    const double dx  = calo.x() - m_point.x() ;
    const double dy  = calo.y() - m_point.y() ;
    const double dz  = calo.z() - m_point.z() ;
    const double sx        =  dx / dz ;
    const double sy        =  dy / dz ;
    const double sz        =  1.0     ;
    const double sx2       =  sx * sx ;
    const double sy2       =  sy * sy ;
    const double sz2       =  1.0     ;
    const double gamma     =  1.0 / sqrt( sx2 + sy2 + sz2 );    
    const double gz        = gamma / dz ;
    
    // Evaluate the 4-momentum
    if( 0 != ( LHCb::CaloMomentum::Momentum & param ) ){
      px += calo.e () * gamma * sx ;
      py += calo.e () * gamma * sy ;
      pz += calo.e () * gamma * sz ;
      e  += calo.e ()              ;
    }
    
    // Evaluate the covariance
    if( 0 != ( LHCb::CaloMomentum::Covariance & param )){
      
      //  vector of "old" parameters 
      //    vold( 1 ) = pos   -> x () ;   // x-position in Calorimeter 
      //    vold( 2 ) = pos   -> y () ;   // y-position in Calorimeter 
      //    vold( 3 ) = pos   -> e () ;   // energy     in Calorimeter
      //    vold( 4 ) = point .  x () ;   // x-position at production vertex  
      //    vold( 5 ) = point .  y () ;   // y-position at production vertex 
      //    vold( 6 ) = point .  z () ;   // z-position at production vertex
      
      // "old" covariance matrix has the block structure 
      //     --------------
      //     |  S11 | S12 |
      //     --------------
      //     |  S21 | S22 | 
      //     --------------
      // Where S11 is 3x3 covariance matrix of E  , xc and yc 
      //   and S22 is 3x3 covariance matrix of xv , yv and zv 
      //   matrices S12 and S21 both are equal to null matrix 
      //   
      LHCb::CaloPosition::Covariance      S11 = calo.covariance();  // (X,Y,E)
      LHCb::CaloMomentum::PointCovariance S22 =  C22;               // (x,y,z)
      
      
      const double dx_dxc  =  1.0 ; // d( dx ) / d( xc ) 
      const double dx_dyc  =  0.0 ; // d( dx ) / d( yc )
      const double dx_dxv  = -1.0 ; // d( dx ) / d( xv ) 
      const double dx_dyv  =  0.0 ; // d( dx ) / d( yv ) 
      const double dx_dzv  =  0.0 ; // d( dx ) / d( zv )
      
      const double dy_dxc  =  0.0 ; // d( dy ) / d( xc ) 
      const double dy_dyc  =  1.0 ; // d( dy ) / d( yc ) 
      const double dy_dxv  =  0.0 ; // d( dy ) / d( xv ) 
      const double dy_dyv  = -1.0 ; // d( dy ) / d( yv ) 
      const double dy_dzv  =  0.0 ; // d( dy ) / d( zv ) 
      
      const double dz_dxc  =  0.0 ; // d( dz ) / d( xc ) 
      const double dz_dyc  =  0.0 ; // d( dz ) / d( yc ) 
      const double dz_dxv  =  0.0 ; // d( dz ) / d( xv )
      const double dz_dyv  =  0.0 ; // d( dz ) / d( yv ) 
      const double dz_dzv  = -1.0 ; // d( dz ) / d( zv ) 
      
      const double dgz_dxc = -1.0 * dx * gz * gz ; // d( gz ) / d( xc ) 
      const double dgz_dyc = -1.0 * dy * gz * gz ; // d( gz ) / d( yc ) 
      const double dgz_dxv =        dx * gz * gz ; // d( gz ) / d( xv ) 
      const double dgz_dyv =        dy * gz * gz ; // d( gz ) / d( yv ) 
      const double dgz_dzv =        dz * gz * gz ; // d( gz ) / d( zv ) 
      
      //    vnew ( 1 ) = px ;   // x-component of photon momentum 
      //    vnew ( 2 ) = py ;   // y-component of photon momentum  
      //    vnew ( 3 ) = pz ;   // z-component of photon momentum  
      //    vnew ( 4 ) = e  ;   // photon energy 
      //    vnew ( 5 ) = x0 ;   // x-component of photon production point 
      //    vnew ( 6 ) = y0 ;   // y-component of photon production point 
      //    vnew ( 7 ) = z0 ;   // z-component of photon production point 
      
      //  evaluate the "transport matrix"
      
      // 
      //  The transport matrix F = d(vnew) / d(vold)    (7x6) 
      //
      //   has block structure
      //        --------------
      //        | F11 |  F12 | 
      //        --------------
      //        | F21 |  F22 | 
      //        --------------
      //  Where F11 = d( px , py , pz , e) / d(  e , xc , yc )   4x3 matrix 
      //        F12 = d( px , py , pz , e) / d( xv , yv , zv )   4x4 matrix 
      //        F21 = d( x0 , y0 , z0    ) / d(  e , xc , yc )   3x3 matrix 
      //        F22 = d( x0 , y0 , z0    ) / d( xv , yv , zv )   3x3 matrix
      //
      //  From definition of parameters it is obvious that 
      //  F21 is null matrix ans F22 is unit matrix
      
      LHCb::CaloMomentum::MomPointCovariance F11  ;
      LHCb::CaloMomentum::MomPointCovariance F12  ;
      //   px =     e * dx * gz  
      //   d(px)/d(e) 
      F11 ( 0 , 2 ) = dx * gz ;
      //   d(px)/d(xc) 
      F11 ( 0 , 0 ) =  e * ( dx_dxc * gz + dx * dgz_dxc ) ;
      //   d(px)/d(yc) 
      F11 ( 0 , 1 ) =  e * ( dx_dyc * gz + dx * dgz_dyc ) ;


      //   d(px)/d(xv) 
      F12 ( 0 , 0 ) =  e * ( dx_dxv * gz + dx * dgz_dxv ) ;
      //   d(px)/d(yv) 
      F12 ( 0 , 1 ) =  e * ( dx_dyv * gz + dx * dgz_dyv ) ;
      //   d(px)/d(zv) 
      F12 ( 0 , 2 ) =  e * ( dx_dzv * gz + dx * dgz_dzv ) ;
      
      //   py = e * dy * gz  
      //   d(py)/d(e) 
      F11 ( 1 , 2 ) = dy * gz;
      //   d(py)/d(xc) 
      F11 ( 1 , 0 ) =  e * ( dy_dxc * gz + dy * dgz_dxc ) ;
      //   d(py)/d(yc) 
      F11 ( 1 , 1 ) =  e * ( dy_dyc * gz + dy * dgz_dyc ) ;

      //   d(py)/d(xv) 
      F12 ( 1 , 0 ) =  e * ( dy_dxv * gz + dy * dgz_dxv ) ;
      //   d(py)/d(yv) 
      F12 ( 1 , 1 ) =  e * ( dy_dyv * gz + dy * dgz_dyv ) ;
      //   d(py)/d(zv) 
      F12 ( 1 , 2 ) =  e * ( dy_dzv * gz + dy * dgz_dzv )  ;
      
      //   pz = e * dz * gz   
      //   d(pz)/d(e) 
      F11 ( 2 , 2 ) = dz * gz ;
      //   d(pz)/d(xc) 
      F11 ( 2 , 0 ) =  e * ( dz_dxc * gz + dz * dgz_dxc ) ;
      //   d(pz)/d(yc) 
      F11 ( 2 , 1 ) =  e * ( dz_dyc * gz + dz * dgz_dyc ) ;

      //   d(pz)/d(xv) 
      F12 ( 2 , 0 ) =  e * ( dz_dxv * gz + dz * dgz_dxv ) ;
      //   d(pz)/d(yv) 
      F12 ( 2 , 1 ) =  e * ( dz_dyv * gz + dz * dgz_dyv ) ;
      //   d(pz)/d(zv) 
      F12 ( 2 , 2 ) =  e * ( dz_dzv * gz + dz * dgz_dzv ) ;
      
      //   e (new) = e (old)          
      //   d(e)/d(e) 
      F11 ( 3 , 2 ) =  1.0 ;
      //   d(e)/d(xc) 
      F11 ( 3 , 0 ) =  0.0 ;
      //   d(e)/d(yc) 
      F11 ( 3 , 1 ) =  0.0 ;

      //   d(e)/d(xv) 
      F12 ( 3 , 0 ) =  0.0 ;
      //   d(e)/d(yv) 
      F12 ( 3 , 1 ) =  0.0 ;
      //   d(e)/d(zv) 
      F12 ( 3 , 2 ) =  0.0 ;
      
      // ok, now all blocks of transport matrix are 
      // known and we could evaluate the covariance matrix 
      // of new parameters
      
      // The covariance matrix of new parameters has block structure:
      //     -------------
      //     | C11 | C12 |  
      //     -------------
      //     | C21 | C22 | 
      //     -------------
      // 
      // C = F * S * F^T 
      //
      // Where:
      //
      // C11 = F11*S11*F11^T + F12*S21*F11^T + F11*S12*F12^T + F12*S22*F12^T
      // C12 = F11*S11*F21^T + F12*S21*F21^T + F11*S12*F22^T + F12*S22*F22^T
      // C21 = F21*S11*F11^T + F22*S21*F11^T + F21*S12*F12^T + F22*S22*F12^T 
      // C22 = F21*S11*F21^T + F22*S21*F21^T + F21*S12*F22^T + F22*S22*F22^T 
      //
      // taking into account that S12 = 0 , S21 = 0 , F21 = 0 , F22 = 1 on gets 
      //
      // C11 = F11 * S11 * F11^T + F12 * S22 * F12^T 
      // C12 = F12 * S22 * F22^T                     = F12 * S22 
      // C21 = F22 * S22 * F12^T                     = S22 * F12^T 
      // C22 = F22 * S22 * F22^T                     = S22
      
      //    C11 += S11.similarity( F11 ) + S22.similarity( F12 ) ;
      
      // -----------------------
      // Covariance are additive
      // Assume the various CaloPositions are NOT correlated
      // THIS IS NOT THE CASE FOR MERGED PI0
      // @ToDo : adapt to the case of correlated CaloPositions
      
      C11 += Similarity( F11,S11 ) + Similarity( F12, S22 ) ;
      C12 +=  F12   * S22 ;
    }    
  }    
  if( 0 != (LHCb::CaloMomentum::Momentum & param) ){
    m_momentum .SetPx ( px ) ;
    m_momentum .SetPy ( py ) ;
    m_momentum .SetPz ( pz ) ;
    m_momentum .SetE  ( e  ) ;
    m_flag |= LHCb::CaloMomentum::MomentumEvaluated;
  }
  if( 0 != (LHCb::CaloMomentum::Covariance & param ) ){
    m_momCovMatrix = C11;
    m_momPointCovMatrix= C12;
    m_flag |= LHCb::CaloMomentum::CovarianceEvaluated;
  }
}



//--- mass error
//--------------
const double LHCb::CaloMomentum::emass(){
  this->evaluate();
  Gaudi::Vector4 momentum(m_momentum.Px(),m_momentum.Py(),m_momentum.Pz(),m_momentum.E());
  return sqrt( Similarity(momentum, m_momCovMatrix) / m_momentum.M2() );
}
