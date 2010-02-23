// $Id: $
#ifndef DETDESC_BEAMLINEINFO_H 
#define DETDESC_BEAMLINEINFO_H 1

// Include files
#include <vector>
#include <sstream>

#include "VeloDet/DeVelo.h"

#include "GaudiKernel/Point3DTypes.h"
#include "GaudiKernel/Vector3DTypes.h"
#include "GaudiKernel/GenericVectorTypes.h"
#include "GaudiKernel/SymmetricMatrixTypes.h"
#include "GaudiKernel/GenericMatrixTypes.h"


/** @class BeamLineInfo BeamLineInfo.h DetDesc/BeamLineInfo.h
 *  
 *  Holds informations about the beam line : the interaction point and 
 *   its diagonalized covariance matrix
 *
 *  @author Neal Gauvin
 *  @date   2009-09-16
 */
class BeamLineInfo {
public: 
  /// Standard constructor
  BeamLineInfo( ); 

  virtual ~BeamLineInfo( ); ///< Destructor

  void SetNbEntries( int & ); ///<Set the number of entries
  void SetIPPosition( Gaudi::XYZPoint & ); ///<Set interaction point position
  ///Set interaction point position
  void SetIPPosition( double, double, double ); 
  void SetEigenVectors( Gaudi::Matrix3x3 & ); ///<Set the eigenvectors
  void SetXEigenVector( Gaudi::XYZVector & ); ///<Set X eigenvector
  void SetYEigenVector( Gaudi::XYZVector & ); ///<Set Y eigenvector
  void SetZEigenVector( Gaudi::XYZVector & ); ///<Set Z eigenvector
  void SetZEigenVector( double, double, double ); ///<Set Z eigenvector
  void SetEigenValues( double &, double &, double & ); ///<Set eigenvalues
  void SetEigenValues( Gaudi::Vector3 & ); ///<Set eigenvalues
  void SetEigenValues( Gaudi::XYZPoint & ); ///<Set eigenvalues

  ///Move the interaction point to the given frame
  void MoveIPToFrame( Gaudi::Transform3D & ); 
  ///Move the Z eigenvector to the given frame
  void MoveZEVToFrame( Gaudi::Transform3D & ); 
  ///Move all members to the given frame
  void MoveToFrame( Gaudi::Transform3D & );
  ///  Set X,Y,Z eigenvectors as being the vectors closest to the X,Y,Z axis,
  ///     respectively, with positive eigenvalues.
  void SetOrder();
  ///return true if the beam has moved radially by a factor > threshold
  bool HasMoved( Gaudi::XYZPoint &, double & );
  ///Nice print out of the mean interaction point with errors
  std::string PrintIP();

  int & GetNbEntries();///<Get the number of PVs used so far
  Gaudi::XYZPoint & GetIPPosition();///<Get interaction point position
  Gaudi::XYZPoint GetIPPositionErrors();///<Get IP position errors
  Gaudi::XYZVector & GetXEigenVector();///<Get X eigenvector
  Gaudi::XYZVector & GetYEigenVector();///<Get Y eigenvector
  Gaudi::XYZVector & GetZEigenVector();///<Get Z eigenvector
  Gaudi::XYZPoint GetEigenValues();///<Get eigenvalues
  double & GetXEigenValue();///<Get X eigenvalue
  double & GetYEigenValue();///<Get Y eigenvalue
  double & GetZEigenValue();///<Get Z eigenvalue
  

protected:

private:

  ///From a XYZVector, get a Vector3, easier to handle
  Gaudi::Vector3 MkV( Gaudi::XYZVector & );
  ///Turn a Vector3 into a XYZVector
  Gaudi::XYZVector MkV( Gaudi::Vector3 & );

  Gaudi::XYZPoint   m_ipPosition ; ///<the interaction point
  ///Covariance matrix eigenvectors
  Gaudi::XYZVector  m_axis1; ///<Covariance matrix X eigenvector
  Gaudi::XYZVector  m_axis2; ///<Covariance matrix Y eigenvector
  Gaudi::XYZVector  m_axis3; ///<Covariance matrix Z eigenvector (the beamline)
  
  double m_sigma1; ///<Covariance matrix x eigenvalues
  double m_sigma2; ///<Covariance matrix y eigenvalues
  double m_sigma3; ///<Covariance matrix z eigenvalues
  
  int    m_nbentries;

};
#endif // DETDESC_BEAMLINEINFO_H
