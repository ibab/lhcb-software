// $Id: PatCalibV0.h,v 1.1 2008-01-10 15:36:43 ocallot Exp $
#ifndef PATCALIBKSHORT_H
#define PATCALIBKSHORT_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiHistoAlg.h"  //MB


// from AIDA Histogramming  //MB
#include "AIDA/IHistogram1D.h"  //MB
#include "AIDA/IHistogram2D.h"  //MB


#include "Event/Track.h"
#include "Linker/LinkedTo.h"
#include "Event/MCParticle.h"
#include "GaudiKernel/IRegistry.h"

#include "Event/MCTrackInfo.h"

/** @class PatCalibV0 PatCalibV0.h
 *
 *
 *  @author Maurice Benayoun
 *  @date   2007-02-09
 */
class PatCalibV0 : public GaudiHistoAlg {
public:
  /// Standard constructor
  PatCalibV0( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~PatCalibV0( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization
  // Small data class to hold a V0 candidate

  class PatV0Candidate {
  public:
    PatV0Candidate( const LHCb::Track* posTrk,const LHCb::Track* negTrk,
    			bool hasNoVeloPair,double chi2,double allInvariantMasses[],
                     double massKs, double zVtxKs,double angle, int flagV0,
                    bool mcCheck ,bool mcCheckBd ,bool mcCheckLambda_b,
		    double primOK, int codeSolMass, double Armanteros, double pTrans ) {
      m_negTrk = negTrk;
      m_posTrk = posTrk;
      m_hasNoVeloPair =hasNoVeloPair;
      m_chi2   = chi2;
      m_massKs = massKs;
      m_zVtxKs = zVtxKs;
      m_mcCheck = mcCheck;
      m_mcCheckBd = mcCheckBd;
      m_mcCheckLambda_b = mcCheckLambda_b;
      m_primOK  = primOK;
      m_keep    = true;
      m_angDist=angle;
      m_pTrans=pTrans;
      m_Armanteros=Armanteros;
      
      m_flagV0=flagV0; //is 0,1,2 or 4 (wrong, k0, lambda,lambdaBar
      m_codeSolMass=codeSolMass; // from 0 to 7
      m_allV0Masses[0]=allInvariantMasses[0];
      m_allV0Masses[1]=allInvariantMasses[1];
      m_allV0Masses[2]=allInvariantMasses[2];
    }

    ~PatV0Candidate() {};

    const LHCb::Track* negTrk() const  { return m_negTrk; 		}
    const LHCb::Track* posTrk() const  { return m_posTrk; 		}
    double chi2()         	const  { return m_chi2;    		}
    double massKs()      	const { return m_massKs; 		}
    double zVtxKs()       	const  { return m_zVtxKs;   		}
    bool   mcCheck()      	const  { return m_mcCheck;  		}
    bool   mcCheckBd()    	const  { return m_mcCheckBd;		}
    bool   mcCheckLambda_b()    const  { return m_mcCheckLambda_b ;	}
    bool   hasNoVeloPair()    	const  { return m_hasNoVeloPair;	}
    double primOK()       	const  { return m_primOK;   		}
    double keep()         	const  { return m_keep;     		}
    double angle()		const  { return m_angDist;  		}
    int codeSolMass()		const  { return m_codeSolMass ;	 	}
    double invMassK0()		const  { return m_allV0Masses[0];	}
    double invMassLambda()	const  { return m_allV0Masses[1];	}
    double invMassLambdaBar()	const  { return m_allV0Masses[2];	}
    int flagTrueV0Kind()	const  { return m_flagV0;		}
    double pTrans()		const  { return m_pTrans;		}
    double Armanteros()		const  { return m_Armanteros ;		}
    void setKeep( bool keep )    { m_keep = keep;     }

  private:
    const LHCb::Track* m_negTrk;
    const LHCb::Track* m_posTrk;
    bool m_hasNoVeloPair;
    double m_chi2;
    double m_primOK;
    int m_codeSolMass;
    double m_massKs;
    double m_allV0Masses[3];
    double m_zVtxKs;
    bool   m_mcCheck;
    bool   m_mcCheckBd;
    bool   m_mcCheckLambda_b;
    bool   m_keep;
    int   m_flagV0;
    double m_angDist;
    double m_pTrans;
    double m_Armanteros;
  };

  // Another small class to hold a track while processing

  class LocalV0Track {
  public:
    LocalV0Track() {};

    ~LocalV0Track() {};

    const LHCb::Track* track;
    const LHCb::MCParticle* mc;
    const LHCb::MCParticle* parent;
    
    int trackCode;
    int parentCode;
    int grandParentCode;
    bool hasNoVeloTrack;
    
    double xOrg;
    double yOrg;
    double slpxOrg;
    double slpyOrg;
    
    bool notInPhSp;
    double cx[3];
    double cy[2];
    double slpx[5];
    double slpy[3];
    double p;
 
    double primitiveX;
    double primitiveY;

    double x( double z ) const { return cx[0] + z * (cx[1] + z* cx[2]); }
    double y( double z ) const { return cy[0] + z * cy[1]; }

    double slx( double z ) const { return slpx[0] + z*( slpx[1] + z*( slpx[2] + z*( slpx[3] + z*slpx[4]))); }
    double sly( double z ) const { return slpy[0] + z*( slpy[1] + z * slpy[2] ); }
     
  };

protected:

  LHCb::MCParticle* mcTruth( LHCb::Track* track ) {
    //== Get the truth on this track
    std::string containerName = track->parent()->registry()->identifier();
    std::string linkerName = "Link/"+containerName;
    if ( "/Event/" == containerName.substr(0,7) ) {
      linkerName = "Link/" + containerName.substr(7);
    }
    if ( exist<LHCb::LinksByKey>( linkerName ) ) {
      LinkedTo<LHCb::MCParticle, LHCb::Track> trLink( evtSvc(), msgSvc(), containerName );
      LHCb::MCParticle* part = trLink.first( track->key() );
      if ( 0 != part ) return part;
    }
    return 0;
  }

private:
  // class variables

  double m_Pion;    // pion mass
  double m_Proton;    // proton mass
  double m_Kshort;   // ks mass
  double m_Lambda;   // Lambda mass
  double m_zV0Vtx;
  double m_xV0Vtx;
  double m_yV0VTx;
  
  double m_momTrackPlus[4];
  double m_momTrackMinus[4];
  double m_pTrans;
  double m_Armanteros;
  double m_pTransRef;
  double m_ArmanterosRef;
    
  double m_KsMom[4];   		//E=[0],px=[1],py=[2],pz=[3]
  double m_LambdaMom[4];   	//E=[0],px=[1],py=[2],pz=[3]
  double m_V0Mom[4];   		//[0] dummy ,px=[1],py=[2],pz=[3]
  
  double m_unitVector[3];  //z =[0],x=[1],y=[2]
  
  double m_invariantKsMass;
  double m_invariantLambdaMass;
  double m_invariantLambdaBarMass;
  double m_xImpact;
  double m_yImpact;
  double m_impactCurrent;
  double m_impactCurrentNormed;

  double  m_sigmaX;
  double  m_sigmaY;
  double  m_sigmaZ;
  double  m_chiSq;
  int m_iterVtx;
  double  m_angleCut;
  double m_angle;
   
  double m_impactParV0;
  double m_impactParTrkX;
  double m_impactParTrkY;
  double m_impactParSlp;
  double  m_zMinVtxImp;
  double  m_zMaxVtxImp;
  double  m_chi2VtxLim;
  double  m_primVtxOKLim; 
  double  m_impactCurNormLim; 
  std::vector<double> m_zPrimVtx;

  double m_SigK0 ;
  double m_SigLambda ;
  double m_Nsig;
  double m_invariantV0MassRef;
  double m_allInvariantMasses[3];
  double m_xImpactRef;
  double m_yImpactRef;
  double m_V0vtxRefZ;   //
  bool m_MCcheckV0;
  int m_flagV0;
  bool m_MCcheckBd;
  bool m_MCcheckLambda_b;
  double m_primOK;
  double m_bestSol;
  int m_codeSolMass;
  int m_GrandParentCode;
  double m_xPhSpOrg;
  double m_yPhSpOrg;
  double m_slpxPhSpOrg;
  double m_slpyPhSpOrg;
	int m_nbPrimVtxInEvt;
bool m_checkPt;
double m_cutPt;

  std::vector<PatV0Candidate> m_solutions;
  std::vector<PatV0Candidate> m_selected;
  double m_QOverPTruth;

  double m_vectUnits[55];  // conversion to mm /rad/MeV (cf initialiseRichVelo_Ks()
  double m_vectUnitsZ[5];  // conversion to mm (cf initialiseRiches_Ks()

  double m_zRefRich1;    // positions of input states
  double m_zKsMin;    // lowest z location for Ks Vtx
  double m_pMinRich1;  // minimum momentum for extrapol. in rich1

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

  void   storeV0Info(int iflagV0,double zV, double primOK, bool check,
			bool grandMotherIsBd,bool grandMotherIsLambda_b,int codeSolMass) ; 
  bool MCTruthPair_K0(const LHCb::MCParticle* i_MCPos, const LHCb::MCParticle* i_MCNeg) ;
  bool MCTruthPair_Lambda(const LHCb::MCParticle* i_MCPos, const LHCb::MCParticle* i_MCNeg) ;
  bool MCTruthPair_LambdaBar(const LHCb::MCParticle* i_MCPos, const LHCb::MCParticle* i_MCNeg) ;

  void selectSols();


  std::vector<LocalV0Track> m_pos;
  std::vector<LocalV0Track> m_neg;

  //-------------------------------------------------------------------------------
  bool PatCalibV0::checkPhaseSpaceAtOrigin(){
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
  void PatCalibV0::storePhaseSpaceAtOrigin(){
    m_xPhSpOrg=xCoordFast(0.);
    m_yPhSpOrg=yCoordFast(0.);
    m_slpxPhSpOrg=xSlopeFast(0.);
    m_slpyPhSpOrg=ySlopeFast(0.);
  }
  //-------------------------------------------------------------------------------
  void PatCalibV0::testLambda_b(LHCb::MCParticle* mcP){		//unused (ceck of filiation
  if(!mcP) return;
 const LHCb::MCParticle* parent=0;
 
 int trackCode=mcP->particleID().pid(); 
   int grandParentCode=0;
   int parentCode=0;
   int grandGrandParentCode=0;
   int grandGrandGrandParentCode=0;
   
	int code[100];
	int imax=0;
 parent=mcP->mother();
  if(parent){
	imax=100;
  	parentCode=parent->particleID().pid();
	const LHCb::MCParticle* grandParent=parent->mother();
	const LHCb::MCParticle* grandGrandParent;
	const LHCb::MCParticle* grandGrandGrandParent;
	if(grandParent) {
		grandParentCode=grandParent->particleID().pid();
		grandGrandParent = grandParent->mother();
		if(grandGrandParent){
			grandGrandParentCode=grandGrandParent->particleID().pid();
			grandGrandGrandParent=grandGrandParent->mother();
			if(grandGrandGrandParent)grandGrandGrandParentCode=grandGrandGrandParent->particleID().pid();
		}
	}
	const LHCb::MCVertex * vtx= 	parent->originVertex () ;
	const SmartRefVector< LHCb::MCParticle > qq= vtx->products ()  ;
	
	int ii=qq.size();
	if(ii < imax) imax=ii;

	const LHCb::MCParticle *  zz;
	for(int i=0; imax>i;i++){
		zz=qq[i];
		int uu = zz->particleID().pid();
		code[i]=uu;
	}
	
  }
  std::cout<<" -------- new track ----------" <<std::endl;
 std::cout<< " trackCode = "<<trackCode<<"  parent code = " << parentCode <<std::endl;
 std::cout << "grandparent code = " << grandParentCode <<std::endl;
 std::cout << " ancestors to grandparent ="<< grandGrandParentCode<<" , "<<grandGrandGrandParentCode<<std::endl;
 if(grandParentCode){
 std::cout << " sister to parent track codes ="<<std::endl;  
 	for(int i=0; imax>i;i++){
	std::cout <<  code[i] << "  , ";
	}
std::cout <<std::endl;}
 }
  //-------------------------------------------------------------------------------
 StatusCode  PatCalibV0::intersection( LocalV0Track& pos, LocalV0Track& neg ) {
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

    debug() << "      zX1 " << zX1 << " zX2 " << zX2 << " test1 " << test1 << " test2 " << test2 << endreq;

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
    m_yV0VTx = .5 * ( pos.y( m_zV0Vtx ) + neg.y( m_zV0Vtx ) );
    debug() << "     Found vertex z " << m_zV0Vtx << " x " << m_xV0Vtx << " y "<< m_yV0VTx << endreq;
    return StatusCode::SUCCESS;
  }
  //-------------------------------------------------------------------------------
  StatusCode  PatCalibV0::improveVTX( LocalV0Track& pos, LocalV0Track& neg ) {

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
    m_yV0VTx=yV;
    double uu = (a*zV*zV+b*zV+c)/m_sigmaX;
    double vv = (e+f*zV)/m_sigmaY;
    m_chiSq   = uu*uu+vv*vv;

    return StatusCode::SUCCESS;
  }
  //-------------------------------------------------------------------------------
     StatusCode  PatCalibV0::improveVTX_new( LocalV0Track& pos, LocalV0Track& neg ) {
    double aX[6];
    aX[5]=(pos.slpx[4]-neg.slpx[4])/5;
    aX[4]=(pos.slpx[3]-neg.slpx[3])/4;
    aX[3]=(pos.slpx[2]-neg.slpx[2])/3;
    aX[2]=(pos.slpx[1]-neg.slpx[1])/2;
    aX[1]=(pos.slpx[0]-neg.slpx[0])/1;
    aX[0]=pos.primitiveX-neg.primitiveX;
    double aY[4];
    aY[3]=(pos.slpy[2]-neg.slpy[2])/3;
    aY[2]=(pos.slpy[1]-neg.slpy[1])/2;
    aY[1]=(pos.slpy[0]-neg.slpy[0])/1;
    aY[0]=pos.primitiveY-neg.primitiveY;
    
    double vx = m_sigmaX * m_sigmaX;
    double vy = m_sigmaY * m_sigmaY;
    
    double ccX[10];
    ccX[0]=aX[0]*aX[1]/vx;
    ccX[1]=(aX[1]*aX[1] + 2*aX[2]*aX[0])/vx;
    ccX[2]=3*(aX[0]*aX[3]+ aX[1]*aX[2])/vx;
    ccX[3]=2*(2*aX[0]*aX[4]+2*aX[1]*aX[3]+ aX[2]*aX[2])/vx;
    ccX[4]=5*(aX[1]*aX[4]+aX[2]*aX[3]+aX[0]*aX[5])/vx;
    ccX[5]=3*(aX[3]*aX[3]+2*aX[2]*aX[4]+2*aX[1]*aX[5])/vx;
    ccX[6]=7*(aX[2]*aX[5]+aX[3]*aX[4])/vx;
    ccX[7]=4*(aX[4]*aX[4]+2*aX[3]*aX[5])/vx;
    ccX[8]=9*aX[4]*aX[5]/vx;
    ccX[9]=5*aX[5]*aX[5]/vx;
    
    double ccY[6];
    ccY[0]=aY[0]*aY[1]/vy;
    ccY[1]=(aY[1]*aY[1] + 2*aY[2]*aY[0])/vy;
    ccY[2]=3*(aY[0]*aY[3]+ aY[1]*aY[2])/vy;
    ccY[3]=2*(2*aY[0]*aY[4]+2*aY[1]*aY[3]+ aY[2]*aY[2])/vy;
    ccY[4]=5*(aY[1]*aY[4]+aY[2]*aY[3]+aY[0]*aY[5])/vy;
    ccY[5]=3*aY[3]*aY[3]/vy;
    
     bool stop = false;
    int nIterate=0;
    double zV=m_zV0Vtx;
   
    while ( !stop ) {
      nIterate++;
   // eq is D+E*dz=0
    double D=ccX[0]+ccY[0];
    for(int i=1;i<10;i++){
    	D+=ccX[i]*pow(zV,i);
    }
    
    for(int i=1;i<9;i++){
    	D+=ccY[i]*pow(zV,i);
    }
   
    double E=ccY[1]+ccX[1];
    for(int i=2;i<10;i++){
    	E+=i*ccX[i]*pow(zV,i-1);
    }
    for(int i=1;i<9;i++){
    	E+=i*ccY[i]*pow(zV,i-1);
    }
	if(fabs(E) < 1.e-12) return StatusCode::FAILURE;
	double dz=-D/E;
       zV += dz;
      stop = fabs(dz) < 5 || nIterate > m_iterVtx;
    }
 // coordinates     	
	m_zV0Vtx =zV;
   	double xV = 0.5 * ( pos.x(zV) + neg.x(zV) );
    	double yV = 0.5 * ( pos.y(zV) + neg.y(zV) );
    	m_xV0Vtx=xV;
    	m_yV0VTx=yV;
// chi2	
	double Sx= aX[0];
    for(int i=1;i<6;i++){
		Sx += aX[i] * pow(zV,i);
     }

	double Sy= aY[0];
    for(int i=1;i<4;i++){
		Sy += aY[i] * pow(zV,i);
     }
 
    m_chiSq  = Sx * Sx /vx +Sy* Sy /vy ;
   return StatusCode::SUCCESS;
    
    
    }
  //-------------------------------------------------------------------------------
  StatusCode PatCalibV0::reco4MomentumOld( LocalV0Track& pos, LocalV0Track& neg ) { //unused

    double zV=m_zV0Vtx;
    double derXPos = pos.slx( zV );
    double derXNeg = neg.slx( zV );
    double derYPos = pos.sly( zV );
    double derYNeg = neg.sly( zV );

    double qNormP=sqrt(1.+derXPos*derXPos+derYPos*derYPos);
    double qNormN=sqrt(1.+derXNeg*derXNeg+derYNeg*derYNeg);
    double piPlus[4];
    double piMinus[4];

    piPlus[1]=derXPos/qNormP * pos.p;
    piPlus[2]=derYPos/qNormP * pos.p;
    piPlus[3]=1./qNormP      * pos.p;

    piMinus[1]=derXNeg/qNormN * neg.p;
    piMinus[2]=derYNeg/qNormN * neg.p;
    piMinus[3]=1./qNormN      * neg.p;

    double pplusSq=0.;
    double pminusSq=0.;

    for(int i=1; i < 4; i++){
      pplusSq  +=piPlus[i]*piPlus[i];
      pminusSq +=piMinus[i]*piMinus[i];
    }
    double Eplus=sqrt( m_Pion*m_Pion+pplusSq);
    double Eminus=sqrt( m_Pion*m_Pion+pminusSq);
    piPlus[0]  = Eplus;
    piMinus[0] = Eminus;
    for( int i=0 ; i<4 ;i++){
      m_KsMom[i]=piPlus[i]+piMinus[i];
    }
    m_invariantKsMass=m_KsMom[0]*m_KsMom[0];
    double modPsq=0;
    for(int i=1; i < 4; i++){
      modPsq +=m_KsMom[i]*m_KsMom[i];
    }
    m_invariantKsMass -= modPsq;
    if(m_invariantKsMass < 0) return StatusCode::FAILURE;
    m_invariantKsMass=sqrt(m_invariantKsMass);

    double modP=sqrt(modPsq);

    m_unitVector[0]=m_KsMom[3]/modP;
    m_unitVector[1]=m_KsMom[1]/modP;
    m_unitVector[2]=m_KsMom[2]/modP;
// transverse coordinates of impact at z==0
    m_xImpact=m_xV0Vtx-m_unitVector[1]/m_unitVector[0]*m_zV0Vtx;
    m_yImpact=m_yV0VTx-m_unitVector[2]/m_unitVector[0]*m_zV0Vtx;

    return StatusCode::SUCCESS;
  }
  
  //-------------------------------------------------------------------------------
  void PatCalibV0::reco3Momentum( LocalV0Track& pos, LocalV0Track& neg ) {

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
    m_momTrackPlus[0]  = pplusSq;
    m_momTrackMinus[0] = pminusSq;
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
    m_yImpact=m_yV0VTx-m_unitVector[2]/m_unitVector[0]*m_zV0Vtx;
    
// construct the Armanteros-Podolanski variables
//      m_momTrackMinus, m_momTrackPlus,m_V0Mom with modulus = modP
	double pTrackPlusParallel=0.;
	double pTrackMinusParallel=0.;
	double pTrackPlusTrans=0.;
	double pTrackMinusTrans=0.;
	for(int i=1;i<4;i++){
		pTrackPlusParallel += m_momTrackPlus[i]*m_V0Mom[i];
		pTrackMinusParallel+= m_momTrackMinus[i]*m_V0Mom[i];
	}
	
	pTrackPlusParallel  /=modP;
	pTrackMinusParallel /=modP;
	
    m_Armanteros=(pTrackPlusParallel-pTrackMinusParallel)/(pTrackPlusParallel+pTrackMinusParallel);
     
    pTrackPlusTrans=m_momTrackPlus[0]-pTrackPlusParallel*pTrackPlusParallel;
    pTrackPlusTrans=sqrt(pTrackPlusTrans);
    pTrackMinusTrans=m_momTrackMinus[0]-pTrackMinusParallel*pTrackMinusParallel;
    pTrackMinusTrans=sqrt(pTrackMinusTrans);
    
    m_pTrans=0.5 *(pTrackPlusTrans+pTrackMinusTrans);
//    std::cout << "reco3Momentum  : armanteros x, y ="<<m_Armanteros<<" , " <<m_pTrans<<std::endl;
    
}
  //-------------------------------------------------------------------------------
  StatusCode PatCalibV0::recoInvariantMasses () {
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
  StatusCode PatCalibV0::reco4Momentum(LocalV0Track& pos, LocalV0Track& neg ) {
  
  reco3Momentum( pos , neg ) ;
  StatusCode  sc= recoInvariantMasses (); 
  if( sc.isFailure() ) return sc;
//  if(m_invariantKsMass <0) return StatusCode::FAILURE; 
  return StatusCode::SUCCESS;
}   
  
  //-------------------------------------------------------------------------------
  void PatCalibV0::RecoPhSp
  (double cx[],double cy[],double slpx[],double slpy[],
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
  double PatCalibV0::CheckPrimaryVtx( LocalV0Track& pos, LocalV0Track& neg ) {
    double distRef = 1.e10;
    for( std::vector<double>::iterator itZ = m_zPrimVtx.begin();
         m_zPrimVtx.end() != itZ; ++itZ ) {
      double zv = (*itZ);
      double xv = 0.5 * ( pos.x( zv ) + neg.x( zv ) );
      double yv = 0.5 * ( pos.y( zv ) + neg.y( zv ) );
      double dist2x=(xv-m_xV0Vtx)/m_sigmaX;
      double dist2y=(yv-m_yV0VTx)/m_sigmaY;

      double distCurrent=dist2x*dist2x +dist2y*dist2y;

      if(distCurrent < distRef) distRef=distCurrent;
    }
    return distRef ;

  }
 //-------------------------------------------------------------------------------
double PatCalibV0::SmallestImpactParameter()   {
double distRef = 1.e10;
for( std::vector<double>::iterator itZ = m_zPrimVtx.begin();
     m_zPrimVtx.end() != itZ; ++itZ ) {
     double zPrimVtx= (*itZ);
     double xAtPrimVtx=m_xV0Vtx+m_unitVector[1]/m_unitVector[0]*(zPrimVtx-m_zV0Vtx);
     double yAtPrimVtx=m_yV0VTx+m_unitVector[2]/m_unitVector[0]*(zPrimVtx-m_zV0Vtx);
     double distCurrent=xAtPrimVtx*xAtPrimVtx + yAtPrimVtx*yAtPrimVtx;
     distCurrent=sqrt(distCurrent);
      if(distCurrent < distRef) distRef=distCurrent;
}
   return distRef ;
}
 //-------------------------------------------------------------------------------
double PatCalibV0::vtxMomAngle(LHCb::RecVertices* primaryVertices)   {
double distRef = 1.e10;
int VtxLoc=-1;
for( std::vector<double>::iterator itZ = m_zPrimVtx.begin();
     m_zPrimVtx.end() != itZ; ++itZ ) {
     double zPrimVtx= (*itZ);
     double xAtPrimVtx=m_xV0Vtx+m_unitVector[1]/m_unitVector[0]*(zPrimVtx-m_zV0Vtx);
     double yAtPrimVtx=m_yV0VTx+m_unitVector[2]/m_unitVector[0]*(zPrimVtx-m_zV0Vtx);
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
 	flightDir[2]=m_yV0VTx-yPrimVtx;
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
double PatCalibV0::NormedSmallestImpactParameter()   {
double distRef = 1.e10;
for( std::vector<double>::iterator itZ = m_zPrimVtx.begin();
     m_zPrimVtx.end() != itZ; ++itZ ) {
     double zPrimVtx= (*itZ);
     double xAtPrimVtx=m_xV0Vtx+m_unitVector[1]/m_unitVector[0]*(zPrimVtx-m_zV0Vtx);
     xAtPrimVtx/=m_sigmaX;
     double yAtPrimVtx=m_yV0VTx+m_unitVector[2]/m_unitVector[0]*(zPrimVtx-m_zV0Vtx);
     yAtPrimVtx/=m_sigmaY;
     double distCurrent=xAtPrimVtx*xAtPrimVtx + yAtPrimVtx*yAtPrimVtx;
     distCurrent=sqrt(distCurrent);
      if(distCurrent < distRef) distRef=distCurrent;
}
   return distRef ;
}
 //-------------------------------------------------------------------------------

  AIDA::IHistogram1D * m_resolution;
  AIDA::IHistogram1D * m_recoMomentum;
  AIDA::IHistogram2D * m_pQuality;
  AIDA::IHistogram1D * m_MCdz_K0;
  AIDA::IHistogram1D * m_MCdx_K0;
  AIDA::IHistogram1D * m_MCdy_K0;
  
  AIDA::IHistogram1D * m_MCdz_Lambda;
  AIDA::IHistogram1D * m_MCdx_Lambda;
  AIDA::IHistogram1D * m_MCdy_Lambda;
  
  AIDA::IHistogram1D * m_trkX_K0;
  AIDA::IHistogram1D * m_trkY_K0;
  AIDA::IHistogram1D * m_trkSlpX_K0;
  AIDA::IHistogram1D * m_trkSlpY_K0;

  AIDA::IHistogram1D * m_trkX_Lambda;
  AIDA::IHistogram1D * m_trkY_Lambda;
  AIDA::IHistogram1D * m_trkSlpX_Lambda;
  AIDA::IHistogram1D * m_trkSlpY_Lambda;

  AIDA::IHistogram1D * m_trkX_LambdaBar;
  AIDA::IHistogram1D * m_trkY_LambdaBar;
  AIDA::IHistogram1D * m_trkSlpX_LambdaBar;
  AIDA::IHistogram1D * m_trkSlpY_LambdaBar;

  AIDA::IHistogram1D * m_MCKsNumber;
  AIDA::IHistogram1D * m_MCKsNumberReco;
  AIDA::IHistogram1D * m_MCKsNumberReco2;
  AIDA::IHistogram1D * m_MCKsNumberReco3;
  AIDA::IHistogram1D * m_MCKsNumberReco4;
  AIDA::IHistogram1D * m_MCKsNumberReco5;
  AIDA::IHistogram1D * m_MCKsNumberReco6;
  AIDA::IHistogram1D * m_MCKsNumberReco7;
  
  AIDA::IHistogram1D * m_xOrg;
  AIDA::IHistogram1D * m_yOrg;
  AIDA::IHistogram1D * m_txOrg;
  AIDA::IHistogram1D * m_tyOrg;

  AIDA::IHistogram1D * m_zPrimVtxh;
  AIDA::IHistogram2D * m_xyPrimVtx;
  AIDA::IHistogram1D * m_checkPrimChi2;
  AIDA::IHistogram1D * m_checkPrimChi2Ks;
  AIDA::IHistogram1D * m_checkPrimChi2Lambda;
  AIDA::IHistogram1D * m_checkPrimChi2LambdaBar;


  AIDA::IHistogram1D * m_invMassKs;
  AIDA::IHistogram1D * m_invMassLambda;
  AIDA::IHistogram1D * m_invMassLambdaBar;
   AIDA::IHistogram1D * m_invMassBothLambda;
 
  AIDA::IHistogram1D * m_zTrue;
 		AIDA::IHistogram1D * m_invMassAllPairs;
  AIDA::IHistogram1D * m_zRec;
  AIDA::IHistogram2D * m_impactAtOrigin;
    
 AIDA::IHistogram1D * m_ptMCTrue_K0;
 AIDA::IHistogram1D * m_ptMCTrue_Lambda;
 AIDA::IHistogram1D * m_ptMCTrue_LambdaBar;
 
  AIDA::IHistogram1D * m_ptMCRec_K0;
  AIDA::IHistogram1D * m_ptMCRec_Lambda; 
  AIDA::IHistogram1D * m_ptMCRec_LambdaBar; 
 
 AIDA::IHistogram1D * m_impactParam;
 AIDA::IHistogram1D * m_impactParamV0;
 AIDA::IHistogram1D * m_impactParamNorm;
 AIDA::IHistogram1D * m_impactParamV0Norm;
 AIDA::IHistogram1D * m_nbPrimVtx;
  
  AIDA::IHistogram1D *   m_plotChi2;
  AIDA::IHistogram1D *   m_plotChi2Ks;
  AIDA::IHistogram1D *   m_plotChi2Lambda;
  AIDA::IHistogram1D *  m_vtxMomAngV0;
  AIDA::IHistogram1D *  m_vtxMomAngAll;
  
  AIDA::IHistogram1D * m_multiplicityKs;
  AIDA::IHistogram1D * m_multiplicityPair;
  AIDA::IHistogram1D * m_multiplicityKsBd;
  
  
  AIDA::IHistogram1D * m_multiplicitySelect;
  AIDA::IHistogram1D * m_multiplicityPairSelect;
  AIDA::IHistogram1D * m_V0Degeneracy  ;
  
//
  AIDA::IHistogram1D * m_invMassReco;
  AIDA::IHistogram2D * m_impactAtOriginReco;
  AIDA::IHistogram2D * m_multVsnVTX;
  
  AIDA::IHistogram1D * m_KsMassSelect;
  AIDA::IHistogram1D * m_LambdaMassSelect;
  AIDA::IHistogram1D * m_antiLambdaMassSelect;
  
  AIDA::IHistogram1D * m_bestchi2Select;
  AIDA::IHistogram1D * m_bestPrimOKSelect;
  AIDA::IHistogram1D *   m_zRecSelect;
  
  AIDA::IHistogram2D * m_armanteros_clear;
  AIDA::IHistogram2D * m_armanteros_ambigu;
AIDA::IHistogram2D * m_armanteros_K0;
AIDA::IHistogram2D * m_armanteros_Lambda;
AIDA::IHistogram2D * m_armanteros_antiLambda;
  
  
// compteurs
int m_nCounters_K0[10];
int m_nCounters_Lambda[10];
int m_nCounters_LambdaBar[10];

int m_nCounters_NoVelo_K0[10];
int m_nCounters_NoVelo_Lambda[10];
int m_nCounters_NoVelo_LambdaBar[10];

int m_nCounters[10];
int m_nCountersNoVelo[10];
int m_nLambda;
int m_nLambdaBar;
int m_nK0courts;
int m_nLambda_b;
int m_nLambda_b_Bar;
double m_BdVtx[3]; // for checks (to be removed)
int m_acc_Ks;
int m_acc_Lambda;
int m_acc_LambdaBar;
};
#endif // PATCALIBKSHORT_H
