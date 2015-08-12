/** @class MuonMeasurementProvider MuonMeasurementProvider.cpp
 *
 * Implementation of VeloRMeasurementProvider tool
 * see interface header for description
 *
 *  @author Wouter Hulsbergen + Stefania Vecchi
 *  @date   30/12/2005            18/12/2009
 */
  
#include "TrackInterfaces/IMeasurementProvider.h"
#include "GaudiAlg/GaudiTool.h"
#include "GaudiKernel/ToolHandle.h"

#include "TrackInterfaces/ITrackExtrapolator.h"
#include "Event/MuonMeasurement.h"
#include "Event/StateVector.h"
#include "MuonDet/DeMuonDetector.h"
#include "Event/TrackParameters.h"

class MuonMeasurementProvider : public GaudiTool, virtual public IMeasurementProvider
{
public:
  
  /// constructer
  MuonMeasurementProvider(const std::string& type,
                          const std::string& name,
                          const IInterface* parent);
  
  ~MuonMeasurementProvider() {}
  
  StatusCode initialize() ;
  /// measurement for single hits
  LHCb::Measurement* measurement( const LHCb::LHCbID& id, bool localY ) const  ;              
  LHCb::Measurement* measurement( const LHCb::LHCbID& id, 
                                  const LHCb::ZTrajectory& /*reftraj*/,  bool localY ) const {
    return measurement(id,localY) ;
  }
  /// measurement for clustrer of hits
  LHCb::Measurement* measurement( const std::vector<LHCb::LHCbID>& ids, bool localY ) const  ; 
  LHCb::Measurement* measurement( const std::vector<LHCb::LHCbID>& ids, 
                                  const LHCb::ZTrajectory& /*reftraj*/,  bool localY ) const {
    return measurement(ids,localY) ;
  }
  
  double nominalZ( const LHCb::LHCbID& id ) const ;
  
  void addToMeasurements( const std::vector<LHCb::LHCbID>& lhcbids,
                          std::vector<LHCb::Measurement*>& measurements,
                          const LHCb::ZTrajectory& reftraj) const ;
  

  StatusCode load( LHCb::Track&  ) const {
    info() << "sorry, MeasurementProviderBase::load not implemented" << endmsg ;
    return StatusCode::FAILURE ;
  }

private:
  // pointer to detector
  mutable DeMuonDetector* m_det;
  bool m_clusterize;
  // to calculate the coordinate of the cluster
  void recomputePos(std::vector<double> data, std::vector<double> error, 
                           double *pos, double *dpos, int *clsize) const;
  void recomputePos(std::vector<double> data, std::vector<double> error, 
                           double *pos, double *dpos, int *clsize, std::vector<double> data_z ) const;
} ;

//=============================================================================
// Declare to tool factory
//=============================================================================


DECLARE_TOOL_FACTORY( MuonMeasurementProvider )

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
MuonMeasurementProvider::MuonMeasurementProvider( const std::string& type,
						const std::string& name,
						const IInterface* parent )
  :  GaudiTool( type, name , parent ),
     m_det(0)
{
  declareInterface<IMeasurementProvider>(this);
  declareProperty("clusterize",m_clusterize = false) ;
}

//-----------------------------------------------------------------------------
/// Initialize
//-----------------------------------------------------------------------------

StatusCode MuonMeasurementProvider::initialize()
{
  StatusCode sc = GaudiTool::initialize();
  if( sc.isFailure() ) { return Error( "Failed to initialize!", sc ); }

  // Retrieve the detector element
  m_det = getDet<DeMuonDetector>( "/dd/Structure/LHCb/DownstreamRegion/Muon" ) ;
  
  return sc;
}

//-----------------------------------------------------------------------------
/// Create a measurement
//-----------------------------------------------------------------------------

LHCb::Measurement* MuonMeasurementProvider::measurement( const LHCb::LHCbID& id, bool localY ) const
{
  LHCb::Measurement* meas(0) ;
  if( !id.isMuon() ) {
    error() << "Not a Muon measurement" << endmsg ;
  } else {
    LHCb::MuonTileID muid = id.muonID();
    double x,y,z,dx,dy,dz;
    StatusCode sc = m_det->Tile2XYZ(muid,x,dx,y,dy,z,dz);
    if (sc.isFailure()){
      Warning("Failed to get x,y,z of tile ",sc).ignore();
      if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) 
        debug() << "Failed to get x,y,z of tile " << muid << endmsg;
    } else {
      LHCb::MuonMeasurement::MuonMeasurementType dir = localY ? 
        LHCb::MuonMeasurement::Y : LHCb::MuonMeasurement::X ;
      meas = new LHCb::MuonMeasurement(id,*m_det,Gaudi::XYZPoint(x,y,z),dx,dy, dir);
      if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) 
        debug() << " Created muon measurement! " << muid
                << " x "<< x<<" y "<<y<<" z "<<z
                <<" dx "<<dx<<" dy "<<dy<<" dz "<<dz << endmsg; 
    }
  }
  return meas ;
}
//-----------------------------------------------------------------------------
/// Create a measurement
//-----------------------------------------------------------------------------

LHCb::Measurement* MuonMeasurementProvider::measurement( const std::vector< LHCb::LHCbID>& ids, bool localY ) const
{
  LHCb::Measurement* meas(0) ;
  std::vector<double> m_padx, m_pady, m_padz;
  std::vector<double> m_paddx, m_paddy, m_paddz;
  double  Cx, Cdx, Cy, Cdy, Cz, Cdz;  
  double  m_hit_minx = 100000; 
  double  m_hit_maxx =-100000; 
  double  m_hit_miny = 100000; 
  double  m_hit_maxy =-100000; 
  double  m_hit_minz = 100000; 
  double  m_hit_maxz =-100000;
  
  int m_clsize_x, m_clsize_y, m_clsize_z;
  double x,y,z,dx,dy,dz;
  
  for( std::vector<LHCb::LHCbID>::const_iterator id = ids.begin() ; id != ids.end(); ++id) {
    if( !id->isMuon() ) {
      error() << "Not a Muon measurement" << endmsg ;
    } else {
      LHCb::MuonTileID muid = id->muonID();
      StatusCode sc = m_det->Tile2XYZ(muid,x,dx,y,dy,z,dz);
      if (sc.isFailure()){
        Warning("Failed to get x,y,z of tile ",sc).ignore();
        if(msgLevel(MSG::DEBUG)) debug() << "Failed to get x,y,z of tile " << muid << endmsg;
      } else {
        m_padx.push_back(x);
        m_pady.push_back(y);
        m_padz.push_back(z);
        m_paddx.push_back(dx);
        m_paddy.push_back(dy);
        m_paddz.push_back(dz);
        if ((x-dx) < m_hit_minx) m_hit_minx = x-dx;
        if ((x+dx) > m_hit_maxx) m_hit_maxx = x+dx;
        if ((y-dy) < m_hit_miny) m_hit_miny = y-dy;
        if ((y+dy) > m_hit_maxy) m_hit_maxy = y+dy;
        if ((z-dz) < m_hit_minz) m_hit_minz = z-dz;
        if ((z+dz) > m_hit_maxz) m_hit_maxz = z+dz;

      }
    }    
  }
  /*
  recomputePos(m_padx,m_paddx,&Cx,&Cdx,&m_clsize_x,m_padz);
  recomputePos(m_pady,m_paddy,&Cy,&Cdy,&m_clsize_y,m_padz);
  */
  
  recomputePos(m_padx,m_paddx,&Cx,&Cdx,&m_clsize_x);
  recomputePos(m_pady,m_paddy,&Cy,&Cdy,&m_clsize_y);  
  recomputePos(m_padz,m_paddz,&Cz,&Cdz,&m_clsize_z);

  LHCb::LHCbID Cid;  
  double min_dist2 = 100000000;  
  for( std::vector<LHCb::LHCbID>::const_iterator id = ids.begin() ; id != ids.end(); ++id) {
    if( id->isMuon() ) {
      LHCb::MuonTileID muid = id->muonID();
      
      StatusCode sc = m_det->Tile2XYZ(muid,x,dx,y,dy,z,dz);
      if(sc.isSuccess()){
        double dist2 = pow(x-Cx,2) + pow(y-Cy,2) + pow(z-Cz,2) ;
        if(min_dist2 > dist2 ){
          min_dist2 = dist2;
          Cid = *id;    // choose the id closest to the cluster center
        }
      }
    }
  }
  if(!Cid.muonID().isValid()){
    error() << " IMPOSSIBLE to Create muon measurement from a cluster of "<<m_padx.size()<<" hits ! " << Cid.muonID()<<endmsg;
    error() << " x "<< Cx<<" +/- "<<Cdx<<" in the range ["<<m_hit_minx<<","<<m_hit_maxx<<"] "<<endmsg;
    error() << " y "<< Cy<<" +/- "<<Cdy<<" in the range ["<<m_hit_miny<<","<<m_hit_maxy<<"] "<<endmsg;
    error() << " z "<< Cz<<" +/- "<<Cdz<<" in the range ["<<m_hit_minz<<","<<m_hit_maxz<<"] "<<endmsg;
  }
  
  
  LHCb::MuonMeasurement::MuonMeasurementType dir = localY ? LHCb::MuonMeasurement::Y : LHCb::MuonMeasurement::X ;
  meas = new LHCb::MuonMeasurement(Cid,*m_det,Gaudi::XYZPoint(Cx,Cy,Cz),Cdx,Cdy, dir);

  if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) {
    debug() << " Created muon measurement from a cluster of "<<m_padx.size()<<" hits ! " << Cid.muonID()<<endmsg;
    debug() << " x "<< Cx<<" +/- "<<Cdx<<" in the range ["<<m_hit_minx<<","<<m_hit_maxx<<"] "<<endmsg;
    debug() << " y "<< Cy<<" +/- "<<Cdy<<" in the range ["<<m_hit_miny<<","<<m_hit_maxy<<"] "<<endmsg;
    debug() << " z "<< Cz<<" +/- "<<Cdz<<" in the range ["<<m_hit_minz<<","<<m_hit_maxz<<"] "<<endmsg;
  }
  
  return meas ;
}


//-----------------------------------------------------------------------------
// Return the z-coordinate of this lhcb-id (w/o creating the hit)
//-----------------------------------------------------------------------------

double MuonMeasurementProvider::nominalZ( const LHCb::LHCbID& id ) const
{
  return m_det->getStationZ(id.muonID().station()) ;
}

//-----------------------------------------------------------------------------
/// Create measurements for list of LHCbIDs
//-----------------------------------------------------------------------------

void MuonMeasurementProvider::addToMeasurements( const std::vector<LHCb::LHCbID>& lhcbids,
                                                 std::vector<LHCb::Measurement*>& measurements,
                                                 const LHCb::ZTrajectory& reftraj) const
{
  
  if(!m_clusterize){
    
    if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) 
      debug()<<" MuonMeasurementProvider makes measurements for each LHCbID"<<endmsg;
    
    for( std::vector<LHCb::LHCbID>::const_iterator id = lhcbids.begin() ;
         id != lhcbids.end(); ++id) {
      measurements.push_back( measurement(*id,reftraj,false) )  ;
      measurements.push_back( measurement(*id,reftraj,true) )  ;
    }
  }else{

    if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) 
      debug()<<" MuonMeasurementProvider makes measurements for each CLUSTER of LHCbIDs"<<endmsg;
    std::vector<LHCb::LHCbID> lhcbids_byStation[5];
    
    for( std::vector<LHCb::LHCbID>::const_iterator id = lhcbids.begin() ; id != lhcbids.end(); ++id) {
      if( id->isMuon() ) {
        LHCb::MuonTileID muid = id->muonID();
        int iMS = muid.station();
        lhcbids_byStation[iMS].push_back(*id);
      }
    }
    
    for( int iMS=0; iMS<5; ++iMS) {
      if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) 
        debug()<<" Station M"<<iMS<<" lhcbIDS "<<lhcbids_byStation[iMS].size()<<endmsg;
      if(lhcbids_byStation[iMS].size()!=0){        
        measurements.push_back( measurement(lhcbids_byStation[iMS],reftraj,false) )  ;
        measurements.push_back( measurement(lhcbids_byStation[iMS],reftraj,true) )  ;
      }
    }
  }
}


void MuonMeasurementProvider::recomputePos(std::vector<double> data, 
                                           std::vector<double> error, 
                                           double* Cpos, double* Cdpos,
                                           int* clsize) const {
  int np=0;
  double sum=0.,sum2=0.;
  std::vector<double>::iterator ip,previp;
  double step=0;
  unsigned int i,j;
  for (i=0 ; i < data.size(); ++i) {   
    step=error[i];
    bool prendila=true;
    // check that this position is not already the same of a previous pad
    for ( j=0; j<i; ++j) {
      if ( fabs(data[i]-data[j])< 0.5*step) { 
        prendila=false;
        break;
      }
    }
    if (prendila) {
      np++;
      sum += data[i];
      sum2 += data[i]*data[i];
    }
  }
  *Cpos = sum/np;
  if (np>1) {
    //*Cdpos = sqrt( (sum2-sum*sum/np)/(np*np-np)  ); // mean error = sigma/sqrt(n-1)
    *Cdpos = sqrt( (sum2-sum*sum/np)/(np-1)  ); // sigma
  } else {
    *Cdpos = step;
  }  
  *clsize = np;
}

void MuonMeasurementProvider::recomputePos(
                                           std::vector<double> data_x, 
                                           std::vector<double> error_x, 
                                           double* Cpos, double* Cdpos,
                                           int* clsize,
                                           std::vector<double> data_z
                                           ) const {
  int np=0;
  double sum=0.,sum2=0.;
  unsigned int i,j;
  
  double step_z=70; //mm chamber dimension
  double step_x =0;
  
  for (i=0; i<data_z.size(); ++i){

    bool prendila=true;
    step_x  = error_x[i];      
    
    // check that this position is not already the same of a previous pad
    for (j=0 ; j < i; ++j) {
      if ( fabs(data_z[i] - data_z[j])< 0.5*step_z) {  // same chamber
        
        if ( fabs(data_x[i] - data_x[j])< 0.5*step_x) {  // same x
          prendila=false;
          break;
        }        
      }
    }
    if (prendila) {
      np++;
      sum += data_x[i];
      sum2 += data_x[i]*data_x[i];
    }
  }
  *Cpos = sum/np;
  if (np>1) {
    //*Cdpos = sqrt( (sum2-sum*sum/np)/(np*np-np)  ); // mean error = sigma/sqrt(n-1)
    *Cdpos = sqrt( (sum2-sum*sum/np)/(np-1)); // sigma
  } else {
    *Cdpos = step_x;
  }  
  *clsize = np;
  
}
