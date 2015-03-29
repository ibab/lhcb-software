#ifndef CALOCORRECTIONBASE_H 
#define CALOCORRECTIONBASE_H 1

// Include files
// from Gaudi
#include "GaudiAlg/GaudiTool.h"
#include "DetDesc/Condition.h"
#include "CaloUtils/CaloAlgUtils.h"
#include "Kernel/CaloCellID.h"
#include "CaloUtils/ClusterFunctors.h"
#include "CaloDet/DeCalorimeter.h"
#include "CaloDAQ/ICaloDigitFilterTool.h"
#include "Event/CaloHypo.h"
#include "Event/CaloDigit.h"
#include "GaudiKernel/Point3DTypes.h"
#include "CaloUtils/CaloAlgUtils.h"
#include "CaloUtils/CaloMomentum.h"
#include "CaloUtils/ICaloElectron.h"
#include "CaloUtils/Calo2Track.h"
#include "Relations/IRelationWeighted2D.h"
#include "Event/Track.h"
#include "CaloCorrectionBase.h"
#include "CaloInterfaces/ICaloRelationsGetter.h"
#include "GaudiKernel/IIncidentListener.h"
#include "GaudiKernel/IIncidentSvc.h" 


static const InterfaceID IID_CaloCorrectionBase ( "CaloCorrectionBase", 1, 0 );

/** @class CaloCorrectionBase CaloCorrectionBase.h
 *  
 *
 *  @author Olivier Deschamps
 *  @date   2010-05-07
 */


namespace CaloCorrectionUtils{
  class DigitFromCalo : public std::unary_function<const LHCb::CaloDigit*,bool>{ 
  public:
    explicit DigitFromCalo( const std::string& calo ): m_calo( CaloCellCode::CaloNumFromName( calo ) ) {} ;
    explicit DigitFromCalo( const int  calo ): m_calo(                                calo   ) {} ;
    inline bool operator() ( const LHCb::CaloDigit* digit ) const{
      if( 0 == digit ) { return false ; }
      return ( ( (int) digit->cellID().calo() ) == m_calo ) ;
    };
  private:
    DigitFromCalo();
  private:
    int m_calo ;
  };
}
// DO NOT CHANGE THE FUNCTION ORDER FOR BACKWARD COMPATIBILITY WITH EXISTING CONDITIONS DB
namespace CaloCorrection {
  enum  Function{
    InversPolynomial    =0,
    Polynomial          =1,
    ExpPolynomial       =2,  
    ReciprocalPolynomial=3,
    Sigmoid             =4,
    Sshape              =5,
    ShowerProfile       =6,
    SshapeMod           =7,
    Sinusoidal          =8,
    ParamList           =9, // simple parameter access (by area)
    GlobalParamList     =10, // simple parameter access (ind. of area)
    Unknown // MUST be the last item  
  };
  typedef std::pair<CaloCorrection::Function , std::vector<double> > Parameters;
  enum Type{
    // E-Correction parameters
    alphaG, // global alpha factor
    alphaE, // alpha(E)
    alphaB, // alpha(Bary)
    alphaX, // alpha(Dx)
    alphaY, // alpha(Dy) 
    alphaP, // alpha(ePrs)
    beta,   // Prs correction (with possible eEcal dependency)
    betaP,  // Prs correction  (with possible ePrs dependency)
    betaPR, //  Prs correction (with possible ePrs/eEcal dependency)
    betaC,   // Prs correction for converted photons (use beta if not defined)
    betaCP,  // ""
    betaCPR, // ""
    globalC,   // global factor for converted photons
    globalT,   // global(DeltaTheta) function of incidence angle
    offsetT,   // offset(DeltaTheta) function of incidence angle
    offset,   // offset( sinTheta ) energy (or ET ) offset
    offsetC,   // offset( sinTheta ) energy (or ET ) offset for converted photons
    //
    ClusterCovariance, // parameters for cluster covariance estimation
    // L-Correction parameters
    gamma0,
    delta0,
    gammaP,
    deltaP,
    // S-correction parameters
    shapeX,
    shapeY,
    residual,
    residualX,
    residualY,
    asymP,
    asymM,
    angularX,
    angularY,
    // ShowerShape profile
    profile,
    profileC, // for converted photons
    // Cluster masking
    EnergyMask,
    PositionMask,
    lastType  // MUST BE THE LAST LINE 
  };
  static const int nT = lastType+1;
  static const int nF = Unknown+1;
  static const std::string typeName[nT] = { "alphaG", "alphaE","alphaB","alphaX","alphaY","alphaP","beta","betaP","betaPR","betaC","betaCP","betaCPR"          // E-corrections
                                            ,"globalC","globalT" ,"offsetT","offset","offsetC","ClusterCovariance"  
                                            ,"gamma0","delta0","gammaP","deltaP"                                                                      // L-Corrections
                                            ,"shapeX","shapeY","residual","residualX","residualY","asymP","asymM","angularX","angularY"               // S-Corrections
                                            ,"profile","profileC"                                                                                     // Profile shape
                                            ,"EnergyMask","PositionMask"
                                            ,"Unknown"};    

  static const std::string funcName[nF] = { "InversPolynomial", "Polynomial","ExpPolynomial","ReciprocalPolynomial","Sigmoid"
                                            ,"Sshape","ShowerProfile","SshapeMod","Sinusoidal","ParamList","GlobalParamList","Unknown"};  
}

class CaloCorrectionBase : public GaudiTool, virtual public IIncidentListener {
public: 

  // Return the interface ID
  static const InterfaceID& interfaceID() { return IID_CaloCorrectionBase; }

  /// Standard constructor
  CaloCorrectionBase( const std::string& type, 
        const std::string& name,
        const IInterface* parent);

  virtual ~CaloCorrectionBase( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode finalize  ();    ///< Algorithm finalization

  void setOrigin(Gaudi::XYZPoint origin){m_origin = origin;}
  StatusCode updParams();
  StatusCode setConditionParams(std::string cond,bool force=false){ // force = true : forcing access via condDB only
    if(cond != m_conditionName)m_conditionName = cond;

    // get parameters from options  :
    if( !m_useCondDB && !force )return setOptParams();

    // get from DB if exists :
    if( !existDet<DataObject>( m_conditionName)  ){
      if(force){
        if( m_conditionName != "none" )info() << "Condition '"<< m_conditionName << "' has not been found " << endmsg; 
        return StatusCode::SUCCESS;
      }else{
        if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) debug() << " Condition '" << m_conditionName << "' has not found -- try options parameters !" << endmsg;
        return setOptParams();
      }
    }
    return setDBParams();
  };
  

  // accessors
  CaloCorrection::Parameters getParams(CaloCorrection::Type type, const LHCb::CaloCellID id=LHCb::CaloCellID()) const;
  std::vector<double> getParamVector(CaloCorrection::Type type, 
                                     const LHCb::CaloCellID id=LHCb::CaloCellID()) const{return getParams(type,id).second;}
  double getParameter(CaloCorrection::Type type, unsigned int i, 
                      const LHCb::CaloCellID id=LHCb::CaloCellID(),double def=0.) const{
 		const std::vector<double> params =    getParams(type,id).second;
    return (i < params.size()) ? params[i] : def;
  }
  /// return value of analytic derivative for a given function type in cell id at a given point var with default value def
  double getCorrectionDerivative(CaloCorrection::Type type, const LHCb::CaloCellID id , double var = 0.,double def = 0.) const;
  //// propagate cov.m. cov0 according to Jacobian jac: cov1 = (jac * cov * jac^T), see comments in CaloECorrection.cpp and CaloSCorrection.cpp
  // void recalculate_cov(const TMatrixD &jac, const TMatrixDSym &cov0, TMatrixDSym &cov1) const;


  double getCorrection(CaloCorrection::Type type, const LHCb::CaloCellID id , double var = 0.,double def = 1.) const;
  double incidence(const LHCb::CaloHypo* hypo,bool straight=false) const;
  void getPrsSpd(const LHCb::CaloHypo* hypo,double& ePrs,double& eSpd) const{
    typedef const LHCb::CaloHypo::Digits   Digits   ;
    const Digits& digits = hypo->digits();
    for( Digits::const_iterator d = digits.begin() ; digits.end() != d ; ++d ){ 
      if     ( *d == 0     ) { continue           ; }
      else if( m_prs( *d ) ) { ePrs  += (*d)->e() ; } 
      else if( m_spd( *d ) ) { eSpd  += (*d)->e() ; }
    }
  }

  ICaloDigitFilterTool* pileup(){ return m_pileup;}

  bool hasConditionChanged(){return m_update;}
  virtual void handle(const Incident&  ) { m_update = false; }// reset update flag 
  
    
  
  
protected:
  std::string m_conditionName;
  std::vector<std::string> m_corrections;
  //
  typedef std::vector<LHCb::CaloHypo::Hypothesis>   Hypotheses  ;
  typedef std::vector<int>                          Hypotheses_ ;
  Hypotheses  m_hypos     ;
  Hypotheses_ m_hypos_    ;
  LHCb::ClusterFunctors::ClusterArea      m_area;
  LHCb::ClusterFunctors::ClusterFromCalo  m_calo;
  CaloCorrectionUtils::DigitFromCalo    m_spd;
  CaloCorrectionUtils::DigitFromCalo    m_prs;
  std::string m_detData;
  const DeCalorimeter* m_det;
  Gaudi::XYZPoint  m_origin;
  ICaloDigitFilterTool* m_pileup;
  bool m_correctCovariance;

private:

  ICaloElectron* m_caloElectron;
  bool accept(std::string name){
    for( std::vector<std::string>::iterator it = m_corrections.begin() ; m_corrections.end() != it ; ++it){
      if( name == *it || *it == "All")return true;
    }    
    return false;
  }

  
  StatusCode setOptParams();
  StatusCode setDBParams();
  void checkParams();

  std::map<std::string, std::vector<double> > m_params;
  std::map<std::string, std::vector<double> > m_optParams;
  mutable std::array< GaudiUtils::VectorMap<std::string, StatEntity* >, CaloCorrection::lastType+1 > m_counters;
  Condition* m_cond;
  std::string m_cmLoc;
  ICaloRelationsGetter*    m_tables;
  bool m_useCondDB;
  bool m_update;

  // Cache counters, as looking them up as 
  //    counter( CaloCorrection::typeName[ type ] + " correction processing (" + areaName + ")" )
  // requires the creation of a temporary string every time, which in turn
  // involves (eventually) a call to 'new' and 'delete'  -- and the above was the source of 10% (!) 
  // of the # of calls to 'new' and 'delete' in the HLT!!!! (FYI: there are, on average, 1K calls 
  // per event to this method!!!)
  //
  // On top of that, this also speeds up the actual search for the correct counter, 
  // by making it a two-step process, and the first step is a direct lookup
  // instead of a 'find'.
  //
  // in the end, this change alone speeds up the total HLT by about 1%...
  StatEntity& kounter(CaloCorrection::Type type, const std::string& areaName) const {
      assert( type < CaloCorrection::lastType+1 );
      auto a = m_counters[type].find(areaName);
      if ( UNLIKELY( a == std::end(m_counters[type]) ) ) {
          std::string name = CaloCorrection::typeName[ type ] + " correction processing (" + areaName + ")";
          auto r = m_counters[type].insert( areaName, &counter(name) );
          assert(r.second);
          a = r.first;
      }
      assert(a->second);
      return *(a->second);
  }

};
#endif // CALOCORRECTIONBASE_H
