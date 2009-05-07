//--------------------------------------------------------------------------
// File and Version Information:
// 	$Id: VtxFitParams.h,v 1.1 2009-05-07 13:02:10 wouter Exp $
//
// Description:
//	Class VtxFitParams
//
// 
//
//------------------------------------------------------------------------

#ifndef VtxFitParams_hh
#define VtxFitParams_hh

//----------------------
// Base Class Headers --
//----------------------

//---------------
// C++ Headers --
//---------------

//-------------------------------
// Collaborating Class Headers --
//-------------------------------

#include "GaudiKernel/SymmetricMatrixTypes.h"
#include "GaudiKernel/Vector4DTypes.h"
#include "GaudiKernel/Vector3DTypes.h"
#include "DecayTreeFitter/VtxDoubleErr.h"

namespace Vtx
{
  class FitParams 
  {
  
    enum Param { 
      i_x=1,   // starts at 1 to be used as matrix indexes
      i_y, 
      i_z, 
      i_px, 
      i_py,
      i_pz,
      i_E, 
      i_m,
      i_p
    };
    
  public:
    
    VtxFitParams() {}
    
    // this one takes the full 7X7 covariance
    VtxFitParams( double charge,
		  const Gaudi::XYZPoint& pos,
		  const Gaudi::LorentzVector& p4,
		  const Gaudi::SymMatrix7x7& cov7 )
      : m_q(charge),m_pos(pos),m_p4(p4),m_cov7(cov7) {}
    
    // this one assumes the mass is fixed (i.e. the error on the mass is 0)
    //  for computing the energy terms in the covariance matrix
    //VtxFitParams( double charge,
    //	double mass,
    //	const BbrPointErr& posErr,
    //	const BbrVectorErr& p3Err );
    
    double mass() const { return m_p4.M() ; }
    
    
    //   // modifiers
    //   void setCharge( double charge );
    //   void setMass( double mass );
    //   void setEnergy( double energy );
    //   void setMassAndEnergy( double mass, double energy );
    //   void setPos( const Gaudi::XYZPoint& pos );
    //   void setP3( const Gaudi::XYZVector& p3 );
    //   void setP4( double mass, const Gaudi::XYZVector& p3 );
    //   void setP4( const Gaudi::LorentzVector& p4 );
    //   void setP7( const Gaudi::XYZPoint& pos, const Gaudi::LorentzVector& p4 );
    //   void setCovP4( const HepSymMatrix& covP4 );
    //   void setCov7( const HepSymMatrix& cov7 );
    //   void setCov7( const HepSymMatrix& covPos, 
    // 		const HepSymMatrix& covP4 );
    //   void setCov7( const HepSymMatrix& covPos, 
    // 		const HepSymMatrix& covP4,
// 		const HepMatrix& covPosP4 );
//   void setDecayLength( const BbrDoubleErr& l) { m_decayLength=l ; }
//   void set( double mass, 
// 	    const VtxAbsRecoObject& recoObject, 
// 	    const Gaudi::XYZPoint& origin=Gaudi::XYZPoint(0.,0.,0.),
// 	    VtxAbsRecoObject::DIM dim=VtxAbsRecoObject::XYZ);
//   void set( const Gaudi::XYZPoint& pos, 
// 	    const Gaudi::LorentzVector& p4, 
// 	    const HepSymMatrix& cov7 );
//   void set( double mass,
// 	    const BbrPointErr& posErr, 
// 	    const BbrVectorErr& p3Err,
// 	    const HepMatrix& xp3Cov); 
//   // operators
//   bool  operator == (const VtxFitParams& c) const;

//   // accessors
//   inline double             charge()   const;
//   inline Gaudi::XYZPoint              pos()   const;
//   inline BbrPointErr           posWCov()   const;
//   inline BbrError           posCov()   const;
//   inline Gaudi::LorentzVector       p4()   const;
//   inline BbrError            p4Cov()   const;
//   inline BbrLorentzVectorErr p4WErr()  const;
//   inline Gaudi::XYZVector             p3()   const;
//   inline BbrError            p3Cov()   const;
//   inline BbrVectorErr        p3WErr()  const;
//   inline double                  e()   const;
//   inline double               eVar()   const;
//   inline double                  m()   const;
//   double                      mVar()   const;
//   inline double                  p()   const;
//   double                      pVar()   const;
//   inline const HepSymMatrix&  cov7()   const;
//   inline HepMatrix           xpCov()   const;
//   inline HepMatrix           xp4Cov()  const;
//   const BbrDoubleErr&        decayLength() const { return m_decayLength ; }
//   BbrDoubleErr               lifeTime(const PdtEntry* pdt) const ;

  private:    
    //--------------------
    // Instance Members --
    //--------------------
    
    // the params
    double m_q;
    Gaudi::XYZPoint m_pos;
    Gaudi::LorentzVector m_p4;
    BbrDoubleErr m_decayLength ; // decay length in lab frame [cm]
    
    // the 7X7 covariance
    Gaudi::SymMatrix7x7 m_cov7 ;
  };

/*
//
// inline functions 
//
 
double           
VtxFitParams::charge()   const
{
  return m_q;
}

Gaudi::XYZPoint     
VtxFitParams::pos()   const
{
  return m_pos;
}

BbrError          
VtxFitParams::posCov()   const
{
  return m_cov7.sub( i_x, i_z );
}

BbrPointErr          
VtxFitParams::posWCov()   const
{
  return BbrPointErr(pos(),posCov());
}

Gaudi::LorentzVector     
VtxFitParams::p4()   const
{
  return m_p4;
}

BbrError          
VtxFitParams::p4Cov()   const
{
  return m_cov7.sub( i_px, i_E );
}

Gaudi::XYZVector           
VtxFitParams::p3()   const
{
  return m_p4.vect() ;
}

BbrError          
VtxFitParams::p3Cov()   const
{
  return m_cov7.sub( i_px, i_pz );
}

BbrVectorErr          
VtxFitParams::p3WErr()   const
{
  return BbrVectorErr(p3(),p3Cov());
}

BbrLorentzVectorErr          
VtxFitParams::p4WErr()   const
{
  return BbrLorentzVectorErr(p4(),p4Cov());
}

double           
VtxFitParams::e()   const
{
  return m_p4.t();
}

double           
VtxFitParams::eVar()   const
{
  return m_cov7( i_E, i_E );
}

double                
VtxFitParams::p()   const
{
  return m_p4.rho();
}

double             
VtxFitParams::m()   const
{
  return m_p4.mag();
}

const HepSymMatrix&
VtxFitParams::cov7()   const
{
  return m_cov7;
}

HepMatrix
VtxFitParams::xpCov()   const
{
  HepMatrix dum( m_cov7 );
  
  // I'm not sure...  rows      columns
  return dum.sub(     i_x, i_z, i_px, i_pz );
}

HepMatrix
VtxFitParams::xp4Cov()   const
{
  HepMatrix dum( m_cov7 );
  return dum.sub( i_x, i_z, i_px, i_E );
}

*/
#endif




