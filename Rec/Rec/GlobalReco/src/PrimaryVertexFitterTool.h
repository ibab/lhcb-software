// $Id: PrimaryVertexFitterTool.h,v 1.3 2002-10-22 21:56:36 gcorti Exp $
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

  HepVector3D pos;
  HepVector3D slope;
  double p; ///< momentum at 1st measured point
  HepSymMatrix Cov; ///< covariant matrix of all above parameters
  double weight; 
  double d0; ///< distance to PV position
  double errd0; ///< error of d0
  HepVector3D vd0; ///< vector d0
  double chi2; ///< chi2 of track fit
  bool isUsed; ///< flag  to using or rejecting track
  int nBin; ///< number of bin in zclose 
  HepVector3D unitVect;
  int nbSta;
  double L; ///< dist between last and first point in velo
  double Z; ///< z of 1st point in Velo
  SmartRef<TrStoredTrack> track;
};

class MyVertex {
public:
  HepVector3D pos;
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
  //  virtual MyVertex* fitter(std::vector< MyTrack > mytr, 
  virtual StatusCode fitter(std::vector< MyTrack > mytr, 
                           //       std::vector< MyVertex >,
                            int m_maxIteration,
                            double m_chi2min,
                            std::string fitModel,
                            int m_minNumbersOfTracks,
                      MyVertex* vtx) = 0;

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
  
  //  MyVertex* fitter(std::vector< MyTrack >, 
  StatusCode fitter(std::vector< MyTrack > mytr, 
                    int m_maxIteration,
                    double m_chi2min,
                    std::string fitModel,
                    int m_minNumbersOfTracks,
                    MyVertex* vtx);

protected: ///< protected members
private: ///< private members
  

};
#endif // PRIMARYVERTEXFITTERTOOL_H
