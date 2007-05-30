// $Id: Calo2Calo.cpp,v 1.1 2007-05-30 14:45:31 odescham Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/ToolFactory.h" 

#include "Kernel/CaloCellCode.h"
// local
#include "CaloUtils/Calo2Calo.h"

//-----------------------------------------------------------------------------
// Implementation file for class : Calo2Calo
//
// 2007-05-29 : Olivier Deschamps
//-----------------------------------------------------------------------------

// Declaration of the Tool Factory
DECLARE_TOOL_FACTORY( Calo2Calo );


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
Calo2Calo::Calo2Calo( const std::string& type,
                      const std::string& name,
                      const IInterface* parent )
  : GaudiTool ( type, name , parent )
    ,m_cells()
    ,m_digits()
    ,m_energy()
{
  declareInterface<ICalo2Calo>(this);

}
//=============================================================================
// Destructor
//=============================================================================
Calo2Calo::~Calo2Calo() {}
//=============================================================================
StatusCode Calo2Calo::initialize(){
  debug() << "Initialize Calo2Calo tool " << endreq;

  // CaloDigit locations
  m_loc["Ecal"]= LHCb::CaloDigitLocation::Ecal;
  m_loc["Hcal"]= LHCb::CaloDigitLocation::Hcal;
  m_loc["Prs"]= LHCb::CaloDigitLocation::Prs;
  m_loc["Spd"]= LHCb::CaloDigitLocation::Spd;
  // DeCalorimeter* pointers
  m_det["Ecal"]=getDet<DeCalorimeter>( DeCalorimeterLocation::Ecal );
  m_det["Hcal"]=getDet<DeCalorimeter>( DeCalorimeterLocation::Hcal );
  m_det["Prs"]=getDet<DeCalorimeter>( DeCalorimeterLocation::Prs   );
  m_det["Spd"]=getDet<DeCalorimeter>( DeCalorimeterLocation::Spd   );
  // CellSize reference (outer section)  Warning : factor 2 for Hcal
  m_refSize["Hcal"] = (*(m_det["Hcal"]->cellParams().begin())).size()/2.;
  m_refSize["Ecal"] = (*(m_det["Ecal"]->cellParams().begin())).size(); 
  m_refSize["Prs"]  = (*(m_det["Prs"]->cellParams().begin())).size(); 
  m_refSize["Spd"]  = (*(m_det["Spd"]->cellParams().begin())).size(); 

  return StatusCode::SUCCESS;
}

void Calo2Calo::reset(){
  m_digits.clear();
  m_cells.clear();
  m_energy = 0;
}

std::vector<LHCb::CaloCellID> Calo2Calo::cellIDs(LHCb::CaloCluster fromCluster, std::string toCalo){

  reset();

  for(LHCb::CaloCluster::Entries::const_iterator ent = fromCluster.entries().begin();
      ent != fromCluster.entries().end(); ++ent){
    SmartRef<LHCb::CaloDigit> digit = (*ent).digit();
    cellIDs( (*digit).cellID() , toCalo, false );
  }
  return m_cells;
}



std::vector<LHCb::CaloCellID> Calo2Calo::cellIDs(LHCb::CaloCellID fromId, std::string toCalo , bool init){

  if( init )reset();

  m_fromCalo = CaloCellCode::CaloNameFromNum( fromId.calo() );
  m_toCalo   = toCalo;

  DeCalorimeter* fromDet = m_det[ m_fromCalo ];
  DeCalorimeter* toDet   = m_det[ m_toCalo ];
  
  // z-scaling
  double scale = m_refSize[ m_toCalo ] / m_refSize[ m_fromCalo ] ;
  
  Gaudi::XYZPoint  center  = 
    toDet->plane(CaloPlane::Middle).ProjectOntoPlane( fromDet->cellCenter( fromId )*scale );
  // connect
  LHCb::CaloCellID toId  = toDet->Cell( center );


    //cell-center does is outside 'toCalo' - check corners  
  if( LHCb::CaloCellID() == toId){
    for( int i = 0 ; i != 2; ++i){
      for( int j = 0 ; j != 2; ++j){
        double x = fromDet->cellCenter( fromId ).X() + (i*2-1) * fromDet->cellSize( fromId );        
        double y = fromDet->cellCenter( fromId ).Y() + (j*2-1) * fromDet->cellSize( fromId );
        Gaudi::XYZPoint  corner = Gaudi::XYZPoint(x*scale,y*scale,center.Z());
        LHCb::CaloCellID cornerId  = toDet->Cell( corner );
        if( LHCb::CaloCellID() == cornerId)continue;
        toId=cornerId;
      }   
    }
  }
  if( LHCb::CaloCellID() == toId)return m_cells;

  
  
  double fromSize = fromDet->cellSize( fromId )*scale;
  double toSize   = toDet->cellSize( toId ) ;
  int pad =  (int) floor( fromSize  / toSize + 0.25); // warning int precision
  if(pad < 1)pad=1;
  double x0 = center.X() - (pad-1)*fromSize/2./pad;
  double y0 = center.Y() - (pad-1)*fromSize/2./pad;  
  for( int i = 0 ; i != pad; ++i){
    for( int j = 0 ; j != pad; ++j){
      double x = x0 + i * fromSize/pad;
      double y = y0 + j * fromSize/pad;
      
      Gaudi::XYZPoint  pos(x,y,center.Z());
      toId  = toDet->Cell( pos ) ;
      if( LHCb::CaloCellID() == toId)continue;
      bool ok = true;
      for(std::vector<LHCb::CaloCellID>::const_iterator icel = m_cells.begin();
          icel != m_cells.end();++icel){
        if( toId == (*icel) )ok=false;
      }
      if(ok){
        m_cells.push_back( toId );
        LHCb::CaloDigits* digits = get<LHCb::CaloDigits>( m_loc[ m_toCalo ] );
        LHCb::CaloDigit* digit = digits->object( toId );
        if( NULL != digit ) {
          m_digits.push_back( *digit );
          m_energy += digit->e();
        } 
      }
    }        
  }
  return m_cells;
}


// Digits
std::vector<LHCb::CaloDigit> Calo2Calo::digits(LHCb::CaloCellID fromId, std::string toCalo){  
  cellIDs( fromId, toCalo);
  return m_digits;
}
std::vector<LHCb::CaloDigit> Calo2Calo::digits(LHCb::CaloCluster fromCluster, std::string toCalo){  
  cellIDs( fromCluster, toCalo);
  return m_digits;
}  
// Energy
double Calo2Calo::energy(LHCb::CaloCellID fromId, std::string toCalo){
  cellIDs(fromId, toCalo);
  return m_energy;
}
double Calo2Calo::energy(LHCb::CaloCluster fromCluster, std::string toCalo){
  cellIDs(fromCluster, toCalo);
  return m_energy;
}


// Additional method : isLocalMax
bool Calo2Calo::isLocalMax(LHCb::CaloDigit digit){
  LHCb::CaloCellID id = digit.cellID();
  std::string calo = CaloCellCode::CaloNameFromNum( id.calo() );
   DeCalorimeter* det   = m_det[ calo ];
   std::vector<LHCb::CaloCellID> neighbors = det->neighborCells( id );
   for(std::vector<LHCb::CaloCellID>::iterator iid = neighbors.begin(); iid!=neighbors.end();++iid){
     LHCb::CaloDigits* digits = get<LHCb::CaloDigits>( m_loc[ calo ] );
     LHCb::CaloDigit* neighbor = digits->object( *iid );
     if( NULL == neighbor )continue;
     if( digit.e() <= neighbor->e() )return false;
   }
   return true;   
}


