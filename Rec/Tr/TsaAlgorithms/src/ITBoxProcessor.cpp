// $Id: ITBoxProcessor.cpp,v 1.3 2006-09-05 15:48:51 mneedham Exp $ 
// GaudiKernel
#include "GaudiKernel/ToolFactory.h"

// Tsa
#include "ITBoxProcessor.h"
#include "TsaKernel/IITDataSvc.h"
#include "ITDataCache.h"
#include "TsaKernel/TypeMap.h"

// CLHEP
#include "GaudiKernel/SystemOfUnits.h"
#include "Kernel/Plane3DTypes.h"
#include "Kernel/STChannelID.h"

// Geometry
#include "STDet/DeSTDetector.h"
#include "STDet/DeSTLayer.h"

#include <algorithm>

#include <boost/assign/std/vector.hpp>
using namespace boost::assign;
using namespace boost;

DECLARE_TOOL_FACTORY( ITBoxProcessor );


ITBoxProcessor::ITBoxProcessor(const std::string& type,
                     const std::string& name,
                     const IInterface* parent):
  GaudiTool(type, name, parent){
  // constructer
  declareProperty("itDataSvc", m_dataSvcName = "ITDataSvc");
  declareProperty("station", m_station);
  declareProperty("type", m_sType);
  declareProperty("cutSlopeX1", m_cutSlopeX1 = 400.0*Gaudi::Units::mrad);
  declareProperty("cutSlopeX2", m_cutSlopeX2 = 300.0*Gaudi::Units::mrad);
  declareProperty("vTol", m_vTol = 3.5*Gaudi::Units::mm);
  declareProperty("cutDeltaY", m_cutDeltaY = 4.5*Gaudi::Units::mm);
  declareProperty("cloneSearch", m_cloneSearch = true);
  declareProperty("uClone", m_uClone = 0.15*Gaudi::Units::mm);
  m_data.reserve(4);

  declareInterface<IITBoxProcessor>(this);
}

ITBoxProcessor::~ITBoxProcessor(){
  // destructer
  if (m_data.size() != 0) {
    dataCont::iterator iterC = m_data.begin();
    while (iterC != m_data.end()) {
      ITDataCache* aCache = *iterC;
      delete aCache;
      m_data.erase(iterC);
    } // while
 } // if

}

StatusCode ITBoxProcessor::initialize(){

  StatusCode sc = GaudiTool::initialize();
  if (sc.isFailure()){
     return Error("Failed to initialize",sc);
  }
  
  // convert type string to enum
  m_eType = Tsa::theMap().toType(m_sType);

  // data service tool !
  m_dataSvc = tool<IITDataSvc>(m_dataSvcName);  

  initCache();

  debug() << " Box "  << name() << " station " 
          << m_station  << " Type " << m_eType << endreq;

  return StatusCode::SUCCESS;
}


std::string ITBoxProcessor::boxType() const{
  return m_sType;
}

StatusCode ITBoxProcessor::execute(Tsa::SpacePointVector* pCont){

  // update data cache...
  
  initializeEvent();
  m_pass = 1;

  if  (m_nEmpty >1){
    debug() << "empty box" << endreq;
  }
  else if (m_nEmpty == 1){
    // can only build three hit spacepoint
    if (m_missingX == true){
      xuv(pCont);
    } 
    else {
      xux(pCont);
    }
  }
  else {
    // build four hit spacepoint
    xuvx(pCont);   
    ++m_pass;
    xux(pCont);
    ++m_pass;
    xuv(pCont);
  }

   
  // clone
  if (m_cloneSearch == true) cloneFind(pCont);

  return StatusCode::SUCCESS;
}

void ITBoxProcessor::initCache(){

  // get geometry info and use it to make data cache objects 
  DeSTDetector* tracker = getDet<DeSTDetector>(DeSTDetLocation::IT);
  const DeSTDetector::Layers& tLayers = tracker->layers();
  for ( DeSTDetector::Layers::const_iterator iterLayer = tLayers.begin(); 
        iterLayer != tLayers.end() ; ++iterLayer){  
    LHCb::STChannelID elemID = (*iterLayer)->elementID();
    if ((elemID.station() == (unsigned int)m_station)&&(elemID.detRegion() == (unsigned int)m_eType)){
      ITDataCache* aCache = new ITDataCache((*iterLayer)->id());
      m_data.push_back(aCache);
    } 
  }
}

void ITBoxProcessor::initializeEvent(){
 
  // get the required data
  m_nEmpty = 0; 
  m_missingX = false;

  for (unsigned int iL = 0; iL < m_data.size(); ++iL){
    Tsa::STRange range = m_dataSvc->partition(m_station,m_data[iL]->layerID(),m_eType); 
    m_data[iL]->configure(range);  
    if (range.empty() == true) {
      if ((iL == 0) || (iL == 3)) m_missingX = true;
      ++m_nEmpty;
    }
  } // iL
}

void ITBoxProcessor::xuvx(Tsa::SpacePointVector* pCont){

  // loop x
  Gaudi::Plane3D aPlane;
  Gaudi::XYZPoint aPoint1;
  Gaudi::XYZPoint aPoint2;

  for (iter_type iterX1 = m_data[0]->begin(); iterX1 != m_data[0]->end(); ++iterX1){
    for (iter_type iterX2 = m_data[3]->begin(); iterX2 != m_data[3]->end(); ++iterX2){
       // cut on slope between x1,x2
       double tX = Tsa::ClusFun::dxdz(*iterX1,*iterX2);
       if (fabs(tX) < m_cutSlopeX1){

	 // make a plane
	 aPlane = Tsa::ClusFun::plane(*iterX1,*iterX2);

         for (iter_type iterU = m_data[1]->begin() ; iterU != m_data[1]->end(); ++iterU){
           if (!Tsa::ClusFun::intersection(*iterU,aPlane,aPoint1)) continue;

           if ((*iterX1)->isInsideY(aPoint1.y(),m_vTol) == true){	    
	     for (iter_type iterV = m_data[2]->begin(); iterV != m_data[2]->end(); ++iterV){ 
               if (!Tsa::ClusFun::intersection(*iterV,aPlane,aPoint2)) continue;

               if ((fabs(aPoint2.y() - aPoint1.y()) <m_cutDeltaY)
                &&((*iterX1)->isInsideY(aPoint2.y(),m_vTol)) == true){
                 const double tY = (aPoint1.y() - aPoint2.y())/(aPoint1.z() - aPoint2.z());
		 Gaudi::XYZPoint thePoint = aPoint1 + 0.5*(aPoint2 - aPoint1); 
		 Tsa::SpacePoint* sPoint = create4point(*iterX1,*iterX2,*iterU,*iterV,tX,tY,
                                                      thePoint);
                 pCont->push_back(sPoint);
	       }  
	     }	// iterV			  
	   } // if			
	 } // iterU
       }  // if
    }  // iterX2
  } // iterX1 
}


void ITBoxProcessor::xuv(Tsa::SpacePointVector* pCont){

  // search fir xuv combination 

 // init
 iter_type firstX = m_data[0]->begin();
 Gaudi::XYZPoint aPoint1; 
 static int index[2] = {0,3};

  // loop u
 for (iter_type iterU = m_data[1]->begin(); iterU != m_data[1]->end(); ++iterU){
    if (used(*iterU) == true) continue;
    // loop V
    for (iter_type iterV = m_data[2]->begin(); iterV != m_data[2]->end(); ++iterV){
      if ((used(*iterV) == true) || !(Tsa::ClusFun::poca(*iterU, *iterV, aPoint1))) continue;       
       
       // validity check.....
       if ((*firstX)->isInsideY(aPoint1.y(),m_vTol)){
         for (int ix = 0; ix < 2; ++ix ){
           for (iter_type iterX = m_data[index[ix]]->begin(); 
                iterX != m_data[index[ix]]->end(); ++iterX){
             if (used(*iterX) == false){

               Gaudi::XYZPoint aPoint2 = Tsa::ClusFun::poca(*iterU, aPoint1);     
               double tX = (aPoint2.x() - aPoint1.x())/(aPoint2.z() - aPoint1.z()); 

      	       if (fabs(tX)< m_cutSlopeX2){
		 // make a three point
                 Tsa::SpacePoint* aPoint = create3point(*iterU,*iterV,*iterX,tX,aPoint1);
                 pCont->push_back(aPoint);
	       }
	     }  // not used
	   } // iterX  
	 } // ix
       } 

    } // loop V
 }  // loop U

}

void ITBoxProcessor::xux(Tsa::SpacePointVector* pCont){

 //search for xux combination
 // loop x

 Gaudi::Plane3D aPlane;
 Gaudi::XYZPoint aPoint;

 for (iter_type iterX1 = m_data[0]->begin(); iterX1 != m_data[0]->end(); ++iterX1){
   if (used(*iterX1) == true) continue;
   for (iter_type iterX2 = m_data[3]->begin(); iterX2 != m_data[3]->end(); ++iterX2){

      // cut on slope between x1,x2
      if (used(*iterX2) == true) continue;
      double tX = Tsa::ClusFun::dxdz(*iterX1,*iterX2);
      if (fabs(tX) < m_cutSlopeX2){

	// work out the equation of the plane containing the 2 clusters
	aPlane = Tsa::ClusFun::plane(*iterX1,*iterX2);

        for (unsigned int iL = 1; iL <3; ++iL){    
           for (iter_type iterU = m_data[iL]->begin() ; iterU != m_data[iL]->end(); ++iterU){

             if ((used(*iterU) == true) || (!Tsa::ClusFun::intersection(*iterU,aPlane,aPoint))) continue;

             if ((*iterX1)->isInsideY(aPoint.y(),m_vTol) == true){
                Tsa::SpacePoint* sPoint = create3point(*iterX1,*iterX2,*iterU,tX,aPoint);
                pCont->push_back(sPoint);
  	     }
	   } // iterU
        } // iL
      } 
   } // iterX2
 } // iterX1

}

void ITBoxProcessor::cloneFind(Tsa::SpacePointVector* pCont) const{

  // look for overlap clones
  Tsa::SpacePointVector tmpCont;
  for (Tsa::SpacePointVector::iterator iter1 = pCont->begin();
       iter1 !=pCont->end(); ++iter1){
    for (Tsa::SpacePointVector::iterator iter2 = pCont->begin();
       iter2 !=pCont->end(); ++iter2){
       clonePair info = isClone(*iter1,*iter2);
       if (info.first == true){
         Tsa::SpacePoint* newPoint = (*iter1)->clone();
         const std::vector<Tsa::Cluster*>& clusVec = (*iter2)->clusters();
         newPoint->addToClusters(clusVec[info.second]);
         tmpCont.push_back(newPoint);
       }
    } // iter2
  } // iter1

  pCont->insert(pCont->begin(),tmpCont.begin(),tmpCont.end());

}

ITBoxProcessor::clonePair ITBoxProcessor::isClone(const Tsa::SpacePoint* point1,
                                                  const Tsa::SpacePoint* point2) const{

  // id clone hits
  bool isC = false;
  int index = -1;
  if (point1->size() == point2->size()){
    unsigned int nDiff = 0u;
    const std::vector<Tsa::Cluster*>& clusVec1 = point1->clusters();
    const std::vector<Tsa::Cluster*>& clusVec2 = point2->clusters();
    for (unsigned int iC = 0u; iC < point1->size(); ++iC) {

      if (clusVec1[iC] != clusVec2[iC]){
        ++nDiff ;
        index = iC;
      }
    } //  hits

    if ((nDiff == 1u)
        &&(fabs(clusVec1[index]->midPoint().z() - clusVec2[index]->midPoint().z())>0.5*Gaudi::Units::mm)
        &&(fabs(clusVec1[index]->midPoint().x()-
                clusVec2[index]->midPoint().x()) < m_uClone)) {
        LHCb::STChannelID chan = clusVec1[index]->id().stID();
        if ((chan.strip() <= 3 ||(chan.strip() >= 382 ))) isC = true;
    }
  } // if same size

  return std::make_pair(isC,index);
}


Tsa::SpacePoint* ITBoxProcessor::create4point(Tsa::STCluster* clusX1, 
                                              Tsa::STCluster* clusX2,
                                              Tsa::STCluster* clusU, 
                                              Tsa::STCluster* clusV, 
                                              double tX, double tY, 
                                              Gaudi::XYZPoint aPoint ){
  std::vector<Tsa::Cluster*> clusters;
  clusters += clusX1, clusU, clusV, clusX2;
  for (std::vector<Tsa::Cluster*>::iterator iter = clusters.begin(); 
       iter != clusters.end();++iter){
    (*iter)->updatePass(m_pass);
  }

  //  double tY =  y1-y2;
  return new Tsa::SpacePoint(aPoint,tX, tY, m_eType, m_station,clusters); 
}

Tsa::SpacePoint* ITBoxProcessor::create3point(Tsa::STCluster* clus1, 
                                            Tsa::STCluster* clus2,
                                            Tsa::STCluster* clus3, 
                                            double tX, Gaudi::XYZPoint aPoint){

  std::vector<Tsa::Cluster*> clusters;
  clusters += clus1, clus2, clus3;
  for (std::vector<Tsa::Cluster*>::iterator iter = clusters.begin(); 
       iter != clusters.end();++iter){
    (*iter)->updatePass(m_pass);
  }

  std::sort(clusters.begin(), clusters.end(), Tsa::ClusFun::Less_by_z<const Tsa::Cluster*>());
 
  return new Tsa::SpacePoint(aPoint,tX,aPoint.y()/aPoint.z() , m_eType, m_station,clusters); 
}











