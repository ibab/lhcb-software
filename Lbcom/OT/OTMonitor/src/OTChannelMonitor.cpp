// Include files 

// from Gaudi
#include "GaudiKernel/SystemOfUnits.h"

// AIDA
#include "AIDA/IHistogram1D.h"
#include "AIDA/IProfile1D.h"
#include "GaudiUtils/Aida2ROOT.h"
#include "TH1.h"

// OTDet
#include "OTDet/DeOTDetector.h"
#include "OTDet/DeOTModule.h"

// local
#include "OTChannelMonitor.h"

#define longModuleLength 2421  // Half module without inefficient region in mm
//-----------------------------------------------------------------------------
// Implementation file for class : OTChannelMonitor
//
// 2012-11-15 : Francesco Dettori
// 
// Algorithm to store histograms for a monitoring of bad channels in the Outer Tracker 
// It stores one histogram for each otis and one histogram for each layer
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
DECLARE_ALGORITHM_FACTORY( OTChannelMonitor )


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
OTChannelMonitor::OTChannelMonitor( const std::string& name,
                                    ISvcLocator* pSvcLocator)
  : GaudiHistoAlg ( name , pSvcLocator ),
  m_otdet(NULL),
  m_numEvents(0),
  m_isOnline(false),
  m_scaleHistos(true),
  m_decoder("OTRawBankDecoder")
{
  declareProperty( "RawBankDecoder",m_decoder ) ;
  declareProperty( "scaleHistos", m_scaleHistos) ; 
  declareProperty( "Online", m_isOnline = false);
}
//=============================================================================
// Destructor
//=============================================================================
OTChannelMonitor::~OTChannelMonitor() {} 



//=============================================================================
// Initialization
//=============================================================================
StatusCode OTChannelMonitor::initialize() {
  StatusCode sc = GaudiHistoAlg::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiHistoAlg
  sc = m_decoder.retrieve() ;
  if(sc.isFailure()) return sc ;

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Initialize" << endmsg;

  m_otdet = getDet<DeOTDetector>(DeOTDetectorLocation::Default );

  // root file directories and base names for the histograms
  std::string otisprefix = std::string("Otis") + "/" ;
  std::string otisname = std::string("");
  
  std::string layerprefix = std::string("Layer") + "/" ;
  std::string layername = std::string("");
  
  // Loope on detector elements
  for(int s=1; s<=3; s++){         // Station
    for(int l=0; l<4; l++){        // Layer
      
      int uniqueLayer = (s-1)*nLayers +l;
      
      for(int q=0; q<4; q++){      // Quadrant
        for(int m=1; m<=9; m++){   // Module 
          for(int o=0; o<4; o++){  // Otis
            otisname = "T"+std::to_string( s )+
              "L"+std::to_string( l )+
              "Q"+std::to_string( q )+
              "M"+std::to_string( m )+
              "Otis"+std::to_string( o );
	    
            int uniqueQuadrant = uniqueLayer*nQuadrants + q;
            int uniqueModule = uniqueQuadrant * nModules + m-1;
            int uniqueOtis = uniqueModule * nOtis +o;  
            if ( msgLevel(MSG::DEBUG) ) 
              debug() << "Initializing Otis : " << otisname 
                      << " which is unique number " << uniqueOtis << endmsg;
            
            m_OtisHist[uniqueOtis] = this->book1D( otisprefix + otisname, 
                                                   "Occupancy per channel ", 0, 32,32);
            m_otisNames[uniqueOtis] = otisname;
            
          }// Otis
        }// Module 
      }// Quadrant

      // Set layer name
      layername = "T"+std::to_string( s )+
        "L"+std::to_string( l );

      // Get layer from detector 
      //      LHCb::OTChannelID  aChan(uniqueLayer/4 +1 , uniqueLayer%4, 1, 0u, 0u, 0u);
      //      const DeOTLayer * layer = m_otdet->findLayer( aChan );

      // Get layer binning from function (this could be optimized)
      std::vector<double> min_modules_x,  min_modules_y;
      getLayerGeometry(uniqueLayer, min_modules_x, min_modules_y); 
      double min = min_modules_x.front(), max = min_modules_x.back();
      if ( msgLevel(MSG::DEBUG) ) debug() << "Layer " << uniqueLayer << " " 
                                          <<layername << "  interval = " << min << " " << max << endmsg;

      const int nbin = (max-min)/5.25; // binning roughly per straw.
      
      // Book layer histogram 
      m_LayerHist[uniqueLayer] = this->book1D(  layerprefix + layername, 
                                                "Occupancy per channel ", 
                                                min, max, nbin);
      m_layerNames[uniqueLayer] = layername;
      
    } // Layer
  }// Station
  
  
  return sc;
}

//=============================================================================
// Main execution
//=============================================================================
StatusCode OTChannelMonitor::execute() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Execute" << endmsg;
  
  // Number of events: used at the end to normalize histograms 
  ++m_numEvents;
  
  // decode all OT hits
  LHCb::OTLiteTimeContainer ottimes;
  StatusCode sc = m_decoder->decode( ottimes ) ;
  if( sc ) {
    // Loop on all the hits in the hit map
    for( LHCb::OTLiteTimeContainer::const_iterator ihit = ottimes.begin() ;
         ihit != ottimes.end(); ++ihit){
      
      // Get otis channel 
      int otisChannel =  ( ihit->channel().straw()-1) % 32;

      // Fill otis histogram 
      fill(m_OtisHist[uniqueOtis(ihit->channel())], otisChannel, 1.);
      
      if ( msgLevel(MSG::DEBUG) )debugChannel(ihit->channel());  // deep debugging of the geometry

      // Get module for the given hit for position information 
      const  DeOTModule * tmpModule = m_otdet->findModule (ihit->channel());
      if(!tmpModule)continue;

      // Each point is weighted according to the size of the module 
      // A correction is implemented which takes into account the variation of the particle flux 
      // with distance from the beam axis 
      double weight = getChannelWeight(tmpModule, ihit->channel());
      
      // Get point 
      Gaudi::XYZPoint mypoint =  tmpModule->centerOfStraw(ihit->channel().straw());
      

      // Get unique layer number 
      int uniqueLayer = ihit->channel().sequentialUniqueLayer(); 
      if ( msgLevel(MSG::DEBUG) ) 
	debug() << " Going to fill layer " << uniqueLayer << " at point " << mypoint.X() << endmsg;

      // Fill layer histogram
      fill(m_LayerHist[uniqueLayer], mypoint.X(),weight);

      // TODO
      // - insert correction for average luminosity
      
    }

      
  }
  
  return sc ;
}

//=============================================================================
//  Finalize
//=============================================================================
StatusCode OTChannelMonitor::finalize() {

  if ( msgLevel(MSG::DEBUG) ) debug() << "==> Finalize" << endmsg;

  // Scale histograms to number of events 
  // only done for Otis histograms
  if(m_scaleHistos){
    for(int otis=0; otis<NumUniqueOtis; otis++){
      scaleHistogram(m_OtisHist[otis]);
    }
  }
  
  
  m_decoder.release().ignore() ;

  return GaudiHistoAlg::finalize();  // must be called after all other actions
}


//=============================================================================
void OTChannelMonitor::debugChannel(const LHCb::OTChannelID& channel){
  // Function to debug completely the geometry of a given OT channel ID 
  // OutputLevel = DEBUG or VERBOSE should be set in order to see this debugging

  int realOtis =  ( channel.straw()-1) / 32; // This should be in OTChannelID but is not 
  int otisChannel =  ( channel.straw()-1) % 32; // And this too

  if ( msgLevel(MSG::DEBUG) ) debug() << "Considering channel " << channel.channelID() << endmsg;
  if ( msgLevel(MSG::DEBUG) ) debug() << "Considering Otis " << m_otisNames[uniqueOtis(channel)] 
				      <<  " sequential number " << uniqueOtis(channel) << endmsg;
  if ( msgLevel(MSG::DEBUG) ) debug()  << " T " << channel.station() 
				       << " L "  << channel.layer() 
				       << " Q " << channel.quarter() 
				       << " M " << channel.module() 
				       << " Otis " << channel.sequentialUniqueOtis() 
				       << " ROtis " << realOtis    
				       << " Straw " << channel.straw() 
				       << " RStraw " << otisChannel 
				       << endmsg;

}

//=============================================================================
void OTChannelMonitor::scaleHistogram(AIDA::IHistogram1D* hist) {
  // Function to scale histograms by the number of analysed events 
  
  if(!hist) Exception( "You have passed a null histogram!");
  
  TH1* h1 = Gaudi::Utils::Aida2ROOT::aida2root(hist);
  if(h1) h1->Scale(1/(float)m_numEvents);
}

//=============================================================================
void OTChannelMonitor::getLayerGeometry(int uniqueLayer, 
					std::vector<double> & min_modules_x,  
					std::vector<double> & min_modules_y){
  // Function to get the geometry of each layer to configure bin sizes 
  // ----------
  if ( msgLevel(MSG::DEBUG) ) debug() << "Getlayergeometry called for layer " << uniqueLayer  <<endmsg; 
  LHCb::OTChannelID  aChan(uniqueLayer/4 +1 , uniqueLayer%4, 1, 0u, 0u, 0u);

  // Probably all this can be optimized
  // Get layer 
  const DeOTLayer * layer = m_otdet->findLayer( aChan );
  if(!layer){
    warning() << " Bad layer provided " << uniqueLayer <<endmsg;
    return; 
  }

  // Get quarters 
  const DeOTDetector::Quarters& quarters = layer->quarters();
  for (DeOTDetector::Quarters::const_iterator iterQ = quarters.begin(); 
       iterQ != quarters.end() ; ++iterQ ){ 

    // Get the modules for this quarter 
    const DeOTDetector::Modules& modules = (*iterQ)->modules();
    double  firstpoint, lastpoint, maxpoint(0);

    // Loop on modules 
    for (DeOTDetector::Modules::const_iterator iterM = modules.begin(); 
	 iterM != modules.end() ; ++iterM ){ 

      // Get width for this module 
      Gaudi::XYZPoint xwidth((((*iterM)->nChannels()/4. ) +0.25)*(*iterM)->xPitch(), 0.0, 0.0);
      
      // Get minimum point of module, center and last point
      const Gaudi::XYZPoint centermodule = (*iterM)->centerOfModule();
      min_modules_x.push_back(layer->geometry()->
                              toGlobal(Gaudi::XYZPoint(centermodule.x()-xwidth.x(), 0.0, 0.0)).x()) ;// min point
      min_modules_x.push_back(layer->geometry()->toGlobal(Gaudi::XYZPoint(centermodule.x(), 0.0, 0.0)).x()) ; // center point 
      firstpoint = layer->geometry()->toGlobal(Gaudi::XYZPoint(centermodule.x()-xwidth.x(), 0.0, 0.0)).x(); 
      lastpoint = layer->geometry()->toGlobal(Gaudi::XYZPoint(centermodule.x()+xwidth.x(), 0.0, 0.0)).x();
      maxpoint = std::max(maxpoint, lastpoint); 
      
      if ( msgLevel(MSG::DEBUG) )debug() << "First and last points [" << firstpoint << " " << lastpoint <<  " ]" <<endmsg;
    }// Modules
    
    min_modules_x.push_back(maxpoint); // put also the max one. 
  }// Quarters

  // Binning in y is done more simply
  const double min_y(-2800), max_y(2800);
  const int nbiny = 100; 
  for(int i=0; i<=nbiny; i++) min_modules_y.push_back(min_y+ (max_y-min_y)*(double)i/(double)nbiny);
  std::sort(min_modules_x.begin(), min_modules_x.end());
  std::sort(min_modules_y.begin(), min_modules_y.end());
  
  if ( msgLevel(MSG::DEBUG) ){
    for(double num: min_modules_x) {
      verbose() << "get layer geometry  value " << num << endmsg; 
    }
  }
  
 
}
//=============================================================================
double OTChannelMonitor::getChannelWeight(const DeOTModule *module, LHCb::OTChannelID aChan){
  // Function to get a weight which corrects for different modules.
  // Each module is weighted according to the size and distance to the beam axis.
  // A gaussian approximation is used for this goal.
  // Only the y direction is weighted, we want to keep the x dependence.
  // All this is correct for X layer's modules and only approximate for U and V

  if(!module) Exception( "You have passed a null module!");
  
  const double ymax = 2800;    // Maximum lenght of a module from the beam axis

  // Standard deviation of the gaussian of particles from the beam axis
  const double sigma = 1000; 

  double weight = 1;
  if(!(module->longModule())){ // long modules are weighted one by definition 
    // Coordinate of min in y (approximate for U-V layers) 
    // of the given module 
    double ymin = ymax  - module->wireLength(aChan);

    // Coordinate of the maximum possible lenght module 
    double ymin_long = ymax - longModuleLength;

    // Weight calculated as the integral of the gaussian divided by the maximum possible weight
    weight =  (erf(ymax/sigma) - erf(ymin_long/sigma))/
      ((erf(ymax/sigma) - erf(ymin/sigma)));
    if ( msgLevel(MSG::DEBUG) ) debug() << "Module min is " << ymin 
                                        << " instead of "<< ymin_long << "   correction is " << weight << endmsg;
    
  }
  return weight;
}
//=============================================================================

