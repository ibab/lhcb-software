// $Header: 

/// Include files
/// Gaudi interfaces
#include "GaudiKernel/IDataProviderSvc.h"

/// Gaudi factories for algorithms 
#include "GaudiKernel/AlgFactory.h"

/// Gaudi base classes
#include "GaudiKernel/SmartDataPtr.h"

/// Utility classes
#include "GaudiKernel/MsgStream.h"

#include "CLHEP/Geometry/Vector3D.h"

/// Private classes to the example
#include "L0Muon/L0MuonCandidate.h"
#include "L0mTriggerProc.h"
#include "L0mTower.h"

//------------------------------------------------------------------------------
//
/// Implementation of class :  L0mTriggerProc
//
/// Author: A.Tsaregorodtsev
//
//------------------------------------------------------------------------------


static const AlgFactory<L0mTriggerProc>  Factory;
const IAlgFactory& L0mTriggerProcFactory = Factory;

/// Algorithm parameters which can be set at run time must be declared.
/// This should be done in the constructor.
L0mTriggerProc::L0mTriggerProc(const std::string& name, ISvcLocator* pSvcLocator)
: Algorithm(name, pSvcLocator)
{
   declareProperty("FoiXSize", m_foiXSize);
   declareProperty("FoiYSize", m_foiYSize);
   m_d1 = 545. ;
   m_d2 = 665. ;
   m_d3 = 309.5 ;
   m_alpha = 1.21 ;
   declareProperty("D1", m_d1);
   declareProperty("D2", m_d2);
   declareProperty("D3", m_d3);
   declareProperty("Alpha", m_alpha);
   m_outputCandidates = "/Event/MC/L0MuonCandidates";
   declareProperty("outputCandidates", m_outputCandidates);
}


StatusCode L0mTriggerProc::initialize()   { 

    MsgStream log(msgSvc(), name());

    log << MSG::INFO << "Initialization starting..." << endreq;
    
    m_foi = new L0mFoI(m_foiXSize,m_foiYSize);
    
    // cout << "Dumping FoI's: " << endl;
    // m_foi->dump();
    return StatusCode::SUCCESS;
}

StatusCode L0mTriggerProc::execute() {

  MsgStream log(msgSvc(), name());
  
  log << MSG::DEBUG << "execute" << endreq;
  
// create the collection of L0mTowers
  
  SmartDataPtr<L0mTowers>  towers(eventSvc(),"/Event/MC/L0mTowers");
  L0mTowers::iterator it;
  
// register the trigger candidates  

  StatusCode sc;

  ObjectVector<L0MuonCandidate>* cand = new ObjectVector<L0MuonCandidate>;
  log << MSG::DEBUG << "Registering candidates ...  "  ;  
  sc = eventSvc()->registerObject(m_outputCandidates,cand);
  log << MSG::DEBUG << "done, status " << sc << endreq;  
  L0MuonCandidate* lcd;
 
  
  int i=1;
  for ( it=towers->begin(); it != towers->end(); it++,i++ ) {
      if ((*it)->isFull()) {
          L0mPad* lpd1 = (*it)->findTrack(m_foi);
	  if(lpd1) {
	      
	      //  Track found ! 
	        
	      log << MSG::DEBUG << "Track found; Tower # " << i << endreq;
	      (*it)->draw(log);
	      (*it)->padM3()->print(log );
	      (*it)->padM2()->print(log );
	      (*it)->padM1()->print(log );
	      double ptm = pt((*it)->padM1(), (*it)->padM2());
	      double thetam = theta((*it)->padM1(), (*it)->padM2());
	      double phim = phi((*it)->padM1(), (*it)->padM2());
	      log << MSG::DEBUG << " Pt= " << ptm 
	                        << " Theta= " << thetam 
				<< " Phi= " << phim << endreq; 
	      lcd = new L0MuonCandidate(ptm,lpd1->x(),lpd1->y(),
	                                thetam,phim,
					(*it)->padM1(),
					(*it)->padM2(),
					(*it)->padM3()); 
	      cand->push_back(lcd); 
          }
      }	  
  }
  
  return StatusCode::SUCCESS;
}

StatusCode L0mTriggerProc::finalize()  {
  return StatusCode::SUCCESS;
}

double L0mTriggerProc::pt( L0mPad* pm1, L0mPad* pm2 ) {

    double d1 = m_d1 ;
    double d2 = m_d2 ;
    double d3 = m_d3 ;
    double alpha = m_alpha ;
    
    double x1 = pm1->x();
    double y1 = pm1->y();
    double x2 = pm2->x();
    double x0 = x1 - d2*(x2-x1)/d3;
    double y0 = y1*d1/(d1+d2);
        
    double sq = (d1+d2)*x2-(d1+d2+d3)*x1;
    if(sq == 0.) {
        sq = 0.0001;
    } else {
        sq = 1./((d1+d2)*x2-(d1+d2+d3)*x1);
    }
    
    double sr = sqrt(((d2+d3)*x1 - d2*x2)*((d2+d3)*x1 - d2*x2) +
                     (d1*d3*y1/(d1+d2))*(d1*d3*y1/(d1+d2))   );
    double sc = sqrt((d1+d2)*(d1+d2)+y1*y1)/(d1+d2);
    double st = (d1*d3*d3+2.*(x1*(d3+d2)-x2*d2)*(x2-x1))/(d1*d3*d3);
    double pt = fabs(alpha*sq*sr*sc*st);		     
    
    return pt; 
}

double L0mTriggerProc::theta( L0mPad* pm1, L0mPad* pm2 ) {
    
    double x1 = pm1->x();
    double y1 = pm1->y();
    double x2 = pm2->x();
    double x0 = x1 - m_d2*(x2-x1)/m_d3;
    double y0 = y1*m_d1/(m_d1+m_d2);
        
    HepVector3D v(x0,y0,m_d1);	
	
    return v.theta(); 
}

double L0mTriggerProc::phi( L0mPad* pm1, L0mPad* pm2 ) {
    
    double x1 = pm1->x();
    double y1 = pm1->y();
    double x2 = pm2->x();
    double x0 = x1 - m_d2*(x2-x1)/m_d3;
    double y0 = y1*m_d1/(m_d1+m_d2);
    
    HepVector3D v(x0,y0,m_d1);
    return v.phi(); 
    
}
