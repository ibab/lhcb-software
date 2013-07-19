// $Id: GetMCRichInfoBase.cpp,v 1.4 2009-10-23 10:47:46 jonrob Exp $
// Include files

// local
#include "GaussRICH/GetMCRichInfoBase.h"

//-----------------------------------------------------------------------------
// Implementation file for class : GetMCRichInfoBase
//
// 2005-12-06 : Sajan EASO
//-----------------------------------------------------------------------------

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
GetMCRichInfoBase::GetMCRichInfoBase( const std::string& name,
                                      ISvcLocator* pSvcLocator)
  : Rich::AlgBase             ( name , pSvcLocator )
  , m_gigaSvc                 ( NULL )
  , m_gigaKineCnvSvc          ( NULL )
  , m_RichG4HitCollectionName ( NULL )
  , m_colRange                ( 2 )
  , m_RICHes                  ( Rich::NRiches, true )
  , m_relationTable           ( NULL )
  , m_deRichSys               ( NULL )
{
  declareProperty( "GiGaService",    m_gigaSvcName = "GiGa" );
  declareProperty( "KineCnvService", m_kineSvcName = IGiGaCnvSvcLocation::Kine );
  declareProperty( "RichDetectors",  m_RICHes );
}

//=============================================================================
// Initialization
//=============================================================================
StatusCode GetMCRichInfoBase::initialize()
{
  const StatusCode sc = Rich::AlgBase::initialize();
  if ( sc.isFailure() ) return Error( "Failed to initialise", sc );

  // initialise
  getRichG4CollectionRange();

  info() << "Filling '" << rootInTES() << dataLocationInTES() << "' from G4 collections";
  for ( int iii = colRange()[0]; iii < colRange()[1]+1 ; ++iii )
  {
    info() << " '" << RichG4HitCollectionName()->RichHCName(iii) << "'";
  }
  info() << endreq;

  info() << "Using";
  if ( richIsActive(Rich::Rich1) ) info() << " Rich1";
  if ( richIsActive(Rich::Rich2) ) info() << " Rich2";
  info() << " Detectors" << endmsg;

  return sc;
}

// Builds collection info
void GetMCRichInfoBase::getRichG4CollectionRange()
{
  int collection_size  = RichG4HitCollectionName()->RichHCSize();
  int collection_init  = RichG4HitCollectionName()->InitCollectListNumForOutput();
  int collection_final = RichG4HitCollectionName()->FinalCollectListNumForOutput();
  if ( (collection_init < 0)                ||
       (collection_init > collection_final) ||
       (collection_init >= collection_size) )
  {
    warning() << "Inappropriate setting of Rich Collection list init "
              << collection_init << endmsg;
    collection_init = 0;
  }
  if ( (collection_final < collection_init)  ||
       (collection_final < 0)                ||
       (collection_final >= collection_size) )
  {
    warning() << "Inappropriate setting of Rich Collection list final "
              << collection_init << endmsg;
    collection_final = collection_size;
  }
  if ( (collection_init !=0)                     ||
       (collection_final != (collection_size-1)) )
  {
    warning() << "Only partial storage of Rich Hits for Output: init final = "
              << collection_init << " " << collection_final << endmsg;
  }
  m_colRange[0]=collection_init;
  m_colRange[1]=collection_final;
}

//=============================================================================
//  sysExecute
//=============================================================================
StatusCode GetMCRichInfoBase::sysExecute()
{
  // reset pointers
  m_relationTable = NULL;
  // execute
  return Rich::AlgBase::sysExecute();
} 

//=============================================================================
//  Finalize
//=============================================================================
StatusCode GetMCRichInfoBase::finalize()
{
  // clean up
  delete m_RichG4HitCollectionName;
  // return
  return Rich::AlgBase::finalize();
}

//=============================================================================
//  getMCRichHit
//=============================================================================
const LHCb::MCRichHit * GetMCRichInfoBase::getMCRichHit( const RichG4Hit* g4hit )
{
  // load the RichG4Hit -> MCRichHit relations table
  if ( !m_relationTable )
  {
    m_relationTable = get<G4HitTable>( g4HitToMCRichHitRelLoc() );
  }

  // Find associated MCRichHit using relations table
  const LHCb::MCRichHit * mchit(NULL);
  
  // get relations
  const G4HitTable::Range range = m_relationTable->relations(g4hit);
  // If empty, this just means no MCRichHit was created for this G4 hit, so skip
  if ( !range.empty() )
  {
    // If not empty, size should be precisely one
    if ( range.size() != 1 ) 
    {
      Warning("More than one MCRichHit associated to a given RichG4Hit").ignore();
    }
    mchit = range.begin()->to();
    if ( !mchit ) { Error( "Null MCRichHit pointer" ); }
  }
  
  // return
  return mchit;
}

//=============================================================================
