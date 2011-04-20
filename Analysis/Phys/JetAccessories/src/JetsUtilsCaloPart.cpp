// $Id: JetsUtilsCaloPart.cpp,v 1.3 2007/06/18 11:27:44 jpalac Exp $
// JetsUtilsCaloPart created 25.02.06
// A file to select events with b-jets comming from heavy mother
//
// ============================================================================
// Include files
// ============================================================================
// ============================================================================
// from Gaudi
// ============================================================================
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/IAlgTool.h"
#include "GaudiKernel/AlgFactory.h"
// ============================================================================
// DaVinci Kernel 
// ============================================================================
#include "Kernel/DVAlgorithm.h"
// ============================================================================
// Event 
// ============================================================================
#include "Event/Particle.h"
// ============================================================================
// Event 
// ============================================================================
#include "Event/RecHeader.h"
//Calo includegg
#include "CaloDet/DeCalorimeter.h"
#include "Event/CaloDigit.h"




using namespace Gaudi::Units ;
using namespace LHCb ;
using namespace std ;



// ============================================================================
// Main Algorithm
// ============================================================================
class JetsUtilsCaloPart :  public DVAlgorithm
{
  friend class AlgFactory<JetsUtilsCaloPart> ;
public:
  /** Standard constructor
   *  @param name instance name
   *  @param pSvc pointer to Service Locator
   */
  JetsUtilsCaloPart(const std::string& name,
		    ISvcLocator* pSvcLocator)
    : DVAlgorithm(name,pSvcLocator) 
		   //   , m_bestPV(false)  
  {
    declareProperty ( "requestedCalo"        ,  m_Calo_in    ) ;
    declareProperty ( "bestPV"        ,  m_bestPV = false, "chose the best pv (in fct of PT) instead of (0,0,) as the origine of the particle"    ) ;

  } ;





  /// destructor
  virtual ~JetsUtilsCaloPart( ){};
public:
  /** standard initialization of the algorithm
   *  @see LoKi::Algo
   *  @return status code
   */

  virtual StatusCode finalize();
  virtual StatusCode initialize ()
  {
    StatusCode sc = DVAlgorithm::initialize(); 
    if ( sc.isFailure() ) return sc;
    //check input
    for ( std::vector<std::string>::iterator iloc = m_Calo_in.begin();
	  iloc != m_Calo_in.end(); ++iloc ){  
      
      std::string  m_CaloA = *iloc;     
      
      if(m_CaloA != "Ecal" && m_CaloA != "Hcal"   && m_CaloA != "Prs"   && m_CaloA != "Spd"  ) {
	warning()<<"the CALO "<<m_CaloA<<" does not exsit"<<endmsg;
	continue;
      }
      m_Calo.push_back(m_CaloA);
    }
    return StatusCode::SUCCESS ;
  } ;

  /** standard execution of the algorithm
   *  @return status code
   */
  virtual StatusCode execute   () ;
private:
  // the default constructor is disabled
  JetsUtilsCaloPart () ;
  // the copy constructor is disabled
  JetsUtilsCaloPart ( const  JetsUtilsCaloPart& )  ;
  // number of particle that have to verify the cut
  JetsUtilsCaloPart operator=( const  JetsUtilsCaloPart& )  ;

  std::vector<std::string>  m_Calo_in ;
  std::vector<std::string>  m_Calo ;

  bool  m_bestPV ;


};



// ============================================================================
/** @file
 *  Implementation file for class  JetsUtilsCaloPart
 *  @date  2011-02-10
 *  @author Cédric Potterat  cedric.potterat@cern.ch
 */
// ============================================================================
DECLARE_ALGORITHM_FACTORY( JetsUtilsCaloPart );
// ============================================================================

// ===========================================================================
/** standard execution of the algorithm
 *  @return status code
 */
// ===========================================================================
StatusCode JetsUtilsCaloPart::execute()
{ 
  
  Gaudi::XYZPoint v1 = Gaudi::XYZPoint(0,0,0);
  LHCb::RecVertex V0 = Gaudi::XYZPoint(0,0,0);
  
  
  if(m_bestPV){
    const LHCb::RecVertex::Range PVs =  this->primaryVertices();
    LHCb::RecVertex::Range::const_iterator ipv = PVs.begin();  
    if(PVs.size() == 1){
      V0 = *(*ipv);      
    }else if (PVs.size()>1) {
      debug() << "multiple Primary Vertex found" << endmsg;    
      debug() << "N Primary Vertices " << PVs.size() << endmsg;
      double maxPT = 0.;
      for(LHCb::RecVertex::Range::const_iterator  ipv2= PVs.begin();ipv2!=PVs.end();++ipv2){
	
	LHCb::RecVertex myV0 = *(*ipv2);
	debug() << "PV  position : "<<myV0.position()<<endmsg;
	debug() << "PV  nb   trk : "<<myV0.tracks().size()<<endmsg;
	const SmartRefVector<LHCb::Track> mySmartVT =  myV0.tracks();
	double sumPT = 0.;
	
	for(int j = 0; j<(int) myV0.tracks().size();j++)
	  sumPT += myV0.tracks().at(j).target()->pt();
	
	debug() << "PV sum pt trk : "<<sumPT<<endmsg;
	
	if(sumPT  > maxPT ){
	  maxPT = sumPT;
	  V0 = *(*ipv2);  
	}
      }
      
      debug() << "try with the PV with the largest sum of trk pt" << endmsg;
      
    }
    else if (PVs.size()<1) {
      debug() << "no Primary Vertex found" << endmsg;
    }  
    
    v1 = V0.position();
  }
  
  for ( std::vector<std::string>::iterator iloc = m_Calo.begin();
	iloc != m_Calo.end(); ++iloc ){
    
    std::string  m_CaloA = *iloc;
    Particles* myCaloPart = new Particles();
    
    std::string location;
    location = "/Event/Phys/PartHit_"+m_CaloA+"/Particles";
    put(myCaloPart , location );
    if (msgLevel( MSG::DEBUG )) {
      debug() << "writeOnTES -------------------------"<< endreq;
      debug() << "writeOnTES at "<< location << endreq;
    }

    const   LHCb::CaloDigits*   digitsCalo = get<LHCb::CaloDigits>( "Raw/"+m_CaloA+"/Digits" ) ;
    const   DeCalorimeter*  Dcalo = getDet<DeCalorimeter>( "/dd/Structure/LHCb/DownstreamRegion/"+m_CaloA );

    for (  LHCb::CaloDigits::const_iterator idigit=digitsCalo->begin(); digitsCalo->end()!=idigit ; ++idigit ){  
      const  LHCb::CaloDigit* digit = *idigit ;

      if ( 0 == digit ) { continue ; }
      // get unique calorimeter cell identifier
      const   LHCb::CaloCellID& cellID = digit->cellID() ;
      // get the energy of the digit
      const double en = digit->e();
    
      // get the position of the cell (center)
      const Gaudi::XYZPoint xcenter = Dcalo->cellCenter( cellID ) ;
      const Gaudi::XYZPoint xcenter2 = Gaudi::XYZPoint(xcenter.x() - v1.x(),
						       xcenter.y() - v1.y(),
						       xcenter.z() - v1.z()) ;
    

      double x =  xcenter2.x();
      double y =  xcenter2.y();
      double z =  xcenter2.z();
      //   double eT =  en*sqrt((x*x+y*y)/(x*x + y*y + z*z));
        
      double px =  en*x/sqrt(x*x + y*y + z*z);
      double py =  en*y/sqrt(x*x + y*y + z*z);
      double pz =  en*z/sqrt(x*x + y*y + z*z);
    
      Gaudi::LorentzVector p1 = Gaudi::LorentzVector(px,py,pz,en);
      LHCb::Particle   pJet ;
        
      int myID=0;
      if(m_CaloA == "Ecal") myID = 30001;
      if(m_CaloA == "Hcal") myID = 30002;
      if(m_CaloA == "Spd")  myID = 30003;
      if(m_CaloA == "Prs")  myID = 30004;
        
      pJet.setParticleID     ( LHCb::ParticleID( myID ) );
      pJet.setReferencePoint ( xcenter ) ;
      pJet.setMomentum       ( Gaudi::LorentzVector ( p1 )  ) ;     
    
      myCaloPart->insert(pJet.clone());  
    }    
  }
  setFilterPassed(true);
  return StatusCode::SUCCESS ;
}


//=============================================================================
// Finalization
//=============================================================================
StatusCode JetsUtilsCaloPart::finalize() {

  if (msgLevel(MSG::DEBUG)) debug() << "==> Finalize" << endmsg;
  return StatusCode::SUCCESS;

}

