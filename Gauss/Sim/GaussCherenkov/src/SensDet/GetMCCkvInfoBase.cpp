// $Id: GetMCCkvInfoBase.cpp,v 1.4 2009-10-23 10:47:46 jonrob Exp $
// Include files

// local
#include "GaussCherenkov/GetMCCkvInfoBase.h"
#include "GaussCherenkov/CkvGeometrySetupUtil.h"


//-----------------------------------------------------------------------------
// Implementation file for class : GetMCCkvInfoBase
//
// 2005-12-06 : Sajan EASO
//-----------------------------------------------------------------------------

//=============================================================================
// Standard constructor, initializes variables
//=============================================================================
GetMCCkvInfoBase::GetMCCkvInfoBase( const std::string& name,
                                      ISvcLocator* pSvcLocator)
  : Rich::AlgBase               ( name , pSvcLocator )
  , m_gigaSvc                 ( NULL )
  , m_gigaKineCnvSvc          ( NULL )
  , m_RichG4HitCollectionName ( NULL )
  , m_colRange                ( 2 )
  , m_RICHes                  ( Rich::NRiches, true )
  , m_relationTable           ( NULL )
  , m_SuperRichFlag           (false)
{
  declareProperty( "GiGaService",    m_gigaSvcName = "GiGa" );
  declareProperty( "KineCnvService", m_kineSvcName = IGiGaCnvSvcLocation::Kine );
  declareProperty( "RichDetectors",  m_RICHes );
}

//=============================================================================
// Initialization
//=============================================================================
StatusCode GetMCCkvInfoBase::initialize()
{
  const StatusCode sc = Rich::AlgBase::initialize();
  if ( sc.isFailure() ) return Error( "Failed to initialise", sc );
   CkvGeometrySetupUtil * aCkvGeometrySetup= CkvGeometrySetupUtil::getCkvGeometrySetupUtilInstance();
   m_SuperRichFlag = aCkvGeometrySetup ->isSuperRich();

  // initialise
  getRichG4CollectionRange();

  info() << "Filling '" << rootInTES() << dataLocationInTES() << "' from G4 collections";
  for ( int iii = colRange()[0]; iii < colRange()[1]+1 ; ++iii )
  {
    info() << " '" << RichG4HitCollectionName()->RichHCName(iii) << "'";
  }
  info() << endreq;

  if(!m_SuperRichFlag) {
    
     info() << "Using";
     if ( richIsActive(Rich::Rich1) ) info() << " Rich1";
     if ( richIsActive(Rich::Rich2) ) info() << " Rich2";
     info() << " Detectors" << endmsg;
  
  }else {
     info() << "Using";
      
     info() << " SuperRich Detector" << endmsg;
    
  }
  
  
  return sc;
}

// Builds collection info
void GetMCCkvInfoBase::getRichG4CollectionRange()
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
StatusCode GetMCCkvInfoBase::sysExecute()
{
  // reset pointers
  m_relationTable = NULL;
  // execute
  return Rich::AlgBase::sysExecute();
} 

//=============================================================================
//  Finalize
//=============================================================================
StatusCode GetMCCkvInfoBase::finalize()
{
  // clean up
  delete m_RichG4HitCollectionName;
  // return
  return Rich::AlgBase::finalize();
}

//=============================================================================
//  getMCRichHit
//=============================================================================
const LHCb::MCRichHit * GetMCCkvInfoBase::getMCRichHit( const CkvG4Hit* g4hit )
{
  // load the CkvG4Hit -> MCRichHit relations table
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
      Warning("More than one MCRichHit associated to a given CkvG4Hit").ignore();
    }
    mchit = range.begin()->to();
    if ( !mchit ) { Error( "Null MCRichHit pointer" ); }
  }
  
  // return
  return mchit;
}

//=============================================================================
