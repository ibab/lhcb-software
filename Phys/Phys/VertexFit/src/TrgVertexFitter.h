// $Id: TrgVertexFitter.h,v 1.1 2005-02-04 10:12:53 pkoppenb Exp $
#ifndef TRGVERTEXFITTER_H 
#define TRGVERTEXFITTER_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "Kernel/IVertexFitter.h"            // Interface
#include "Event/TrgTrack.h"


/** @class TrgVertexFitter TrgVertexFitter.h
 *  
 *  Fast unconstrained vertex fitter that works only with particles 
 *  made from TrgTracks and assumes cylindrical errors and straight 
 *  trajectories
 *
 *  @author Hugo Ruiz Perez
 *  @date   2005-01-31
 */
class TrgVertexFitter : public GaudiTool, virtual public IVertexFitter {
public: 
  /// Standard constructor
  TrgVertexFitter( const std::string& type, 
                   const std::string& name,
                   const IInterface* parent);

  /// Method to fit a vertex from a vector of Particles
  StatusCode fitVertex( const  ParticleVector&,  Vertex& ); 

  /// Method to fit a vertex from an ARRAY of Particles
  StatusCode fitVertex( const int, Particle* partArray[],  Vertex& ); 

  /// Method to fit a vertex between two given Particles
  StatusCode fitVertex( Particle&, Particle&,  Vertex& ); 

  /// Method to fit a vertex between three given Particles
  StatusCode fitVertex( Particle&, Particle&, Particle&, Vertex&); 

  /// Method to fit a vertex from a vector of tracks
  StatusCode fitVertexFromTracks( const std::vector<const TrgTrack*>,  Vertex&);

  /// Method to fit a vertex from a vector of tracks
  StatusCode fitVertexFromTracks( const int, const TrgTrack* trackArray[],  Vertex& );

  /// Method to fit a vertex from two tracks
  StatusCode fitVertexFromTracks( const TrgTrack&, const TrgTrack&,  Vertex&);
  
  /// Method to fit a vertex from three tracks
  StatusCode fitVertexFromTracks( const TrgTrack&, const TrgTrack&, const TrgTrack&,  Vertex&);
  
  virtual ~TrgVertexFitter( ); ///< Destructor

protected:
  StatusCode TrgVertexFitter::vertexPositionAndError(const double& AX, const double& BX, const double& CX,
                                                     const double& DX, const double& EX,
                                                     const double& AY, const double& BY, const double& CY,
                                                     const double& DY, const double& EY,
                                                     double& vX, double& vY, double& vZ, Vertex &V);

private:

};
#endif // TRGVERTEXFITTER_H
