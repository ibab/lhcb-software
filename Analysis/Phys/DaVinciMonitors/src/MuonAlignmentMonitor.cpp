// $Id: MuonAlignmentMonitor.cpp,v 1.1 2010-02-02 11:42:36 ggiacomo Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/DeclareFactoryEntries.h" 

// local
#include "MuonAlignmentMonitor.h"

// Extrapolator
#include "TrackInterfaces/ITrackExtrapolator.h" 

// from TrackTools
#include "TrackInterfaces/ITrackChi2Calculator.h"


//#include "GaudiAlg/GaudiHistoAlg.h"

// Tools for histogramming
#include "AIDA/IHistogram1D.h"
#include "AIDA/IProfile1D.h"
#include "AIDA/IHistogram2D.h"
#include "AIDA/IProfile2D.h"

#include "Event/MuonPID.h"
#include "MuonDet/DeMuonDetector.h"

#include <boost/lexical_cast.hpp>

using namespace Gaudi::Units;

//-----------------------------------------------------------------------------
// Implementation file for class : MuonAlignmentMonitor
//
// 2009-02-25 
//                                                                              
//                                                        .--. ::/.             
//    a.k.a. "ciupeta"                                 -//:::::////.            
//                                                    :/:::::::/++o/            
//    a Ferrara speciality                        ..-//::::::/++oo:             
//                                              ://::::::///+/:-.               
//                                        -://::::---::://+o                    
//                                   .-::/::::::://:///++++:                    
//       ...                .---::///:::/+++/:::::/+/o+/:-                      
//   .-///////:--.-:///::--///:::/+/++ooo++++////::/:                           
//  :o++//::::::++/::::::/+////+o++++/://////+++/////                           
//   /ooo++/:/:++/////::/+++/o+++/++//::::////++++++o-                          
//     .---:/oooo++++++/+oo++o++++++//://oo+/:-::::/+/                          
//           :osssssooooosooossoo+//////::::::::::::///::-.-::--.               
//              ....   .://:-.+o+////::///////:::::::/::::/::////:              
//                            .o+//://////++++////////:-///////:--:-::::--.     
//                            -+//::///+++oooooooo++++//++++++/--:::::::////:   
//                          .//+/////++oosssssysssssosooooooo++/++/////+o+/::/  
//                      .-://::/++o++oossso/:::/+oso: .-/++:../sssoooooooo+++o  
//                     /+//////::+osssss/.                      :+sso:.  .---.  
//                    :/::://++++oo///:                                         
//                  .:/::--:/+ooo/.                                             
//                :+////////++..                                                
//              /+++////+++oo.                                                  
//            .//+ooooooosso.                                                   
//            ++/::/+o/::-.                                                     
//           .sooso+/                                                           
//            /oo+:.                                                            
//                                                                              
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( MuonAlignmentMonitor );


MuonAlignmentMonitor::MuonAlignmentMonitor( const std::string& name,
					    ISvcLocator* pSvcLocator)
  : GaudiHistoAlg ( name , pSvcLocator )
{
  declareProperty( "Extrapolator", m_extrapolatorName = "TrackMasterExtrapolator" );
  declareProperty( "Chi2Calculator", m_Chi2CalculatorName = "TrackChi2Calculator" );
  declareProperty( "IsLongTrackState"   , m_IsLongTrackState = true               );
  declareProperty( "LongToMuonMatch"   ,  m_LongToMuonMatch  = true               );
  declareProperty( "pCut"       , m_pCut = 0./GeV);
  declareProperty( "IsCosmics"       , m_IsCosmics = false);
}

MuonAlignmentMonitor::~MuonAlignmentMonitor() {} 

StatusCode MuonAlignmentMonitor::initialize() {

  const StatusCode sc = GaudiHistoAlg::initialize();

  GaudiAlg::HistoID  name;

  m_muonDet = getDet<DeMuonDetector>(DeMuonLocation::Default); 

  double ulow = m_IsCosmics ? -1.5 : -0.7;
  double uhigh = -ulow;

  name = "p";
  m_h_p = book1D( name, name, 0, 100, 100);

  name = "x_vs_y";
  m_h_xy = book2D( name, name, -4000, 4000, 80, -4000, 4000, 80);
  
  name = "tx_vs_ty";
  m_h_txty = book2D( name, name, ulow, uhigh, 100, ulow, uhigh, 100);

  if(m_LongToMuonMatch) {

    //chi2 match
    name = "chi2_match";
    m_h_chi2 = book1D( name, "chi2 match", -3, 50, 53);

    // x coord
    name = "prof_resx_x";
    m_p_resxx = bookProfile1D( name, "profile res. x vs x", -4000, 4000, 80);
    
    name = "prof_resx_y";
    m_p_resxy = bookProfile1D( name, "profile res. x vs y", -4000, 4000, 80);
    
    name = "prof_resx_tx";
    m_p_resxtx = bookProfile1D( name, "profile res. x vs tx", ulow, uhigh, 60);
    
    name = "prof_resx_ty";
    m_p_resxty = bookProfile1D( name, "profile res. x vs ty", ulow, uhigh, 60);
    
    name = "prof_restx_x";
    m_p_restxx = bookProfile1D( name, "profile res. tx vs x", -4000, 4000, 80);

    name = "prof_restx_y";
    m_p_restxy = bookProfile1D( name, "profile res. tx vs y", -4000, 4000, 80);

    name = "prof_restx_tx";
    m_p_restxtx = bookProfile1D( name, "profile res. tx vs tx", ulow, uhigh, 60);

    name = "prof_restx_ty";
    m_p_restxty = bookProfile1D( name, "profile res. tx vs ty", ulow, uhigh, 60);

    // y coord

    name = "prof_resy_x";
    m_p_resyx = bookProfile1D( name, "profile res. y vs x", -4000, 4000, 80);

    name = "prof_resy_y";
    m_p_resyy = bookProfile1D( name, "profile res. y vs y", -4000, 4000, 80);

    name = "prof_resy_tx";
    m_p_resytx = bookProfile1D( name, "profile res. y vs tx", ulow, uhigh, 60);
    
    name = "prof_resy_ty";
    m_p_resyty = bookProfile1D( name, "profile res. y vs ty", ulow, uhigh, 60);
    
    name = "prof_resty_x";
    m_p_restyx = bookProfile1D( name, "profile res. ty vs x", -4000, 4000, 80);
  
    name = "prof_resty_y";
    m_p_restyy = bookProfile1D( name, "profile res. ty vs y", -4000, 4000, 80);
    
    name = "prof_resty_tx";
    m_p_restytx = bookProfile1D( name, "profile res. ty vs tx", ulow, uhigh, 60);

    name = "prof_resty_ty";
    m_p_restyty = bookProfile1D( name, "profile res. ty vs ty", ulow, uhigh, 60);
  }
  
  for(int i = 0; i < nSTATION; i++){
    name = "residx_aSide_station_" + boost::lexical_cast<std::string>(i);
    m_h_resx_a[i] = book1D( name, name, -500, 500, 100 );
    name = "residy_aSide_station_" + boost::lexical_cast<std::string>(i);
    m_h_resy_a[i] = book1D( name, name, -500, 500, 100 );
    name = "residx_cSide_station_" + boost::lexical_cast<std::string>(i);
    m_h_resx_c[i] = book1D( name, name, -500, 500, 100 );
    name = "residy_cSide_station_" + boost::lexical_cast<std::string>(i);
    m_h_resy_c[i] = book1D( name, name, -500, 500, 100 );
  }

  m_extrapolator      = tool<ITrackExtrapolator>( m_extrapolatorName, "Extrapolator" ,this );
  m_chi2Calculator    = tool<ITrackChi2Calculator>( m_Chi2CalculatorName, "Chi2Calculator", this );
  m_muonDet           = getDet<DeMuonDetector>(DeMuonLocation::Default);  //  /dd/Structure/LHCb/DownstreamRegion/Muon

  if ( sc.isFailure() ) { return sc; }

  return StatusCode::SUCCESS;
}

StatusCode MuonAlignmentMonitor::execute() {

  // loop on MuonPIDs
  LHCb::MuonPIDs* pMuids = get<LHCb::MuonPIDs>(LHCb::MuonPIDLocation::Default);
  if ( !pMuids ){
    error() << " Can't find MuonPID objects, has the MuonID been called before me?" << endmsg;
    return StatusCode::FAILURE;
  }
  
  LHCb::MuonPIDs::iterator ip;
  for ( ip = pMuids->begin(); ip != pMuids->end(); ip++) {
    
    const LHCb::Track *muTrack = (*ip)->muonTrack();
    const LHCb::Track *longTrack = (*ip)->idTrack();
    
    if( longTrack->p()/GeV > m_pCut ) {
      
      if(muTrack) {
        double zM1 = 12100;
        LHCb::State muState = muTrack->closestState( zM1 ); 
        LHCb::State longState = longTrack->closestState( muState.z() ); 

        LHCb::ParticleID pid(13);
        StatusCode sc = m_extrapolator->propagate(longState,zM1,pid);
        //      if ( sc.isFailure() ) 
        //      debug() << "Extrapolating longState to z = " << zM1 << " failed " << endmsg;
        sc = m_extrapolator->propagate(muState,zM1,pid);
        //      if ( sc.isFailure() ) 
        //      debug() << "Extrapolating muState to z = " << zM1 << " failed " << endmsg;
        
        double chi2;
        
        m_h_p->fill( longState.p()/GeV );
        m_h_xy->fill( longState.x(), longState.y() );
        m_h_txty->fill( longState.tx(), longState.ty() );
        
        sc = m_chi2Calculator->calculateChi2( longState.stateVector(), longState.covariance(), 
                                              muState.stateVector(), muState.covariance(),
                                              chi2 );
        if ( !sc.isSuccess() ) Error( "Could not invert matrices", StatusCode::FAILURE );
        
        double resx = longState.x() - muState.x();
        double resy = longState.y() - muState.y();
        double restx = longState.tx() - muState.tx();
        double resty = longState.ty() - muState.ty();
        double x = muState.x();
        double y = muState.y();
        double tx = muState.tx();
        double ty = muState.ty();
        double dummy,z;
        
        if(m_LongToMuonMatch) {
          
          m_p_resxx->fill( x, resx ); 
          m_p_resxy->fill( y, resx );
          
          m_p_resxtx->fill( tx, resx ); 
          m_p_resxty->fill( ty, resx );
          
          m_p_restxx->fill( x, restx ); 
          m_p_restxy->fill( y, restx );
          
          m_p_restxtx->fill( tx, restx ); 
          m_p_restxty->fill( ty, restx );
          
          m_p_resyx->fill( x, resy ); 
          m_p_resyy->fill( y, resy );
          
          m_p_resytx->fill( tx, resy ); 
          m_p_resyty->fill( ty, resy );
          
          m_p_restyx->fill( x, resty ); 
          m_p_restyy->fill( y, resty );
          
          m_p_restytx->fill( tx, resty ); 
          m_p_restyty->fill( ty, resty );
          
          m_h_chi2->fill(chi2);
        }
        
        std::vector<LHCb::LHCbID>  list_of_tile = (muTrack)->lhcbIDs();
        std::vector<LHCb::LHCbID>::const_iterator itTile;
        
        for (itTile = list_of_tile.begin(); itTile != list_of_tile.end(); itTile++){
          if ((*itTile).isMuon() == true) {
            
            LHCb::MuonTileID tile = itTile->muonID();
            std::vector<DeMuonChamber*> vchambers;
            m_muonDet->Tile2XYZ( tile, x, dummy, y, dummy, z, dummy );
            vchambers = m_muonDet->Tile2Chamber( tile );
            
            debug() << "*** tile position ***" << tile << endreq;
            debug() << " x = " << x << " y = " << y << " z = " << z << endreq;
            debug() << " region " << tile.region() <<" station " << tile.station() << endreq;
            debug() << "*********************" << tile << endreq;
            
            sc = m_extrapolator->propagate( longState, z, pid );
            
            AIDA::IHistogram1D *tempx, *tempy;
            
            tempx = x > 0 ? m_h_resx_a[ tile.station() ] : m_h_resx_c[ tile.station() ];
            tempy = x > 0 ? m_h_resy_a[ tile.station() ] : m_h_resy_c[ tile.station() ];	  	  
            
            LHCb::State fitState = m_IsLongTrackState ? longState : muState;
            
            tempx->fill( x - fitState.x() );
            tempy->fill( y - fitState.y() );
          }
        }
      }
    }
  }
  
  setFilterPassed(true);  
  return StatusCode::SUCCESS;
}


StatusCode MuonAlignmentMonitor::finalize() {

  return   GaudiHistoAlg::finalize();
}


