// Include files

// local
#include "MuonDet/MuonBasicGeometry.h"
#include "MuonDet/MuonL1Board.h"
#include "MuonDet/MuonStationCabling.h"
#include "MuonDet/MuonODEBoard.h"
#include "MuonDet/MuonTSMap.h"
#include "Event/RawBank.h"
#include "Event/MuonBankVersion.h"
#include "MuonRawBuffer.h"
#include "GaudiKernel/IIncidentSvc.h"

//-----------------------------------------------------------------------------
// Implementation file for class : MuonRawBuffer
//
// 2005-10-18 : Alessia Satta
//-----------------------------------------------------------------------------

DECLARE_TOOL_FACTORY( MuonRawBuffer )

using namespace LHCb;

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
MuonRawBuffer::MuonRawBuffer( const std::string& type,
                              const std::string& name,
                              const IInterface* parent )
  : Decoder::ToolBase ( type, name , parent ),
    m_M1Tell1(0),
    m_muonDet(0),
    m_checkTell1HeaderPerformed(false),
    m_checkTell1HeaderResult(false),
    m_statusCreated(false)
{
  declareInterface<IMuonRawBuffer>(this);
  
  //new for decoders, initialize search path, and then call the base method
  m_rawEventLocations = {LHCb::RawEventLocation::Muon, LHCb::RawEventLocation::Default};
  initRawEventSearch();
  
  m_NLink=24;
  m_ODEWord=35;
  m_TESChanged=false;

}
//=============================================================================
// Destructor
//=============================================================================
MuonRawBuffer::~MuonRawBuffer() {}

//=============================================================================



StatusCode MuonRawBuffer::initialize()
{
  StatusCode sc = Decoder::ToolBase::initialize() ;
  if(sc.isFailure())return sc;

  m_muonDet=getDet<DeMuonDetector>(DeMuonLocation::Default);
  if(!m_muonDet) return Error("Could not locate Muon detector element");
  m_M1Tell1=(m_muonDet->getDAQInfo())->M1TellNumber();
  incSvc()->addListener( this, IncidentType::BeginEvent );
  for (unsigned int i=0;i<MuonDAQHelper_maxTell1Number;i++){
    m_counter_invalid_hit[i]=0;
    m_processed_bank[i]=0;
    m_NZScounter_invalid_hit[i]=0;
    m_NZSprocessed_bank[i]=0;
    m_pad_checkSize[i]=0;
    m_hit_checkSize[i]=0;
    m_tell1_header_error[i]=0;
    m_tell1_header_ORODE_error[i]=0;
    m_tell1_header_SYNCH_data_error[i]=0;
    m_tell1_header_SYNCH_BC_error[i]=0;
    m_tell1_header_SYNCH_Evt_error[i]=0;

  }

  clearData();
  
  
  return StatusCode::SUCCESS ;
}


void MuonRawBuffer::handle ( const Incident& incident )
{
  if ( IncidentType::BeginEvent == incident.type() ) clearData() ;
}


void MuonRawBuffer::clearData(){
  if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) debug()<<" reset all buffers "<<endmsg;
  m_checkTell1HeaderPerformed=false;
  m_checkTell1HeaderResult=true;
  m_statusCreated=false;

  for (unsigned int i=0;i<MuonDAQHelper_maxTell1Number;i++){
    m_storage[i].clear();
    m_alreadyDecoded[i]=false;
    m_padStorage[i].clear();
    m_padAlreadyDecoded[i]=false;
    m_ODEAlreadyDecoded[i]=false;
    m_already_decoded_headerTell1[i]=false;
    m_eventHeader[i]=0;
    for(int j=0;j<24;j++)m_hitNumInLink[i*24+j]=0;
    for(int j=0;j<4;j++)m_hitNumInLink[i*4+j]=0;
    // m_tell1Header[i].setWord(0);

  }
  for (unsigned int i=0;i<MuonDAQHelper_maxODENumber;i++){
    m_ODEData[i].resetODEData();
  }
}


StatusCode MuonRawBuffer::finalize() {

  bool print_ZS=false;
  for (unsigned int i=0;i<MuonDAQHelper_maxTell1Number;i++){
    if(  m_counter_invalid_hit[i]>0)print_ZS=true;
    if(m_pad_checkSize[i]>0)print_ZS=true;
    if(m_hit_checkSize[i]>0)print_ZS=true;
    //info()<<" in Tell1 " <<i<<" invalid hit address "<<
    //    m_counter_invalid_hit[i]<<" processed banks # "<<
    //    m_processed_bank[i]<<endmsg;
        // break;

  }
  bool print_NZS=false;

  for (unsigned int i=0;i<MuonDAQHelper_maxTell1Number;i++){
    if(  m_NZScounter_invalid_hit[i]>0)print_NZS=true;
    //info()<<" in Tell1 " <<i<<" invalid hit address "<<
    //    m_counter_invalid_hit[i]<<" processed banks # "<<
    //    m_processed_bank[i]<<endmsg;
    // break;

  }
  bool print_errors=false;
  for (unsigned int i=0;i<MuonDAQHelper_maxTell1Number;i++){
    if(m_tell1_header_error[i]>0||m_tell1_header_ORODE_error[i]>0||
       m_tell1_header_SYNCH_data_error[i]>0||
       m_tell1_header_SYNCH_BC_error[i]>0||m_tell1_header_SYNCH_Evt_error[i]>0)
    {
      print_errors=true;
      break;

    }

  }

  if(print_ZS||print_NZS||print_errors){

    info()<<" Summary of Muon banks decoding errors "<<endmsg;
  }

  if(print_ZS)
  {
    info()<<" Zero suppressed part "<<endmsg;
    for (unsigned int i=0;i<MuonDAQHelper_maxTell1Number;i++){
      if(  m_counter_invalid_hit[i]>0){

        info()<<" in Tell1 " <<i<<" invalid hit address "<<
          m_counter_invalid_hit[i]<<" processed banks # "<<
          m_processed_bank[i]<<endmsg;
      }
      if(  m_pad_checkSize[i]>0){
        info()<<" in Tell1 pad decoding " <<i<<" invalid bank size "<<
          m_pad_checkSize[i]<<" processed banks # "<<
          m_processed_bank[i]<<endmsg;
      }
      if(  m_hit_checkSize[i]>0){
        info()<<" in Tell1 hit decoding " <<i<<" invalid bank size "<<
          m_hit_checkSize[i]<<" processed banks # "<<
          m_processed_bank[i]<<endmsg;
      }
    }
  }

  if(print_errors){
    for (unsigned int i=0;i<MuonDAQHelper_maxTell1Number;i++){
      if(m_tell1_header_error[i]>0){
//commented out waiting for a proper Tell1 firmware 2011-04-24
//        info()<<" in Tell1 "<<i<<" error inside Tell1 "<<m_tell1_header_error[i]<<endmsg;

      }

      if(m_tell1_header_ORODE_error[i]>0)
      {
        info()<<" in Tell1 "<<i<<" ODE reported error "<<m_tell1_header_ORODE_error[i]<<endmsg;
      }

//commented out waiting for a proper Tell1 firmware 2011-04-24
      if( m_tell1_header_SYNCH_data_error[i]>0){
        //info()<<" in Tell1 "<<i<<" SYNCH data consistency  error "
        //       <<m_tell1_header_SYNCH_data_error[i]<<endmsg;
      }

      if(m_tell1_header_SYNCH_BC_error[i]>0){
        info()<<" in Tell1 "<<i<<" SYNCH BC consistency  error "
              <<m_tell1_header_SYNCH_BC_error[i]<<endmsg;
      }
      if(m_tell1_header_SYNCH_Evt_error[i]>0){

        info()<<" in Tell1 "<<i<<" SYNCH L0Evt consistency  error "
              <<m_tell1_header_SYNCH_BC_error[i]<<endmsg;
      }

    }

  }



  if(print_NZS){

    info()<<" Non Zero suppressed part "<<endmsg;
    for (unsigned int i=0;i<MuonDAQHelper_maxTell1Number;i++){
      info()<<" in Tell1 " <<i<<" invalid hit address "<<
        m_NZScounter_invalid_hit[i]<<" processed banks # "<<
        m_NZSprocessed_bank[i]<<endmsg;

    }
  }

  clearData();
  return Decoder::ToolBase::finalize() ;
}


StatusCode MuonRawBuffer::getTile(std::vector<LHCb::MuonTileID>& storage)
{

  LHCb::RawEvent* raw = findFirstRawEvent();
  if( raw == NULL ) return Error("Failed to find raw data");

  StatusCode sc= getTile(raw,storage,rootInTES());
  return sc;

}

StatusCode MuonRawBuffer::getTileAndTDC(std::vector<std::pair<LHCb::MuonTileID,unsigned int> > & storage)
{
  LHCb::RawEvent* raw = findFirstRawEvent();
  if( raw == NULL ) return Error("Failed to find raw data");

  StatusCode sc= getTileAndTDC(raw,storage,rootInTES());
  return sc;
}






StatusCode  MuonRawBuffer::decodeTileAndTDCDC06(const RawBank* rawdata){

   if( RawBank::MagicPattern != rawdata->magic() ) {
     error()<<"magic pattern not correct in muon bank "<<endmsg;
     return StatusCode::FAILURE;
   }

   unsigned int chIterator=0;
   const unsigned char* it=rawdata->begin<unsigned char>();
   short skip=0;

   unsigned int tell1Number=(rawdata)->sourceID();
   if(tell1Number>=m_M1Tell1){
    //how many pads ?
    const short * itPad=rawdata->begin<short>();
    short nPads=*itPad;
    if((nPads+1)%2==0){
      skip=(nPads+1)/2;
    }else {
      skip=(nPads+1)/2+1;
    }
  }
  for(int k=0;k<4*skip;k++){
    it++;
  }

  // how many ODE in this tell1?

  unsigned int nODE=(m_muonDet->getDAQInfo())->ODEInTell1((rawdata)->sourceID());
  std::vector<unsigned int> firedInODE;
  firedInODE.resize(nODE) ;
  unsigned int itODE=0;
  unsigned int channelsInTell1=0;
  for(itODE=0; itODE < nODE ; itODE++) {
    //first decode the header
    firedInODE[itODE]=(unsigned int)(*it);
    channelsInTell1=channelsInTell1+(unsigned int)(*it);
    it++;

  }
  for(itODE=0;itODE<nODE;itODE++){
    unsigned int channels=0;
    unsigned int odenumber=(m_muonDet->getDAQInfo())->
      getODENumberInTell1((rawdata)->sourceID(),itODE);
    for(channels=0;channels<firedInODE[itODE];channels++){
      unsigned int address=(unsigned int)(*it);
      MuonTileID tile=(m_muonDet->getDAQInfo())->
        getADDInODENoHole(odenumber-1,address);
      std::pair<MuonTileID,unsigned int> tileAndTDC;
      tileAndTDC.first=tile;
      m_storage[tell1Number].push_back(tileAndTDC);
      it++;
    }
  }

  //then decode the TDC info

  //first skip the byte required for padding
  //how many?
  int padding=0;
  if((channelsInTell1+nODE)%4){
    padding=4-(channelsInTell1+nODE)%4;
  }
  for(int ipad=0;ipad<padding;ipad++){
    it++;
  }


  unsigned int TDCword=0;
  if(channelsInTell1%2==0) TDCword=channelsInTell1/2;
  else TDCword=channelsInTell1/2+1;
  unsigned int countChannels=0;

  for(unsigned int ch=0;ch<TDCword;ch++){
    unsigned int time1 = (*it)&((unsigned char)15 );
    unsigned int time2 = ((*it)>>4)&((unsigned char) 15);

    (m_storage[tell1Number])[chIterator].second=time1;
    chIterator++;

    if(countChannels+1<channelsInTell1) {
      (m_storage[tell1Number])[chIterator].second=time2;
      chIterator++;
    }
    countChannels=countChannels+2;
      it++;

  }

  return StatusCode::SUCCESS;

}



StatusCode  MuonRawBuffer::decodeTileAndTDCV1(const RawBank* rawdata){


  StatusCode sc=checkBankSize(rawdata);
  unsigned int tell1Number=(rawdata)->sourceID();
  if(sc.isFailure()){
    m_hit_checkSize[tell1Number]++;

    return StatusCode::SUCCESS;
  }

  bool print_bank=false;

  bool muon_spec_header=true;

  const unsigned short* it=rawdata->begin<unsigned short>();
  short skip=0;

  //printout
  if(print_bank)
  {
    int bank_size=rawdata->size();
    //    int read_data=0;

    int count_word=0;
    info()<<"Tell1 "<<tell1Number<<" "<<"bank length "<<bank_size<<endmsg;

    for (it=rawdata->begin<unsigned short>();it<rawdata->end<unsigned short>();it++)
    {
      info()<<count_word<<" "<<((*it)&(0xFFF))<<" "<<(((*it)>>12)&(0xF))<<endmsg;

      count_word++;

    }
    it=rawdata->begin<unsigned short>();
  }






  m_processed_bank[tell1Number]++;



  if(muon_spec_header)
  {


    //how many pads ?
    //const unsigned short * itPad=rawdata->begin<unsigned short>();

    unsigned  short nPads=*it;

    if((nPads+2)%2==0){
      skip=(nPads+2)/2;
    }else {
      skip=(nPads+2)/2+1;
    }
    for(int k=0;k<2*skip;k++){
      if(k==1)fillTell1Header(tell1Number,*it);

      //  m_eventHeader[tell1Number]=*it;
            //if(k==1)info()<<tell1Number<<" "<<((*it)&(0xFF))<<" "<<(((*it)&(0xFF00))>>8)<<endmsg;

      it++;
    }
  }


  unsigned int hit_link_cnt[24];
  for(int i=0;i<24;i++){
    hit_link_cnt[i]=0;
  }

  for(int i=0;i<4;i++){
    //now go to the single pp counter
    unsigned int pp_cnt=*it;
    m_hitNumInPP[tell1Number*4+i]=pp_cnt;

    if( UNLIKELY( msgLevel(MSG::VERBOSE) ) )
      verbose()<<" hit in PP "<<pp_cnt<<endmsg;

    it++;

    for (unsigned int loop=0;loop<pp_cnt;loop++){
      unsigned int add=(*it)&(0x0FFF);
      unsigned int tdc_value=(((*it)&(0xF000))>>12);
      MuonTileID tile=(m_muonDet->getDAQInfo())->getADDInTell1(tell1Number,add);
      if( UNLIKELY( msgLevel(MSG::VERBOSE) ) )
        verbose()<<" add "<<add<<" "<<tile <<endmsg;
      if(tile.isValid())
      {
        if( UNLIKELY( msgLevel(MSG::DEBUG) ) )
          debug()<<" valid  add "<<add<<" "<<tile <<endmsg;
        std::pair<MuonTileID,unsigned int> tileAndTDC;
        tileAndTDC.first=tile;
        tileAndTDC.second=tdc_value;
        m_storage[tell1Number].push_back(tileAndTDC);
      }else {
//        info()<<" "<<add<<" "<<tdc_value<<" "<<i<<endmsg;

        m_counter_invalid_hit[tell1Number]++;
      }
      //update the hitillink counter
      unsigned int link=add/192;
      hit_link_cnt[link]++;
      it++;
    }
  }
  int active_link_counter=0;

  for(int i=0;i<24;i++){
    if((m_muonDet->getDAQInfo())->getODENumberInLink(tell1Number,i)==0){
      m_hitNumInLink[tell1Number*24+i]=0;
   }else{
     m_hitNumInLink[tell1Number*24+i]= hit_link_cnt[active_link_counter];
     active_link_counter++;
    }
  }

  return StatusCode::SUCCESS;

}




StatusCode MuonRawBuffer::getPads(std::vector<LHCb::MuonTileID>& storage)
{
  LHCb::RawEvent* raw = findFirstRawEvent();
  if( raw == NULL ) return Error("Failed to find raw data");

  // const std::vector<RawBank*>& b = raw->banks(RawBank::Muon);
  std::vector<RawBank*>::const_iterator itB;
  storage.clear();



  StatusCode sc= getPads(raw,storage,rootInTES());

    return sc;

//  //first decode data and insert in buffer
//   for( itB = b.begin(); itB != b.end(); itB++ ) {
//     const RawBank* r = *itB;
//     StatusCode sc=DecodeDataPad(r);
//     if(sc.isFailure())return sc;
//   }

//   //compact data  in one container

//   for( itB = b.begin(); itB != b.end(); itB++ ) {
//     unsigned int tell1Number=(*itB)->sourceID();

//      std::vector<LHCb::MuonTileID>::iterator itStorage;
//      for(itStorage=(m_padStorage[tell1Number]).begin();
//          itStorage<(m_padStorage[tell1Number]).end();itStorage++){
//        storage.push_back(*itStorage);
//      }
//   }



//   return StatusCode::SUCCESS;

}


StatusCode MuonRawBuffer::decodePadsV1(const LHCb::RawBank* r)
{
  unsigned int tell=(unsigned int)(r)->sourceID();
  StatusCode sc=checkBankSize(r);
  if(sc.isFailure()){
    m_pad_checkSize[tell]++;
    return StatusCode::SUCCESS;;
  }

  const short * it=r->begin<short>();
  if((unsigned int)(r)->sourceID()>=m_M1Tell1){
    unsigned int pads=*it;
    it++;
    m_eventHeader[tell]=*it;
    it++;
    for(unsigned int loop=0;loop<pads;loop++){
      unsigned int address=*it;
      MuonTileID tile =(m_muonDet->getDAQInfo())->getPadInTell1V1(tell, address);
      (m_padStorage[tell]).push_back(tile);
      it++;
    }
  }
  return StatusCode::SUCCESS;
}

StatusCode MuonRawBuffer::decodePadsDC06(const LHCb::RawBank* r)
{

if( RawBank::MagicPattern != r->magic() ) {
    error()<<"magic pattern not correct in muon bank "<<endmsg;
    return StatusCode::FAILURE;
  }


  const short * it=r->begin<short>();
  if((unsigned int)(r)->sourceID()>=m_M1Tell1){
    unsigned int tell=(unsigned int)(r)->sourceID();
    unsigned int pads=*it;
    it++;
    for(unsigned int loop=0;loop<pads;loop++){
      unsigned int address=*it;
      MuonTileID tile =(m_muonDet->getDAQInfo())->getPadInTell1DC06(tell, address);
      (m_padStorage[tell]).push_back(tile);
      it++;
    }
  }
  return StatusCode::SUCCESS;
}

StatusCode MuonRawBuffer::getTile(const LHCb::RawBank* r,std::vector<LHCb::MuonTileID>& tile)
{
  tile.clear();

  unsigned int tell1Number=(r)->sourceID();
  StatusCode sc=DecodeData(r);
  if(sc.isFailure()) return StatusCode::SUCCESS;
  //return sc;

  //copy in output container
  int output_size=m_storage[tell1Number].size();

  tile.reserve(output_size);
  std::vector<std::pair<LHCb::MuonTileID, unsigned int> >::const_iterator itData;
  for(itData=m_storage[tell1Number].begin();itData<m_storage[tell1Number].end();itData++)
  {
    tile.push_back((*itData).first);

  }
  return StatusCode::SUCCESS;

}

StatusCode MuonRawBuffer::getTile( LHCb::RawEvent* raw,std::vector<LHCb::MuonTileID>& storage,std::string offset)
{


  setTESOffset(offset);
  initStatus();

  StatusCode sc=checkAllHeaders(raw);

  storage.clear();
  if(!sc.isFailure()){

    const std::vector<RawBank*>& b = raw->banks(RawBank::Muon);
    std::vector<RawBank*>::const_iterator itB;
    if( UNLIKELY( msgLevel(MSG::VERBOSE) ) )
      verbose()<<" tell1 "<<b.size()<<endmsg;


    //first decode data and insert in buffer
    for( itB = b.begin(); itB != b.end(); itB++ ) {
      const RawBank* r = *itB;
      StatusCode sc=DecodeData(r);
      if(sc.isFailure())return sc;
    }
    //compact storage in one container

    for( itB = b.begin(); itB != b.end(); itB++ ) {
      unsigned int tell1Number=(*itB)->sourceID();
      std::vector<std::pair<LHCb::MuonTileID,unsigned int> >::iterator itStorage;
      for(itStorage=(m_storage[tell1Number]).begin();
          itStorage<(m_storage[tell1Number]).end();itStorage++){
        storage.push_back(itStorage->first);
      }
    }
  }

  putStatusOnTES();
  restoreTESOffset();

  return StatusCode::SUCCESS;

}


StatusCode MuonRawBuffer::getTileAndTDC(LHCb::RawEvent* raw,std::vector<std::pair<LHCb::MuonTileID,unsigned int> > & storage,
                                        std::string offset)
{

  setTESOffset(offset);
  initStatus();

  StatusCode sc=checkAllHeaders(raw);
  if(!sc.isFailure()){



    storage.clear();
    if( UNLIKELY( msgLevel(MSG::VERBOSE) ) )
      verbose()<<" start the real decoding "<<endmsg;

    const std::vector<RawBank*>& b = raw->banks(RawBank::Muon);
    std::vector<RawBank*>::const_iterator itB;

    //first decode data and insert in buffer
    for( itB = b.begin(); itB != b.end(); itB++ ) {
      const RawBank* r = *itB;
      StatusCode sc=DecodeData(r);
      if(sc.isFailure())return sc;
    }
    if( UNLIKELY( msgLevel(MSG::VERBOSE) ) )
      verbose()<<" the decoding is finished "<<endmsg;
    //compact data  in one container

    for( itB = b.begin(); itB != b.end(); itB++ ) {
      unsigned int tell1Number=(*itB)->sourceID();

      std::vector<std::pair<LHCb::MuonTileID,unsigned int> >::iterator itStorage;
      for(itStorage=(m_storage[tell1Number]).begin();
          itStorage<(m_storage[tell1Number]).end();itStorage++){
        storage.push_back(*itStorage);
      }
    }
  }

  putStatusOnTES();
  restoreTESOffset();

  return StatusCode::SUCCESS;
}







StatusCode MuonRawBuffer::getTileAndTDC(const LHCb::RawBank* r,std::vector<std::pair<LHCb::MuonTileID,unsigned int> > & tile)

{
  tile.clear();

  StatusCode sc=DecodeData(r);
  //if(sc.isFailure())return sc;
  if(sc.isFailure()) return StatusCode::SUCCESS;
  unsigned int tell1Number=(r)->sourceID();

  //copy in output container
  int output_size=m_storage[tell1Number].size();

  tile.reserve(output_size);
  std::vector<std::pair<LHCb::MuonTileID, unsigned int> >::const_iterator itData;
  for(itData=m_storage[tell1Number].begin();itData<m_storage[tell1Number].end();itData++)
  {
    tile.push_back((*itData));

  }
  return StatusCode::SUCCESS;

}



StatusCode MuonRawBuffer::getPads(LHCb::RawEvent* raw,std::vector<LHCb::MuonTileID> & pads, std::string offset)
{
  setTESOffset(offset);
  initStatus();


  StatusCode sc=checkAllHeaders(raw);
  pads.clear();
  if(!sc.isFailure()){
    pads.clear();

    const std::vector<RawBank*>& b = raw->banks(RawBank::Muon);
    std::vector<RawBank*>::const_iterator itB;

    //first decode data and insert in buffer
    for( itB = b.begin(); itB != b.end(); itB++ ) {
      const RawBank* r = *itB;
      StatusCode sc=DecodeDataPad(r);
      if(sc.isFailure())return sc;
    }

    //compact data  in one container

    for( itB = b.begin(); itB != b.end(); itB++ ) {
      unsigned int tell1Number=(*itB)->sourceID();

      std::vector<LHCb::MuonTileID>::iterator itStorage;
      for(itStorage=(m_padStorage[tell1Number]).begin();
         itStorage<(m_padStorage[tell1Number]).end();itStorage++){
        pads.push_back(*itStorage);
      }
    }
  }

  putStatusOnTES();
  restoreTESOffset();
  return StatusCode::SUCCESS;
}





StatusCode MuonRawBuffer::getPads(const LHCb::RawBank* r,std::vector<LHCb::MuonTileID>& pads)
{
  pads.clear();
  StatusCode sc=DecodeDataPad(r);
  if(sc.isFailure())return sc;

  unsigned int tell=(r)->sourceID();
  std::vector<MuonTileID>::iterator itStorage;
  for(itStorage=(m_padStorage[tell]).begin();itStorage<(m_padStorage[tell]).end();itStorage++){
    MuonTileID tile=*itStorage;
    pads.push_back(tile);

  }
  return StatusCode::SUCCESS;
}



StatusCode MuonRawBuffer::DecodeData(const LHCb::RawBank* r)
{
  StatusCode sc=StatusCode::FAILURE;
  unsigned int tell1Number=(r)->sourceID();
  if(tell1Number>=MuonDAQHelper_maxTell1Number){
    error()<<" in muon data a Tell1 Source ID is gretare than maximum "<<endmsg;

    return StatusCode::FAILURE;
  }

  if(!m_alreadyDecoded[tell1Number]){
    if(r->version()==MuonBankVersion::DC06){
      sc=decodeTileAndTDCDC06(r);


    }else if(r->version()==MuonBankVersion::v1){

      sc=decodeTileAndTDCV1(r);

    }m_alreadyDecoded[tell1Number]=true;
    if(sc.isFailure())return sc;
  }


  return StatusCode::SUCCESS;
}

StatusCode MuonRawBuffer::DecodeDataPad(const LHCb::RawBank* r)
{
  StatusCode sc=StatusCode::FAILURE;
  sc.ignore();
  unsigned int tell=(r)->sourceID();
  if(tell>=MuonDAQHelper_maxTell1Number){
    error()<<" in raw data there is a muon bank with source ID greater than maximum "<<endmsg;
    return StatusCode::FAILURE;
  }

  //  debug()<<tell<<" "<<m_padAlreadyDecoded[tell]<<endmsg;
  if((unsigned int)(r)->sourceID()>=m_M1Tell1){
    if(!m_padAlreadyDecoded[tell]){
      m_padAlreadyDecoded[tell]=true;
      if(r->version()==MuonBankVersion::DC06){
        sc=decodePadsDC06(r);
        if(sc.isFailure())return sc;
      }else if(r->version()==MuonBankVersion::v1){
        sc=decodePadsV1(r);
        if(sc.isFailure())return sc;
      }
    }
  }else m_padAlreadyDecoded[tell]=true;
  return StatusCode::SUCCESS;
}


StatusCode MuonRawBuffer::decodeNZSupp(unsigned int tell1Number)
{
  LHCb::RawEvent* raw = findFirstRawEvent();
  if( raw == NULL ) return Error("Failed to find raw data");

  StatusCode sc=StatusCode::SUCCESS;
  const std::vector<RawBank*>& b = raw->banks(RawBank::MuonFull);
  std::vector<RawBank*>::const_iterator itB;
  if( UNLIKELY( msgLevel(MSG::VERBOSE) ) )
    verbose()<<" tell1 "<<b.size()<<endmsg;
  if( b.size()==0)
  {
    info()<<" you have request NZS bank for muon detectorct ---> "<<
      "such bank are not presente in input file "<<endmsg;
  }


  for( itB = b.begin(); itB != b.end(); itB++ ) {
    unsigned int tell1BankNumber=(*itB)->sourceID();
    if(tell1Number==tell1BankNumber)
    {

      sc=decodeNZSupp(*itB);
      return sc;



    }
  }
  return sc;

}




StatusCode  MuonRawBuffer::decodeNZSupp(const LHCb::RawBank* r){

  if( UNLIKELY( msgLevel(MSG::VERBOSE) ) )
    verbose()<<" start decoding "<<endmsg;
  unsigned int tell1Num=(r)->sourceID();
  if( RawBank::MagicPattern != r->magic() ) {
    error()<<"magic pattern not correct in muon bank "<<endmsg;
    return StatusCode::FAILURE;
  }


  if(tell1Num>=MuonDAQHelper_maxTell1Number)return StatusCode::FAILURE;

  unsigned int data[24][35];
  const unsigned int * it=r->begin<unsigned int>();

  //loop on PP
  for (int pp_num=0;pp_num<4;pp_num++){
    unsigned int off=pp_num*6;
    if( UNLIKELY( msgLevel(MSG::VERBOSE) ) )
      verbose()<<" pp 0 "<<pp_num<<endmsg;

  //skip first 6 words ... always set to zero
    for(int i=0;i<6;i++){
      it++;
    }

  //now the  data
    for(unsigned int i=0;i<m_ODEWord*6;i++){
      unsigned int data1 = (*it)&(0xFFFF );
      //      unsigned int time2 = (((*it)&(unsigned char) 240) >> 4 );
      unsigned int data2 = ((*it)>>16)&(0xFFFF);
      unsigned int ODE=i%3;
      unsigned int wordNum=i/6;
      //first or second data part?
      if(i%6<3){
        data[off+ODE*2][wordNum]=data1&0xFFFF;
        data[off+ODE*2+1][wordNum]=data2&0xFFFF;
      }else{
        data[off+ODE*2][wordNum]|=((data1<<16)&0xFFFF0000);
        data[off+ODE*2+1][wordNum]|=((data2<<16)&0xFFFF0000);
      }
      it++;
    }
    // }

    //now the trailer
    for(int i=0;i<8;i++){
      //      unsigned int dd=(*it)&(0x0FFF);
      m_PPEventInfo[tell1Num*4+pp_num].setWord(i,*it);
      it++;
    }

  }
  if( UNLIKELY( msgLevel(MSG::VERBOSE) ) )
    verbose()<<" now fill the buffer "<<endmsg;
  //now copy the data to the buffer
  for(int i=0;i<24;i++){
    m_ODEData[tell1Num*24+i].setData(data[i]);
  }
  return StatusCode::SUCCESS;
}

StatusCode MuonRawBuffer::dumpNZSupp(const LHCb::RawBank* r,unsigned int ode_num)
{
  unsigned int tell1Number=(r)->sourceID();
  //info()<<" t1 "<<tell1Number<<endmsg;
  if(tell1Number>=MuonDAQHelper_maxTell1Number)return StatusCode::FAILURE;
  if(!m_ODEAlreadyDecoded[tell1Number]){
      decodeNZSupp( r);
  }
  for(int iLink=0;iLink<24;iLink++)
  {
    unsigned int ODE_num=(m_muonDet->getDAQInfo())
      ->getODENumberInLink(tell1Number,iLink);
    if(ODE_num==ode_num)
    {

      info()<<" ODE words for link # "<<iLink<<endmsg;
      for( unsigned int w=0;w<m_ODEWord;w++){

      info()<<(m_ODEData[tell1Number*24+iLink]).getWord(w)<<endmsg;
      }
    }

  }
  return StatusCode::SUCCESS;
}



StatusCode MuonRawBuffer::getNZSupp(const LHCb::RawBank* r,std::vector<std::pair<LHCb::MuonTileID,unsigned int> > & tileAndTDC)
{
  unsigned int tell1Number=(r)->sourceID();
  m_NZSprocessed_bank[tell1Number]++;
  if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) debug()<<" t1 "<<tell1Number<<endmsg;
  int fix_size=(m_NLink*36+8*4)*4;
   if(r->size()!=fix_size)return StatusCode::FAILURE;

   if(!m_ODEAlreadyDecoded[tell1Number]){
     decodeNZSupp( r);
   }

   //now copy the ODE info in a more suitable format
   //first loop on ODE
   for (unsigned int iLink=0;iLink<m_NLink;iLink++){

     unsigned int ODE_num=(m_muonDet->getDAQInfo())
       ->getODENumberInLink(tell1Number,iLink);
     if(ODE_num>0){
       if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) debug()<<" ODE "<<ODE_num<<endmsg;

       unsigned int ch=0;
       for(unsigned int iData=1;iData<m_NLink+1;iData++){
         for (int iTDC=0;iTDC<8;iTDC++){
           unsigned int TDC_value=(m_ODEData[m_NLink*tell1Number+iLink]).
             getTDC(iData,iTDC);
           if(TDC_value>0){
             MuonTileID tile=(m_muonDet->getDAQInfo())->
               getADDInODE(ODE_num-1, ch);
             if(tile.isValid()){
               std::pair<LHCb::MuonTileID,unsigned int>  ODE_info;
               ODE_info.first=tile;
               ODE_info.second=TDC_value;
               tileAndTDC.push_back(ODE_info);
             }else{
               m_NZScounter_invalid_hit[tell1Number]++;
             }
           }
           ch++;
         }
       }
     }
   }
   return StatusCode::SUCCESS;

}

MuonPPEventInfo MuonRawBuffer::getPPInfo(const LHCb::RawBank* r,unsigned int pp_num){

  unsigned int tell1Number=r->sourceID();
  if(!m_ODEAlreadyDecoded[tell1Number]){
    StatusCode sc= decodeNZSupp( r);
    if( sc.isFailure() && UNLIKELY( msgLevel(MSG::DEBUG) ) )
      debug()<<"error in decoding NZSP bank"<<endmsg;
  }
  return m_PPEventInfo[tell1Number*4+pp_num];

}

MuonPPEventInfo MuonRawBuffer::getPPInfo(unsigned int tell1Number,unsigned int pp_num){
  if(!m_ODEAlreadyDecoded[tell1Number]){
    //serach for bank and decode it
    StatusCode sc= decodeNZSupp( tell1Number);
    if( sc.isFailure() && UNLIKELY( msgLevel(MSG::DEBUG) ) )
      debug()<<"error in decoding NZSP bank"<<endmsg;
  }

  return m_PPEventInfo[tell1Number*4+pp_num];


}

MuonODEData MuonRawBuffer::getODEData(const LHCb::RawBank* r,
                                      unsigned int link){
  MuonODEData empty_data;

  if(link>23)return empty_data;

  unsigned int tell1Number=(r)->sourceID();
  if(!m_ODEAlreadyDecoded[tell1Number]){
    decodeNZSupp( r);
  }
  return (m_ODEData[tell1Number*24+link]);


}



unsigned int  MuonRawBuffer::BXCounter(unsigned int tell1Number)
{
  if( m_alreadyDecoded[ tell1Number]){
    return m_eventHeader[tell1Number]&(0xFF);

  }else{
    info()<<" the bank has not been decode yet "<<endmsg;
    return 0;



  }


}

StatusCode MuonRawBuffer::getNZSupp(std::vector<std::pair<LHCb::MuonTileID,
                                    unsigned int> > & tileAndTDC)
{
  LHCb::RawEvent* raw = findFirstRawEvent();
  if( raw == NULL ) return Error("Failed to find raw data");

  StatusCode sc= getNZSupp(raw,tileAndTDC,rootInTES());
  return sc;

}


StatusCode MuonRawBuffer::getNZSupp( LHCb::RawEvent* raw,
                                     std::vector<std::pair<LHCb::MuonTileID,
                                     unsigned int> > & tileAndTDC,
                                     std::string offset)
{


  setTESOffset(offset);

  initStatusNZS();

  const std::vector<RawBank*>& b = raw->banks(RawBank::MuonFull);
  std::vector<RawBank*>::const_iterator itB;
  if( UNLIKELY( msgLevel(MSG::VERBOSE) ) )
    verbose()<<" tell1 "<<b.size()<<endmsg;
  if( b.size()==0)
  {
    info()<<" you have request NZS bank for muon detectorct ---> "<<
      "such bank ar enot presente in input file "<<endmsg;
  }


  for( itB = b.begin(); itB != b.end(); itB++ ) {
    //    unsigned int tell1Number=(*itB)->sourceID();
    std::vector<std::pair<LHCb::MuonTileID,unsigned int> >::iterator
      itStorage;
    std::vector<std::pair<LHCb::MuonTileID,unsigned int> > bank_container;
    StatusCode sc=getNZSupp(*itB,bank_container);

    for(itStorage=bank_container.begin();
        itStorage<bank_container.end();itStorage++){
           tileAndTDC.push_back(*itStorage);
    }
  }
  putStatusOnTES();
  restoreTESOffset();
  return StatusCode::SUCCESS;
}


StatusCode MuonRawBuffer::checkBankSize(const LHCb::RawBank* rawdata)
{
  if( RawBank::MagicPattern != rawdata->magic() ) {
    error()<<"magic pattern not correct in muon bank "<<endmsg;
    return StatusCode::FAILURE;
  }

  const unsigned short * it=rawdata->begin<unsigned short>();
  unsigned int tell1Number=(rawdata)->sourceID();
  int bank_size=rawdata->size();
  int read_data=0;
  //minimum length is 3 words --> 12 bytes
  if(bank_size<12){
    err()<< " muon bank "<<tell1Number<<" is too short "<<
      bank_size<<endmsg;
    m_status.addStatus(tell1Number,RawBankReadoutStatus::Incomplete);
    return StatusCode::FAILURE;
  }
  //how many pads ?
  //const unsigned short * itPad=rawdata->begin<unsigned short>();
  unsigned short nPads=*it;
  //  if ( msgLevel(MSG::VERBOSE) ){
  //    verbose()<< " pad # "<<nPads<<endmsg;
  //  }

  int skip=0;

  if((nPads+2)%2==0){
    skip=(nPads+2)/2;
  }else {
    skip=(nPads+2)/2+1;
  }

  if((bank_size-skip*4)<0){
    err()<<"bank_size "<<bank_size<<" pad size to read "<<nPads*4<<endmsg;
    err()<< "so muon bank "<<tell1Number<<" is too short in pad part "<<endmsg;
  m_status.addStatus(tell1Number,RawBankReadoutStatus::Incomplete);
    return StatusCode::FAILURE;

  }

  it=it+2*skip;
  read_data=read_data+skip*2;
  if(read_data<0)info()<<nPads<<" "<<skip<<" "<<
                   bank_size<<" "<<read_data<<endmsg;

  for(int i=0;i<4;i++){
    //now go to the single pp counter
    int pp_cnt=*it;
    //    if ( msgLevel(MSG::VERBOSE) ){
    //    verbose()<<" hit in PP "<<pp_cnt<<endmsg;
    //    }
    it++;
    read_data++;

    //check size before start looping
    if(bank_size-read_data*2<pp_cnt*2){
      err()<<"bank_size "<<bank_size<<"read data "<<read_data<<" hit size to read "<<pp_cnt*2<<endmsg;
      err()<< "so muon bank "<<tell1Number<<" is too short in hit part "<<endmsg;
      m_status.addStatus(tell1Number,RawBankReadoutStatus::Incomplete);
      //break;

      return StatusCode::FAILURE;
    }
    it=it+pp_cnt;

  }

  return StatusCode::SUCCESS;

}


bool MuonRawBuffer::PPReachedHitLimit(unsigned int Tell1Number,int pp_num)
{
  if(!m_alreadyDecoded[Tell1Number]){
    std::vector<LHCb::MuonTileID> tile;
    StatusCode sc=getTile(tile);
    if(sc.isFailure())return true;
  }


  if(m_hitNumInPP[Tell1Number*4+pp_num]>262)return true;

  return false;

}



bool MuonRawBuffer::LinkReachedHitLimit(unsigned int Tell1Number,int link_num)
{

  if(!m_alreadyDecoded[Tell1Number]){
    std::vector<LHCb::MuonTileID> tile;
    StatusCode sc=getTile(tile);
    if(sc.isFailure())return true;
  }
  if(m_hitNumInLink[Tell1Number*24+link_num]>62)return true;

  return false;

}


StatusCode MuonRawBuffer::getPadsInStation(int station,std::vector<std::vector<LHCb::MuonTileID>* > & pads)
{

  std::vector<int> tell1_list=
    (m_muonDet->getDAQInfo())-> getTell1InStation(station);
  std::vector<int>::iterator iTell;
  for(iTell=tell1_list.begin();iTell<tell1_list.end();iTell++){
    StatusCode sc=getPads(*iTell);
    if(sc.isFailure())return sc;
    pads.push_back(&m_padStorage[*iTell]);
  }
  return StatusCode::SUCCESS;

}


StatusCode MuonRawBuffer::getPads( int tell1,std::vector<LHCb::MuonTileID>& pads)
{
  LHCb::RawEvent* raw = findFirstRawEvent();
  if( raw == NULL ) return Error("Failed to find raw data");

  const std::vector<RawBank*>& b = raw->banks(RawBank::Muon);
  std::vector<RawBank*>::const_iterator itB;
  pads.clear();



 //first decode data and insert in buffer
  for( itB = b.begin(); itB != b.end(); itB++ ) {
    const RawBank* r = *itB;
    if( r->sourceID()==tell1){

      StatusCode sc=DecodeDataPad(r);
      if(sc.isFailure())return sc;
      std::vector<LHCb::MuonTileID>::iterator itStorage;
      pads=(m_padStorage[tell1]);

    }
  }
  return StatusCode::SUCCESS;

}
StatusCode MuonRawBuffer::getPads( int tell1)
{
  LHCb::RawEvent* raw = findFirstRawEvent();
  if( raw == NULL ) return Error("Failed to find raw data");

  const std::vector<RawBank*>& b = raw->banks(RawBank::Muon);
  std::vector<RawBank*>::const_iterator itB;
 //first decode data and insert in buffer
  for( itB = b.begin(); itB != b.end(); itB++ ) {
    const RawBank* r = *itB;
    if( r->sourceID()==tell1){

      StatusCode sc=DecodeDataPad(r);
      if(sc.isFailure())return sc;
    }
  }
  return StatusCode::SUCCESS;

}


MuonTell1Header MuonRawBuffer::getHeader(const LHCb::RawBank* r)
{
  StatusCode sc=DecodeData(r);
  if(sc.isFailure()){
    MuonTell1Header tellHeader;

    return  tellHeader;
  }else{
    unsigned int tell1Number=(r)->sourceID();
    MuonTell1Header tellHeader(m_eventHeader[tell1Number]);

    return   tellHeader;

  }
}

std::vector<std::pair<MuonTell1Header, unsigned int> > MuonRawBuffer::getHeaders(LHCb::RawEvent* raw)
{
  std::vector<std::pair<MuonTell1Header, unsigned int> > return_value;


  const std::vector<RawBank*>& b = raw->banks(RawBank::Muon);
  std::vector<RawBank*>::const_iterator itB;

  //first decode data and insert in buffer
  for( itB = b.begin(); itB != b.end(); itB++ ) {
    MuonTell1Header header=getHeader(*itB);
    std::pair<MuonTell1Header, unsigned int> pair_data;
    pair_data.first=header;
    unsigned int tell1Number=(*itB)->sourceID();
    pair_data.second=tell1Number;
    return_value.push_back(pair_data);

  }
  return  return_value;

}



std::vector<std::pair<MuonTell1Header, unsigned int> > MuonRawBuffer::getHeaders()
{
  LHCb::RawEvent* raw = findFirstRawEvent();
  
  if( raw == NULL ) {
    Error("Failed to find raw data").ignore();
    std::vector<std::pair<MuonTell1Header, unsigned int> > return_value;
    return_value.clear();
    return return_value;
  }
  else {
    return getHeaders(raw);
  }
}


StatusCode MuonRawBuffer::checkAllHeaders(LHCb::RawEvent* raw)
{
  if( UNLIKELY( msgLevel(MSG::VERBOSE) ) )
    verbose()<<" check headers consistency "<<endmsg;

  if( m_checkTell1HeaderPerformed)return m_checkTell1HeaderResult;
  m_checkTell1HeaderPerformed=true;
  if( UNLIKELY( msgLevel(MSG::VERBOSE) ) )
    verbose()<<" check headers consistency not yet done"<<endmsg;

  std::vector<unsigned int> tell1InEvent;

  std::vector<unsigned int>::iterator  iList;
  const std::vector<RawBank*>& b = raw->banks(RawBank::Muon);
  std::vector<RawBank*>::const_iterator itB;

  if(b.size()==0){
    if( UNLIKELY( msgLevel(MSG::VERBOSE) ) )
      verbose()<<" no muon banks in event"<<endmsg;
    for(int i=0;i<  static_cast<int>(MuonDAQHelper_maxTell1Number);i++){
      m_status.addStatus( i,RawBankReadoutStatus::Missing);
    }
    m_checkTell1HeaderResult=false;
    return StatusCode::SUCCESS;
  }
  //first decode data and insert in buffer
  bool foundError=false;

  for( itB = b.begin(); itB != b.end(); itB++ ) {

    unsigned int tell1Number=(*itB)->sourceID();
    m_status.addStatus( tell1Number,RawBankReadoutStatus::OK);

    if((*itB)->size()==0)  m_status.addStatus( tell1Number,RawBankReadoutStatus::Empty);
    iList=std::find(tell1InEvent.begin(), tell1InEvent.end(),tell1Number );
    if(iList<tell1InEvent.end()){
      m_checkTell1HeaderResult=false;
      m_checkTell1HeaderPerformed=true;
      if( UNLIKELY( msgLevel(MSG::VERBOSE) ) ) verbose()<<" failed "<<endmsg;
      foundError=true;
      m_status.addStatus( tell1Number,RawBankReadoutStatus::NonUnique);
      break;

      //return StatusCode::FAILURE;
    }
    tell1InEvent.push_back(tell1Number);
  }

  //set missing bank readout status
  for(int i=0;i< static_cast<int>( MuonDAQHelper_maxTell1Number);i++){
    if(m_status.status(i)==LHCb::RawBankReadoutStatus::Unknown){
      m_status.addStatus( i,RawBankReadoutStatus::Missing);
    }
  }
  if(foundError){
    m_checkTell1HeaderResult=false;
    return StatusCode::FAILURE;
  }

  //there is repeated Tell1
  //now check the fw version

  //compact data  in one container
  int ReferenceVersion=(*(b.begin()))->version();

  for( itB = b.begin(); itB != b.end(); itB++ ) {
    unsigned int tell1Number=(*itB)->sourceID();
    if((*itB)->version()!=ReferenceVersion){
      error()<<
        " The muon Tell1 boards: not all the same version so  skip the event"
             << endmsg;
      m_checkTell1HeaderResult=false;
      m_checkTell1HeaderPerformed=true;
      m_status.addStatus( tell1Number,RawBankReadoutStatus::Tell1Error );
      return StatusCode::FAILURE; // return m_checkTell1HeaderResult;
    }
  }
  if( UNLIKELY( msgLevel(MSG::VERBOSE) ) )
    verbose()<<" test successeful "<<endmsg;
  return StatusCode::SUCCESS;
}


void MuonRawBuffer::fillTell1Header(unsigned int tell1Number,unsigned int data)
{
  m_eventHeader[tell1Number]=data;
  if(m_already_decoded_headerTell1[tell1Number])return;

  MuonTell1Header dataWord(data);
  if( UNLIKELY( msgLevel(MSG::VERBOSE) ) )
    verbose()<<dataWord.getError()<<endmsg;

  if(dataWord.getError())m_tell1_header_error[tell1Number]++;
  if(dataWord.getOROfODEErrors())m_tell1_header_ORODE_error[tell1Number]++;
  if(dataWord.getSYNCHDataErrorInODE())m_tell1_header_SYNCH_data_error[tell1Number]++;
  if(dataWord.getSYNCHBCNCntErrorInODE())m_tell1_header_SYNCH_BC_error[tell1Number]++;
  if(dataWord.getSYNCHEventCntErrorInODE())m_tell1_header_SYNCH_Evt_error[tell1Number]++;

}


void MuonRawBuffer::setTESOffset(std::string offset)
{
  if(m_TESChanged)return;

  if( UNLIKELY( msgLevel(MSG::DEBUG) ) ) debug()<<" changing rootInTES "<<endmsg;
  m_storeOriginalValue=this->rootInTES();
  IProperty* prop = dynamic_cast<IProperty*>( this );
  if( prop ) {
    StatusCode sc = prop->setProperty( "RootInTES", offset );
    if(sc.isFailure())warning()<<" eunable to set property "<<endmsg;

    m_TESChanged=true;

  }

}

void MuonRawBuffer::restoreTESOffset()
{
  IProperty* prop = dynamic_cast<IProperty*>( this );
  if( prop ) {
    StatusCode sc = prop->setProperty( "RootInTES", m_storeOriginalValue );
    if(sc.isFailure())warning()<<" eunable to set property "<<endmsg;
    m_TESChanged=false;

  }

}





void MuonRawBuffer::putStatusOnTES(){
  // Readout Status
  typedef LHCb::RawBankReadoutStatus Status;
  typedef LHCb::RawBankReadoutStatuss Statuss;


  Statuss* statuss = getOrCreate<Statuss,Statuss>( LHCb::RawBankReadoutStatusLocation::Default );
  Status* status = statuss->object ( m_status.key() );

  if( NULL == status ){
    std::stringstream type("");
    type << LHCb::RawBank::typeName(m_status.key()) ;
    if( UNLIKELY( msgLevel(MSG::VERBOSE) ) )
      verbose() << "Status for bankType " <<  type.str()  << " created now" << endmsg;
    status = new Status( m_status  );
    statuss->insert( status );


  } else {
    std::stringstream type("");
    type << LHCb::RawBank::typeName(m_status.key()) ;

    if( UNLIKELY( msgLevel(MSG::DEBUG) ) )
      debug() << "Status for bankType " <<  type.str()  << " already exists" << endmsg;
    if( status->status() != m_status.status() ){
      Warning("Status for bankType " +  type.str() + " already exists  with different status value -> merge both"
              , StatusCode::SUCCESS).ignore();
      for( std::map< int, long >::iterator it = m_status.statusMap().begin() ; it != m_status.statusMap().end() ; ++it){
        status->addStatus((*it).first , (*it).second);
      }
    }
  }

}

void  MuonRawBuffer::initStatus()
{
  if(!m_statusCreated){
    m_statusCreated=true;
    m_status= LHCb::RawBankReadoutStatus(RawBank::Muon );
  }

  if( UNLIKELY( msgLevel(MSG::VERBOSE) ) )
    verbose()<<" init "<<m_status.key()<<" "<<m_status<<endmsg;
}
void  MuonRawBuffer::initStatusNZS()
{
    m_statusFull= LHCb::RawBankReadoutStatus(RawBank::MuonFull );
}
