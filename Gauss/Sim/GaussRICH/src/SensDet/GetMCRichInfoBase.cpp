// $Id: GetMCRichInfoBase.cpp,v 1.1 2005-12-22 16:42:43 jonrob Exp $
// Include files

// local
#include "GetMCRichInfoBase.h"

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
  : RichAlgBase               ( name , pSvcLocator )
  , m_gigaSvc                 ( 0 )
  , m_gigaKineCnvSvc          ( 0 )
  , m_RichG4HitCollectionName ( 0 )
  , m_colRange                ( std::vector<int>(2) )
{
  declareProperty( "GiGaService",    m_gigaSvcName = "GiGa" );
  declareProperty( "KineCnvService", m_kineSvcName = IGiGaCnvSvcLocation::Kine );
}

//=============================================================================
// Initialization
//=============================================================================
StatusCode GetMCRichInfoBase::initialize()
{
  const StatusCode sc = RichAlgBase::initialize();
  if ( sc.isFailure() ) return Error( "Failed to initialise", sc );

  // initialise
  getRichG4CollectionRange();

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
//  Finalize
//=============================================================================
StatusCode GetMCRichInfoBase::finalize()
{
  debug() << "Finalize" << endmsg;

  // clean up
  delete m_RichG4HitCollectionName;

  return RichAlgBase::finalize();
}

//=============================================================================
