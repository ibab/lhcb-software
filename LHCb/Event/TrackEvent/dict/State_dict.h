
//   **************************************************************************
//   *                                                                        *
//   *                      ! ! ! A T T E N T I O N ! ! !                     *
//   *                                                                        *
//   *  This file was created automatically by GaudiObjDesc, please do not    *
//   *  delete it or edit it by hand.                                         *
//   *                                                                        *
//   *  If you want to change this file, first change the corresponding       *
//   *  xml-file and rerun the tools from GaudiObjDesc (or run make if you    *
//   *  are using it from inside a Gaudi-package).                            *
//   *                                                                        *
//   **************************************************************************

#ifndef TrEvent_State_H
#define TrEvent_State_H 1

// Include files
#include "CLHEP/Geometry/Point3D.h"
#include "CLHEP/Geometry/Vector3D.h"
#include "GaudiKernel/KeyedContainer.h"
#include "CLHEP/Matrix/Vector.h"
#include "CLHEP/Matrix/SymMatrix.h"


// Forward declarations

// Class ID definition
static const CLID& CLID_State = 10011;

// Namespace for locations in TDS
namespace StateLocation {
  static const std::string& Default = "Rec/Track/States";
}


/** @class State State.h
 *
 * State is the base class of offline and online track states. 
 *
 * @author Jose Hernando, Eduardo Rodrigues
 * created Thu Feb 10 18:12:25 2005
 * 
 */

class State
{
public:

  friend class State_dict;

  /// enumeration for the state type
  enum Type{ HasMomentum=1,
             StraightLine=2 };
  /// enumeration for the location of the state
  enum Location{ Unknown=1,
                 ClosestToBeam=2,
                 FirstMeasurement=4,
                 EndVelo=8,
                 AtTT=16,
                 AtT=32,
                 BegRich1=64,
                 EndRich1=128,
                 BegRich2=256,
                 EndRich2=512,
                 Calo=1024,
                 Muon=2048 };

  /// Default constructor. State defined to be of type State::HasMomentum.
  State();

  /// Default Destructor
  virtual ~State() {}

  // Retrieve pointer to class definition structure
  virtual const CLID& clID() const;
  static const CLID& classID();

  /// Retrieve the number of state parameters
  unsigned int nParameters() const;

  /// Retrieve the position and momentum vectors and the corresponding 6D covariance matrix (pos:1->3,mom:4-6) of the state
  void positionAndMomentum(HepPoint3D& pos,
                           HepVector3D& mom,
                           HepSymMatrix& cov6D) const;

  /// Retrieve the 3D-position vector of the state
  HepPoint3D position() const;

  /// Retrieve the x-position of the state
  double x() const;

  /// Retrieve the y-position of the state
  double y() const;

  /// Retrieve the z-position of the state
  double z() const;

  /// Retrieve the slopes (Tx=dx/dz,Ty=dy/dz,1.) of the state
  HepVector3D slopes() const;

  /// Retrieve the Tx=dx/dz slope of the state
  double tx() const;

  /// Retrieve the Ty=dy/dz slope of the state
  double ty() const;

  /// Retrieve the charge-over-momentum Q/P of the state
  virtual double qOverP() const;

  /// Retrieve the momentum of the state
  virtual double p() const;

  /// Retrieve the transverse momentum of the state
  virtual double pt() const;

  /// Retrieve the momentum vector of the state
  HepVector3D momentum() const;

  /// Retrieve the Q/Pperp (ratio of the charge to the component of the momentum transverse to the magnetic field) of the state
  double qOverPperp() const;

  /// Retrieve the 6D covariance matrix (x,y,z,px,py,pz) of the state
  virtual HepSymMatrix posMomCovariance() const;

  /// Retrieve the errors on the 3D-position vector of the state
  HepSymMatrix errPosition() const;

  /// Retrieve the squared error on the x-position of the state
  double errX2() const;

  /// Retrieve the squared error on the y-position of the state
  double errY2() const;

  /// Retrieve the errors on the slopes of the state
  HepSymMatrix errSlopes() const;

  /// Retrieve the squared error on the x-slope Tx=dx/dz of the state
  double errTx2() const;

  /// Retrieve the squared error on the y-slope Ty=dy/dz of the state
  double errTy2() const;

  /// Retrieve the squared error on the charge-over-momentum Q/P of the state
  virtual double errQOverP2() const;

  /// Retrieve the squared error on the momentum of the state
  virtual double errP2() const;

  /// Retrieve the errors on the momentum vector of the stateRetrieve the errors on the momentum vector of the state
  virtual HepSymMatrix errMomentum() const;

  /// Retrieve the squared error on the Q/Pperp of the state
  virtual double errQOverPperp2() const;

  /// Clone the state
  virtual State* clone() const;

  /// Clear the state before re-using it
  virtual void reset();

  /// Update the state vector
  void setState(const HepVector& state);

  /// Update the state vector (presumably of type State::HasMomentum)
  virtual void setState(double x,
                        double y,
                        double z,
                        double tx,
                        double ty,
                        double qOverP);

  /// Update the state vector (presumably of type State::StraightLine)
  void setState(double x,
                double y,
                double z,
                double tx,
                double ty);

  /// Update the state covariance
  void setCovariance(const HepSymMatrix& value);

  /// Update the x-position of the state
  void setX(double value);

  /// Update the y-position of the state
  void setY(double value);

  /// Update the z-position of the state
  void setZ(double value);

  /// Update the Tx slope of the state
  void setTx(double value);

  /// Update the Ty slope of the state
  void setTy(double value);

  /// Update the Q/P value of the state
  virtual void setQOverP(double value);

  /// Retrieve const  the variety of state flags
  unsigned int flags() const;

  /// Check state type
  bool checkType(const Type& value) const;

  /// Update state location
  void setLocation(const Location& value);

  /// Check state location
  bool checkLocation(const Location& value) const;

  /// Retrieve const  the state vector
  const HepVector& state() const;

  /// Retrieve  the state vector
  HepVector& state();

  /// Retrieve const  the state covariance matrix (indexes 1,...,5 for x, y, tx, ty, Q/p)
  const HepSymMatrix& covariance() const;

  /// Retrieve  the state covariance matrix (indexes 1,...,5 for x, y, tx, ty, Q/p)
  HepSymMatrix& covariance();

protected:

  /// Update the type of the state
  void setType(const State::Type& value);

  /// Offsets of bitfield flags
  enum flagsBits{typeBits     = 0,
                 locationBits = 2};

  /// Bitmasks for bitfield flags
  enum flagsMasks{typeMask     = 0x3L,
                  locationMask = 0xFFCL};


  unsigned int m_flags; ///< the variety of state flags

private:

  HepVector    m_state;      ///< the state vector
  HepSymMatrix m_covariance; ///< the state covariance matrix (indexes 1,...,5 for x, y, tx, ty, Q/p)
  double       m_z;          ///< the z-position of the state

};

// -----------------------------------------------------------------------------
//   end of class
// -----------------------------------------------------------------------------

// Including forward declarations

inline const CLID& State::clID() const
{
  return State::classID();
}

inline const CLID& State::classID()
{
  return CLID_State;
}

inline unsigned int State::flags() const 
{
  return m_flags;
}

inline bool State::checkType(const Type& value) const
{
  unsigned int val = (unsigned int)value;  return 0 != ( m_flags & typeMask & val );
}

inline void State::setLocation(const Location& value)
{
  unsigned int val = (unsigned int)value;
  m_flags &= ~locationMask;
  m_flags |= ((((unsigned int)val) << locationBits) & locationMask);
}

inline bool State::checkLocation(const Location& value) const
{
  unsigned int val = (unsigned int)value;  return 0 != ( m_flags & locationMask & val );
}

inline const HepVector& State::state() const 
{
  return m_state;
}

inline HepVector& State::state() 
{
  return m_state;
}

inline const HepSymMatrix& State::covariance() const 
{
  return m_covariance;
}

inline HepSymMatrix& State::covariance() 
{
  return m_covariance;
}

inline unsigned int State::nParameters() const 
{

  return (unsigned int) m_state.num_row();
        
}

inline void State::positionAndMomentum(HepPoint3D& pos,
                                       HepVector3D& mom,
                                       HepSymMatrix& cov6D) const 
{

  pos   = position();
  mom   = momentum();
  cov6D = posMomCovariance();
        
}

inline HepPoint3D State::position() const 
{

  return HepPoint3D( m_state[0], m_state[1], m_z );
        
}

inline double State::x() const 
{

  return m_state[0];
        
}

inline double State::y() const 
{

  return m_state[1];
        
}

inline double State::z() const 
{

  return m_z;
        
}

inline HepVector3D State::slopes() const 
{

  return HepVector3D( m_state[2], m_state[3], 1.);
        
}

inline double State::tx() const 
{

  return m_state[2];
        
}

inline double State::ty() const 
{

  return m_state[3];
        
}

inline HepVector3D State::momentum() const 
{

  HepVector3D mom = slopes();
  mom *= ( p() / mom.mag() );
  return mom;
        
}

inline double State::qOverPperp() const 
{

  double tx2 = tx() * tx();
  return ( qOverP() * sqrt( (1.+ tx2 + ty()*ty()) / (1. + tx2 ) ) );
        
}

inline HepSymMatrix State::errPosition() const 
{

  const HepSymMatrix temp = posMomCovariance();
  return temp.sub(1,3);
        
}

inline double State::errX2() const 
{

  return m_covariance.fast(1,1);
        
}

inline double State::errY2() const 
{

  return m_covariance.fast(2,2);
        
}

inline HepSymMatrix State::errSlopes() const 
{

  const HepSymMatrix temp = m_covariance;
  HepSymMatrix err = temp.sub(3,5);
  err.fast(2,1) = 0.;
  err.fast(3,1) = 0.;
  err.fast(3,2) = 0.;
  err.fast(3,3) = 0.;
  return err;
        
}

inline double State::errTx2() const 
{

  return m_covariance.fast(3,3);
        
}

inline double State::errTy2() const 
{

  return m_covariance.fast(4,4);
        
}

inline void State::setState(const HepVector& state) 
{

  m_state = state;
        
}

inline void State::setState(double x,
                            double y,
                            double z,
                            double tx,
                            double ty) 
{

  m_state[0] = x;
  m_state[1] = y;
  m_state[2] = tx;
  m_state[3] = ty;
  m_z        = z;
        
}

inline void State::setCovariance(const HepSymMatrix& value) 
{

  if ( value.num_row() == (int) nParameters() ) {
    m_covariance = value;
  }
  else {
    std::cerr << "ERROR   The dimension of the covariance matrix does not match that of the state!"
              << "ERROR   Using the default covariance matrix." << std::endl;
    int dim = nParameters();
    m_covariance = HepSymMatrix(dim,0);
  }
        
}

inline void State::setX(double value) 
{

  m_state[0] = value;
        
}

inline void State::setY(double value) 
{

  m_state[1] = value;
        
}

inline void State::setZ(double value) 
{

  m_z = value;
        
}

inline void State::setTx(double value) 
{

  m_state[2] = value;
        
}

inline void State::setTy(double value) 
{

  m_state[3] = value;
        
}

// Definition of Keyed Container for State
typedef KeyedContainer<State, Containers::HashMap> States;



#endif ///TrEvent_State_H
