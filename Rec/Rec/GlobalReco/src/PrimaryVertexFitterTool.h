// $Id: PrimaryVertexFitterTool.h,v 1.1 2002-07-11 16:24:20 gcorti Exp $
#ifndef PRIMARYVERTEXFITTERTOOL_H 
#define PRIMARYVERTEXFITTERTOOL_H 1

// Include files
// from STL

// from Gaudi
#include "GaudiKernel/AlgTool.h"

#include "CLHEP/Matrix/SymMatrix.h"
#include "CLHEP/Geometry/Point3D.h"
#include "CLHEP/Matrix/Matrix.h"
#include "CLHEP/Matrix/Vector.h"

#include "GaudiKernel/SmartRefVector.h"
#include "Event/TrStoredTrack.h"


/// local store

class MyTrack {
public:

  double X; ///< x position of 1st measured point
  double Y; ///< y position of 1st measured point
  double Z; ///< z position of 1st measured point
  double Slx; ///< Slx = dx/dz at 1st measured point
  double Sly; ///< Sly = dy/dz at 1st measured point
  double Pt; ///< momentum at 1st measured point
  HepSymMatrix Cov; ///< covariant matrix of all above parameters
  double d0; ///< distance to PV position
  double errd0; ///< error of d0
  HepVector3D vd0; ///< vector d0
  double chi2; ///< chi2 of track fit 
  int NbDeg; ///< number of degrees od freedom 
  bool isUsed; ///< flag  to using or rejecting track
  int idPart; ///< id of particle
  int nBin; ///< number of bin in zclose 
  HepVector3D unitVect;
  HepPoint3D fpoint;
  HepPoint3D lpoint;
  int firstSta;
  int lastSta;
  SmartRef<TrStoredTrack> track;
};

class MyVertex {
public:
  
  double X;
  double Y;
  double Z;
  HepSymMatrix Cov; ///< covariant matrix
  double chi2;
  int NbDeg;
  long event;
  SmartRefVector<TrStoredTrack> tracks;
};



/** @class PrimaryVertexFitterTool PrimaryVertexFitterTool.h
 *  
 *
 *  @author Krzysztof Ciba
 *  @date   03/04/2002
 */

static const InterfaceID IID_IPrimaryVertexFitterTool("IPrimaryVertexFitterTool"
                                                      , 1 , 0); 


class MyTrack;
class MyVertex;


///< interface
class IPrimaryVertexFitterTool : virtual public IAlgTool {
 public:
  /// Retrieve interface ID
  static const InterfaceID& interfaceID() {return IID_IPrimaryVertexFitterTool;}
  /// fit
  virtual MyVertex* fitter(std::vector< MyTrack >, 
                           //       std::vector< MyVertex >,
                            int m_maxIteration,
                            double m_chi2min,
                            std::string fitModel,
                            int m_minNumbersOfTracks) = 0;

protected:
  
  
  
};



///< main tool class
class PrimaryVertexFitterTool : public AlgTool, 
                                virtual public IPrimaryVertexFitterTool {
public:

  /// Standard constructor
  PrimaryVertexFitterTool( const std::string& type, 
                           const std::string& name,
                           const IInterface* parent);

  ~PrimaryVertexFitterTool() {} ///< Destructor
  
  MyVertex* fitter(std::vector< MyTrack >, 
                    int m_maxIteration,
                    double m_chi2min,
                    std::string fitModel,
                    int m_minNumbersOfTracks);

protected: ///< protected members
private: ///< private members
  

};
#endif // PRIMARYVERTEXFITTERTOOL_H
