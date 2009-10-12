// $Id: Calo2Dview.cpp,v 1.18 2009-10-12 16:04:35 odescham Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 
#include "AIDA/IHistogram2D.h"
#include "GaudiUtils/Aida2ROOT.h"
// from LHCb
#include "Kernel/CaloCellCode.h"
// local
#include "CaloUtils/Calo2Dview.h"

//-----------------------------------------------------------------------------
// Implementation file for class : Calo2Dview
//
// 2008-01-17 : Olivier Deschamps
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
//DECLARE_ALGORITHM_FACTORY( Calo2Dview );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
Calo2Dview::Calo2Dview( const std::string& name,
                        ISvcLocator* pSvcLocator)
  : GaudiHistoAlg ( name , pSvcLocator ),
    m_calo(),
    m_caloType(-1),
    m_reg(),
    m_centre(),
    m_xsize(),
    m_ysize(),
    m_refCell(),
    m_threshold(-256.),
    m_offset(0.0),
    m_dim(true),
    m_l0(false),
    m_pin(false),
    m_energyWeighted(true),
    m_flux(false)
{
  declareProperty( "OneDimension"   ,  m_1d  = false);         // 1D histo (value versus CaloCellID) default is 2D view
  declareProperty( "Profile"        ,  m_profile  = false);         // 1D histo profile type ?
  declareProperty( "Bin1D"          ,  m_bin1d  = 16384);      // 1D histo binning (default : full 14 bits dynamics)
  declareProperty( "GeometricalView",  m_geo = true);          // 2D :  true : geometrical (x,y) | false : readout (FEB,channel)
  declareProperty( "Threshold"      ,  m_threshold  );         // threshold on the input value (bin weight)
  declareProperty( "PinView"        ,  m_pin   );              // produce a separate view for PIN channels (geometrical view only)
  declareProperty( "Offset"         ,  m_offset );             // bin weight =  (value + offset)
  declareProperty( "ActualSize"     ,  m_dim );                // true: (x,y) | false : (col,row) (geometrical view only)
  declareProperty( "L0ClusterView"  ,  m_l0  );                // bin weight on 2x2 cluster (geometrical view only)
  declareProperty( "EnergyWeighted" ,  m_energyWeighted  );    // true : bin weight = energy | false :  1
  declareProperty( "Flux"           ,  m_flux  );              // bin weight is normalized to the cell area (for both views)
  declareProperty( "SplitAreas"     ,  m_split = false  );     // produce one view per area (for geometrical view only)
  declareProperty( "xLabelOptions"  , m_lab ="v" );            // Crate-FEB text label on Xaxis (readout view only)
  declareProperty( "ProfileError"   ,  m_prof = "");
  setHistoDir( name );
}
//=============================================================================
// Destructor
//=============================================================================
Calo2Dview::~Calo2Dview() {} 



//=============================================================================
StatusCode Calo2Dview::initialize() {
  StatusCode sc = GaudiHistoAlg::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  debug() << "==> Initialize" << endmsg;

  //protection against splitting when non-goemetrical view or 1D is requested.
  if( !m_geo)m_split =false;
  if (m_1d) { m_split = false; }


  // Calo parameters :
  // Hcal
  m_caloMap[3]=getDet<DeCalorimeter>( DeCalorimeterLocation::Hcal );
  // geometrical view
  m_centreMap[3] = 16;
  m_regMap[3]    = 2;
  m_refCellMap[3].push_back(LHCb::CaloCellID(3, 0, 3 ,0)) ; // Outer corner cell
  m_refCellMap[3].push_back(LHCb::CaloCellID(3, 1, 2 ,0)) ; // Middle corner cell
  // readout view
  m_fCardMap[3] = 352;
  m_lCardMap[3] = 415;
  m_nChanMap[3] = 32;
  // Ecal
  m_caloMap[2]=getDet<DeCalorimeter>( DeCalorimeterLocation::Ecal );
  m_centreMap[2] = 32;
  m_regMap[2]    = 6;
  m_refCellMap[2].push_back(LHCb::CaloCellID(2, 0, 6 ,0) ) ; // outer
  m_refCellMap[2].push_back(LHCb::CaloCellID(2, 1, 12 ,0)) ; //middle
  m_refCellMap[2].push_back(LHCb::CaloCellID(2, 2, 14  ,8)); // inner
  m_fCardMap[2] = 128;
  m_lCardMap[2] = 351;
  m_nChanMap[2] = 32;
  // Prs
  m_caloMap[1]=getDet<DeCalorimeter>( DeCalorimeterLocation::Prs );
  m_centreMap[1] = 32;
  m_regMap[1]    = 6;
  m_refCellMap[1].push_back(LHCb::CaloCellID(1, 0, 6 ,0) ) ; // outer
  m_refCellMap[1].push_back(LHCb::CaloCellID(1, 1, 12 ,0) ); //middle
  m_refCellMap[1].push_back(LHCb::CaloCellID(1, 2, 14  ,8)); // inner
  m_fCardMap[1] = 0;
  m_lCardMap[1] = 127;
  m_nChanMap[1] = 64;
  // Spd
  m_caloMap[0]=getDet<DeCalorimeter>( DeCalorimeterLocation::Spd );
  m_centreMap[0] = 32;
  m_regMap[0]    = 6;
  m_refCellMap[0].push_back(LHCb::CaloCellID(0, 0, 6 ,0)  ); // outer
  m_refCellMap[0].push_back(LHCb::CaloCellID(0, 1, 12 ,0) ); // middle
  m_refCellMap[0].push_back(LHCb::CaloCellID(0, 2, 14  ,8)); // inner
  m_fCardMap[0] = 0;
  m_lCardMap[0] = 127;
  m_nChanMap[0] = 64;
  return StatusCode::SUCCESS;
}



//=============================================================================
//  Finalize
//=============================================================================
StatusCode Calo2Dview::finalize() {

  debug() << "==> Finalize" << endmsg;

  return GaudiHistoAlg::finalize();  // must be called after all other actions
}

//=============================================================================
void Calo2Dview::getCaloParam(unsigned int calo){

  if((int)calo == m_caloType)return;
  m_calo= m_caloMap[calo];
  m_centre=m_centreMap[calo];
  m_reg=m_regMap[calo];
  m_refCell = m_refCellMap[calo];

  int nArea = (6+m_reg)/4;
  m_xsize.clear();
  m_ysize.clear();
  for (int i = 0 ; i < nArea; ++i){
    m_xsize.push_back( m_calo->cellSize( m_refCell[i] ) );
    m_ysize.push_back( m_xsize[i] )  ;// * m_calo->YtoXratio()
  }
  m_caloType = calo;
  m_fCard = m_fCardMap[calo];
  m_lCard = m_lCardMap[calo];
  m_nChan = m_nChanMap[calo];
}

  
void Calo2Dview::bookCalo2D(const HistoID unit,const std::string title,  std::string name , int area){
  int calo =  CaloCellCode::CaloNumFromName(name);
  if(calo < 0){
    error() << "Calo name : " << name << "is unknown " <<endreq;
    return;
  }
  bookCalo2D(unit, title, calo, area);
  return; 
}


//=============================================================================
void Calo2Dview::bookCalo2D(const HistoID unit,const std::string title,  unsigned  int calo , int area){

  getCaloParam(calo);
  caloViewMap[unit]=calo;

  // 1D view : caloCellID as xAxis
  if( m_1d ){
    if(m_profile) 
      GaudiHistoAlg::bookProfile1D(unit, title , 0.,(double) m_bin1d , m_bin1d, m_prof);
    else
      GaudiHistoAlg::book1D(unit, title , 0.,(double) m_bin1d , m_bin1d);
    return ;
  }

  // electronics 2D view : FEB vs Channel (include PIN-diode channels)
  if( !m_geo){
    AIDA::IHistogram2D* h2d =
      GaudiHistoAlg::book2D( unit, title, (double) m_fCard, (double) m_lCard, m_lCard-m_fCard, 0. , (double) m_nChan , m_nChan);
    // set Xaxis bin labels
    TH2D* th2d = Gaudi::Utils::Aida2ROOT::aida2root( h2d );
    for( int bin =1 ; bin <=  m_lCard-m_fCard ; ++bin){
      if( (bin-1)%8 != 0 )continue;
      int code = m_fCard + bin -1;
      int crate =  code /16;
      int feb   =  code - crate * 16;
      std::stringstream loc("");
      loc <<"c" << format("%02i",crate) << "f" << format("%02i",feb) ;
      th2d->GetXaxis()->SetBinLabel( bin  , loc.str().c_str() );
    } 
    th2d->GetXaxis()->LabelsOption( m_lab.c_str() );  
    return ;
  }  

  // 2D geometrical view with small bining to simulate variable bin size (separated PMT/PIN histo)
  AIDA::IHistogram2D* h = NULL;
  int fArea = 0;
  int lArea = m_split ? (6+m_reg)/4 : 1;
  if(m_split && area >= 0 ){
    fArea = area;
    lArea = area+1;
  }
  int ppcm  = m_split ? 1 : m_reg ;
  for(int i = fArea ; i < lArea ; ++i){
    double xmin = m_refCell[i].col() ;
    double xmax = (double) m_centre * 2. -xmin;
    double ymin = m_refCell[i].row() ;
    double ymax = (double) m_centre * 2  -ymin;
    int xbin = (m_centre-m_refCell[i].col() )*2 * ppcm; 
    int ybin = (m_centre-m_refCell[i].row() )*2 * ppcm;
    if(m_dim){
      xmax = m_xsize[i] * (xmax-xmin) / 2.;
      xmin = -xmax;
      ymax = m_ysize[i] * (ymax-ymin) / 2.;
      ymin = -ymax;
    }
    const HistoID& lun = getUnit(unit, calo , i) ;
    h=GaudiHistoAlg::book2D( lun,
                             getTitle(title, calo, i),
                             xmin , xmax, xbin , ymin , ymax , ybin);  

    //info() << "histo bounds" <<  xmin<< " " <<  xmax << " " << ymin << " " << ymax << endmsg;
    //info() << "cell size " << m_xsize[i] << " " << m_ysize[i] << endmsg;
  } 
  return ; // return outer view
}

int Calo2Dview::centre(int x , int area){
  int theArea = m_split ? area : 0;
  return (x==0) ?   m_centre - m_refCell[theArea].col() : m_centre - m_refCell[theArea].row();
}
//--------------------------------------------------
const GaudiAlg::HistoID Calo2Dview::getUnit(std::string unit, int calo, int area)const{
  if(!m_split)return unit;
  const std::string& nArea = CaloCellCode::caloArea ( calo , area);
  int index = unit.find_last_of("/")+1 ;
  unit.insert(index, nArea+"/");
  return  (HistoID) unit;
}

std::string Calo2Dview::getTitle(std::string title, int calo, int area){
  if(!m_split)return title;
  const std::string& nArea = CaloCellCode::caloArea ( calo , area);
  std::string tit =  title + " (" +nArea + " area)";
  return  tit ;
}


//=============================================================================
void Calo2Dview::resetTitle(const HistoID unit , std::string title){  

  std::map<HistoID,unsigned int>::iterator it = caloViewMap.find( unit );
  if( it == caloViewMap.end())return;
  int calo = (*it).second;
  int lArea = 1;
  if(m_split){
    lArea = (6+m_regMap[calo])/4;
  }
  for(int i=0;i<lArea;++i){
    HistoID  lun = (HistoID) getUnit( unit, calo , i);
    std::string tit = getTitle( title,calo, i);
    if( histoExists(lun))histo2D(lun)->setTitle(tit);
    std::string name = (std::string) unit;
    int index = name.find_last_of("/") +1 ; 
    std::string suff = name.substr( index, name.length()  );
    std::string pref = name.substr( 0,index  );
    lun =  pref + "PIN/" +  suff ;
    const HistoID& lun2 = getUnit( lun , calo, i );
    if( histoExists((HistoID) lun2 )){
      std::string tit = title + " PIN-diode " ;
      std::string tit2 = getTitle(title,calo,i);
      histo2D( lun2 )->setTitle(tit2);
    }
  }  
}
//=============================================================================
void Calo2Dview::reset(const HistoID unit , std::string title){
  std::map<HistoID, unsigned int>::iterator it = caloViewMap.find( unit );
  if( it == caloViewMap.end())return;
  int calo = (*it).second;
  int lArea = 1;
  if(m_split){
    lArea = (6+m_regMap[calo])/4;
  }
  for(int i=0;i<lArea;++i){
    HistoID lun = (HistoID) getUnit( unit, calo , i);
    if( histoExists(lun) ){histo2D(lun)->reset();}
    std::string name = (std::string) unit;
    int index = name.find_last_of("/") +1 ; 
    std::string pref = name.substr( 0,index  );
    std::string suff = name.substr( index, name.length()  );
    lun  = pref + "PIN/" + suff ;
    const HistoID& lun2 = getUnit( lun , calo, i );
    if( histoExists( lun2 ) )histo2D((HistoID) lun2 )->reset();
  } 
  if("" != title)resetTitle(unit, title);
}


//=============================================================================
void  Calo2Dview::fillCalo2D(const HistoID unit, LHCb::MCCaloHit mchit, const std::string title){
  double weight = m_energyWeighted ? mchit.activeE() : 1.;
  fillCalo2D(unit, mchit.cellID(), weight , title);
}
void  Calo2Dview::fillCalo2D(const HistoID unit, LHCb::MCCaloDigit mcdigit, const std::string title){
  double weight = m_energyWeighted ? mcdigit.activeE() : 1.;
  fillCalo2D(unit, mcdigit.cellID(), weight , title);
}
void  Calo2Dview::fillCalo2D(const HistoID unit, LHCb::CaloDigit digit, const std::string title){
  double weight = m_energyWeighted ? digit.e() : 1.;
  fillCalo2D(unit, digit.cellID(), weight  , title);
}
void  Calo2Dview::fillCalo2D(const HistoID unit, LHCb::CaloAdc adc, const std::string title){
  double weight = m_energyWeighted ? (double) adc.adc() : 1.;
  fillCalo2D(unit, adc.cellID(), weight , title);
}
void  Calo2Dview::fillCalo2D(const HistoID unit, LHCb::L0CaloAdc l0adc, const std::string title){
  double weight = m_energyWeighted ? (double) l0adc.adc() : 1.;
  fillCalo2D(unit, l0adc.cellID(), weight , title);
}
void  Calo2Dview::fillCalo2D(const HistoID unit, LHCb::L0PrsSpdHit hit, const std::string title){
  fillCalo2D(unit, hit.cellID(), 1. , title);
}
void  Calo2Dview::fillCalo2D(const HistoID unit, LHCb::L0CaloCandidate l0calo, const std::string title){
  setL0ClusterView(true);
  double weight = m_energyWeighted ? l0calo.et() : 1.;
  fillCalo2D(unit, l0calo.id(), weight , title);
}
void  Calo2Dview::fillCalo2D(const HistoID unit, LHCb::CaloCluster cluster, const std::string title){
  setL0ClusterView(true);
  double weight = m_energyWeighted ? cluster.e() : 1.;
  fillCalo2D(unit, cluster.seed(), weight , title);
}

// The main implementation of fillCalo2D
void  Calo2Dview::fillCalo2D(const HistoID unit, LHCb::CaloCellID id , double value, const std::string title){

  // threshold
  if( value < m_threshold)return;
  // separate treatment for PIN-diode channels for 2D geometrical view
  if( m_geo && id.isPin() ){
    if( m_pin ) fillCaloPin2D(unit,  id , value,title) ;
  }
  
  // get calo number
  unsigned int calo= id.calo();
  unsigned int area= id.area();
  // book histo if not found

  const HistoID& lun = getUnit(unit, calo, area );  
  if( !histoExists( lun ) )bookCalo2D(unit, title, calo, area);

  // get calo parameters
  getCaloParam(calo);


  // check the cellID is consistent with the calo
  
  if(caloViewMap[ unit ] !=  calo){
    debug() << "Cannot put the  CaloCellID " << id << " in the "
            << CaloCellCode::CaloNameFromNum( caloViewMap[unit] ) 
            << " view '" << unit << "'" << endreq;
    return; 
  } 
  
  // -------------- 1D view
  if( m_1d ){
    if(m_profile)
      fill(profile1D(lun), (double) id.index() , value , 1.);
    else
      fill(histo1D(lun), (double) id.index() , value);
    return;
  }
  // -------------- Electronics 2D view (crate/feb .vs. channel)
  if( !m_geo ){
    int feb   = m_calo->cardNumber( id );
    int code  = m_calo->cardCode( feb );
    int chan  = m_calo->cardColumn( id) + nColCaloCard * m_calo->cardRow( id );
    double weight = (value + m_offset);
    if( m_flux )weight /=   (m_xsize[id.area()] * m_ysize[id.area()]);
    GaudiHistoAlg::fill( histo2D( lun ) ,  (double) code , (double) chan , weight);
    return;
  }  

  // -------------- 2D geometrical view with small bining to simulate variable cell sizes
  IHistogram2D* h2D = histo2D(lun);  
  double xs = m_xsize[area];
  double ys = m_xsize[area];  
  double xs0 = m_xsize[0];
  double ys0 = m_xsize[0];  
  double xResize = m_split ? xs : xs0;
  double yResize = m_split ? ys : ys0;
  unsigned int ibox = m_split ? 1 : m_reg/(area+1);
  int ppcm  = m_split ? 1 : m_reg ;

  // PMT channels
  unsigned int row = id.row();
  unsigned int col = id.col();

  // L0-cluster mode (2x2 cells area are filled)
  unsigned int icl = 1;
  if(m_l0)icl  = 2;
  // loop over cluster area (1x1 or 2x2 cells depending on L0 mode)
  for(unsigned int kr = 0 ; kr < icl ; kr++){
    for(unsigned int kc = 0 ; kc < icl ; kc++){
      LHCb::CaloCellID temp = id;
      if( m_l0 ){
        temp = LHCb::CaloCellID(calo, area , row + kr ,  col+ kc);
        if( !m_calo->valid(temp) )continue;
      }      
      unsigned int theRow = row+kr;
      unsigned int theCol = col+kc;
      // Fill the standard view  for PMT
      // loop over cell area ( ibox * ibox bins depending on Calo/Area)
      for(unsigned int ir = 0 ; ir < ibox ; ir++){
        for(unsigned int ic = 0 ; ic < ibox ; ic++){
          int iic = (theCol-m_centre )*ibox + ic ;
          int iir = (theRow-m_centre )*ibox + ir ;
          if(!m_dim){
            iic += ppcm * m_centre;
            iir += ppcm * m_centre;
            xResize = 1.;
            yResize = 1.;            
          }
          double x = xResize *  ((double) iic+0.5 ) /(double) ppcm ;
          double y = yResize *  ((double) iir+0.5 ) /(double) ppcm ;
          double weight = (value + m_offset);
          if( m_flux )weight /=   ( xs * ys );
          GaudiHistoAlg::fill( h2D ,  x, y , weight);
        }
      }
    }
  }
  return;
}

void  Calo2Dview::fillCaloPin2D(const HistoID unit, LHCb::CaloCellID id , double value, const std::string title){

  // filter PIN ADC (sanity check)
  if( !id.isPin() )return;

  // threshold
  if( value < m_threshold)return; 

  // re-set title and unit for PIN  
  std::string name = (std::string) unit;
  int index = name.find_last_of("/") +1 ; 
  std::string pref = name.substr( 0,index  );
  std::string suff = name.substr( index, name.length()  );
  std::string lun =  pref + "PIN/" +  suff ;

  // Fill the standard view for PIN-diode 
  // -> fill the PMT area covered by the PIN-diode with the PIN ADC value
  HistoID lun2;
  const std::vector<LHCb::CaloCellID>& cells =  m_calo->pinChannels( id );
  for( std::vector<LHCb::CaloCellID>::const_iterator icel = cells.begin() ; icel != cells.end() ; ++icel){
    unsigned int row = (*icel).row();
    unsigned int col = (*icel).col();
    unsigned int area =(*icel).area();
    // split area naming :
    lun2 = getUnit(lun, id.calo(), area);
    // book histo if not found 
    if( !histoExists(  lun2  )){
      std::string tit = title + " PIN-diode " ;
      bookCalo2D(  (HistoID)  lun , getTitle(tit, id.calo(), area ), id.calo(), area);
    }  
    // check the cellID is consistent with the calo
    if(caloViewMap[(HistoID) lun ] !=  id.calo() ){
      debug() << "Cannot put the  CaloCellID " << id << " in the "
              << CaloCellCode::CaloNameFromNum( caloViewMap[(HistoID) lun ] ) 
              << " view " << unit << endreq;
      return;
    }
    unsigned int ibox = m_split ? 1 : m_reg/(area+1); 
    int ppcm  = m_split ? 1 : m_reg ;
    for(unsigned int ir = 0 ; ir < ibox ; ir++){
      for(unsigned int ic = 0 ; ic < ibox ; ic++){
        int iic = (col-m_centre )*ibox + ic;
        int iir = (row-m_centre )*ibox + ir;
        double xResize = m_split ? m_xsize[area] : m_xsize[0];
        double yResize = m_split ? m_ysize[area] : m_ysize[0];
        if(!m_dim){
          iic += ppcm * m_centre;
          iir += ppcm * m_centre;
          xResize = 1.;
          yResize = 1.;
        }
        double x = (xResize * ((double) iic +0.5) )/(double) ppcm ;
        double y = (yResize * ((double) iir +0.5) )/(double) ppcm ;
        double weight = (value + m_offset);
        if( m_flux )weight /=   (m_xsize[area] * m_ysize[area]);
        GaudiHistoAlg::fill( histo2D(  lun2 ), x , y , weight );
      }
    }
  }
  //  return histo2D( (HistoID) lun2);
  return;
}

