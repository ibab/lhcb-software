// Include files
// local
#include "MuonDAQ/SortDigitInODE.h"
#include "MuonDAQ/SortDigitInL1.h"
#include "MuonDigitToRawBuffer.h"
#include "MuonDAQ/MuonHLTDigitFormat.h"
#include "MuonDet/MuonTSMap.h"
#include "MuonDet/MuonL1Board.h"
#include "MuonDet/MuonODEBoard.h"
#include "MuonDet/MuonStationCabling.h"
#include "MuonDet/MuonBasicGeometry.h"
#include "Event/RawEvent.h"
#include "Event/RawBank.h"
#include "Event/BankWriter.h"
#include "Event/MuonDigit.h"
#include "Event/MuonBankVersion.h"
//-----------------------------------------------------------------------------
// Implementation file for class : MuonDigitToRawBuffer
//
// 2004-01-16 : Alessia Satta
//-----------------------------------------------------------------------------

DECLARE_ALGORITHM_FACTORY( MuonDigitToRawBuffer )

using namespace LHCb;

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
MuonDigitToRawBuffer::MuonDigitToRawBuffer( const std::string& name,
                                            ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator ),
    m_muonDet(NULL),
    m_TotL1Board(0),
    m_M1Tell1(0)
{
  declareProperty("VType" , m_vtype=2) ;
}
//=============================================================================
// Destructor
//=============================================================================
MuonDigitToRawBuffer::~MuonDigitToRawBuffer(){}

//=============================================================================
// Initialisation. Check parameters
//=============================================================================
StatusCode MuonDigitToRawBuffer::initialize() {
  StatusCode sc = GaudiAlgorithm::initialize(); // must be executed first
  if ( sc.isFailure() ) return sc;  // error printed already by GaudiAlgorithm

  if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) debug() << "==> Initialise" << endmsg;

  m_muonDet=getDet<DeMuonDetector>(DeMuonLocation::Default);
  m_TotL1Board=(m_muonDet->getDAQInfo())->TotTellNumber();

  m_M1Tell1=(m_muonDet->getDAQInfo())->M1TellNumber();

  for(int i=0;i<m_TotL1Board;i++){
    unsigned int ODE_in_L1=(m_muonDet->getDAQInfo())->ODEInTell1(i);

    //std::vector<unsigned int> listOfODE=m_ODEInL1[i];
    unsigned int iODE=0;
    for(iODE=0;iODE<ODE_in_L1;iODE++){
      unsigned int odenumber=(m_muonDet->getDAQInfo())->getODENumberInTell1(i,iODE)-1;

      m_digitsInODE[odenumber].resize(0);
      firedInODE[odenumber]=0;
    }
    for(int j=0;j<4;j++)firedInPP[i*4+j]=0;

  }

  if( UNLIKELY( msgLevel(MSG::DEBUG) ) )
    debug()<<"board number "<<m_TotL1Board<<endmsg;

  return StatusCode::SUCCESS;
}
//=============================================================================
// Main execution
//=============================================================================
StatusCode MuonDigitToRawBuffer::execute() {

  if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) debug() << "==> Execute" << endmsg;

  //MuonBankVersion::versions v;
  // v=m_vtype;
  if(m_vtype==MuonBankVersion::DC06) {
    StatusCode sc=ProcessDC06();
    if(sc.isFailure())return sc;
  }else if(m_vtype==MuonBankVersion::v1){

    StatusCode sc=ProcessV1();
    if(sc.isFailure())return sc;
  }

  if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) debug()<<" exit "<<endmsg;
  return StatusCode::SUCCESS;
}
///
StatusCode MuonDigitToRawBuffer::ProcessDC06()
{

  StatusCode sc=StatusCode::FAILURE;

  LHCb::RawEvent* raw = get<LHCb::RawEvent>( LHCb::RawEventLocation::Default );

  sc=ProcessDigitDC06();
  if(sc.isFailure())return sc;

  sc=ProcessPads();
  if(sc.isFailure())return sc;

  for(unsigned int i=0;i<(unsigned int) m_TotL1Board;i++){
    unsigned int ODE_in_L1=(m_muonDet->getDAQInfo())->ODEInTell1(i);
    unsigned int totalChInL1=0;
    unsigned int odeInTell1=0;

    //std::vector<unsigned int> listOfODE=m_ODEInL1[i];
    unsigned int iODE=0;
    for(iODE=0;iODE<ODE_in_L1;iODE++){
      unsigned int odenumber=(m_muonDet->getDAQInfo())->getODENumberInTell1(i,iODE);
      //build header == channels fired for each ode
      unsigned int firedChannels=firedInODE[odenumber-1];
      //fill in bank.....
      totalChInL1=totalChInL1+firedChannels;
      odeInTell1++;
    }
    // now the dimension of the bank can be fixed....
    //the algo if (odeInTell1+2*totalChInL1+0.5*totalChInL1)/4
    unsigned int bankLenght=0;
    unsigned int bankLenPad=0;
    //  info()<<" pad lenght "<<i<<" "<<m_padInL1[i].size()<<endmsg;
    if(i>=m_M1Tell1)bankLenPad=(m_padInL1[i].size()+1)*16;

    unsigned int bankLenAdd=odeInTell1*8+8*totalChInL1;
    unsigned int bankLenTime=4*totalChInL1;


    //info()<<"ode in Tell1 "<<odeInTell1<<" tot ch "<<totalChInL1<<endmsg  ;
     if(bankLenPad%32!=0){
      bankLenght=bankLenght+bankLenPad/32+1;
    }else{
      bankLenght=bankLenght+bankLenPad/32;
    }
    if(bankLenAdd%32!=0){
      bankLenght=bankLenght+bankLenAdd/32+1;
    }else{
      bankLenght=bankLenght+bankLenAdd/32;
    }
    if(bankLenTime%32!=0){
      bankLenght=bankLenght+bankLenTime/32+1;
    }else{
      bankLenght=bankLenght+bankLenTime/32;
    }

    //    info()<<"Tell 1 "<<i<<" "<<bankLenPad<<" "
    //<<bankLenAdd<<" "<<bankLenTime<<" "<<bankLenght<<endmsg;
    LHCb::BankWriter bank(bankLenght);
    if(i>=m_M1Tell1){
      short padSize=m_padInL1[i].size();
      bank<<padSize;
      for(std::vector<unsigned int>::iterator itPad=m_padInL1[i].begin();
          itPad<m_padInL1[i].end();itPad++){

        //fill the pad address
        short padADD=*itPad;
        bank<<padADD;
      }
    }
    //padding at the end
    short nullWord=0;
    if(bankLenPad%32!=0)bank<<nullWord;
    //  info()<<"after creation "<<sizeof(int)<<" "<<sizeof(char)<<endmsg;
    for(iODE=0;iODE<ODE_in_L1;iODE++){
      unsigned int odenumber=(m_muonDet->getDAQInfo())->getODENumberInTell1(i,iODE);

      //build header == channels fired for each ode
      unsigned int firedChannels=firedInODE[odenumber-1];
      // //fill in bank.....
      unsigned char fired=(unsigned char) firedChannels;
      bank<<fired;
    }
    std::vector<unsigned int>::iterator itDigit;


    for(iODE=0;iODE<ODE_in_L1;iODE++){
      unsigned int odenumber=(m_muonDet->getDAQInfo())->getODENumberInTell1(i,iODE)-1;


      for(itDigit=m_digitsInODE[odenumber].begin();
          itDigit<m_digitsInODE[odenumber].end();
          itDigit++){
        MuonHLTDigitFormat temp(*itDigit,MuonBankVersion::DC06);
        unsigned int adress=temp.getAddress();
        unsigned char chad=(unsigned char) adress;
        bank<< chad;
      }
    }
    // padding if needed
    if(bankLenAdd%32!=0){
      unsigned int padding=(bankLenAdd%32)/8;
      for(unsigned int addpad=0;addpad<4-padding;addpad++){
        //the padding is set to zero
        unsigned char chad=(unsigned char) 0;
        bank<< chad;
      }
    }


    bool even=false;
    unsigned char time=0;
    unsigned int timetemp=0;

    for(iODE=0;iODE<ODE_in_L1;iODE++){
      unsigned int odenumber=(m_muonDet->getDAQInfo())->getODENumberInTell1(i,iODE)-1;


      for(itDigit=m_digitsInODE[odenumber].begin();
          itDigit<m_digitsInODE[odenumber].end();
          itDigit++){
        MuonHLTDigitFormat temp1(*itDigit,MuonBankVersion::DC06);
        unsigned int time1=temp1.getTime();
        if(even){
          timetemp=timetemp+(time1<<4);
        }else{
          timetemp=time1;
        }

        time=(unsigned char) timetemp;

        if(even)bank<<time;
        even=!even;

      }
    }
    if(even)bank<<time;
    //fill with zeros the padding
    if(bankLenTime%32!=0){
      unsigned int padding=(bankLenTime%32)/8;
      for(unsigned int addpad=0;addpad<padding;addpad++){
        //the padding is set to zero
        unsigned char chad=(unsigned char) 0;
        bank<< chad;
      }
    }

    raw->addBank(i,RawBank::Muon,1,bank.dataBank());
  }

  for(int i=0;i<m_TotL1Board;i++){
    unsigned int iODE=0;
    m_padInL1[i].clear();
    unsigned int ODE_in_L1=(m_muonDet->getDAQInfo())->ODEInTell1(i);

    for(iODE=0;iODE<ODE_in_L1;iODE++){
      unsigned int odenumber=(m_muonDet->getDAQInfo())->getODENumberInTell1(i,iODE)-1;
      m_digitsInODE[odenumber].resize(0);
      firedInODE[odenumber]=0;
    }
  }
  return StatusCode::SUCCESS;

}
StatusCode MuonDigitToRawBuffer::ProcessV1()
{

  StatusCode sc=StatusCode::FAILURE;

  LHCb::RawEvent* raw = get<LHCb::RawEvent>( LHCb::RawEventLocation::Default );

  sc=ProcessDigitV1();
  if(sc.isFailure())return sc;

  sc=ProcessPads();
  if(sc.isFailure())return sc;

  unsigned int pp_counter[4];



  for(unsigned int i=0;i<(unsigned int) m_TotL1Board;i++){
    for(int pp=0;pp<4;pp++)pp_counter[pp]=firedInPP[i*4+pp];
    unsigned int totHit=0;
    for(int pp=0;pp<4;pp++)totHit=totHit+pp_counter[pp];

    unsigned int totalChInL1=0;
    //    unsigned int odeInTell1=0;

    // now the dimension of the bank can be fixed....
    //the algo if (odeInTell1+2*totalChInL1+0.5*totalChInL1)/4
    unsigned int bankLenght=0;
    unsigned int bankLenPad=0;
    //  info()<<" pad lenght "<<i<<" "<<m_padInL1[i].size()<<endmsg;
    bankLenPad=(m_padInL1[i].size()+1)*16+16;
    totalChInL1=(m_digitsInL1[i].size());
    unsigned int bankLenHit=16*totalChInL1+16*4;

    if(totHit!= totalChInL1){
      err()<< " wrong # hit in L1: cross check failed"<<endmsg;
      err()<<totHit<<" "<<totalChInL1<<endmsg;

      return StatusCode::FAILURE;
    }


    //info()<<"ode in Tell1 "<<odeInTell1<<" tot ch "<<totalChInL1<<endmsg  ;
    if(bankLenPad%32!=0){
      bankLenght=bankLenght+bankLenPad/32+1;
    }else{
      bankLenght=bankLenght+bankLenPad/32;
    }
    if(bankLenHit%32!=0){
      bankLenght=bankLenght+bankLenHit/32+1;
    }else{
      bankLenght=bankLenght+bankLenHit/32;
    }


    //    info()<<"Tell 1 "<<i<<" "<<bankLenPad<<" "
    //<<bankLenAdd<<" "<<bankLenTime<<" "<<bankLenght<<endmsg;
    LHCb::BankWriter bank(bankLenght);

    short padSize=m_padInL1[i].size();
    short evt=0;

    bank<<padSize;
    bank<<evt;
    if( UNLIKELY( msgLevel(MSG::DEBUG) ) )
      debug()<< i<<" pad size "<<padSize<<" "<<bankLenPad<<endmsg;

    for(std::vector<unsigned int>::iterator itPad=m_padInL1[i].begin();
        itPad<m_padInL1[i].end();itPad++){

      //fill the pad address
      short padADD=*itPad;
        bank<<padADD;
    }

    //padding at the end
    short nullWord=0;
    if(bankLenPad%32!=0)bank<<nullWord;

    std::vector<unsigned int>::iterator itHit;


    //  info()<<"after creation "<<sizeof(int)<<" "<<sizeof(char)<<endmsg;
    for(unsigned int pp_data=0;pp_data<4;pp_data++)
    {
        bank<<(short)pp_counter[pp_data];
        unsigned int hit_counter=0;
        if(pp_data==0) itHit=m_digitsInL1[i].begin();
        for(hit_counter=0;  hit_counter<pp_counter[pp_data];hit_counter++)
          {
             short hit=(short) (*itHit);
             if( UNLIKELY( msgLevel(MSG::DEBUG) ) )
               debug()<<"'digit written "<<(hit&0x0FFF)<<endmsg;
             bank<<hit;
             itHit++;

          }


    }



//     bank<<(short)pp_counter[0];
//     unsigned int hit_counter=1;
//     unsigned int pp_data=0;
//     info()<<" pp data "<<pp_counter[0]<<" "<<pp_counter[1]<<" "<<pp_counter[2]<<" "<<pp_counter[3]<<endmsg;
//     info()<<" data in bank "<<(bank.dataBank())[1]<<endmsg;

//     for(std::vector<unsigned int>::iterator itHit=m_digitsInL1[i].begin();
//         itHit<m_digitsInL1[i].end();itHit++){
//       short hit=(short) (*itHit);
//       // hit_counter++;
//       if(hit_counter<=pp_counter[pp_data])
//       {
//         bank<<hit;
//       }else if(hit_counter>pp_counter[pp_data]){
//         for(int ppl=pp_data+1;ppl<4;ppl++)
//         {
//           pp_data++;
//           bank<<(short)pp_counter[ppl];
//           if(pp_counter[ppl]>0) {
//             bank<<hit;
//             hit_counter=1;
//             break;
//           }

//         }


//         /*pp_data++;
//         bank<<(short)pp_counter[pp_data];
//         hit_counter=0;
//         if(pp_counter[pp_data]>0) bank<<hit;*/
//       }



//       hit_counter++;

//     }


    if(bankLenHit%32!=0)bank<<nullWord;
    raw->addBank(i,RawBank::Muon,MuonBankVersion::v1,bank.dataBank());
  }


  for(int i=0;i<m_TotL1Board;i++){
    unsigned int iODE=0;
    //  m_padInL1[i].clear();
    //m_digitsInL1[i].clear();
     m_padInL1[i].resize(0);
    m_digitsInL1[i].resize(0);

    unsigned int ODE_in_L1=(m_muonDet->getDAQInfo())->ODEInTell1(i);

    for(iODE=0;iODE<ODE_in_L1;iODE++){
      unsigned int odenumber=(m_muonDet->getDAQInfo())->getODENumberInTell1(i,iODE)-1;
      m_digitsInODE[odenumber].resize(0);
      firedInODE[odenumber]=0;


    }
    for(int j=0;j<4;j++){
      firedInPP[i*4+j]=0;

    }
  }
  return StatusCode::SUCCESS;

}





StatusCode MuonDigitToRawBuffer::ProcessDigitDC06()
{

  LHCb::MuonDigits* digit = get<LHCb::MuonDigits>( LHCb::MuonDigitLocation::MuonDigit );
  LHCb::MuonDigits::iterator idigit;
  for(idigit=digit->begin();idigit<digit->end();idigit++){
    LHCb::MuonTileID digitTile=(*idigit)->key();
    unsigned int time=(*idigit)->TimeStamp();
    //time=0;
    long L1Number=0;
    long ODENumber=0;
    long DigitOutputPosition=(m_muonDet->getDAQInfo())->DAQaddressInODE(digitTile,L1Number,ODENumber,false);
    if( DigitOutputPosition>=192)info()<<" che diavolo succede "<<endmsg;

    if( UNLIKELY( msgLevel(MSG::DEBUG) ) ){
      debug()<<"L1Number "<<L1Number<<" "<<ODENumber<<endmsg;
      debug()<<" digitOutputPosition "<<DigitOutputPosition<<endmsg;
    }
    unsigned int digitInDAQ=0;
    MuonHLTDigitFormat temp(MuonBankVersion::DC06);
    temp.setAddress(DigitOutputPosition);
    temp.setTime(time);
    digitInDAQ=temp.getWord();
    //     debug()<<" digit word "<<L1Number-1<<" "<<ODENumber-1<<" "<<
    //       temp.getAddress()<<endmsg;
    m_digitsInODE[ODENumber-1].push_back(digitInDAQ);
    firedInODE[ODENumber-1]++;
  }
  //  debug()<<" tot ODE "<<m_TotODEBoard<<endmsg;

  for(unsigned int i=0;i<MuonDAQHelper_maxODENumber;i++){
    std::stable_sort(m_digitsInODE[i].begin(),
                     m_digitsInODE[i].end(),SortDigitInODE());
  }
  return StatusCode::SUCCESS;

}


StatusCode MuonDigitToRawBuffer::ProcessPads()
{
  MuonBasicGeometry basegeometry( detSvc(),msgSvc());
  
  for(unsigned int i=m_M1Tell1;i<(unsigned int) m_TotL1Board;i++){
    //get Tell1 information
    std::string L1path=(m_muonDet->getDAQInfo())->Tell1Name(i);

    SmartDataPtr<MuonL1Board>  l1(detSvc(),L1path);
    // this method implies 5 stations ! Don't use it !
    //    int station=l1->getStation();
    // patch with this for the moment
    std::string::size_type findLastSeparator=L1path.rfind("/");
    std::string stname=L1path.substr(findLastSeparator+1,2);
    //
    //    std::string cablingBasePath=(m_muonDet->getDAQInfo())->getBasePath(basegeometry.getStationName(station));
    std::string cablingBasePath=(m_muonDet->getDAQInfo())->getBasePath(stname);
    
    unsigned int padsInTell1=0;
    unsigned int ODE_in_L1=(m_muonDet->getDAQInfo())->ODEInTell1(i);
    unsigned int maxPads=0;
    unsigned int TSInODE=0;
    

    //loop on ODE
    unsigned int iODE=0;
    for(iODE=0;iODE<ODE_in_L1;iODE++){
      
      if( UNLIKELY( msgLevel(MSG::DEBUG) ) )
	debug()<<" start a new ODE "<<endmsg;
      
      unsigned int odenumber=(m_muonDet->getDAQInfo())->getODENumberInTell1(i,iODE)-1;;
      
      std::string ODEpath=cablingBasePath
        +l1->getODEName(iODE);
      SmartDataPtr<MuonODEBoard>  ode(detSvc(),ODEpath);
      // how many digit in the TS?
      TSInODE=ode->getTSNumber();
      if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) debug()<<"TS "<<TSInODE<<endmsg;
      
      std::string  TSPath= cablingBasePath+ode->getTSName(0);
      SmartDataPtr<MuonTSMap>  TS(detSvc(),TSPath);
      long channelInTS=TS->numberOfOutputSignal();
      if(TS->numberOfLayout()==2){
        maxPads=TS->gridXLayout(0)*TS->gridYLayout(1);
      }
      else{
        maxPads=channelInTS;
      }
      if( UNLIKELY( msgLevel(MSG::DEBUG) ) )
        debug()<<" max pad "<<maxPads<<" "<<channelInTS<<endmsg;
      
      std::vector<unsigned int> list_input(channelInTS,0);
      //debug()<<" channelInTS "<<channelInTS<<endmsg;
      //build header == channels fired for each ode
      unsigned int TSnumberInODE=0;
      std::vector<unsigned int>::iterator itDigit;
      for(itDigit=m_digitsInODE[odenumber].begin();
          itDigit<m_digitsInODE[odenumber].end();
          itDigit++){
        MuonHLTDigitFormat temp(*itDigit,MuonBankVersion::DC06);
        unsigned int address=temp.getAddress();
        if( UNLIKELY( msgLevel(MSG::DEBUG) ) )
          debug()<<address<<" "<<channelInTS<<" "<<TSInODE<<" "<<TSnumberInODE<<endmsg;

        if(itDigit==m_digitsInODE[odenumber].begin()){
	  TSnumberInODE=address/channelInTS;
        }
        //debug()<<" address "<<address<<" "<<TSnumberInODE<<endmsg;
        bool swap=true;
	
        if(channelInTS*(TSnumberInODE+1)<=address||
           itDigit==m_digitsInODE[odenumber].end()-1){
          // info()<<" entra qui "<<TSnumberInODE<<endmsg;
          if(itDigit==(m_digitsInODE[odenumber].end()-1)&&
             address<channelInTS*(TSnumberInODE+1)) {
	    list_input[address-channelInTS*TSnumberInODE]=1;
	    swap=false;
          }
          std::string TSPath=cablingBasePath+
            ode->getTSName(TSnumberInODE);
          // debug()<<" TSPath "<<  TSPath <<endmsg;
          std::vector<unsigned int> resultsAddress=
            (m_muonDet->getDAQInfo())->padsinTS(list_input,
						TSPath);
          //store the output
          if( UNLIKELY( msgLevel(MSG::DEBUG) ) )
            debug()<<"size "<<resultsAddress.size()<<endmsg;
	  
          if(resultsAddress.size()){
            for(std::vector<unsigned int>::iterator itpad=
                  resultsAddress.begin();itpad<resultsAddress.end();
                itpad++){
              m_padInL1[i].push_back(padsInTell1+maxPads*TSnumberInODE+
                                     (*itpad));
            }
          }
	  
          //add te offset padsInTell1;
          //padsInTell1=padsInTell1+maxPads;
          //clear the area
          std::vector<unsigned int>::iterator itClear;
          for(itClear= list_input.begin();itClear<list_input.end();itClear++){
            *itClear=0;
          }
          if( UNLIKELY( msgLevel(MSG::DEBUG) ) )
            debug()<<" qui qui "<<swap<<endmsg;
	  
          TSnumberInODE=address/channelInTS;
          if(swap) {
            if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) {
              debug()<<"adding in position "<<address-channelInTS*TSnumberInODE
                     <<" "<<address<<endmsg;
              debug()<< list_input.size()<<endmsg;
            }
	    
	    list_input[address-channelInTS*TSnumberInODE]=1;
	    //add the pads mechanism....
	    if(itDigit==m_digitsInODE[odenumber].end()-1){
	      if( UNLIKELY( msgLevel(MSG::DEBUG) ) )
		debug()<<" before "<<endmsg;
	      
	      std::vector<unsigned int> resultsAddress=
		(m_muonDet->getDAQInfo())->padsinTS(list_input,
						    TSPath);
	      if( UNLIKELY( msgLevel(MSG::DEBUG) ) )
		debug()<<" after "<<endmsg;
	      if(resultsAddress.size()){
		for(std::vector<unsigned int>::iterator itpad=
		      resultsAddress.begin();
		    itpad<resultsAddress.end();itpad++){
		  m_padInL1[i].push_back(padsInTell1+
					 maxPads*TSnumberInODE+
					 (*itpad));
		}
	      }
	    }
          }
        }else{
          list_input[address-channelInTS*TSnumberInODE]=1;
        }
      }
      padsInTell1=padsInTell1+maxPads*TSInODE;
      
      if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) debug()<<" eccoci "<<endmsg;

    } // end of loop on ODEs
  }
   return StatusCode::SUCCESS;

}


StatusCode MuonDigitToRawBuffer::ProcessDigitV1()
{

  LHCb::MuonDigits* digit = get<LHCb::MuonDigits>( LHCb::MuonDigitLocation::MuonDigit );
  LHCb::MuonDigits::iterator idigit;

  for(idigit=digit->begin();idigit<digit->end();idigit++){
    LHCb::MuonTileID digitTile=(*idigit)->key();
    unsigned int time=(*idigit)->TimeStamp();
    //time=0;
    long L1Number=0;
    long ODENumber=0;
    unsigned int ODEAdd=0;
    
    
    if( UNLIKELY( msgLevel(MSG::DEBUG) ) )
      debug()<<"Processing V1 digit: "<<digitTile.toString()<<" time "<<time<<endmsg;

    //    continue;
    
    long DigitOutputPosition=(m_muonDet->getDAQInfo())->DAQaddressInL1(digitTile,L1Number,ODENumber,ODEAdd);
    if( UNLIKELY( msgLevel(MSG::DEBUG) ) )
      debug()<<" L1 add "<<L1Number<<" "<<ODENumber<<endmsg;
    
    long DigitOutputPositionInODE=(m_muonDet->getDAQInfo())->DAQaddressInODE(digitTile,L1Number,ODENumber,false);
    if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) {
      debug()<<"L1Number "<<L1Number<<" "<<ODENumber<<endmsg;
      debug()<<" digitOutputPosition "<<DigitOutputPosition<<endmsg;
    }

    // GP getPPNumber must be called with L1 index 
    // and not with L1 serial number
    // notice that when M1 is present the two numbers coincide.
    // unsigned int pp_num=(m_muonDet->getDAQInfo())->getPPNumber(L1Number,ODENumber);

    std::string l1name = (m_muonDet->getDAQInfo())->findL1(digitTile);
    int L1Index = (m_muonDet->getDAQInfo())->findL1Index(l1name);
    if( UNLIKELY( msgLevel(MSG::DEBUG) ) )
      debug()<<"Found index for L1board: "<<l1name<<" "<<L1Index<<endmsg;

    unsigned int pp_num=(m_muonDet->getDAQInfo())->getPPNumber(L1Index,ODENumber);

    unsigned int digitInDAQ=0;
    MuonHLTDigitFormat temp(MuonBankVersion::v1);
    temp.setAddress(DigitOutputPosition);
    temp.setTime(time);
    digitInDAQ=temp.getWord();

    //GP also here these arrays are filled with serial numebr as index;
    //Must use progressive index instead

    //    m_digitsInL1[L1Number].push_back(digitInDAQ);
    //    firedInPP[L1Number*4+pp_num]++;

    m_digitsInL1[L1Index].push_back(digitInDAQ);
    firedInPP[L1Index*4+pp_num]++;

    if( UNLIKELY( msgLevel(MSG::DEBUG) ) )
      debug()<<" pp_num/L1index/L1num "<<pp_num<<" "<<L1Index<<" "<<L1Number<<endmsg;

    //     debug()<<" digit word "<<L1Number-1<<" "<<ODENumber-1<<" "<<
    //       temp.getAddress()<<endmsg;

    MuonHLTDigitFormat tempode(MuonBankVersion::v1);
    tempode.setAddress(DigitOutputPositionInODE);
    tempode.setTime(time);
    digitInDAQ=tempode.getWord();
    
    m_digitsInODE[ODENumber-1].push_back(digitInDAQ);
    
    firedInODE[ODENumber-1]++;
    
    
  }
  //  debug()<<" tot ODE "<<m_TotODEBoard<<endmsg;


  for(unsigned int i=0;i<MuonDAQHelper_maxTell1Number;i++){
    if( UNLIKELY( msgLevel(MSG::DEBUG) ) )
      debug()<<" start sorting "<<m_digitsInL1[i].size()<<endmsg;
    std::stable_sort(m_digitsInL1[i].begin(),
                     m_digitsInL1[i].end(),SortDigitInL1());
  }
  for(unsigned int i=0;i<MuonDAQHelper_maxODENumber;i++){
    if( UNLIKELY( msgLevel(MSG::DEBUG) ) )
      debug()<<" start sorting "<<m_digitsInODE[i].size()<<endmsg;
    std::stable_sort(m_digitsInODE[i].begin(),
                     m_digitsInODE[i].end(),SortDigitInODE());
  }

  return StatusCode::SUCCESS;

}

/*StatusCode MuonDigitToRawBuffer::setDigitLimit()
{


  for (int ode=0;ode<MuonDAQHelper_maxODENumber;ode++)
  {
    unsigned int max=63;
    if(m_digitsInODE[ode].size()>max)
    {
    }

  }


}
*/

//=============================================================================
/*std::vector<unsigned int> MuonDigitToRawBuffer::padsinTS(
                                                         std::vector<unsigned
                                                         int>& TSDigit,
                                                         std::string TSPath){
  //input the sequence of 0/1 for fired and not fired channels..
  std::vector<unsigned int> list_of_pads;
  //maxPads=0;
  SmartDataPtr<MuonTSMap>  TS(detSvc(),TSPath);
  //debug()<<"----- start a trigger sector cross ---------"<<endmsg;
  //debug()<<TSPath<<" "<<TS->numberOfOutputSignal()<<endmsg;
  //debug()<<" seq ";
  //std::vector<unsigned int>::iterator idebug;
  //for(idebug=TSDigit.begin();idebug<TSDigit.end();idebug++){
  //  debug()<<" "<<*idebug;
  //}
  //debug()<<endmsg;

  if(TS->numberOfLayout()==2){
    //how many subsector????
    int NX=TS->gridXLayout(1);
    int NY=TS->gridYLayout(0);
    int Nsub=NX*NY;
    //debug()<<"number NX NY "<<NX<<" "<<NY<<endmsg;
    //maxPads=TS->gridXLayout(0)*TS->gridYLayout(1);
    // work on sub sector
    if(Nsub>8){
      err()<<"error the dimensioning of the TS subsector is wrong "<<endmsg;
      return list_of_pads;

    }

    std::vector<unsigned int> horiz[8];
    std::vector<unsigned int> hvert[8];
    // clear the memory;

    // start fill the sub sector matrices
    std::vector<unsigned int>::iterator it;
    int index=0;

    for(it=TSDigit.begin();it<TSDigit.end();it++,index++){
      //also zero must be set
      // which subsector?
      int mx=TS->gridXOutputChannel(index)/
        (TS->gridXLayout(TS->layoutOutputChannel(index))/NX);
      int my=TS->gridYOutputChannel(index)/
        (TS->gridYLayout(TS->layoutOutputChannel(index))/NY);
      //debug()<<" digit "<<index<<" "<<mx<<" "<<my<<" "<<*it<<endmsg;
      int Msub=mx+my*NX;
      // horizntal o vertical?
      bool horizontal=false;
      if(TS->layoutOutputChannel(index)==0)horizontal=true;
      if(horizontal)horiz[Msub].push_back(*it);
      else hvert[Msub].push_back(*it);
      // debug()<<" horizontal ? "<<     horizontal<<endmsg;
    }
    // now the address of fired pads..
    for(int i=0;i<Nsub;i++){
      // cross only local to each sub matrix
      std::vector<unsigned int>::iterator itx;
      std::vector<unsigned int>::iterator ity;
      //debug
      //debug()<<" sub matrix "<<i<<endmsg;
      //debug()<<" horizontal sequence ";

      // for(itx=horiz[i].begin();
      //    itx<horiz[i].end();itx++)
      // {
      //  debug()<<*itx<<" ";
      //}
      //debug()<<endmsg;
      //debug()<<" vertical sequence ";

      //for(ity=hvert[i].begin();ity<hvert[i].end();ity++){
      //  debug()<<*ity<<" ";
      //}
      //debug()<<endmsg;
      //end debug
      unsigned int y_index=0;
      unsigned int subY=i/NX;
      unsigned int subX=i-subY*NX;

      unsigned int offsetY=subY*(TS->gridYLayout(1)/NY);
      unsigned int offsetX=subX*(TS->gridXLayout(0)/NX);
      for(ity=hvert[i].begin();
          ity<hvert[i].end();ity++,y_index++){
        if(*ity!=0){
          unsigned int x_index=0;
          for(itx=horiz[i].begin();
              itx<horiz[i].end();itx++,x_index++){
            if(*itx!=0){
              unsigned int address=offsetX+x_index+
                (offsetY+y_index)*(TS->gridXLayout(0));
              //      debug()<<" result of the address "<<address<<endmsg;
              list_of_pads.push_back(address);
            }
          }
        }
      }
    }
  }else{
    //easy only zero suppression
    std::vector<unsigned int>::iterator it;
    unsigned int index=0;
    for(it=TSDigit.begin();it<TSDigit.end();it++,index++){
      unsigned int address=index;
      if(*it!=0){list_of_pads.push_back(address);
      //      debug()<<" result of the address "<<address<<endmsg;
      }
    }
  }
  return list_of_pads;
}

*/






