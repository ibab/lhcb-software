#ifndef SVertexOneSeedTool_H
#define SVertexOneSeedTool_H 1

#include "Tagger.h"
#include "Event.h"
#include "GenericTool.h"

#include "TH1F.h"

using namespace std;

/** @class SVertexOneSeedTool
 *
 *  Tool to build an inclusive secondary vertex starting
 *  with a 2-track seed that has been built in DaVinci.
 *  The algorithm search for tracks to be added to the seed,
 *  and calculates a likelihood that can be used in a nnet combination
 *
 *  @author Marco Musy, Marc Grabalosa Gandara
 *  @date   2010-07-15
 */
class SVertexOneSeedTool  : public GenericTool {

 public:

  SVertexOneSeedTool( );
  ~SVertexOneSeedTool( ){} ///< Destructor

  Vertices buildVertex( Particles& parts, Event* evt );

 private:

  //static SVertexOneSeedTool m_svtool;    

  double m_maxprobf;
  double combine(double p1, double p2, double p3,
		 double p4, double p5, double p6);

  TH1F* hvprobf2;
  TH1F* hvprobf0;
  TH1F* hvChi0 ;
  TH1F* hvChi2 ;
  TH1F* hz0    ;
  TH1F* hz2    ;
  TH1F* hvpmin_2 ;
  TH1F* hvpmin_0 ;
  TH1F* hvipmin2 ;
  TH1F* hvipmin0 ;
  TH1F* hvipmax2 ;
  TH1F* hvipmax0 ;
  TH1F* hvipsmin2 ;
  TH1F* hvipsmin0 ;
  TH1F* hvdphi2;
  TH1F* hvdphi0 ;
  TH1F* hvmass2 ;
  TH1F* hvmass0 ;
  TH1F* hmaxprobf2 ;
  TH1F* hmaxprobf0 ;
  TH1F* hvreson2 ;
  TH1F* hvreson0 ;
  TH1F* hvsvpointtheta2 ;
  TH1F* hvsvpointtheta0 ;

};
#endif 
