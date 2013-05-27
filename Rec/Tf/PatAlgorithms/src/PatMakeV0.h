#ifndef PATMAKEKSHORT_H
#define PATMAKEKSHORT_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiHistoAlg.h"  //MB


// from AIDA Histogramming  //MB
#include "AIDA/IHistogram1D.h"  //MB
#include "AIDA/IHistogram2D.h"  //MB


#include "Event/Track.h"

/** @class PatMakeV0 PatMakeV0.h
 *
 *
 *  @author Maurice Benayoun
 *  @date   2007-02-09
 */
namespace LHCb
{class TwoProngVertex;
}

class PatMakeV0 : public GaudiHistoAlg {
public:
  /// Standard constructor
  PatMakeV0( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~PatMakeV0( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

  // Small data class to hold a V0 candidate

  class PatV0Candidate {
  public:
    PatV0Candidate( const LHCb::Track* posTrk,
                    const LHCb::Track* negTrk,
                    double chi2, double massKs,  double zVtxKs,double primOK,
		    double allInvariantMasses[],double angle,int codeSolMass,
		    double xImpactRef, double yImpactRef,
		    double momTrackPlus[],double momTrackMinus[],double vtxLoc[]) {
      m_negTrk = negTrk;
      m_posTrk = posTrk;
      m_chi2   = chi2;
      m_primOK  = primOK;
      m_massKs = massKs;
      m_zVtxKs = zVtxKs;
      m_keep    = true;
      
      m_angDist=angle;
      m_xAtZero=xImpactRef;
      m_yAtZero=yImpactRef;
      m_codeSolMass=codeSolMass; // from 0 to 7
      m_allV0Masses[0]=allInvariantMasses[0];
      m_allV0Masses[1]=allInvariantMasses[1];
      m_allV0Masses[2]=allInvariantMasses[2];
      	for (int i=0;i<4;i++){
      		m_momTrackPlus[i]=momTrackPlus[i];		// momentum at V0 Vtx
    		m_momTrackMinus[i]=momTrackMinus[i];		// momentum at V0 Vtx
      	}
        for(int i=0;i<3;i++){
  		m_vtxLocation[i]=vtxLoc[i];			// Vertex Location 0=z,1=x,2=y
  	}

   }

    ~PatV0Candidate() {};

    const LHCb::Track* negTrk() const  { return m_negTrk; }
    const LHCb::Track* posTrk() const  { return m_posTrk; }
    double chi2()         const  { return m_chi2;   }
    double primOK()       const  { return m_primOK; }
    double massKs()       const  { return m_massKs; }
    double zVtxKs()       const  { return m_zVtxKs; }
    double keep()         const  { return m_keep;   }
    double invMassK0()		const  { return m_allV0Masses[0];	}
    double invMassLambda()	const  { return m_allV0Masses[1];	}
    double invMassLambdaBar()	const  { return m_allV0Masses[2];	}
    int codeSolMass()		const  { return m_codeSolMass ;	 	}
    double angle()		const  { return m_angDist;  		}
    double IPx()		const  { return m_xAtZero;  		}
    double IPy()		const  { return m_yAtZero;  		}
    double momTrackPlusX()	const  { return m_momTrackPlus[1];	}
    double momTrackPlusY()	const  { return m_momTrackPlus[2];	}
    double momTrackPlusZ()	const  { return m_momTrackPlus[3];	}
    double momTrackMinusX()	const  { return m_momTrackMinus[1];	}
    double momTrackMinusY()	const  { return m_momTrackMinus[2];	}
    double momTrackMinusZ()	const  { return m_momTrackMinus[3];	}
    double vtxZ()		const  { return m_vtxLocation[0];	}
    double vtxX()		const  { return m_vtxLocation[1];	}
    double vtxY()		const  { return m_vtxLocation[2];	}
    	
    void setKeep( bool keep )    { m_keep = keep;   }

  private:
    const LHCb::Track* m_negTrk;
    const LHCb::Track* m_posTrk;
    double m_xAtZero;
    double m_yAtZero;
    double m_chi2;
    double m_primOK;
    double m_massKs;
    double m_zVtxKs;
    bool   m_keep;
    int m_codeSolMass;
    double m_angDist;
    double m_allV0Masses[3];
    double m_momTrackPlus[4];	//[0] dummy ,px=[1],py=[2],pz=[3]
    double m_momTrackMinus[4];	//[0] dummy ,px=[1],py=[2],pz=[3]
    double m_vtxLocation[3];	// Vertex Location 0=z,1=x,2=y
};

  // Another small class to hold a track while processing

  class LocalV0Track {
  public:
    LocalV0Track() {};

    ~LocalV0Track() {};

    const LHCb::Track* track;

    double cx[3];
    double cy[2];
    double slpx[5];
    double slpy[3];
    double p;

    double x( double z ) const { return cx[0] + z * (cx[1] + z* cx[2]); }
    double y( double z ) const { return cy[0] + z * cy[1]; }

    double slx( double z ) const { return slpx[0] + z*( slpx[1] + z*( slpx[2] + z*( slpx[3] + z*slpx[4]))); }
    double sly( double z ) const { return slpy[0] + z*( slpy[1] + z * slpy[2] ); }
  };

protected:
 

private:


  // class variables
  typedef KeyedContainer<LHCb::TwoProngVertex, Containers::HashMap> V0s;  	// Sean Modif
  std::string m_outputName;
  LHCb::Tracks* m_outputContainer;
  std::string m_outputV0ContainerName;  					// Sean Modif
  V0s* m_outputV0Container;  							// Sean Modif

  double m_Pion;      // pion mass
  double m_Proton;    // proton mass
  double m_Kshort;    // Ks mass
  double  m_Lambda;   // Lambda mass
  double m_zV0Vtx;
  double m_xV0Vtx;
  double m_yV0Vtx;
  
  double m_momTrackPlus[4];	//[0] dummy ,px=[1],py=[2],pz=[3]
  double m_momTrackMinus[4];	//[0] dummy ,px=[1],py=[2],pz=[3]
  double m_V0Mom[4];   		//[0] dummy ,px=[1],py=[2],pz=[3]
  
  double m_unitVector[3];  //z =[0],x=[1],y=[2]
  double m_invariantKsMass;
  double m_invariantLambdaMass;
  double m_invariantLambdaBarMass;
  double m_xImpact;
  double m_yImpact;

  double  m_sigmaX;
  double  m_sigmaY;
  double  m_sigmaZ;
  double  m_chiSq;
  int 	  m_iterVtx;
  double  m_angleCut;
  double  m_cutPt;

 
  double m_chi2VtxLim;
  double m_primVtxOKLim;
  double m_impactCurNormLim;

  double m_impactParV0;
  double m_impactParTrkX;
  double m_impactParTrkY;
  double m_impactParSlp;
  double  m_zMinVtxImp;
  double  m_zMaxVtxImp;
  std::vector<double> m_zPrimVtx;

  double m_SigK0 ;
  double m_SigLambda ;
  double m_Nsig;
  double m_invariantV0MassRef;
  double m_allInvariantMasses[3];
  double m_xImpactRef;
  double m_yImpactRef;
  double m_V0vtxRefZ;   
  double m_vtxRef[3];  
  double m_bestSol;
  double m_primOK;
    enum EncodedCompatiblePID {none =0 , K0s = 1, Lambda = 2, LambdaBar=4};  	// Sean Modif
  int m_codeSolMass;
  double  m_angle;
  
  LHCb::ParticleID m_K0sPID, m_lambdaPID, m_lambdaBarPID;  			// Sean Modif
  

  std::vector<PatV0Candidate> m_solutions;
  std::vector<PatV0Candidate> m_selected;

  double m_vectUnits[55];  // conversion to mm /rad/MeV (cf initialiseRichVelo_Ks()
  double m_vectUnitsZ[5];  // conversion to mm (cf initialiseRiches_Ks()

  double m_zRefRich1;   // positions of input states
  double m_zKsMin;    	// lowest z location for Ks Vtx
  double m_pMinRich1;  	// minimum momentum for extrapol. in rich1

  double m_vdat[6];  // local copy of state at 2350 mm

  double m_xProj_Rich1_Ks[3][55];
  double m_yProj_Rich1_Ks[2][55];

  double m_xProj_Rich1_Ks_Der[5][55];
  double m_yProj_Rich1_Ks_Der[3][55];

  int m_ideg_x_Rich1_Ks_Der;   // z-polynomial degree *******
  int m_ideg_y_Rich1_Ks_Der;

  int m_ideg_phx_Rich1_Ks_Der;   // phase space degree
  int m_ideg_phy_Rich1_Ks_Der;

  int m_jparx_Rich1_Ks_Der;   // length of parameter vector in ph. sp.
  int m_jpary_Rich1_Ks_Der;

  int m_ideg_phx_Rich1_Ks;   // phase space degree
  int m_ideg_phy_Rich1_Ks;

  int m_ideg_x_Rich1_Ks;   // z-polynomial degree  *******
  int m_ideg_y_Rich1_Ks;

  int m_jparx_Rich1_Ks;   // length of parameter vector in ph. sp.
  int m_jpary_Rich1_Ks;

  mutable  double m_coefDevRich1_X[4];  // maximum degree for z-polynomial is 3
  mutable  double m_coefDevRich1_Y[4];  // maximum degree for z-polynomial is 3

  mutable  double m_coefDevRich1_X_Der[5];  // maximum degree for z-polynomial is 4
  mutable  double m_coefDevRich1_Y_Der[4];  // maximum degree for z-polynomial is 3

  double m_trackExpansionX[3];  // z-polynomials coefficients
  double m_trackExpansionY[2];
  double m_slopeExpansionX[5];
  double m_slopeExpansionY[3];
  //  methods
  void createInputState(LHCb::State& m_state);
  void initialiseRichVelo_Ks();
  void initialiseRich1_Ks();
  void initialiseRich1_Ks_Der();
  void builtVectProj(int & jpar, double vectPar[],int idegPhsp) const ;
  void computeParamRichVelo_Ks() const ;
  void computeParamRichVelo_Ks_Der() const ;
  void storeTrackParams_prov();
  void recoFromParams_prov(double z, double & xRec,double & yRec,double & txRec,double & StyRec);
  double xCoordFast(double z) const;
  double yCoordFast(double z) const;

  double xSlopeFast(double z) const;
  double ySlopeFast(double z) const;
  void   storeV0Info( double zV , double primOK, int codeSolMass) ;
  void   storeV0Info( double vtxLoc[] , double primOK, int codeSolMass) ;
  void selectSols();

  std::vector<LocalV0Track> m_pos;
  std::vector<LocalV0Track> m_neg;

  //-------------------------------------------------------------------------------
  bool checkPhaseSpaceAtOrigin(){
    double xPhSp=xCoordFast(0.);
    if(fabs(xPhSp ) > m_impactParTrkX) return true;
    double yPhSp=yCoordFast(0.);
    if(fabs(yPhSp ) > m_impactParTrkY) return true;
    double txPhSp=xSlopeFast(0.);
    if(fabs(txPhSp ) > m_impactParSlp) return true;
    double tyPhSp=ySlopeFast(0.);
    if(fabs(tyPhSp ) > m_impactParSlp) return true;
    return false;
  }

  //-------------------------------------------------------------------------------
  StatusCode copyV0ParamsForTES(PatV0Candidate& cand){
 
 // list of tranfered variables can be easily extended
  
  m_momTrackPlus[1]=cand.momTrackPlusX();
  m_momTrackPlus[2]=cand.momTrackPlusY();
  m_momTrackPlus[3]=cand.momTrackPlusZ();
  
  m_momTrackMinus[1]=cand.momTrackMinusX();
  m_momTrackMinus[2]=cand.momTrackMinusY();
  m_momTrackMinus[3]=cand.momTrackMinusZ();
  
  m_chiSq=cand.chi2();
  m_V0vtxRefZ=cand.zVtxKs(); // z vetex of the V0
  
  m_xImpactRef=cand.IPx();
  m_yImpactRef=cand.IPy();
  
  m_vtxRef[0]=cand.vtxZ();
  m_vtxRef[1]=cand.vtxX();
  m_vtxRef[2]=cand.vtxY();
  
  
  m_invariantKsMass=cand.invMassK0();
  m_invariantLambdaMass=cand.invMassLambda();
  m_invariantLambdaBarMass=cand.invMassLambdaBar();
  m_codeSolMass=cand.codeSolMass();
  if(m_codeSolMass == 1){
  	if(m_invariantKsMass <0 ) return StatusCode::FAILURE;
  	return StatusCode::SUCCESS;
  } else if(m_codeSolMass == 2){
  	if(m_invariantLambdaMass<0 )return StatusCode::FAILURE;
	return StatusCode::SUCCESS;
  }else if(m_codeSolMass == 4){
  	if(m_invariantLambdaBarMass<0 )return StatusCode::FAILURE;
	return StatusCode::SUCCESS;
   }else if(m_codeSolMass == 3){
  	if(m_invariantKsMass <0 && m_invariantLambdaMass<0) return StatusCode::FAILURE;
 	return StatusCode::SUCCESS;
   }else if(m_codeSolMass == 5){
   	if(m_invariantKsMass <0 && m_invariantLambdaBarMass<0) return StatusCode::FAILURE;
 	return StatusCode::SUCCESS;
  }

  return StatusCode::FAILURE;		// if m_invariantKsMass is none of the listed cases
  }

  //-------------------------------------------------------------------------------
 virtual StatusCode MakeV0(const LHCb::Track *  posTrack,const LHCb::Track *  negTrack, int IDsFlag);
 //-------------------------------------------------------------------------------
  StatusCode intersection( LocalV0Track& pos, LocalV0Track& neg ) {
    double a = pos.cx[2] - neg.cx[2];
    double b = pos.cx[1] - neg.cx[1];
    double c = pos.cx[0] - neg.cx[0];
    // eq is a z^2 + b z + c=0

    double delta = b*b-4.* a *c;

    if( delta < 0 || fabs(a) < 1.0e-12 ) return StatusCode::FAILURE;
    double sq=sqrt(delta);
    double zX1=(-b + sq)/2/a;
    double zX2=(-b - sq)/2/a;

    double e = pos.cy[0] - neg.cy[0];
    double f = pos.cy[1] - neg.cy[1];
    // eq is e +f z=0

    if(fabs(f) < 1.e-12) return StatusCode::FAILURE;
    double zy = -e/f;

    double test1=(zX1-m_zKsMin) * (zX1-m_zRefRich1);
    double test2=(zX2-m_zKsMin) * (zX2-m_zRefRich1);

    debug() << "      zX1 " << zX1 << " zX2 " << zX2 << " test1 " << test1 << " test2 " << test2 << endmsg;

    if( test1 > 0 && test2 > 0)return StatusCode::FAILURE;

    if( test1 < 0 && test2 < 0){
      m_zV0Vtx = zX1;
      if(fabs(zX1-zy) > fabs(zX2-zy)) m_zV0Vtx = zX2;
    } else if( test1 < 0 && test2 > 0){
      m_zV0Vtx = zX1;
    }else if( test1 > 0 && test2 < 0){
      m_zV0Vtx = zX2;
    }
    m_xV0Vtx = pos.x( m_zV0Vtx );
    m_yV0Vtx = .5 * ( pos.y( m_zV0Vtx ) + neg.y( m_zV0Vtx ) );
    debug() << "     Found vertex z " << m_zV0Vtx << " x " << m_xV0Vtx << " y "<< m_yV0Vtx << endmsg;
    return StatusCode::SUCCESS;
  }
//-------------------------------------------------------------------------------
  StatusCode improveVTX( LocalV0Track& pos, LocalV0Track& neg ) {

    double a = pos.cx[2] - neg.cx[2];
    double b = pos.cx[1] - neg.cx[1];
    double c = pos.cx[0] - neg.cx[0];

    double e = pos.cy[0] - neg.cy[0];
    double f = pos.cy[1] - neg.cy[1];

    double vx = m_sigmaX * m_sigmaX;
    double vy = m_sigmaY * m_sigmaY;

    // equation dchi2/dz=0 is A + B z + C z^2 + D z ^3 =0
    double A = c*b/vx+e*f/vy;
    double B = (b*b+2*a*c)/vx+f*f/vy;
    double C = 3 *a*b/vx;
    double D = 2*a*a/vx;
    // improve z :
    double zV= m_zV0Vtx;
    bool stop = false;
    int nIterate=0;
    while ( !stop ) {
      nIterate++;
      double AA = A + zV * ( B + zV * ( C + zV * D));
      double BB = B + zV * ( 2 * C + 3 * zV * D );
      if(fabs(BB) < 1.e-12) return StatusCode::FAILURE;
      double dz = -AA/BB;
      zV += dz;
      stop = fabs(dz) < 5 || nIterate > m_iterVtx;
    }

    double xV = 0.5 * ( pos.x(zV) + neg.x(zV) );
    double yV = 0.5 * ( pos.y(zV) + neg.y(zV) );

    m_zV0Vtx=zV;
    m_xV0Vtx=xV;
    m_yV0Vtx=yV;
    double uu = (a*zV*zV+b*zV+c)/m_sigmaX;
    double vv = (e+f*zV)/m_sigmaY;
    m_chiSq   = uu*uu+vv*vv;

    return StatusCode::SUCCESS;
  }
//-------------------------------------------------------------------------------
  StatusCode reco4Momentum(LocalV0Track& pos, LocalV0Track& neg ) {
  
  reco3Momentum( pos , neg ) ;
  StatusCode  sc= recoInvariantMasses (); 
  if( sc.isFailure() ) return sc;
  return StatusCode::SUCCESS;
}   
  
//-------------------------------------------------------------------------------
  void reco3Momentum( LocalV0Track& pos, LocalV0Track& neg ) {

    double zV=m_zV0Vtx;
    double derXPos = pos.slx( zV );
    double derXNeg = neg.slx( zV );
    double derYPos = pos.sly( zV );
    double derYNeg = neg.sly( zV );

    double qNormP=sqrt(1.+derXPos*derXPos+derYPos*derYPos);
    double qNormN=sqrt(1.+derXNeg*derXNeg+derYNeg*derYNeg);

    m_momTrackPlus[1]=derXPos/qNormP * pos.p;
    m_momTrackPlus[2]=derYPos/qNormP * pos.p;
    m_momTrackPlus[3]=1./qNormP      * pos.p;

    m_momTrackMinus[1]=derXNeg/qNormN * neg.p;
    m_momTrackMinus[2]=derYNeg/qNormN * neg.p;
    m_momTrackMinus[3]=1./qNormN      * neg.p;

    double pplusSq=0.;
    double pminusSq=0.;

    for(int i=1; i < 4; i++){
      pplusSq  +=m_momTrackPlus[i]*m_momTrackPlus[i];
      pminusSq +=m_momTrackMinus[i]*m_momTrackMinus[i];
    }
    m_momTrackPlus[0]  = pplusSq;		// ATTENTION not the energy but the momentum squared 
    m_momTrackMinus[0] = pminusSq;		// ATTENTION not the energy but the momentum squared 
    
    for( int i=1 ; i<4 ;i++){
      m_V0Mom[i]=m_momTrackPlus[i]+m_momTrackMinus[i];
    }
//    
    double modPsq=0;
    for(int i=1; i < 4; i++){
      modPsq +=m_V0Mom[i]*m_V0Mom[i];
    }
   double modP=sqrt(modPsq);
// V0 direction	in space
    m_unitVector[0]=m_V0Mom[3]/modP;
    m_unitVector[1]=m_V0Mom[1]/modP;
    m_unitVector[2]=m_V0Mom[2]/modP;
// transverse coordinates of impact at z==0
    m_xImpact=m_xV0Vtx-m_unitVector[1]/m_unitVector[0]*m_zV0Vtx;
    m_yImpact=m_yV0Vtx-m_unitVector[2]/m_unitVector[0]*m_zV0Vtx;
  }
//-------------------------------------------------------------------------------
  StatusCode recoInvariantMasses () {
// double m_invariantKsMass;
//  double m_invariantLambdaMass;
//  double m_invariantLambdaBarMass;

double u_p=m_momTrackPlus[0];
double u_m=m_momTrackMinus[0];

   double modPsq=0;
    for(int i=1; i < 4; i++){
      	modPsq +=m_V0Mom[i]*m_V0Mom[i];
    } 
    
    int nc=0;
       
// Ks inv mass	
	m_momTrackPlus[0] =sqrt(u_p + m_Pion*m_Pion);	
	m_momTrackMinus[0]=sqrt(u_m + m_Pion*m_Pion);
	
	m_V0Mom[0]=m_momTrackPlus[0]+m_momTrackMinus[0];   
	
	m_invariantKsMass=m_V0Mom[0]*m_V0Mom[0]-modPsq;
	if(m_invariantKsMass>0) {
		m_invariantKsMass=sqrt(m_invariantKsMass);
		nc++;
	} else{
		m_invariantKsMass=-1.;
	}
      
// lambda inv mass     
     
	m_momTrackPlus[0] =sqrt(u_p + m_Proton*m_Proton);	
	m_momTrackMinus[0]=sqrt(u_m + m_Pion*m_Pion);
	
	m_V0Mom[0]=m_momTrackPlus[0]+m_momTrackMinus[0];   
	
	m_invariantLambdaMass=m_V0Mom[0]*m_V0Mom[0]-modPsq;
	if(m_invariantLambdaMass>0) {
		m_invariantLambdaMass=sqrt(m_invariantLambdaMass);
		nc++;
	} else{
		m_invariantLambdaMass=-1.;
	}
	
// lambdaBar inv mass     
     
	m_momTrackPlus[0] =sqrt(u_p +  m_Pion*m_Pion);	
	m_momTrackMinus[0]=sqrt(u_m +  m_Proton*m_Proton);
	
	m_V0Mom[0]=m_momTrackPlus[0]+m_momTrackMinus[0];   
	
	m_invariantLambdaBarMass=m_V0Mom[0]*m_V0Mom[0]-modPsq;
	if(m_invariantLambdaBarMass>0) {
		m_invariantLambdaBarMass=sqrt(m_invariantLambdaBarMass);
		nc++;
	} else{
		m_invariantLambdaBarMass=-1.;
	}
	if(nc == 0) return StatusCode::FAILURE;
	return StatusCode::SUCCESS;
     
}     
//-------------------------------------------------------------------------------

  void RecoPhSp( double cx[],double cy[],double slpx[],double slpy[],
   double & xOrigin, double & txOrigin,double & yOrigin, double & tyOrigin ){
    // coordinates and slopes (LHCb definitions) at zOrigin=0

    xOrigin=cx[0];
    yOrigin=cy[0];

    double derX=slpx[0];
    double derY=slpy[0];
    double qq=derX*derX+derY*derY+1;
    qq=sqrt(qq);
    txOrigin=derX/qq;
    tyOrigin=derY/qq;
  }

  //-------------------------------------------------------------------------------
  double CheckPrimaryVtx( LocalV0Track& pos, LocalV0Track& neg ) { 
  
    double distRef = 1.e10;
    
    for( std::vector<double>::iterator itZ = m_zPrimVtx.begin();
         m_zPrimVtx.end() != itZ; ++itZ ) {
      double zv = (*itZ);
      double xv = 0.5 * ( pos.x( zv ) + neg.x( zv ) );
      double yv = 0.5 * ( pos.y( zv ) + neg.y( zv ) );
      double dist2x=(xv-m_xV0Vtx)/m_sigmaX;
      double dist2y=(yv-m_yV0Vtx)/m_sigmaY;

      double distCurrent=dist2x*dist2x +dist2y*dist2y;

      if(distCurrent < distRef) distRef=distCurrent;
    }
    return distRef ;

  }
 //-------------------------------------------------------------------------------
double vtxMomAngle(LHCb::RecVertices* primaryVertices)   {
  double distRef = 1.e10;
  int VtxLoc=-1;
  for( std::vector<double>::iterator itZ = m_zPrimVtx.begin();
     m_zPrimVtx.end() != itZ; ++itZ ) {
     double zPrimVtx= (*itZ);
     double xAtPrimVtx=m_xV0Vtx+m_unitVector[1]/m_unitVector[0]*(zPrimVtx-m_zV0Vtx);
     double yAtPrimVtx=m_yV0Vtx+m_unitVector[2]/m_unitVector[0]*(zPrimVtx-m_zV0Vtx);
     double distCurrent=xAtPrimVtx*xAtPrimVtx + yAtPrimVtx*yAtPrimVtx;
     distCurrent=sqrt(distCurrent);
      if(distCurrent < distRef) {
		VtxLoc=itZ-  m_zPrimVtx.begin();
       	distRef=distCurrent;}
}
	LHCb::RecVertices::const_iterator itV=primaryVertices->begin()+VtxLoc;
	double xPrimVtx=(*itV)->position().x();
	double yPrimVtx=(*itV)->position().y();
	double zPrimVtx=(*itV)->position().z();
	
 	double flightDir[3];
 	flightDir[0]=m_zV0Vtx-zPrimVtx;
 	flightDir[1]=m_xV0Vtx-xPrimVtx;
 	flightDir[2]=m_yV0Vtx-yPrimVtx;
	double VtxDir=flightDir[0]*flightDir[0]+flightDir[1]*flightDir[1]+flightDir[2]*flightDir[2];
	VtxDir=sqrt(VtxDir);
	for(int i=0;i<3;i++){
	flightDir[i] /=VtxDir;
	}
  	double scal=0.;	
	for(int i=0;i<3;i++){
		scal +=flightDir[i]*m_unitVector[i];
	}
	double dang=acos(scal);
	
   	return dang ;
}
//-------------------------------------------------------------------------------
double NormedSmallestImpactParameter()   {
  double distRef = 1.e10;
  for( std::vector<double>::iterator itZ = m_zPrimVtx.begin();
     m_zPrimVtx.end() != itZ; ++itZ ) {
     double zPrimVtx= (*itZ);
     double xAtPrimVtx=m_xV0Vtx+m_unitVector[1]/m_unitVector[0]*(zPrimVtx-m_zV0Vtx);
     xAtPrimVtx/=m_sigmaX;
     double yAtPrimVtx=m_yV0Vtx+m_unitVector[2]/m_unitVector[0]*(zPrimVtx-m_zV0Vtx);
     yAtPrimVtx/=m_sigmaY;
     double distCurrent=xAtPrimVtx*xAtPrimVtx + yAtPrimVtx*yAtPrimVtx;
     distCurrent=sqrt(distCurrent);
      if(distCurrent < distRef) distRef=distCurrent;
}
   return distRef ;
}
 //-------------------------------------------------------------------------------

  AIDA::IHistogram1D * m_recoMomentum;
  AIDA::IHistogram1D * m_vtxMomAngAll;
  AIDA::IHistogram1D * m_ptRec;
  
  AIDA::IHistogram1D * m_xOrg;
  AIDA::IHistogram1D * m_yOrg;
  AIDA::IHistogram1D * m_txOrg;
  AIDA::IHistogram1D * m_tyOrg;

  AIDA::IHistogram1D * m_zPrimVtxh;
  AIDA::IHistogram2D * m_xyPrimVtx;
  AIDA::IHistogram1D * m_checkPrimChi2;


  AIDA::IHistogram1D * m_zRec;
  AIDA::IHistogram2D * m_rVsz;

  AIDA::IHistogram1D * m_invMassRecoKs;
  AIDA::IHistogram1D * m_invMassRecoLambda;
  AIDA::IHistogram1D * m_invMassRecoLambdaBar;
  
  AIDA::IHistogram2D * m_impactAtOriginReco;
  AIDA::IHistogram1D * m_multiplicity;

  AIDA::IHistogram2D * m_impactAtOriginChoice;
  AIDA::IHistogram1D * m_plotChi2;

  AIDA::IHistogram1D * m_nSolSelect;
  AIDA::IHistogram1D * m_nSolSelectTotal;
  
  AIDA::IHistogram1D * m_KsMassSelect;
  AIDA::IHistogram1D * m_LambdaMassSelect;
  AIDA::IHistogram1D * m_LambdaBarMassSelect;
  
  AIDA::IHistogram1D * m_codeMassSelect;
  AIDA::IHistogram1D * m_wrongCase;
  
  AIDA::IHistogram1D * m_bestchi2Select;
  AIDA::IHistogram1D *   m_zRecSelect;

};
#endif // PATMAKEKSHORT_H
