// Include files 

// from Gaudi
#include "GaudiKernel/SystemOfUnits.h"
// from LHCb
#include "GaudiKernel/Vector3DTypes.h"
#include "GaudiKernel/Plane3DTypes.h"
#include "LHCbMath/LineTypes.h"
#include "LHCbMath/GeomFun.h"
#include "CaloUtils/CaloAlgUtils.h"
// local
#include "CaloHypo2Calo.h"


//-----------------------------------------------------------------------------
// Implementation file for class : CaloHypo2Calo
//
// 2008-09-11 : Olivier Deschamps
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( CaloHypo2Calo )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
CaloHypo2Calo::CaloHypo2Calo( const std::string& type,
                            const std::string& name,
                            const IInterface* parent )
  : Calo2Calo ( type, name , parent )
{
  declareInterface<ICaloHypo2Calo>(this);


  declareProperty("Seed"         , m_seed   = true );
  declareProperty("PhotonLine"   , m_line   = true );
  declareProperty("AddNeighbors" , m_neighb = true );
  declareProperty("xTolerance"   , m_x      =  5.*Gaudi::Units::mm);
  declareProperty("yTolerance"   , m_y      =  5.*Gaudi::Units::mm);
  declareProperty("StatusMask"   , m_status = 0x0  );
  declareProperty("WholeCluster" , m_whole  = false);

}
//=============================================================================
// Destructor
//=============================================================================
CaloHypo2Calo::~CaloHypo2Calo() {} 

//=============================================================================
StatusCode CaloHypo2Calo::initialize(){
  StatusCode sc = Calo2Calo::initialize();
  if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) 
    debug() << "Initialize CaloHypo2Calo tool " << endmsg;
  m_lineID = LHCb::CaloCellID(); // initialize
  m_point  = Gaudi::XYZPoint();
  return sc;
}


//=============================================================================
const std::vector<LHCb::CaloCellID>& CaloHypo2Calo::cellIDs(const LHCb::CaloHypo &fromHypo, const std::string &toCalo){

  if( UNLIKELY( msgLevel(MSG::DEBUG) ) )
    debug() << "Matching CaloHypo to " << toCalo << " hypo energy = " << fromHypo.e() << endmsg;

  // get the cluster 
  const LHCb::CaloCluster* cluster = LHCb::CaloAlgUtils::ClusterFromHypo( &fromHypo );

  if(cluster == 0){
    Error("No valid cluster!").ignore() ;
    return m_cells;
  }

  LHCb::CaloCellID seedID = cluster->seed();
  std::string fromCalo = CaloCellCode::CaloNameFromNum( seedID.calo() );
  if( toCalo != m_toCalo || fromCalo != m_fromCalo)setCalos(fromCalo,toCalo);
  if( !m_ok )return m_cells;
  if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) 
    debug() << "Cluster seed " << seedID << " " << m_fromDet->cellCenter( seedID ) << endmsg;

  if(m_whole){
    return Calo2Calo::cellIDs( *cluster, toCalo);
  }

  m_point  = Gaudi::XYZPoint();
  m_lineID = LHCb::CaloCellID();
  if(m_line && fromHypo.position() != NULL){
    const Gaudi::XYZPoint  ref( fromHypo.position()->x(), fromHypo.position()->y(),fromHypo.position()->z() );
    const Gaudi::XYZVector vec = (ref-Gaudi::XYZPoint(0,0,0));
    const Gaudi::Plane3D   plane = m_toPlane;
    Gaudi::Math::XYZLine   line(ref, vec);
    double mu;
    Gaudi::Math::intersection<Gaudi::Math::XYZLine,Gaudi::Plane3D>(line,plane,m_point,mu);
    m_lineID = m_toDet->Cell( m_point );
    if( UNLIKELY( msgLevel(MSG::DEBUG) ) )
      debug() << "Matching cell " << m_lineID << endmsg;
  }
  return cellIDs( *cluster, toCalo);
}



const std::vector<LHCb::CaloCellID>& CaloHypo2Calo::cellIDs(const LHCb::CaloCluster &fromCluster, const std::string &toCalo){
  if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) 
    debug() << " toCalo " << toCalo << endmsg;
  reset();
  LHCb::CaloCellID seedID = fromCluster.seed();
  std::string fromCalo = CaloCellCode::CaloNameFromNum( seedID.calo() );
  if( toCalo != m_toCalo || fromCalo != m_fromCalo)setCalos(fromCalo,toCalo);
  if( !m_ok)return m_cells;
  
  if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) 
    debug() << "-----  cluster energy " <<  fromCluster.e()<< " " << seedID << endmsg;
  m_neighbour.setDet ( m_fromDet );

  // get data
  m_digs     = m_getter->digits( m_toLoc );  

  //matching cluster  
  const LHCb::CaloCluster::Entries& entries = fromCluster.entries();
  for(LHCb::CaloCluster::Entries::const_iterator ent = entries.begin(); ent != entries.end(); ++ent){
    LHCb::CaloClusterEntry entry = *ent;
    LHCb::CaloCellID cellID = entry.digit()->cellID();
    if( !(m_seed      &&  (LHCb::CaloDigitStatus::SeedCell & entry.status())    != 0 ) &&
        !(m_seed && m_neighb &&   m_neighbour(seedID , cellID)  != 0.                ) &&
        !( (m_status & entry.status()) != 0                                          ) &&
        !(m_whole) )continue;
    SmartRef<LHCb::CaloDigit> digit = (*ent).digit();
    Calo2Calo::cellIDs( digit->cellID() , toCalo, false );
    if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) 
      debug() << toCalo << ":  digit is selected in front of the cluster : " 
              << cellID << "/" << seedID << " " << m_digits.size() << endmsg;
  }
  // photon line
  if(m_line ){
    if( m_lineID == LHCb::CaloCellID() ){
      const Gaudi::XYZPoint  ref( fromCluster.position().x(), fromCluster.position().y(),fromCluster.position().z() );
      const Gaudi::XYZVector vec = (ref-Gaudi::XYZPoint(0,0,0));
      const Gaudi::Plane3D   plane = m_toPlane;
      Gaudi::Math::XYZLine   line(ref, vec);
      double mu;
      Gaudi::Math::intersection<Gaudi::Math::XYZLine,Gaudi::Plane3D>(line,plane,m_point,mu);
      m_lineID = m_toDet->Cell( m_point );
    }
    if( !(m_lineID == LHCb::CaloCellID()) ){
      addCell( m_lineID , toCalo );
      if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) 
        debug() << toCalo << " : digit is selected in the photon line : " 
                << m_lineID << "/" << seedID << " " << m_digits.size() << endmsg;
      if(m_neighb){
        const std::vector<LHCb::CaloCellID>&  neighbors = m_toDet->neighborCells( m_lineID );
        for( std::vector<LHCb::CaloCellID>::const_iterator n = neighbors.begin();n!=neighbors.end();n++){
          double halfCell = m_toDet->cellSize( *n )*0.5;
          const Gaudi::XYZPoint cellCenter = m_toDet -> cellCenter ( *n ) ;
          if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) 
            debug() << *n 
                    << " Point : (" << m_point.X() << "," << m_point.Y() 
                    << " Cell :  ( " << cellCenter.X()<< "," << cellCenter.Y()
                    << " size/2  : " << halfCell << " Tolerance : " << m_x << "/" << m_y << endmsg;
          if( fabs(m_point.X() - cellCenter.X()) < (halfCell+m_x) &&
              fabs(m_point.Y() - cellCenter.Y()) < (halfCell+m_y) ){
            addCell( *n , toCalo );
            if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) 
              debug() << toCalo << " : digit is selected in the photon line neighborhood : " 
                      << *n << "/" << seedID  << " " << m_digits.size() << endmsg;
          } 
        }
      }
    }
  }

  m_lineID = LHCb::CaloCellID();//reset
  return m_cells;
}



const std::vector<LHCb::CaloDigit*>& CaloHypo2Calo::digits(const LHCb::CaloHypo &fromHypo, const std::string &toCalo){  
  cellIDs( fromHypo, toCalo);
  return m_digits;
}  

double CaloHypo2Calo::energy(const LHCb::CaloHypo &fromHypo, const std::string &toCalo){
  cellIDs(fromHypo, toCalo);
  return m_energy;
}
const std::vector<LHCb::CaloDigit*>& CaloHypo2Calo::digits(const LHCb::CaloCluster &fromCluster, const std::string &toCalo){  
  cellIDs( fromCluster, toCalo);
  return m_digits;
}  

double CaloHypo2Calo::energy(const LHCb::CaloCluster &fromCluster, const std::string &toCalo){
  cellIDs(fromCluster, toCalo);
  return m_energy;
}

int CaloHypo2Calo::multiplicity(const LHCb::CaloCluster &fromCluster, const std::string &toCalo){
  cellIDs(fromCluster, toCalo);
  return m_count;
}
int CaloHypo2Calo::multiplicity(const LHCb::CaloHypo &fromHypo, const std::string &toCalo){
  cellIDs(fromHypo, toCalo);
  return m_count;
}

