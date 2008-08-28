// $Id: Calo2Dview.cpp,v 1.11 2008-08-28 09:10:57 odescham Exp $
// Include files 

// from Gaudi
#include "GaudiKernel/AlgFactory.h" 
#include "AIDA/IHistogram2D.h"
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
    m_threshold(-256.),
    m_offset(0.0),
    m_dim(true),
    m_l0(false),
    m_pin(false)
{
  declareProperty( "Threshold"     ,  m_threshold  );
  declareProperty( "PinView"       ,  m_pin   );
  declareProperty( "Offset"        ,  m_offset );
  declareProperty( "ActualSize"    ,  m_dim );
  declareProperty( "L0ClusterView" ,  m_l0  );


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


  // Calo parameters :

  // Ref Cell should be valid for all calo !! To be improved with the next CaloDet version (XtoYratio)
  // Hcal
  const LHCb::CaloCellID refHcal(3, 0, 8 ,4);
  m_caloMap[3]=getDet<DeCalorimeter>( DeCalorimeterLocation::Hcal );
  m_centreMap[3] = 16;
  m_regMap[3]    = 2;
  m_xsizeMap[3]   = m_caloMap[3]->cellSize( refHcal ) ;
  m_ysizeMap[3]  = m_xsizeMap[3] ;

  // Ecal
  const LHCb::CaloCellID refEcal(2, 0, 8 ,4);
  m_caloMap[2]=getDet<DeCalorimeter>( DeCalorimeterLocation::Ecal );
  m_centreMap[2] = 32;
  m_regMap[2]    = 6;
  m_xsizeMap[2]   = m_caloMap[2]->cellSize( refEcal ) ;
  m_ysizeMap[2]  = m_xsizeMap[2] ;
  // Prs
  const LHCb::CaloCellID refPrs(1, 0, 8 ,4);
  m_caloMap[1]=getDet<DeCalorimeter>( DeCalorimeterLocation::Prs );
  m_centreMap[1] = 32;
  m_regMap[1]    = 6;
  m_xsizeMap[1]   = m_caloMap[1]->cellSize( refPrs ) ;
  m_ysizeMap[1]  = m_xsizeMap[1] ;
  // Spd
  const LHCb::CaloCellID refSpd(0, 0, 8 ,4);
  m_caloMap[0]=getDet<DeCalorimeter>( DeCalorimeterLocation::Spd );
  m_centreMap[0] = 32;
  m_regMap[0]    = 6;
  m_xsizeMap[0]   = m_caloMap[0]->cellSize( refSpd ) ;
  m_ysizeMap[0]  = m_xsizeMap[0] ;

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
  m_xsize=m_xsizeMap[calo];
  m_ysize=m_ysizeMap[calo];
  m_caloType = calo;
}

  
AIDA::IHistogram2D* Calo2Dview::bookCalo2D(const HistoID unit,const std::string title,  std::string name ){
  int calo =  CaloCellCode::CaloNumFromName(name);
  if(calo < 0){
    error() << "Calo name : " << name << "is unknown " <<endreq;
    return NULL;
  }
  return bookCalo2D(unit, title, calo);

}


//=============================================================================
AIDA::IHistogram2D* Calo2Dview::bookCalo2D(const HistoID unit,const std::string title,  unsigned  int calo ){

  getCaloParam(calo);
  caloViewMap[unit]=calo;

  
  double xmin = 0.;
  double xmax = (double) m_centre*2.;
  double ymin = 0.;
  double ymax = (double) m_centre*2.;
  int bin = m_centre*2*m_reg ;
  if(m_dim){
    xmax = m_xsize * xmax / 2.;
    xmin = -xmax;
    ymax = m_ysize * ymax / 2.;
    ymin = -ymax;
  }

  return book2D( unit , title , xmin , xmax, bin , ymin , ymax , bin);  
}

//=============================================================================
void Calo2Dview::resetTitle(const HistoID unit , std::string title){  
  if( histoExists(unit))histo2D(unit)->setTitle(title);

  std::string name = (std::string) unit;
  int index = name.find_last_of("/") +1 ; 
  std::string suff = name.substr( index, name.length()  );
  std::string pref = name.substr( 0,index  );
  std::stringstream lun("");
  lun << pref << "PIN/" <<  suff ;
  if( histoExists((HistoID) lun.str() )){
    std::stringstream tit("");
    tit << title << " (PIN-diode) " ;
    histo2D((HistoID) lun.str() )->setTitle(tit.str());
  }
}
//=============================================================================
void Calo2Dview::reset(const HistoID unit , std::string title){  
  if( histoExists(unit) )histo2D(unit)->reset();
  std::string name = (std::string) unit;
  int index = name.find_last_of("/") +1 ; 
  std::string pref = name.substr( 0,index  );
  std::string suff = name.substr( index, name.length()  );
  std::stringstream lun("");
  lun << pref << "PIN/" <<  suff ;
  if( histoExists((HistoID) lun.str() ) )histo2D((HistoID) lun.str() )->reset();
  if("" != title)resetTitle(unit, title);
}


//=============================================================================
AIDA::IHistogram2D*  Calo2Dview::fillCalo2D(const HistoID unit, LHCb::MCCaloHit mchit, const std::string title){
  return fillCalo2D(unit, mchit.cellID(), mchit.activeE() , title);
}
AIDA::IHistogram2D*  Calo2Dview::fillCalo2D(const HistoID unit, LHCb::MCCaloDigit mcdigit, const std::string title){
  return fillCalo2D(unit, mcdigit.cellID(), mcdigit.activeE() , title);
}
AIDA::IHistogram2D*  Calo2Dview::fillCalo2D(const HistoID unit, LHCb::CaloDigit digit, const std::string title){
  return fillCalo2D(unit, digit.cellID(), digit.e() , title);
}
AIDA::IHistogram2D*  Calo2Dview::fillCalo2D(const HistoID unit, LHCb::CaloAdc adc, const std::string title){
  return fillCalo2D(unit, adc.cellID(), (double) adc.adc() , title);
}
AIDA::IHistogram2D*  Calo2Dview::fillCalo2D(const HistoID unit, LHCb::L0CaloAdc l0adc, const std::string title){
  return fillCalo2D(unit, l0adc.cellID(), (double) l0adc.adc() , title);
}
AIDA::IHistogram2D*  Calo2Dview::fillCalo2D(const HistoID unit, LHCb::L0PrsSpdHit hit, const std::string title){
  return fillCalo2D(unit, hit.cellID(), 1. , title);
}
AIDA::IHistogram2D*  Calo2Dview::fillCalo2D(const HistoID unit, LHCb::L0CaloCandidate l0calo, const std::string title){
  setL0ClusterView(true);
  return fillCalo2D(unit, l0calo.id(), l0calo.et() , title);
}
// The main implementation of fillCalo2D
AIDA::IHistogram2D*  Calo2Dview::fillCalo2D(const HistoID unit, LHCb::CaloCellID id , double value, const std::string title){


  // get calo number
  unsigned int calo= id.calo();

  // book histo if not found
  if( !histoExists( unit ) )bookCalo2D(unit, title, calo);  


  // threshold
  if( value < m_threshold){
    if ( msgLevel(MSG::DEBUG) ){
      debug() << "value = " << value << " < threshold = "<< m_threshold << endreq;
    }
    return histo2D(unit);
  }
  

  // check the cellID is consistent with the calo
  if(caloViewMap[unit] !=  calo){
    info() << "Cannot put the  " 
           << CaloCellCode::CaloNameFromNum(calo) 
           << " CaloCellID " << id << " in the "
           << CaloCellCode::CaloNameFromNum( caloViewMap[unit] ) 
           << " view " << unit << endreq;
    return histo2D(unit);
  }
  // get calo parameters
  getCaloParam(calo);
  
  // filter PIN ADC
  unsigned int area= id.area();
  if( m_calo->pinArea() == area){
    if(m_pin){
      return fillCaloPin2D(unit,  id , value,title);
    } else {
      return histo2D(unit);
    }
  }
  
  unsigned int row = id.row();
  unsigned int col = id.col();



  // L0-cluster mode
  unsigned int icl = 1;
  if(m_l0)icl  = 2;

  for(unsigned int kr = 0 ; kr < icl ; kr++){
    for(unsigned int kc = 0 ; kc < icl ; kc++){
      LHCb::CaloCellID temp(calo, area , row + kr ,  col+ kc);
      if( !m_calo->valid(temp) )continue;
      unsigned int theRow = row+kr;
      unsigned int theCol = col+kc;
      
      // Fill the standard view  for PMT
      unsigned int ibox = m_reg/(area+1);

      for(unsigned int ir = 0 ; ir < ibox ; ir++){
        for(unsigned int ic = 0 ; ic < ibox ; ic++){
          int iic = (theCol-m_centre)*ibox + ic ;
          int iir = (theRow-m_centre)*ibox + ir ;
          if(!m_dim){
            iic += m_reg * m_centre;
            iir += m_reg * m_centre;
            m_xsize  = 1.;
            m_ysize  = 1.;            
          }
          double x = m_xsize *  ((double) iic+0.5 ) /(double) m_reg ;
          double y = m_ysize *  ((double) iir+0.5 ) /(double) m_reg ;
          if ( msgLevel(MSG::VERBOSE) ){
          }
          GaudiHistoAlg::fill( histo2D(unit) ,  x, y , value + m_offset);
        }
      }
    }
  }
  return histo2D(unit);
}

AIDA::IHistogram2D*  Calo2Dview::fillCaloPin2D(const HistoID unit, LHCb::CaloCellID id , double value, const std::string title){

  // filter PIN ADC
  if( m_calo->pinArea() != id.area() )return NULL;

  // set title and unit for PIN
  
  std::string name = (std::string) unit;
  int index = name.find_last_of("/") +1 ; 
  std::string pref = name.substr( 0,index  );
  std::string suff = name.substr( index, name.length()  );
  std::stringstream lun("");
  lun << pref << "PIN/" <<  suff ;


  // book histo if not found 
  if( !histoExists( (HistoID) lun.str() ) ){
    std::stringstream tit("");
    tit << title << " (PIN-diode) " ;
    bookCalo2D( (HistoID) lun.str(), tit.str(), id.calo());
  }

  // threshold
  if( value < m_threshold)return histo2D( (HistoID) lun.str());;

  
  // check the cellID is consistent with the calo
  if(caloViewMap[(HistoID)lun.str()] !=  id.calo() ){
    info() << "Cannot put the  " 
           << CaloCellCode::CaloNameFromNum(id.calo() ) 
           << " CaloCellID " << id << " in the "
           << CaloCellCode::CaloNameFromNum( caloViewMap[(HistoID) lun.str() ] ) 
           << " view " << lun.str() << endreq;
    return histo2D( (HistoID) lun.str());
  }



  // book histo if not found
  // Fill the standard view for PIN-diode 
  std::vector<LHCb::CaloCellID>& cells = m_calo->caloPins()[id].cells();

  for( std::vector<LHCb::CaloCellID>::iterator icel = cells.begin() ; icel != cells.end() ; ++icel){    
 
    unsigned int row = (*icel).row();
    unsigned int col = (*icel).col();
    unsigned int area =(*icel).area();

    unsigned int ibox = m_reg/(area+1);
    for(unsigned int ir = 0 ; ir < ibox ; ir++){
      for(unsigned int ic = 0 ; ic < ibox ; ic++){
        int iic = (col-m_centre)*ibox + ic;
        int iir = (row-m_centre)*ibox + ir;
        if(!m_dim){
          iic += m_reg * m_centre;
          iir += m_reg * m_centre;
          m_xsize  = 1.;
          m_ysize  = 1.;
        }
        double x = (m_xsize * ((double) iic +0.5) )/(double) m_reg ;
        double y = (m_ysize * ((double) iir +0.5) )/(double) m_reg ;
        GaudiHistoAlg::fill( histo2D( (HistoID) lun.str()) , x , y , value + m_offset);
      }
    }
  }
  return histo2D( (HistoID) lun.str());
}

