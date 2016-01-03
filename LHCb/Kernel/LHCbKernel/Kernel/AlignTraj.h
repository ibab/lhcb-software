#ifndef KERNEL_ALIGNTRAJ_H
#define KERNEL_ALIGNTRAJ_H 1

#ifdef __INTEL_COMPILER         // Disable ICC remark from ROOT
 #pragma warning(disable:1572) // Floating-point equality and inequality comparisons are unreliable
#endif

// Include files
#include "Math/RotationX.h"
#include "Math/RotationY.h"
#include "Math/RotationZ.h"

// local
#include "Kernel/DifTraj.h"

/** @class AlignTraj AlignTraj.h Kernel/AlignTraj.h
 *
 *  AlignTraj is a DifTraj, which takes an external trajectory
 *  as input, and translates/rotates this trajectory.
 *  It provides the derivatives wrt. the translations and rotations
 *  Note: this trajectory does NOT own the external trajectory
 *        provided...
 *  Note: First we rotate around a pivot point, i.e. we take
 *        a given point, compute its difference with the pivot,
 *        and rotate this difference, and then add the pivot.
 *        Next we translate the result. Basically, think about this
 *        as first getting the orientation around the pivot right,
 *        and then moving the pivot into the right location.
 *  Note: the intended use is for small (delta) alignments wrt a
 *        baseline geometry
 *  Note: in general, it is not guaranteed that the six parameters
 *        are independent -- i.e. for straight lines, only four are...
 *
 *  @author Gerhard Raven
 *  @date   2006-11-27
 */

namespace LHCb
{

  class AlignTraj : public DifTraj<6> {

  public:
    // import types from parent...
    typedef DifTraj<6>::Parameters Parameters;
    typedef DifTraj<6>::Derivative Derivative;
    typedef DifTraj<6>::Point      Point;
    typedef DifTraj<6>::Vector     Vector;

    /// Constructors
    AlignTraj(const Trajectory& traj, const Point& pivot)
      : DifTraj<6>(traj.range()),
        m_pivot(pivot),
        m_traj(&traj) {  }

    AlignTraj(const Trajectory& traj)
      : DifTraj<6>(traj.range()),
        m_pivot(0,0,0),
        m_traj(&traj) {  }

    AlignTraj(const Trajectory& traj, const Parameters& p)
      : DifTraj<6>(traj.range()),
        m_rx(p(3)),m_ry(p(4)), m_rz(p(5)),
        m_pivot(0,0,0), m_traj(&traj)
    {  m_trans.SetX(p(0)); m_trans.SetY(p(1)); m_trans.SetZ(p(2)); }

    AlignTraj(const Trajectory& traj, const Parameters& p, const Point& pivot)
      : DifTraj<6>(traj.range()),
        m_rx(p(3)),m_ry(p(4)), m_rz(p(5)),
        m_pivot(pivot), m_traj(&traj)
    {  m_trans.SetX(p(0)); m_trans.SetY(p(1)); m_trans.SetZ(p(2)); }

    /// destructor
    virtual ~AlignTraj(){}

    // clone thyself...
    virtual std::unique_ptr<Trajectory> clone() const;

    /// Retrieve the derivative of the point at fixed arclength 'arclength'
    /// with respect to the alignment parameters
    virtual Derivative derivative( double arclength ) const ;

    /// Retrieve the alignment parameters
    virtual Parameters parameters() const ;

    /// update the parameters
    virtual AlignTraj& operator+=(const Parameters& delta) ;


    virtual Point  position(  double arclength ) const ;
    virtual Vector direction( double arclength ) const;
    virtual Vector curvature( double arclength ) const;
    virtual void expansion(   double arclength,
                              Point& p,
                              Vector& dp,
                              Vector& ddp ) const ;

    virtual double muEstimate( const Point& ) const ;

    virtual double distTo1stError( double arclength,
                                   double tolerance,
                                   int pathDirection = +1 ) const ;
    virtual double distTo2ndError( double arclength,
                                   double tolerance,
                                   int pathDirection = +1 ) const ;

    /// Distance, along the Trajectory, between position(mu1) and
    /// position(mu2). Trivial because AlignTraj is parameterized in
    /// arclength.
    using DifTraj<6>::arclength;
    virtual double arclength(double mu1, double mu2) const { return mu2 - mu1 ; }

  private:
    template <typename T> T rotate(const T& t) const
    { return m_rx(m_ry(m_rz(t))); }
    template <typename T> T invRotate(const T& t) const
    { return m_rz.Inverse()(m_ry.Inverse()(m_rx.Inverse()(t))); }

    ROOT::Math::RotationX   m_rx;
    ROOT::Math::RotationY   m_ry;
    ROOT::Math::RotationZ   m_rz;
    Vector                  m_trans;
    Point                   m_pivot;
    const Trajectory*       m_traj;
  };

} // namespace LHCb

#endif // KERNEL_ALIGNTRAJ_H
