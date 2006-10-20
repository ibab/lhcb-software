// $Id: L0MuonCandidatesFromRaw.cpp,v 1.2 2006-10-20 14:48:55 cattanem Exp $
// Include files 
#include <boost/dynamic_bitset.hpp>

// from Gaudi
#include "GaudiKernel/AlgFactory.h"

// local
#include "L0MuonCandidatesFromRaw.h"

#include "L0MuonKernel/DecodeRawBufferCtrl.h"
#include "L0MuonKernel/Utilities.h"

//-----------------------------------------------------------------------------
// Implementation file for class : L0MuonCandidatesFromRaw
//
// 2003-12-15 : Julien Cogan
//-----------------------------------------------------------------------------

DECLARE_ALGORITHM_FACTORY( L0MuonCandidatesFromRaw );

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
L0MuonCandidatesFromRaw::L0MuonCandidatesFromRaw( const std::string& name,
                                                  ISvcLocator* pSvcLocator)
  : GaudiAlgorithm ( name , pSvcLocator )
{
  m_ptParameters.push_back(545.);  // 0-> Z(MAG)-Z(VTX)
  m_ptParameters.push_back(665.5); // 1-> Z(M1)-Z(MAG) 
  m_ptParameters.push_back(309.5); // 2-> Z(M2)-Z(M1)  
  m_ptParameters.push_back(1.39);  // 3-> alpha ? (kick ?)
  m_ptParameters.push_back(1210.); // 4-> Z position of M1 
  m_ptParameters.push_back(1527.); // 5-> Z position of M2 
  m_ptParameters.push_back(1647.); // 6-> Z position of M3 
  m_ptParameters.push_back(40.);   // 7-> Precision on PT (MeV)
  m_ptParameters.push_back(7.);    // 8-> Number of bins in PT
  declareProperty("PtParameters"       , m_ptParameters); 
}
//=============================================================================
// Destructor
//=============================================================================
L0MuonCandidatesFromRaw::~L0MuonCandidatesFromRaw() {}; 

//=============================================================================
// Initialisation. Check parameters
//=============================================================================
StatusCode L0MuonCandidatesFromRaw::initialize() {

  debug() << "==> Initialize" << endmsg;;

  // Init counters for final report
  m_totNumCand = 0 ; 
  m_totTrigEvt = 0 ; 
  m_totRawSize = 0 ; 
  m_totEvent   = 0 ;     
  m_averagePt  = 0. ; 
  m_averageCh  = 0. ; 
  return StatusCode::SUCCESS;
};

//=============================================================================
// Main execution
//=============================================================================
StatusCode L0MuonCandidatesFromRaw::execute() {

  debug() << "==> Execute" << endmsg;

  LHCb::L0MuonCandidates* pl0MuonCandidates = new LHCb::L0MuonCandidates();
  put(pl0MuonCandidates , LHCb::L0MuonCandidateLocation::Default );
  debug() << "==> Execute muoncandidates registered" << endmsg;

  LHCb::RawEvent* rawEvt = get<LHCb::RawEvent>( LHCb::RawEventLocation::Default );
  debug() << "==> Execute rawEvt instanciated" << endmsg;

  const std::vector<LHCb::RawBank*>& data = rawEvt->banks( LHCb::RawBank::L0Muon );
  if (data.size()==0) {
    error() << "No L0Muon bank found"<<endmsg;
    return StatusCode::FAILURE;
  }

  debug() << "==> Execute before loop over banks: data.size()= "<<data.size() << endmsg;
  // Loop over banks; search the one with sourceID=0 (TELL1 linked to the ctrl board)
  std::vector<LHCb::RawBank*>::const_iterator itBnk;
  bool bankfound=false;
  for (itBnk= data.begin(); itBnk<data.end(); itBnk++) {
    // Bank source id
    int sourceID = (*itBnk)->sourceID();
    debug() << "==> Execute   bank's source id is sourceID= "<<sourceID << endmsg;
    bankfound=true;
    if (sourceID==0) break;
    bankfound=false;
  } // End of Loop over banks
  if (!bankfound) {
    error() << "Bank with sourceID = 0 not found"<<endmsg;
    return StatusCode::FAILURE;
  }
    
  const unsigned int* ptData = (*itBnk)->data();
  int bankSize = (*itBnk)->size();

  // Fill a bit set with the bank content
  boost::dynamic_bitset<> rawbufbitset(bankSize*32);
  int bankCount=0;
  debug() << "==> Execute   before loop over bank's content; bankSize= "<< bankSize<< endmsg;
  while ( bankCount<bankSize ){
    unsigned long word = (*ptData++);
    boost::dynamic_bitset<> wordBitset(bankSize*32,word);
    wordBitset=wordBitset<<(bankCount*32);
    rawbufbitset|=wordBitset;
    bankCount++;
  }
  debug() << "==> Execute   after loop over bank's content "
          <<" size= " << rawbufbitset.size() <<" " 
          <<" ; # of bits set to= " << rawbufbitset.count() <<"\n" 
          << rawbufbitset << endmsg;
    
    
  // Decode the bitset
  L0Muon::DecodeRawBufferCtrl decodedRawBufCtrl ;
  decodedRawBufCtrl.setBitset(rawbufbitset);
  std::vector<L0Muon::MuonCandidate> candidatesCtrl = decodedRawBufCtrl.candidatesCTRL();
  int ncand = candidatesCtrl.size();
  debug() << "==> Execute    # of candidates sent to L0DU= " <<ncand << endmsg;
  // Loop over candidates
  int sumPt = 0;
  int sumCh = 0;
  std::vector<L0Muon::MuonCandidate>::iterator icand;
  for (icand=candidatesCtrl.begin(); icand<candidatesCtrl.end(); icand++) {
    // Prepare words to be sent to the L0DU
    int quarter = (*icand).quarter();
    int pt      = (*icand).pT();
    int addM3   = (*icand).addM3();
    int offM2   = (*icand).offM2();
    int offM1   = (*icand).offM1();
    int pu      = (*icand).pu();
    int board   = (*icand).board();
    int charge  = (*icand).charge();
        
    // For final report
    sumPt+=pt;
    sumCh+=charge>0 ? +1:-1;
        
    debug() <<"\n"<<(*icand).dump() <<endmsg;
        
    // Encoded pt 
    int encodedpt = charge>0 ? pt:-1*pt;
        
    // Retrieve MuonTileID of the pads in M1,M2 and M3
    std::vector<LHCb::MuonTileID> pads = address2Pads(quarter, board, pu, addM3, offM2, offM1);
    debug()  << "                pads M1 = " << pads[0].toString() <<endmsg;
    debug()  << "                pads M2 = " << pads[1].toString() <<endmsg;
    debug()  << "                pads M3 = " << pads[2].toString() <<endmsg;
    // Compute PT, theta and phi
    std::vector<double> kine = L0Muon::ptcalc(pads[0],pads[1],m_ptParameters,false);
    debug()  << "                kine pt    = " << kine[0] <<endmsg;
    debug()  << "                kine theta = " << kine[1] <<endmsg;
    debug()  << "                kine phi   = " << kine[2] <<endmsg;
    // Consistency check
    int sign = kine[0]>0 ? +1:-1;
    int recodedpt = int( (kine[0]+sign*m_ptParameters[7]/2.)/m_ptParameters[7] );
    if ( (sign*recodedpt<pow(2.,m_ptParameters[8])) && (recodedpt!=encodedpt) ) {
      error() << " MISMATCH ! encoded PT different from recomputed " 
              << encodedpt<<" -VS- "<<recodedpt<<" ( "<<kine[0]<<" )"<<endmsg;
    }
    
    // Fill the L0MuonCandidates container
    LHCb::L0MuonCandidate *pl0muoncandidate = new LHCb::L0MuonCandidate(kine[0], kine[1], kine[2], pads, encodedpt);
    debug()  << "               pl0muoncandidate " <<pl0muoncandidate <<endmsg;
    pl0MuonCandidates->insert(pl0muoncandidate);
  }// End of Loop over candidates 
  
  // Increment counters for final report
  if (m_totNumCand+ncand>0) {
    m_averagePt  = ((m_averagePt*m_totNumCand)+sumPt)/(m_totNumCand+ncand); 
    m_averageCh  = ((m_averageCh*m_totNumCand)+sumCh)/(m_totNumCand+ncand); 
  }
  m_totEvent++ ;     
  m_totNumCand +=ncand;
  for (itBnk= data.begin(); itBnk<data.end(); itBnk++) m_totRawSize+= (*itBnk)->size();
  
  if (ncand>0) m_totTrigEvt++; 

  debug() << "==> Execute return " << StatusCode::SUCCESS << endmsg;
  return StatusCode::SUCCESS;
};

//=============================================================================
//  Finalize
//=============================================================================
StatusCode L0MuonCandidatesFromRaw::finalize() {

  MsgStream msg(msgSvc(), name());
  info() << "- -------------------------------------------------------------------"<<endmsg;
  info() << "- =============> L0MuonCandidatesFromRaw final summary <============="<<endmsg;
  info() << "- Total number of events processed           : "
         <<format("%8d",m_totEvent)<<endmsg;
  if (0!=m_totEvent)
  {  
    info() << "- Total number of candidates                 : "
           <<format("%8d",m_totNumCand) <<endmsg;
    info() << "- Number of events with at least 1 candidate : "
           <<format("%8d",m_totTrigEvt) 
           <<format("  (%6.2f%)",(100.*m_totTrigEvt)/m_totEvent) << endmsg;
    info() << "- Number of candidate per event              : "
           <<format("      %5.2f",(1.*m_totNumCand)/m_totEvent) << endmsg;
    info() << "- Average bank size                          : "
           <<format("  %8.1f Bytes",(1.*m_totRawSize)/m_totEvent)<<endmsg;
    info() << "- Average Pt of the candidates               : "
           <<format("    %7.2f MeV",m_averagePt*m_ptParameters[7])<<endmsg;
    info() << "- Average charge of the candidates           : "
           <<format("      %5.2f (+/-%4.2f)",m_averageCh,sqrt((1-(m_averageCh*m_averageCh))/m_totNumCand))<<endmsg;
  }
  info() << "- -------------------------------------------------------------------"<<endmsg;

  return StatusCode::SUCCESS;
}

//=============================================================================

// Return the MID of the pads corresponding to the given addresses and offsets.
std::vector<LHCb::MuonTileID>   
L0MuonCandidatesFromRaw::address2Pads(int quarter,int board, int pu, int addM3, int offM2, int offM1){
  
  std::vector<LHCb::MuonTileID> pads;

  // MuonTileID of the board where the candidate was found
  int region   = board/3;
  int boardloc = 1+ board - region*3;
  int boardnx  =  boardloc     & 1;
  int boardny  = (boardloc>>1) & 1;  
  LHCb::MuonTileID boardID(0,MuonLayout(1,1),region,quarter,boardnx,boardny);

  // MuonTileID of the PU where the candidate was found
  int punx =  pu     & 1;
  int puny = (pu>>1) & 1;
  LHCb::MuonTileID puID(boardID,MuonLayout(2,2),punx,puny);

  // MuonTileID of the seed of the candidate (pad in M3)
  int M3nx = (addM3>>2) & 0x1F;
  int M3ny =  addM3     & 0x03;
  LHCb::MuonTileID padM3ID(puID,MuonLayout(48,8),M3nx,M3ny);
  padM3ID.setStation(2);
  
  // MuonTileID of the pad in M2 of the candidate
  // M2 Offset can negative
  if (offM2>=0x7) offM2 = offM2-0x10;
  int M2nx = M3nx+offM2;
  int M2ny = M3ny;
  LHCb::MuonTileID padM2ID(puID,MuonLayout(48,8),M2nx,M2ny);
  padM2ID.setStation(1);
  
  // If M2 pad is coming from a upper region, convert it
  if (!padM2ID.isValid()) {
    if ( padM2ID.nX()>=2*padM2ID.layout().xGrid() ) {
      std::vector<LHCb::MuonTileID> lpads =  padM2ID.layout().tilesInRegion(padM2ID,padM2ID.region());
      if (lpads.size()!=1) {
        error() <<" M2 PAD DOES NOT COVER EXACTLY ONE PAD IN UPPER REGION"<<endmsg;
        error() <<" padM2ID= "<<padM2ID.toString()<< "lpads.size()= "<<lpads.size()<<endmsg;
      }
      padM2ID = lpads[0];
    } else {
      //error()  << "M2 PAD IS NOT VALID: padM2ID= " << padM2ID.toString()  <<" ?? UNKNOWN CASE"<<endmsg;
      debug()  << "WARNING: M2 PAD IS NOT VALID: padM2ID= " << padM2ID.toString()  <<" ?? "<<endmsg;
    }
  }

  // Extrapolation in M1
  const int ExtrapolationM1[6]={0,+4,+7,+11,+14,+18};
  int extrapM1 = ExtrapolationM1[abs(offM2)];
  extrapM1 = offM2>0 ? extrapM1 : -1*extrapM1;

  // MuonTileID of the pad in M1 of the candidate
  // M1 Offset can negative (and expressed with M3 granularity)
  if (offM1>=0x7) offM1 = offM1-0x10;
  int M1nx = M3nx+extrapM1+offM1;
  int M1ny = M3ny;  

  // Switch to M1 granularity
  M1nx =  M1nx >> 1;
  LHCb::MuonTileID padM1ID(puID,MuonLayout(24,8),M1nx,M1ny);;
  padM1ID.setStation(0);

  // If M1 pad is coming from a upper region, convert it
  if (!padM1ID.isValid()) {
    if ( padM1ID.nX()>=2*padM1ID.layout().xGrid() ) {
      std::vector<LHCb::MuonTileID> lpads =  padM1ID.layout().tilesInRegion(padM1ID,padM1ID.region());
      if (lpads.size()!=1) {
        error() <<" M1 PAD DOES NOT COVER EXACTLY ONE PAD IN UPPER REGION"<<endmsg;
        error() <<" padM1ID= "<<padM1ID.toString()<< " ; lpads.size()= "<<lpads.size()<<endmsg;
      }
      padM1ID = lpads[0];
    } else {
      //error()  << "M1 PAD IS NOT VALID: padM1ID= " << padM1ID.toString()  <<" ?? UNKNOWN CASE"<<endmsg;
      debug()  << "WARNING: M1 PAD IS NOT VALID: padM1ID= " << padM1ID.toString()  <<" ?? "<<endmsg;
    }
  }

  // Fill the vector to be returned
  pads.push_back(padM1ID);
  pads.push_back(padM2ID);
  pads.push_back(padM3ID);
  
  return pads;
}

  
