// $Id: PrepareL0DUL1Buffer.cpp,v 1.4 2005-03-07 16:30:49 cattanem Exp $
// Include files 

// from STL
#include <math.h> // For pow() on Windows

// from Gaudi
#include "GaudiKernel/AlgFactory.h"
#include "GaudiKernel/MsgStream.h" 

#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/IDataProviderSvc.h"

// L0Event
#include "Event/L0PuVeto.h"
#include "Event/L0MuonCandidate.h"
#include "Event/L0MuonBase.h"
#include "Event/L0DUTELL1Base.h"
#include "Event/L1Buffer.h"

// CLHEP
#include "CLHEP/Units/SystemOfUnits.h"
// local
#include "PrepareL0DUL1Buffer.h"

//-----------------------------------------------------------------------------
// Implementation file for class : PrepareL0DUL1Buffer
//
// 2004-01-13 : Olivier Deschamps
//-----------------------------------------------------------------------------

// Declaration of the Algorithm Factory
static const  AlgFactory<PrepareL0DUL1Buffer>          s_factory ;
const        IAlgFactory& PrepareL0DUL1BufferFactory = s_factory ; 


//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
PrepareL0DUL1Buffer::PrepareL0DUL1Buffer( const std::string& name,
                                          ISvcLocator* pSvcLocator)
  : Algorithm ( name , pSvcLocator ) 
  , m_nameOfInputL0MuonCandidate  ( L0MuonCandidateLocation::Default )
  , m_nameOfInputPileUpVeto       ( L0PuVetoLocation::Default        )
{
  declareProperty( "L0MuonCandidateData" , m_nameOfInputL0MuonCandidate );
  declareProperty( "PuVetoData"          , m_nameOfInputPileUpVeto      );
}

//=============================================================================
// Destructor
//=============================================================================
PrepareL0DUL1Buffer::~PrepareL0DUL1Buffer() {}; 

//=============================================================================
// Initialisation. Check parameters
//=============================================================================
StatusCode PrepareL0DUL1Buffer::initialize() {

  MsgStream msg(msgSvc(), name());
  msg << MSG::DEBUG << "==> Initialize" << endreq;

// ** check for the valid names of the input/output data containers
  if ( m_nameOfInputL0MuonCandidate.empty() ) { 
    msg <<  MSG::ERROR
        << "The name of the LOMuonCandidate data container is empty!" 
	<< endreq; 
    return StatusCode::FAILURE; 
  }
  if ( m_nameOfInputPileUpVeto.empty() ) { 
    msg <<  MSG::ERROR
        << "The name of the L0PuVeto data container is empty!" 
	<< endreq; 
    return StatusCode::FAILURE; 
  }
   return StatusCode::SUCCESS;
};

//=============================================================================
// Main execution
//=============================================================================
StatusCode PrepareL0DUL1Buffer::execute() {


  MsgStream  msg( msgSvc(), name() );
  //msg << MSG::DEBUG << "==> Execute" << endreq;


  // Get the input container : L0PuVeto
   SmartDataPtr<L0PuVeto>  L0PuVetoData
     ( eventSvc(), m_nameOfInputPileUpVeto );
   if ( 0 == L0PuVetoData ) {
     msg << MSG::DEBUG 
         << "Unable to retrieve L0PuVeto data  " 
         <<  m_nameOfInputPileUpVeto 
         << endreq;
   }
  // Get the input container : L0MuonCandidates
   SmartDataPtr<L0MuonCandidates> L0MuonData
     ( eventSvc() ,m_nameOfInputL0MuonCandidate ); 
   if ( 0 == L0MuonData  ) {
     msg << MSG::DEBUG
         << "Unable to retrieve L0MuonCandidates data "
         << m_nameOfInputL0MuonCandidate
         << endreq; 
  }
  // Get the L1 output buffer. Create it if needed
  L1Buffer* buffer;
  
  SmartDataPtr< L1Buffer > theL1Buffer ( eventSvc(), 
                                         L1BufferLocation::Default );
  if ( 0 == theL1Buffer ) {
    buffer = new L1Buffer();
    eventSvc()->registerObject( L1BufferLocation::Default, buffer );
  } else {
    buffer = theL1Buffer;
  }
  //-----------------------------//
  // Prepare L0Muon for L1Buffer //
  //-----------------------------//
  namespace Base = L0DUTELL1Base;
  L0MuonCandidates* L0Muons = (L0MuonCandidates*) L0MuonData;
  unsigned int NMus=0;
  l1_int MuonPosWord[8]={0,0,0,0,0,0,0,0};
  l1_int MuonContWord[8]={0,0,0,0,0,0,0,0};
  for(L0MuonCandidates::const_iterator iMuon = L0Muons->begin()
      ; iMuon != L0Muons->end(); iMuon++ ) {
    if( L0Muon::EMPTY != (*iMuon)->status() ){
      int m_bitcode = (*iMuon)->bitcode() ;

      // From L0Muon trigger word :
      int Status =(m_bitcode & L0MuonBase::MaskStatus)>>L0MuonBase::ShiftStatus;
      int Pt     =(m_bitcode & L0MuonBase::MaskPt    )>>L0MuonBase::ShiftPt;
      int Sign   =(m_bitcode & L0MuonBase::MaskPtSign)>>L0MuonBase::ShiftPtSign;
      int Region =(m_bitcode & L0MuonBase::MaskRegion)>>L0MuonBase::ShiftRegion;
      int PosX   =(m_bitcode & L0MuonBase::MaskX     )>>L0MuonBase::ShiftX;
      int PosY   =(m_bitcode & L0MuonBase::MaskY     )>>L0MuonBase::ShiftY;
      //  Quarter will be recover in L0DU board from hard cabling :
      // Use M1 pad soft index instead :
      int Quarter= (*iMuon)->padM1().quarter();

      msg << MSG::DEBUG << " -----------------------------------"<< endreq;
      msg << MSG::DEBUG << "Code Pt = " << (*iMuon)->pt()        << endreq;
      msg << MSG::DEBUG << "Code nX = " << (*iMuon)->padM1().nX()<< endreq;
      msg << MSG::DEBUG << "Code nY = " << (*iMuon)->padM1().nY()<< endreq;
      msg << MSG::DEBUG << " -----------------------------------"<< endreq;

      // Built L0DU Blocklet for Muon
      setWord(MuonPosWord[NMus],Region ,Base::ShiftMuR, Base::MaskMuR);
      setWord(MuonPosWord[NMus],PosX   ,Base::ShiftMuX, Base::MaskMuX);
      setWord(MuonPosWord[NMus],PosY   ,Base::ShiftMuY, Base::MaskMuY);
      setWord(MuonPosWord[NMus],Quarter,Base::ShiftMuQ, Base::MaskMuQ);
      setWord(MuonContWord[NMus],Status,Base::ShiftMuStatus,Base::MaskMuStatus);
      setWord(MuonContWord[NMus],Sign  ,Base::ShiftMuPtSign,Base::MaskMuPtSign);
      setWord(MuonContWord[NMus],Pt    ,Base::ShiftMuPt    ,Base::MaskMuPt    );
      NMus++;
    }
  }

  //-------------------------------//
  // Prepare L0PUVeto for L1Buffer //
  //-------------------------------//
  //  No L0PU word available. 
  //  Built digitized  components from L0PuVeto container.
  // 'Status' and 'more peak info' bits not included so far


  // Define inputs from L0PuVeto data
  double NPuHits  = L0PuVetoData->sTot();
  double SumPeak1 = L0PuVetoData->sumPeak1();
  double PosPeak1 = L0PuVetoData->zPosPeak1();
  double SumPeak2 = L0PuVetoData->sumPeak2();
  double PosPeak2 = L0PuVetoData->zPosPeak2();
  int bit;

  // Digitize # Hits ; 
  // LSB = 1 ; Dynamics = Base::BitPuHits
  bit = Base::BitPuHits;
  int DigNPuHits = int( (NPuHits < pow(2,bit)-1 )? NPuHits  : pow(2,bit)-1);

  // Digitize Peaks content ; 
  // LSB = 1 ; Dynamics = Base::BitPuSum
  bit = Base::BitPuSum;
  int DigSumPeak1= int( (SumPeak1 < pow(2,bit)-1)? SumPeak1 : pow(2,bit)-1);
  int DigSumPeak2= int( (SumPeak2 < pow(2,bit)-1)? SumPeak2 : pow(2,bit)-1);

  // Digitize Peak position ; 
  // LSB = Base::zPos_BIN_WIDTH ; Dynamics = Base::BitPuPos
  bit = Base::BitPuPos;
  double PosStep = Base::zPos_BIN_WIDTH;
  double Range = ( PosStep * pow(2,bit) ) / 2. ;
  int DigPosPeak1 =int((Range + PosPeak1)/PosStep);
  DigPosPeak1 = (DigPosPeak1 > 0)? DigPosPeak1 : 0;
  DigPosPeak1 = int( (DigPosPeak1 < pow(2,bit)-1)? DigPosPeak1 : pow(2,bit)-1);
  int DigPosPeak2 =int( (Range + PosPeak2)/PosStep);
  DigPosPeak2 = (DigPosPeak2 > 0)? DigPosPeak2 : 0;
  DigPosPeak2 = int( (DigPosPeak2 < pow(2,bit)-1)? DigPosPeak2 : pow(2,bit)-1);
  
  // Built L0DU Blocklet for PuVeto from digitized data
  l1_int Peak1Word =0;
  setWord( Peak1Word, DigPosPeak1 ,Base::ShiftPuPos, Base::MaskPuPos);
  setWord( Peak1Word, DigSumPeak1 ,Base::ShiftPuSum, Base::MaskPuSum);
  l1_int Peak2Word =0;
  setWord( Peak2Word, DigPosPeak2 ,Base::ShiftPuPos, Base::MaskPuPos);
  setWord( Peak2Word, DigSumPeak2 ,Base::ShiftPuSum, Base::MaskPuSum);

  //---------------------------------//
  // Prepare L0DUHeader for L1Buffer //
  //---------------------------------//
  l1_int L0DUHeaderWord=0;
  setWord( L0DUHeaderWord, DigNPuHits ,Base::ShiftPuHits, Base::MaskPuHits );
  setWord( L0DUHeaderWord, NMus  ,Base::ShiftMuons , Base::MaskMuons  );

  msg << MSG::DEBUG << " -----------------------------------"<< endreq;
  msg << MSG::DEBUG << "NPUhits   = " << DigNPuHits << endreq;
  msg << MSG::DEBUG << "NMuons    = " << NMus <<endreq;
  msg << MSG::DEBUG << "Peak1 Pos = " << DigPosPeak1 << endreq;
  msg << MSG::DEBUG << "Peak1 Sum = " << DigSumPeak1 << endreq;
  msg << MSG::DEBUG << "Peak2 Pos = " << DigPosPeak2 << endreq;
  msg << MSG::DEBUG << "Peak2 Sum = " << DigSumPeak2 << endreq;
  msg << MSG::DEBUG << " -----------------------------------"<< endreq;

  //----------------//
  // Fill L1 Buffer //
  //----------------//
  std::vector<l1_int> data;
  
  int sourceID = L1Buffer::L0DUID;
  int bankType = L1Buffer::L0;

  data.push_back( L0DUHeaderWord );
  data.push_back( Peak1Word      );
  data.push_back( Peak2Word      );
  for(unsigned int imu=0;imu!=NMus; imu++){
    data.push_back(MuonPosWord[imu]);
    data.push_back(MuonContWord[imu]);
  }
  buffer->addBank( sourceID, bankType, data );

  return StatusCode::SUCCESS;
};
//=============================================================================
void PrepareL0DUL1Buffer::setWord(l1_int& word,
                                 int value, 
                                 int shift, 
                                 int mask) {
  int tmp1 , tmp2 ;
  tmp1 = ( value << shift ) & mask ; 
  tmp2 = word & ~mask ; 
  word  = ( tmp1 | tmp2 ) ;			     
}
//=============================================================================
//  Finalize
//=============================================================================
StatusCode PrepareL0DUL1Buffer::finalize() {

  MsgStream msg(msgSvc(), name());
  msg << MSG::DEBUG << "==> Finalize" << endreq;


  return StatusCode::SUCCESS;
}

//=============================================================================
