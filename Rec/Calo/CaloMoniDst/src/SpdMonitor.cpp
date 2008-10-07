// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/SmartRef.h"
#include "GaudiKernel/SmartRefVector.h"


// from Event 
#include "Event/RecHeader.h"
#include "Event/CaloHypo.h"
#include "Event/CaloDataFunctor.h"
#include "Event/CaloDigit.h"

#include "CaloUtils/ClusterFunctors.h"

// from CaloDet
#include "CaloDet/DeCalorimeter.h"
#include "CaloDet/CellParam.h"

// from DetectorDataSvc
// #include "DetectorDataSvc/DetDataSvc"

// Chrono Auditor
// #include "Gaudi/ChronoStatSvc/Chrono.h"

// local
#include "CaloMoniAlg.h"


using namespace LHCb; 
using namespace Gaudi::Units;

// ============================================================================
/** @file
 *
 *  Implementation file for class SpdMonitor
 *
 *  SPD Monitoring
 *
 *  @author Albert Puig apuignav@cern.ch
 *  @date   2007-15-07
 */

// ============================================================================
/** Standard constructor
 *  @param name algorithm name
 *  @param pSvc service locator
 */
// ============================================================================
// forward declarations
class DeCalorimeter;
class CaloHypo;
class IDataProviderSvc;
class CaloDigit;
class ICaloHypoLikelyhood;
class IParticlePropertySvc ;
class IIncidentSvc    ;
class IHistogram1D    ;
class IHistogram2D    ;

class SpdMonitor : public CaloMoniAlg
{
	
	friend class AlgFactory<SpdMonitor>;
public:
	virtual StatusCode initialize();	
	virtual StatusCode execute();
	virtual StatusCode finalize();
	
protected:
	
	SpdMonitor::SpdMonitor	( const std::string& name ,   ISvcLocator*       pSvc )
    : CaloMoniAlg ( name , pSvc )
      , m_nEvents   ( 0 )
	{
		declareProperty("HistogramFlag", m_produceHistogram = true);
		declareProperty("EcalThreshold", m_ecalThres=700);
	}
	
	virtual ~SpdMonitor() {};
	
	
private:
		
	bool m_produceHistogram ; // do histos	
	unsigned int m_nEvents;
	DeCalorimeter *m_detSpd;
	DeCalorimeter *m_detEcal;
	CaloVector<int> m_cellHit;
	CaloVector<int> m_neighHit;
	CaloVector<int> m_neighHitEcal;
	CaloVector<int> m_neighN;
	unsigned int n_cells;
	/// ECAL
	double m_ecalThres;
	
	
};		
DECLARE_ALGORITHM_FACTORY( SpdMonitor );
	
// ============================================================================


// ============================================================================
StatusCode SpdMonitor::initialize()
{
  StatusCode sc = GaudiHistoAlg::initialize();
  if (sc.isFailure()) return sc;
  
  
  hBook2("o_flat","Flat outer", 6.,58.,52, 0.,64.,64);		
  hBook2("o_neigh","Avg occupancy of neighbors (outer SPD)", 6.,58.,52,0.,64.,64);
  hBook2("o_occupancy","Occupancy of the outer SPD", 6.,58.,52,0.,64.,64);
  hBook2("m_flat","Flat middle", 12.,52.,40,0.,64.,64); 
  hBook2("m_neigh","Avg occupancy of neighors (middle SPD)", 12.,52.,40,0.,64.,64);
  hBook2("m_occupancy","Occupancy of the middle SPD", 12.,52.,40,0.,64.,64);	
  hBook2("i_flat" ,"Flat inner", 14.,50.,36,8.,56.,48);
  hBook2("i_neigh" ,"Avg occupancy of neighbors (inner SPD)",14.,50.,36,8.,56.,48);
  hBook2("i_occupancy" ,"Occupancy of the inner SPD", 14.,50.,36,8.,56.,48);
  hBook2("o_occupancyEcal","Occupancy: E_Ecal>Threshold (outer)",6.,58.,52,0.,64.,64);
  hBook2("o_ecal+spd","SPD 1's when E_Ecal>Threshold (outer)",6.,58.,52,0.,64.,64);
  hBook2("m_occupancyEcal","Occupancy: E_Ecal>Threshold (middle)",12.,52,40,0.,64.,64); 
  hBook2("m_ecal+spd","SPD 1's when E_Ecal>Threshold (middle)",12.,52,40,0.,64.,64);
  hBook2("i_occupancyEcal","Occupancy: E_Ecal>Threshold (inner)",14.,50.,36,8.,56.,48);
  hBook2("i_ecal+spd","SPD 1's when E_Ecal>Threshold (inner)",14.,50.,36,8.,56.,48);  
  hBook2("o_neighEcal","Avg occupancy of neighbors + ECAL (outer SPD)", 6.,58.,52,0.,64.,64);
  hBook2("m_neighEcal","Avg occupancy of neighors + ECAL (middle SPD)", 12.,52.,40,0.,64.,64);
  hBook2("i_neighEcal" ,"Avg occupancy of neighbors + ECAL (inner SPD)",14.,50.,36,8.,56.,48);
  hBook1("o_mult" ,"SPD Multiplicity (outer)",0,200,200);  
  hBook1("m_mult" ,"SPD Multiplicity (middle)",0,200,200);  
  hBook1("i_mult" ,"SPD Multiplicity (inner)",0,200,200);  
  hBook1("o_multEcal" ,"SPD Multiplicity + ECAL (outer)",0,50,50);  
  hBook1("m_multEcal" ,"SPD Multiplicity + ECAL (middle)",0,50,50);  
  hBook1("i_multEcal" ,"SPD Multiplicity + ECAL (inner)",0,50,50);
  hBook1("mult","SPD Multiplicity",0,500,500);  
  hBook1("multEcal" ,"SPD Multiplicity + ECAL",0,150,150);
  hBook1("o_nclus" ,"ECAL Clusters (outer)",0,100,100);  
  hBook1("m_nclus" ,"ECAL Clusters (middle)",0,100,100);  
  hBook1("i_nclus" ,"ECAL Clusters (inner)",0,100,100);
  hBook1("nclus","ECAL Clusters",0,300,300);
  hBook1("o_elec" ,"Multiplicity / ECAL Clusters (outer)",0,1.,100);  
  hBook1("m_elec" ,"Multiplicity / ECAL Clusters (middle)",0,1.,100);  
  hBook1("i_elec" ,"Multiplicity / ECAL Clusters (inner)",0,1.,100);
  hBook1("elec","Multiplicity / ECAL Clusters",0,1.,100);		
  
  // Loads the detectors
  
  m_detSpd = getDet<DeCalorimeter>( DeCalorimeterLocation::Spd );
  m_detEcal= getDet<DeCalorimeter>( DeCalorimeterLocation::Ecal);
  n_cells = m_detSpd->numberOfCells();

  // Histograms
  if ( m_produceHistogram )	info() << "Histograms will be produced" << endreq; 
  
  // Initialize neighbor matrix
  for(unsigned int cellIt = 0; cellIt!=m_detSpd->numberOfCells(); cellIt++){
    CaloCellID cell = m_detSpd->cellIdByIndex(cellIt);
    const CaloNeighbors& neigh = m_detSpd->zsupNeighborCells(cell);    
    m_neighN.addEntry( neigh.size() , cell);      
  }  
  return StatusCode::SUCCESS;
}
  


// ============================================================================
StatusCode SpdMonitor::finalize()
{
  debug() << "==> Finalize" << endreq;

  info() << "**************** SPD Monitoring *****************"<<endreq;
  info() << "Number of Events Analyzed : " << m_nEvents << endreq;
  info() << "*************************************************"<<endreq;

  return CaloMoniAlg::finalize() ;
};
// ============================================================================

// ============================================================================
/** standard algorithm execution
 *  @see CaloAlgorithm
 *  @see     Algorithm
 *  @see    IAlgorithm
 *  @return status code
 */
// ============================================================================
StatusCode SpdMonitor::execute()
{   
	

  debug() << "==> Execute " << endreq;
  m_nEvents++;  
  debug() << "...processing Event " << m_nEvents << endreq;


  int mult[3]={0};	
  int multEcal[3]={0};
  int nclus[3]={0};  
  
  
// -------------------------------SPD Standalone Histograms---------------------------------------  
  CaloDigits* digitsSpd = get<CaloDigits>(CaloDigitLocation::Spd);  

  // Initialize the occupancy matrices
  for(unsigned int cellIt = 0; cellIt!=n_cells; cellIt++){	
    CaloCellID id = m_detSpd->cellIdByIndex(cellIt);
    m_cellHit.addEntry(0, id);
    m_neighHit.addEntry( 0, id);
    m_neighHitEcal.addEntry( 0, id);
  }

  // Determine which cells have a hit
  for(CaloDigits::iterator idig = digitsSpd->begin() ; digitsSpd->end() !=  idig ; idig++){
	  CaloCellID cell = (*idig)->cellID();
    m_cellHit[cell]=1;
    const CaloNeighbors& neigh = m_detSpd->zsupNeighborCells(cell);    
    for (CaloNeighbors::const_iterator neighIt = neigh.begin(); neighIt != neigh.end(); neighIt++){
      m_neighHit[*neighIt]+=1;
    }
  }

	// fill histo
  for(unsigned int cellIt = 0; cellIt!=n_cells; cellIt++){
	  const CaloCellID cell = m_detSpd->cellIdByIndex(cellIt);    
	  double neighAve=float(m_neighHit[cell])/float(m_neighN[cell]);

	  if( 0 == cell.area()) {		  
      // OUTER CALORIMETER
      hFill2("o_flat",cell.row(), cell.col());  // Generation of a flat histogram
      hFill2("o_neigh",cell.row(), cell.col(), neighAve);
      if ( m_cellHit[cell]){  // The cell has has been hit
        mult[0]++;
        hFill2("o_occupancy",cell.row(), cell.col());
      }
    }
    else if( 1 == cell.area() ){          
      // MIDDLE CALORIMETER
      hFill2("m_flat",cell.row(), cell.col());  // Generation of a flat histogram
      hFill2("m_neigh",cell.row(), cell.col(), neighAve);
      if (m_cellHit[cell]){  // The cell has has been hit
        mult[1]++;
        hFill2("m_avgOcccupancy",cell.row(), cell.col());
      }
    }
    else if( 2 == cell.area() ){
      hFill2("i_flat",cell.row(), cell.col());  // Generation of a flat histograms
      hFill2("i_neigh" ,cell.row(), cell.col(),neighAve);
      if (m_cellHit[cell]){  // The cell has has been hit
        mult[2]++;
        hFill2("i_occupancy" ,cell.row(), cell.col());
      }
    } 
  }
  
    
  debug() <<"-------------------SPD---------------------"<<endreq;
  debug() <<"Number of hits in the event:" << mult[0]+mult[1]+mult[2]<<endreq;
  debug() <<"-------------------------------------------"<<endreq; 
  
  
  
  
//---------------------------------ECAL------------------------------------------------  

  CaloClusters* ecalClus = get<CaloClusters> ( CaloClusterLocation::Ecal ) ;
  
  if (ecalClus != 0){
	  debug() <<"ECAL clusters present"<<endreq;
    for (CaloClusters::const_iterator iClus = ecalClus->begin(); iClus != ecalClus->end(); ++iClus){
      CaloCellID seedCell = (*iClus)->seed();
      double seedE=(*iClus)->e();
      debug() <<"Seed cell: "<<seedCell<<" with energy "<<seedE/GeV<<" GeV"<<endreq;
/*		Histograms for plotting the cluster energy and choosig the threshold
			
		plot(seedE/GeV,"clusterE" ,"Cluster energy in ECAL", 0.,50.,100);
		plot(seedE/MeV,"clusterEMeV" ,"Cluster energy in ECAL", 0.,2000.,200);
		
		switch(seedCell.area()){
			case 0:
				plot(seedE/GeV,"o_clusterE" ,"Cluster energy in ECAL", 0.,50.,100);
				plot(seedE/MeV,"o_clusterEMeV" ,"Cluster energy in ECAL", 0.,1000.,100);
				break;
			case 1:
				plot(seedE/GeV,"m_clusterE" ,"Cluster energy in ECAL", 0.,50.,100);
				plot(seedE/MeV,"m_clusterEMeV" ,"Cluster energy in ECAL", 0.,1000.,100);
				break;
			case 2:
				plot(seedE/GeV,"i_clusterE" ,"Cluster energy in ECAL", 0.,20.,1000);
				plot(seedE/MeV,"i_clusterEMeV" ,"Cluster energy in ECAL", 0.,1500.,150);
				break;
		}*/
		
      if (seedE>=m_ecalThres){
        if( 0 == seedCell.area()){
					hFill2("o_occupancyEcal",seedCell.row(), seedCell.col());
					nclus[0]++;
					if (m_cellHit[seedCell]){
						multEcal[0]++;
						hFill2("o_ecal+spd",seedCell.row(), seedCell.col());
					}
        }
        else if( 1 == seedCell.area()){
					hFill2("m_occcupancyEcal",seedCell.row(), seedCell.col()); 
				debug()<<"1: "<<nclus[1]<<endreq;
				nclus[1]++;
					if (m_cellHit[seedCell]){
						multEcal[1]++;
						hFill2("m_ecal+spd",seedCell.row(), seedCell.col());
					}
        }
        else if( 2 == seedCell.area()){
					hFill2("i_occupancyEcal",seedCell.row(), seedCell.col());
					nclus[2]++;
					if (m_cellHit[seedCell]){
						multEcal[2]++;
						hFill2("i_ecal+spd",seedCell.row(), seedCell.col());  
					}
        }
        
        
        if (m_cellHit[seedCell]){
          const CaloNeighbors& neigh = m_detSpd->zsupNeighborCells(seedCell);    
          for (CaloNeighbors::const_iterator neighIt = neigh.begin(); neighIt != neigh.end(); neighIt++){
            m_neighHitEcal[*neighIt]+=1;
          }
        }
      }	
    }
    for(unsigned int cellIt = 0; cellIt!=n_cells; cellIt++){
      CaloCellID cell = m_detSpd->cellIdByIndex(cellIt);    				
      double neighAve=float(m_neighHitEcal[cell])/float(m_neighN[cell]);
      
      if( 0 == cell.area()){
        hFill2("o_neighEcal",cell.row(), cell.col(), neighAve);
      }
      else if(1 == cell.area()){
        hFill2("m_neighEcal",cell.row(), cell.col(), neighAve);
      }
      else if(2 == cell.area()){
        hFill2("i_neighEcal",cell.row(), cell.col(),neighAve);
      }
    }
    
    
    // Multiplicity histograms
    hFill1("o_mult",mult[0]);  
    hFill1("m_mult",mult[1]);  
    hFill1("i_mult",mult[2]);  
    hFill1("o_multEcal",multEcal[0]);  
    hFill1("m_multEcal",multEcal[1]);  
    hFill1("i_multEcal",multEcal[2]);
    hFill1("mult",mult[0]+mult[1]+mult[2]);  
    hFill1("multEcal",multEcal[0]+multEcal[1]+multEcal[2]);
    hFill1("o_nclus",nclus[0]);  
    hFill1("m_nclus",nclus[1]);  
    hFill1("i_nclus",nclus[2]);
    hFill1("nclus",nclus[0]+nclus[1]+nclus[2]);
    
    
    if (nclus[0]!=0 && nclus[2]!=0 && nclus[1]!=0){  // Just to avoid ugly surprises
      hFill1("o_elec",float(multEcal[0])/float(nclus[0]));  
      hFill1("m_elec",float(multEcal[1])/float(nclus[1]));  
      hFill1("i_elec",float(multEcal[2])/float(nclus[2]));
      hFill1("elec",float(multEcal[0]+multEcal[1]+multEcal[2])/float(nclus[0]+nclus[1]+nclus[2]));
    }
  }
  
  debug() <<"-------------------ECAL--------------------"<<endreq;
  debug() <<"Number of clusters in the event:" << multEcal[0]+multEcal[1]+multEcal[2]<<endreq;
  debug() <<"-------------------------------------------"<<endreq; 
    
  debug() <<"Event "<<m_nEvents<<" correctly processed"<<endreq;
  
  return StatusCode::SUCCESS;
};



