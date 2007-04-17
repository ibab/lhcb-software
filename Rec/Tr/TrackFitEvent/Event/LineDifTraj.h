// $Id: LineDifTraj.h,v 1.1 2007-04-17 15:19:36 graven Exp $
#ifndef LineDifTraj_H
#define LineDifTraj_H 1

// Include files
#include "Kernel/DifTraj.h"

namespace LHCb
{

    class LineDifTraj: public LHCb::DifTraj<4> {
      
    public:
      
      /// Destructor
      virtual ~LineDifTraj() {};

      // clone thyself...
      virtual std::auto_ptr<Trajectory> clone() const;
      
      /// Constructor from the middle point and a direction vector
      LineDifTraj( const Point& middle,
                   const Vector& dir,
                   const Range& range );
      
      /// Constructor from a begin and an end point
      LineDifTraj( const Point& begPoint,
                   const Point& endPoint );
      
      /// Point on the trajectory at arclength from the starting point    
      virtual Point position( double arclength ) const;
      
      /// First derivative of the trajectory at arclength from the starting point
      virtual Vector direction( double arclength=0 ) const;
      
      /// Second derivative of the trajectory at arclength from the starting point
      virtual Vector curvature( double arclength=0 ) const;
      
      /// Create a parabolic approximation to the trajectory
      /// at arclength from the starting point
      virtual void expansion( double arclength,
                              Point& p,
                              Vector& dp,
                              Vector& ddp ) const;
      
      /// Determine the distance in arclenghts to the
      /// closest point on the trajectory to a given point
      virtual double arclength( const Point& point ) const;
      
      /// Number of arclengths until deviation of the trajectory from the
      /// expansion reaches the given tolerance.
      virtual double distTo1stError( double arclength,
                                     double tolerance, 
                                     int pathDirection = +1 ) const;
      
      /// Number of arclengths until deviation of the trajectory from the
      /// expansion reaches the given tolerance. 
      virtual double distTo2ndError( double arclength,
                                     double tolerance, 
                                     int pathDirection = +1 ) const;

      typedef LHCb::DifTraj<4>::Parameters Parameters; 
      typedef LHCb::DifTraj<4>::Derivative Derivative; 
      virtual Parameters parameters() const;
      virtual LineDifTraj& operator+=(const Parameters&);
      virtual Derivative derivative(double arclen) const;

      std::ostream& print(std::ostream& s=std::cout) const;

    private:
      Vector m_dir;
      Point  m_pos;
      
    }; // class LineDifTraj

}
#endif /// LineDifTraj
