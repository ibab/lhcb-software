// Include files

// GaudiKernel
#include "GaudiKernel/IDataProviderSvc.h"
#include "GaudiKernel/IUpdateManagerSvc.h"

#include "GaudiKernel/Kernel.h"
#include "GaudiKernel/IRegistry.h"
#include "GaudiKernel/DataObject.h"
#include "GaudiKernel/SmartDataPtr.h"
#include "GaudiKernel/MsgStream.h"
// DetDesc
#include "DetDesc/IPVolume_predicates.h"
#include "DetDesc/DetDesc.h"
#include "DetDesc/AlignmentCondition.h"
// STD and STL
#include <numeric>
#include <iterator>

#include "DetDesc/GeometryInfoPlus.h"


//-----------------------------------------------------------------------------
/// Implementation file for class : GeometryInfoPlus
/// Developed from Vanya's implementation GeometryInfo.
/// Needs to use standard "ideal" positions of PVolumes
/// as defined in XML detector description AND "deltas to
/// these volumes, as given in CondDB or XML conditions
/// catalogue.
/// Will need to hold containers of transformation matrices
/// of parents in order to obtain global transformations.
///
/// 2005-04-28 : Juan PALACIOS
//-----------------------------------------------------------------------------

//=============================================================================
// create "ghost"
GeometryInfoPlus::GeometryInfoPlus(IDetectorElement* de)
  :
  m_gi_has_logical      (     false   ),
  m_gi_iDetectorElement (      de     )
{
  if ( !de  )
  { throw GeometryInfoException("IDetectorElement* points to NULL!") ; }
  if ( initialize().isFailure() )
  { throw GeometryInfoException("Failed to initialize!") ; }
}
//=============================================================================
// create orphan
GeometryInfoPlus::GeometryInfoPlus(IDetectorElement*  de,
                                   const std::string& LogVol)
  :
  m_gi_lvolumeName      (   LogVol    ),
  m_gi_iDetectorElement (      de     )
{
  if ( !de  )
  { throw GeometryInfoException("IDetectorElement* points to NULL!") ; }
  if ( initialize().isFailure() )
  { throw GeometryInfoException("Failed to initialize!") ; }
}
//=============================================================================
/// create regular  with numeric replica path
GeometryInfoPlus::GeometryInfoPlus(IDetectorElement*            de,
                                   const std::string&           LogVol,
                                   const std::string&           Support,
                                   const ILVolume::ReplicaPath& replicaPath,
                                   const std::string& alignmentPath          )
  :
  m_gi_lvolumeName      (   LogVol    ),
  m_alignmentPath       (alignmentPath),
  m_gi_has_support      (    true     ),
  m_gi_supportName      (   Support   ),
  m_gi_supportPath      ( replicaPath ),
  m_gi_iDetectorElement (      de     )
{
  if ( !de  )
  { throw GeometryInfoException("IDetectorElement* points to NULL!"    ) ; }

  if ( initialize().isFailure() )
  { throw GeometryInfoException("Failed to initialize!") ; }
}
//=============================================================================
/// create regular  with name path
GeometryInfoPlus::GeometryInfoPlus( IDetectorElement*  de,
                                    const std::string& LogVol,
                                    const std::string& Support,
                                    const std::string& ReplicaNamePath,
                                    const std::string& alignmentPath   )
  :
  m_gi_lvolumeName      (   LogVol        ),
  m_alignmentPath       ( alignmentPath   ),
  m_gi_has_support      (    true         ),
  m_gi_supportName      (   Support       ),
  m_gi_supportNamePath  ( ReplicaNamePath ),
  m_gi_iDetectorElement (      de         )
{
  if ( !de  )
  { throw GeometryInfoException("IDetectorElement* points to NULL!"    ) ; }

  if ( initialize().isFailure() )
  { throw GeometryInfoException("Failed to initialize!") ; }
}
//=============================================================================
StatusCode GeometryInfoPlus::initialize()
{

  m_services = DetDesc::services();
  const bool create = true;
  auto * ums = updMgrSvc(create);

  m_hasAlignmentPath = !m_alignmentPath.empty();

  if( !m_log ) m_log.reset( new MsgStream(msgSvc(), "GeometryInfoPlus") );

  VERBO << "initialize" << endmsg;
  VERBO << "alignment path " << m_alignmentPath << endmsg;

  if ( m_hasAlignmentPath )
  {
    this->hasAlignmentCondition(true);
    VERBO << "Using AlignmentCondition with path " << m_alignmentPath << endmsg;
  }
  else
  {
    this->hasAlignmentCondition(false);
    m_alignmentCondition = nullptr;
    VERBO << "No AlignmentCondition requested. Assigning identity transformation"
          << endmsg;
  }

  return registerCondition() && registerSupportGI() && ums->update(this);
}
//=============================================================================
bool GeometryInfoPlus::hasLVolume() const
{
  return m_gi_has_logical;
}
//=============================================================================
bool GeometryInfoPlus::hasSupport() const
{
  return m_gi_has_support;
}
//=============================================================================
const AlignmentCondition* GeometryInfoPlus::alignmentCondition() const
{
  return m_alignmentCondition;
}
//=============================================================================
const Gaudi::Transform3D& GeometryInfoPlus::toLocalMatrix() const
{
  return  *m_matrix;
}
//=============================================================================
const Gaudi::Transform3D& GeometryInfoPlus::toGlobalMatrix() const
{
  return *m_matrixInv;
}
//=============================================================================
const Gaudi::Transform3D& GeometryInfoPlus::toLocalMatrixNominal() const
{
  return *m_idealMatrix;
}
//=============================================================================
const Gaudi::Transform3D& GeometryInfoPlus::toGlobalMatrixNominal() const
{
  return *m_idealMatrixInv;
}
//=============================================================================
Gaudi::Transform3D GeometryInfoPlus::ownMatrix() const
{
  return Gaudi::Transform3D( this->ownToNominalMatrix() * this->ownToLocalMatrixNominal() );
}
//=============================================================================
Gaudi::XYZPoint GeometryInfoPlus::toLocal( const Gaudi::XYZPoint& globalPoint ) const
{
  auto localPoint = toLocalMatrix() * globalPoint;
  // Due to LHCb geometry, many measurements have Z=0 in local frame
  // Next line recovers precision lost in transformations, particularly on 32-bit
  if( fabs(localPoint.Z()) < 1.e-10 ) localPoint.SetZ(0.);
  return localPoint;
}
//=============================================================================
Gaudi::XYZPoint GeometryInfoPlus::toGlobal( const Gaudi::XYZPoint& localPoint  ) const
{
  return ( toGlobalMatrix() * localPoint  );
}
//=============================================================================
Gaudi::XYZVector GeometryInfoPlus::toLocal( const Gaudi::XYZVector& globalDirection ) const
{
  return ( toLocalMatrix() * globalDirection );
}
//=============================================================================
Gaudi::XYZVector GeometryInfoPlus::toGlobal( const Gaudi::XYZVector& localDirection  ) const
{
  return ( toGlobalMatrix() * localDirection  );
}
//=============================================================================
bool GeometryInfoPlus::isInside( const Gaudi::XYZPoint& globalPoint ) const
{
  return ( ( hasLVolume() && lvolume() ) ?
           lvolume()->isInside( toLocal( globalPoint ) ) :
           isInsideDaughter( globalPoint ) );
}
//=============================================================================
StatusCode GeometryInfoPlus::cache()
{
  VERBO << "cache() calculating matrices" << endmsg;
  return calculateMatrices();

}
//=============================================================================
StatusCode GeometryInfoPlus::calculateMatrices()
{

  VERBO << "calculateMatrices: clearing matrices" << endmsg;
  clearMatrices();

  IGeometryInfo* gi = this;
  std::vector<IGeometryInfo*> parentGeomInfos;

  VERBO << "Welcome to calculateMatrices for volume "
        << gi->lvolumeName() << " support " << m_gi_supportName
        << endmsg;

  do
  {
    Assert( gi,
            " GeometryInfoPlus::calculateMatrices() - support is not found!");
    Assert( gi->lvolume(),
            " GeometryInfoPlus::calculateMatrices() - support has no LVolume!");

    VERBO << "storing matrix for volume " << gi->lvolumeName() << endmsg;

    if (!idealMatrixLoaded())
    {
      VERBO << "store ideal..." << endmsg;
      m_pvMatrices.push_back( gi->ownToLocalMatrixNominal() );
    }
    else
    {
      VERBO << "ideal already stored..." << endmsg;
    }

    VERBO << "store delta for " << endmsg;
    m_deltaMatrices.push_back( gi->ownToNominalMatrix() );

    parentGeomInfos.push_back(gi);

    VERBO << "get support IGeometryInfo* for "
          << gi->lvolumeName() << endmsg;

    gi = gi->supportIGeometryInfo();

  } while ( nullptr != gi );

  VERBO << "calculateMatrices: stored "
        << m_pvMatrices.size() << " ideal and "
        << m_deltaMatrices.size() << " delta matrices for volumes "
        << endmsg;

  Assert(m_pvMatrices.size()==m_deltaMatrices.size(),
         "GeometryInfoPlus::calculateMatrices: \n different number of ideal and delta matrices");

  if ( isVerbose() )
  {
    for ( const auto * info : parentGeomInfos )
    {
      VERBO << "\t" << info->lvolumeName() << endmsg;
    }
  }

  return combineMatrices(deltaBegin(), deltaEnd(),
                         idealBegin(), idealEnd());

}
//=============================================================================
StatusCode GeometryInfoPlus::combineMatrices(matrix_iterator deltaFirst,
                                             matrix_iterator deltaEnd,
                                             matrix_iterator pvFirst,
                                             matrix_iterator pvEnd)
{
  if ( !idealMatrixLoaded() ) calculateIdealMatrix(pvFirst, pvEnd);
  return calculateFullMatrices(deltaFirst, deltaEnd, pvFirst);
}
//=============================================================================
StatusCode GeometryInfoPlus::calculateFullMatrices(matrix_iterator deltaFirst,
                                                   matrix_iterator deltaEnd,
                                                   matrix_iterator pvFirst)
{

  Gaudi::Transform3D init;
  m_matrix.reset( new Gaudi::Transform3D( std::inner_product(deltaFirst,
                                                             deltaEnd,
                                                             pvFirst,
                                                             init,
                                                             std::multiplies<Gaudi::Transform3D>(),
                                                             std::multiplies<Gaudi::Transform3D>()
                                                             ) )
                  );

  // Recover precision in cases of rotations by pi or halfpi
  double xx, xy, xz, dx, yx, yy, yz, dy, zx, zy, zz, dz;
  m_matrix->GetComponents( xx, xy, xz, dx, yx, yy, yz, dy, zx, zy, zz, dz );
  if( fabs(xx) < 1.e-15 ) xx = 0.;
  if( fabs(xy) < 1.e-15 ) xy = 0.;
  if( fabs(xz) < 1.e-15 ) xz = 0.;
  if( fabs(dx) < 1.e-12 ) dx = 0.;
  if( fabs(yx) < 1.e-15 ) yx = 0.;
  if( fabs(yy) < 1.e-15 ) yy = 0.;
  if( fabs(yz) < 1.e-15 ) yz = 0.;
  if( fabs(dy) < 1.e-12 ) dy = 0.;
  if( fabs(zx) < 1.e-15 ) zx = 0.;
  if( fabs(zy) < 1.e-15 ) zy = 0.;
  if( fabs(zz) < 1.e-15 ) zz = 0.;
  if( fabs(dz) < 1.e-12 ) dz = 0.;
  m_matrix->SetComponents( xx, xy, xz, dx, yx, yy, yz, dy, zx, zy, zz, dz );

  m_matrixInv.reset( new Gaudi::Transform3D( toLocalMatrix().Inverse() ) );

  // Recover precision
  m_matrixInv->GetComponents( xx, xy, xz, dx, yx, yy, yz, dy, zx, zy, zz, dz );
  if( fabs(xx) < 1.e-15 ) xx = 0.;
  if( fabs(xy) < 1.e-15 ) xy = 0.;
  if( fabs(xz) < 1.e-15 ) xz = 0.;
  if( fabs(dx) < 1.e-12 ) dx = 0.;
  if( fabs(yx) < 1.e-15 ) yx = 0.;
  if( fabs(yy) < 1.e-15 ) yy = 0.;
  if( fabs(yz) < 1.e-15 ) yz = 0.;
  if( fabs(dy) < 1.e-12 ) dy = 0.;
  if( fabs(zx) < 1.e-15 ) zx = 0.;
  if( fabs(zy) < 1.e-15 ) zy = 0.;
  if( fabs(zz) < 1.e-15 ) zz = 0.;
  if( fabs(dz) < 1.e-12 ) dz = 0.;
  m_matrixInv->SetComponents( xx, xy, xz, dx, yx, yy, yz, dy, zx, zy, zz, dz );

  VERBO << "calculated full matrices" << endmsg;

  return StatusCode::SUCCESS;

}
//=============================================================================
void GeometryInfoPlus::calculateIdealMatrix(matrix_iterator pvFirst,
                                            matrix_iterator pvEnd)
{
  Gaudi::Transform3D init;

  m_idealMatrix.reset( new Gaudi::Transform3D( std::accumulate(pvFirst,
                                                               pvEnd,
                                                               init,
                                                               std::multiplies<Gaudi::Transform3D>())) );
  m_idealMatrixInv.reset( new Gaudi::Transform3D( m_idealMatrix->Inverse()) );

}
//=============================================================================
StatusCode GeometryInfoPlus::ownToOffNominalMatrix(const Gaudi::Transform3D& newDelta)
{
  // Need to do this depending on whether there is an
  // AlignmentCondition present. So check for that, and if there is,
  // also change the matrix in the AlignmentCondition data member.
  if (this->hasAlignmentCondition()) myAlignmentCondition()->offNominalMatrix(newDelta );
  return setLocalOffNominalDeltaMatrix(newDelta);
}
//=============================================================================
StatusCode GeometryInfoPlus::ownToOffNominalParams(const std::vector<double>& trans,
                                                   const std::vector<double>& rot,
                                                   const std::vector<double>& pivot)
{
  // should also make children re-calculate matrices by calling their
  // calculateMatrices methods iteratively.

  return ( this->hasAlignmentCondition() ?
           ( myAlignmentCondition()->setOffNominalTransformation(trans, rot, pivot)    ?
             setLocalOffNominalDeltaMatrix(myAlignmentCondition()->offNominalMatrix()) :
             StatusCode::FAILURE ) : StatusCode::FAILURE );
}

//=============================================================================
StatusCode GeometryInfoPlus::setLocalOffNominalDeltaMatrix(const Gaudi::Transform3D&
                                                           newDelta)
{

  // should also make children re-calculate matrices by calling their
  // calculateMatrices methods iteratively.

  if ( m_deltaMatrices.empty() )
  {
    log() << MSG::WARNING << "setLocalDeltaMatrix set failed!" << endmsg;
    return StatusCode::FAILURE;
  }

  VERBO << "updating local delta matrix" << endmsg;
  m_localDeltaMatrix.reset( new Gaudi::Transform3D(newDelta.Inverse()) );
  m_deltaMatrices[0] = *m_localDeltaMatrix;

  if (!calculateFullMatrices(deltaBegin(),
                             deltaEnd(),
                             idealBegin()) ) return StatusCode::FAILURE;

  auto *ums = updMgrSvc();
  ums->invalidate(this);
  return ums->newEvent();
}
//=============================================================================
StatusCode GeometryInfoPlus::updateChildren()
{
  for ( auto gi = this->childBegin(); gi != this->childEnd(); ++gi )
  {
    (*gi)->cache();
    (*gi)->updateChildren();
  }
  return StatusCode::SUCCESS;
}
//=============================================================================
void GeometryInfoPlus::clearMatrices()
{

  if (!m_pvMatrices.empty()) m_pvMatrices.clear();
  if (!m_deltaMatrices.empty()) m_deltaMatrices.clear();

  m_matrix           .reset( nullptr );
  m_idealMatrix      .reset( nullptr );
  m_localIdealMatrix .reset( nullptr );
  m_localDeltaMatrix .reset( nullptr );
  m_matrixInv        .reset( nullptr );
  m_idealMatrixInv   .reset( nullptr );

}
//=============================================================================
// FIXME: to be removed
StatusCode GeometryInfoPlus::getAlignmentCondition()
{
  return StatusCode::SUCCESS;
}
//=============================================================================
StatusCode GeometryInfoPlus::registerCondition()
{
  VERBO << "registerCondition" << endmsg;
  updMgrSvc()->registerCondition(this,
                                 m_alignmentPath,
                                 &GeometryInfoPlus::cache,
                                 m_alignmentCondition);
  return StatusCode::SUCCESS;
}
//=============================================================================
StatusCode GeometryInfoPlus::registerSupportGI()
{
  VERBO << "registerSupportGI" << endmsg;

  auto * gi = this->supportIGeometryInfo();

  //   return (gi) ? m_ums->registerCondition(this, gi, &IGeometryInfo::cache) :
  //     StatusCode::SUCCESS;
  if (gi)
  {
    VERBO << "register parent GI!" << endmsg;
    updMgrSvc()->registerCondition(this, gi, &IGeometryInfo::cache);
    VERBO << "Registered" << endmsg;
  }
  else
  {
    VERBO << "No parent " << endmsg;
  }

  return StatusCode::SUCCESS;
}
//=============================================================================
const Gaudi::Transform3D& GeometryInfoPlus::ownToLocalMatrixNominal() const
{

  if ( m_localIdealMatrix )
  {
    VERBO << "Ideal matrix for " << m_gi_supportNamePath
          << " already present" << endmsg;
    return *m_localIdealMatrix;
  }

  auto * gi = supportIGeometryInfo();

  if ( ( !gi || !hasLVolume()) || !hasSupport() ||
       ( this == ( const IGeometryInfo*) gi ) )
  {
    VERBO << "localIdealMatrix: assigning identity matrix " << endmsg;
    m_localIdealMatrix.reset( new Gaudi::Transform3D() );
  }
  else
  {
    const auto * lv = gi->lvolume();

    ILVolume::PVolumePath volumePath;

    StatusCode sc = lv->traverse( supportPath().begin(),
                                  supportPath().end(),
                                  volumePath );

    Assert( sc.isSuccess()  ,
            " GeometryInfoPlus::localIdealMatrix(): could not traverse the path!");
    m_localIdealMatrix.reset( accumulateMatrices(volumePath) );
  }

  return  *m_localIdealMatrix;

}
//=============================================================================
const Gaudi::Transform3D& GeometryInfoPlus::ownToNominalMatrix() const
{
  if ( m_localDeltaMatrix ) return *m_localDeltaMatrix;

  m_localDeltaMatrix.reset( this->hasAlignmentCondition() ?
                            new Gaudi::Transform3D(myAlignmentCondition()->toNominalMatrix()) :
                            new Gaudi::Transform3D() );

  return *m_localDeltaMatrix;

}
//=============================================================================
Gaudi::Transform3D GeometryInfoPlus::ownToOffNominalMatrix() const
{
  return ownToNominalMatrix().Inverse();
}
//=============================================================================
IGeometryInfo* GeometryInfoPlus::supportIGeometryInfo() const
{

  if ( !hasSupport() ) return nullptr;

  VERBO << "supportIGeometryInfo with support "
        << m_gi_supportName << endmsg;

  if ( !m_gi_support )
  {
    IGeometryInfo* gi = nullptr;
    if ( !m_gi_supportName.empty() )
    {
      gi = geoByName( m_gi_supportName );
    }
    m_gi_support = gi;
  }

  if ( isVerbose() )
  {
    if ( m_gi_support )
    {
      VERBO << "supportIGeometryInfo return IGeometryInfo* with volume "
            << m_gi_support->lvolumeName() << endmsg;
    }
    else
    {
      VERBO << "supportIGeometryInfo found no IGeometryInfo* with support "
            << m_gi_supportName << endmsg;
    }
  }

  return  m_gi_support;

}
//=============================================================================
Gaudi::Transform3D* 
GeometryInfoPlus::accumulateMatrices(const ILVolume::PVolumePath& volumePath) const
{
  const Gaudi::Transform3D init;
  return new Gaudi::Transform3D( std::accumulate( volumePath.begin() ,
                                                  volumePath.end  () ,
                                                  init               ,
                                                  IPVolume_accumulateMatrix() ) );
}

//=============================================================================
const ILVolume::ReplicaPath& GeometryInfoPlus::supportPath() const
{
  VERBO << "supportPath is " << m_gi_supportName << endmsg;

  if ( !m_gi_has_support || !m_gi_supportPath.empty() )
  {
    return m_gi_supportPath;
  }

  // try to extract the replica path from its name (parse the name path)
  if( !m_gi_support ) { m_gi_support = geoByName( m_gi_supportName ) ; }
  ///
  const ILVolume* lv = m_gi_support->lvolume();
  std::string aux( m_gi_supportNamePath );
  while( !aux.empty() && lv )
  {
    // find separator
    std::string::size_type pos     = aux.find_first_of('/');
    std::string            physVol ("") ;
    if( std::string::npos == pos )
    { physVol  = aux                   ; aux=""                       ; }
    else
    { physVol = aux.substr( 0 , pos )  ; aux = aux.substr( pos + 1  ) ; }
    if( !physVol.empty() )
    {
      // get physical volume by name
      const auto * pv = (*lv)[ physVol ];
      // find this name in the sequence
      auto it = std::find( lv->pvBegin() , lv->pvEnd() , pv ) ;
      Assert( lv->pvEnd() != it ,
              "GeometryInfoPlus:: error during retrieve of Replica Path" );
      // extract index and put it into replica path

      m_gi_supportPath.push_back( it - lv->pvBegin() );
      lv = (*it)->lvolume();
    }
  }
  //
  return m_gi_supportPath;
  //
}
//=============================================================================
IGeometryInfo* GeometryInfoPlus::geoByName( const std::string& name ) const
{
  VERBO << "geoByName name is " << name << endmsg;

  IGeometryInfo* gi = nullptr;
  SmartDataPtr<IDetectorElement> ptr( dataSvc() , name );
  if ( ptr )
  {
    gi = ptr->geometry();
    VERBO << "geoByName: Found detector element for " << name << endmsg;
  }

  return gi;
}
//=============================================================================
StatusCode GeometryInfoPlus::fullGeoInfoForPoint
( const Gaudi::XYZPoint&        point      ,
  const int                level      ,
  IGeometryInfo*&          start      ,
  ILVolume::PVolumePath&   volumePath )
{
  // reset output values
  start = nullptr         ;
  if( !volumePath.empty() )            { volumePath.clear();          }
  if( !isInside( point )  )            { return StatusCode::FAILURE ; }
  //
  auto * gi = belongsTo( point , -1 );
  if( !gi || !gi->lvolume() )  { return StatusCode::FAILURE ; }
  start = gi;
  //
  return gi->lvolume()->belongsTo( gi->toLocal( point )  ,
                                   level , volumePath );
}
//=============================================================================
StatusCode GeometryInfoPlus::fullGeoInfoForPoint
( const Gaudi::XYZPoint&        point      ,
  const int                level      ,
  std::string&             start      ,
  ILVolume::PVolumePath&   volumePath )
{
  // reset output values
  start = std::string("");
  if( !volumePath.empty() ) { volumePath.clear()       ; }
  if( !isInside( point ) ) { return StatusCode::FAILURE; }
  //
  auto * gi = belongsTo    ( point , -1 );
  if( !gi || !gi->lvolume() ) { return StatusCode::FAILURE ; }
  start             = belongsToPath( point , -1 );
  //
  return gi->lvolume()->belongsTo( gi->toLocal( point ) ,
                                   level , volumePath );
  //
}
//=============================================================================
StatusCode GeometryInfoPlus::fullGeoInfoForPoint
( const Gaudi::XYZPoint&        point      ,
  const int                level      ,
  IGeometryInfo*&          start      ,
  ILVolume::ReplicaPath&   replicaPath )
{
  // reset output values
  start = nullptr         ;
  if( !replicaPath.empty() )            { replicaPath.clear();         }
  if( !isInside( point )   )            { return StatusCode::FAILURE ; }
  //
  auto * gi = belongsTo( point , -1 );
  if( !gi || !gi->lvolume() )   { return StatusCode::FAILURE ; }
  start = gi;
  //
  return gi->lvolume()->belongsTo( gi->toLocal( point )  ,
                                   level , replicaPath );
}
//=============================================================================
StatusCode GeometryInfoPlus::fullGeoInfoForPoint
( const Gaudi::XYZPoint&        point      ,
  const int                level      ,
  std::string&             start      ,
  ILVolume::ReplicaPath&   replicaPath )
{
  // reset output values
  start = std::string("");
  if( !replicaPath.empty() ) { replicaPath.clear()       ; }
  if( !isInside( point )   ) { return StatusCode::FAILURE; }
  //
  auto * gi = belongsTo    ( point , -1 );
  if( !gi || !gi->lvolume() ) { return StatusCode::FAILURE ; }
  start = belongsToPath( point , -1 );
  //
  return gi->lvolume()->belongsTo( gi->toLocal( point ) ,
                                   level , replicaPath );
}
//=============================================================================
std::string GeometryInfoPlus::belongsToPath( const Gaudi::XYZPoint& globalPoint )
{
  if( !isInside( globalPoint )                     )
  { return std::string(""); }
  if( !childLoaded() && loadChildren().isFailure() )
  { return std::string(""); }
  //
  auto it =  // Look children
    std::find_if( childBegin() , childEnd  () ,
                  IGeometryInfo_isInside(globalPoint) ) ;
  //
  return ( ( childEnd() == it ) ? std::string("") :
           *(m_gi_childrensNames.begin()+(it-childBegin())) );
}
//=============================================================================
std::string GeometryInfoPlus::belongsToPath( const Gaudi::XYZPoint& globalPoint,
                                             const int         level       )
{
  if( 0 == level ){  return detElem()->name() ; }
  IGeometryInfo* gi =  belongsTo( globalPoint );
  return ( !gi  ? detElem()->name() :
           gi->belongsToPath( globalPoint , level - 1 ) );
}
//=============================================================================
IGeometryInfo* GeometryInfoPlus::belongsTo( const Gaudi::XYZPoint& globalPoint )
{
  if( !isInside( globalPoint )                     ) { return nullptr; }
  if( !childLoaded() && loadChildren().isFailure() ) { return nullptr; }
  //
  auto it = std::find_if( childBegin() , childEnd  () ,
                          IGeometryInfo_isInside(globalPoint) ) ;
  //
  return childEnd() != it ? *it : nullptr ;
}
//=============================================================================
IGeometryInfo* GeometryInfoPlus::belongsTo( const Gaudi::XYZPoint& globalPoint ,
                                            const int         level       )
{
  if ( 0 == level ) { return this;  } // do not look throug  daughters!
  IGeometryInfo* gi = belongsTo( globalPoint );
  return ( gi ? gi->belongsTo( globalPoint , level - 1 ) : this );
}
//=============================================================================
StatusCode GeometryInfoPlus::loadChildren() const
{
  m_gi_childrensNames.clear();
  m_gi_childrens.clear();
  m_gi_childLoaded = false;

  Assert( m_gi_iDetectorElement,
          "GeometryInfoPlus::loadChildren IDetectorElement is not available!");

  auto & IDEchildrens = m_gi_iDetectorElement->childIDetectorElements();
  auto it = IDEchildrens.begin();
  while( IDEchildrens.end() != it )
  {
    IDetectorElement* ide = *it++;
    try
    {
      auto * igi      = ( ide  ? ide->geometry() : nullptr );
      auto * pObj     = ( ide  ? dynamic_cast<DataObject*>(ide) : nullptr );
      auto * pReg     = ( pObj ? pObj->registry() : nullptr );
      const auto path = ( pReg ? pReg->identifier() : "" );
      if ( igi && !path.empty() )
      { 
        m_gi_childrensNames.push_back( path );
        m_gi_childrens.push_back     ( igi  );
      }
    }
    catch( const GaudiException& Exception )
    {  
      m_gi_childrensNames.clear();
      m_gi_childrens.clear();
      Assert( false ,
              "GeometryInfoPlus::loadChildren(): exception caught!" ,
              Exception ) ; }   ///  throw new exception
    catch(...)
    {  
      m_gi_childrensNames.clear();
      m_gi_childrens.clear();
      Assert( false ,
              "GeometryInfoPlus::loadChildren(): unknown exception caught!" ) ;
    } //  throw new exception
  }
  //
  m_gi_childLoaded = true;
  //
  return StatusCode::SUCCESS;
}
//=============================================================================
IGeometryInfo*  GeometryInfoPlus::reset()
{
  /// reset logical volume
  m_gi_lvolume = nullptr;

  /// reset matrices
  m_matrix   .reset( nullptr );
  m_matrixInv.reset( nullptr );

  /// reset support
  m_gi_support      = nullptr;

  /// reset parent
  m_gi_parentLoaded = false  ;
  m_gi_parent       = nullptr;
  /// reset daughters
  std::for_each( m_gi_childrens.begin () ,
                 m_gi_childrens.end   () ,
                 std::mem_fun(&IGeometryInfo::reset) );
  m_gi_childrens.clear()      ;
  m_gi_childrensNames.clear() ;
  m_gi_childLoaded  = false   ;
  ///
  return this;
}
//=============================================================================
IGeometryInfo* GeometryInfoPlus::parentIGeometryInfo()
{
  if( !hasSupport() )  { return nullptr; }
  VERBO << "parentIGeometryInfo with support "
        << m_gi_supportName << endmsg;
  if( !m_gi_support && !m_gi_supportName.empty() )
  {
    m_gi_support = geoByName( m_gi_supportName );
  }
  return m_gi_support;
}
//=============================================================================
const IGeometryInfo* GeometryInfoPlus::parentIGeometryInfo() const
{
  if( m_gi_parentLoaded ) { return m_gi_parent; }
  Assert( m_gi_iDetectorElement ,
          "GeometryInfoPlus:: IDetectorElement is not available!" );

  auto ide = m_gi_iDetectorElement->parentIDetectorElement();
  Assert( ide                   ,
          "GeometryInfoPlus:: parent           is not available!" );
  auto igi = ide->geometry();
  Assert( igi                   ,
          "GeometryInfoPlus:: parent geometry  is not available!" );
  return igi;
}
//=============================================================================
const ILVolume* GeometryInfoPlus::lvolume() const
{
  if( !hasLVolume() ) { return  nullptr              ; }
  if(  m_gi_lvolume ) { return  m_gi_lvolume          ; }
  try
  { m_gi_lvolume  = findLogical() ; }
  catch ( const GaudiException Exception )
  { Assert( false ,
            "GeometryInfoPlus::lvolume() : exception caught" , Exception ) ; }
  catch (...)
  { Assert( false ,
            "GeometryInfoPlus::lvolume() : unknown exception caught" ) ; }
  ///
  Assert( m_gi_lvolume ,
          "GeometryInfoPlus::lvolume() : unable to load ILVolume!" );
  return m_gi_lvolume;
}
//=============================================================================
StatusCode GeometryInfoPlus::location( IGeometryInfo*& start ,
                                       ILVolume::ReplicaPath& replicaPath ) const
{
  replicaPath.clear();

  replicaPath.insert( replicaPath.end() ,
                      supportPath().begin() , supportPath().end() );

  if( !hasSupport() )  { start = nullptr; return StatusCode::SUCCESS; }
  //
  if( !m_gi_support && !m_gi_supportName.empty() )
  {
    m_gi_support = geoByName( m_gi_supportName );
  }

  start = m_gi_support;
  if( !start ) { replicaPath.clear(); }

  return  start ? StatusCode::SUCCESS : StatusCode::FAILURE;
}
//============================================================================
StatusCode GeometryInfoPlus::location( std::string& start ,
                                       ILVolume::ReplicaPath& replicaPath ) const
{
  start = m_gi_supportName;
  replicaPath.clear();
  replicaPath.insert( replicaPath.end() ,
                      supportPath().begin() , supportPath().end() );

  return StatusCode::SUCCESS;
}
//============================================================================
const ILVolume* GeometryInfoPlus::lvolume( IGeometryInfo* start,
                                           const ILVolume::ReplicaPath& replicaPath )
{
  if( !start || !start->lvolume() ){ return nullptr; }
  //
  const auto * lv = start->lvolume();
  ILVolume::PVolumePath volumePath;
  if( lv->traverse( replicaPath.begin()  ,
                    replicaPath.end  ()  ,
                    volumePath           ).isFailure() ) { return nullptr; }
  //
  if( volumePath.empty() ) { return lv; }
  //
  const auto * pv = *(volumePath.rbegin()); // get "the last" element
  //
  return pv ? pv->lvolume() : nullptr;
}
//=============================================================================
std::string GeometryInfoPlus::lvolumePath( IGeometryInfo* start,
                                           const ILVolume::ReplicaPath&
                                           replicaPath )
{
  // starting point for address is invalid!
  if( !start || !start->lvolume() ){ return std::string{} ; }
  //
  const auto * lv = start->lvolume();
  ILVolume::PVolumePath volumePath;
  if( lv->traverse( replicaPath.begin()  ,
                    replicaPath.end  ()  ,
                    volumePath           ).isFailure() ) { return ""; }
  if( volumePath.empty() ) { return start->lvolumeName(); }
  const auto * pv = *(volumePath.rbegin()); // get "the last" element
  return pv ? pv->lvolumeName() : std::string{};
}
//=============================================================================
const ILVolume* GeometryInfoPlus::lvolume( const std::string& start,
                                           const ILVolume::ReplicaPath&
                                           replicaPath )
{
  if( start.empty() ) { return nullptr; }
  auto * gi = geoByName( start );
  return  gi ? lvolume( gi , replicaPath ) : nullptr;
}
//=============================================================================
std::string GeometryInfoPlus::lvolumePath( const std::string& start,
                                           const ILVolume::ReplicaPath&
                                           replicaPath )
{
  if( start.empty() ) { return std::string{}; };
  IGeometryInfo* gi = geoByName( start );
  return  gi ? lvolumePath( gi , replicaPath ) : std::string{};
}
//=============================================================================
// StatusCode  GeometryInfoPlus::reset( const int /*Level*/ )
// {
//   this->reset();
//   return StatusCode::SUCCESS;
// }
//=============================================================================

//=============================================================================
std::ostream& GeometryInfoPlus::printOut( std::ostream& os ) const
{
  try
  {
    os << "GeometryInfoPlus::name=" <<  detElem()->name() << ";" ;
    if( !hasLVolume() ) { os << " no LVolume associated ('ghost');"; }
    else
    {
      os << " LVolume=" << lvolumeName() << ";";
      if( !hasSupport() ) { os << "('orphan');"; }
      else
      {
        os << " supported by " << m_gi_supportName << " with ReplicaPath=(";
        std::copy( m_gi_supportPath.begin() ,
                   m_gi_supportPath.end() ,
                   std::ostream_iterator<ILVolume::ReplicaType>(os,",") );
        os << ") ";
        os << "(" << m_gi_supportNamePath << ");";
      }
    }
  }
  catch (...)
  {
    os << " !!! Unable to print GeometryInfo out !!!";
  }
  //
  return os << std::endl;
}
//=============================================================================
MsgStream& GeometryInfoPlus::printOut( MsgStream& os ) const
{
  try
  {
    os << "GeometryInfoPlus::name=" << detElem()->name() << ";" ;
    if (!hasLVolume())
    {
      os << " no LVolume associated ('ghost');";
    }
    else
    {
      os << " LVolume=" << lvolumeName() << ";";
      if (!hasSupport())
      {
        os << "('orphan');";
      }
      else
      {
        os << " supported by " << m_gi_supportName << " with ReplicaPath=(";
        for ( const auto ci : supportPath() ) { os << ci << "," ; }
        os << ") ";
        os << "(" << m_gi_supportNamePath << ");";
      }
    }
  } catch (...) {
    os << " !!! Unable to print GeometryInfo out !!!";
  }
  //
  return os << endmsg ;
}
//=============================================================================
ILVolume* GeometryInfoPlus::findLogical() const
{
  ILVolume* lv = nullptr;
  if( !hasLVolume() ) { return lv; }
  //
  try
  {
    SmartDataPtr<ILVolume> ptr( dataSvc() , lvolumeName() );
    if( ptr ) { lv = ptr ; }
  }
  catch ( const GaudiException& Exception )
  {  Assert( false ,
             "GeometryInfoPlus::findLogical(), exception in retrieving LV=" +
             lvolumeName() , Exception ); }   ///  throw new exception
  catch (...)
  {
    // throw new exception
    Assert( false ,
            "GeometryInfoPlus::findLogical(), exception in retrieving LV=" +
            lvolumeName() );
  }
  Assert( lv ,
          "GeometryInfoPlus::findLogical(), unable to retrieve LV=" +
          lvolumeName() ) ;
  return lv;
}
//=============================================================================
StatusCode GeometryInfoPlus::queryInterface( const InterfaceID& ID,
                                             void** ppI )
{
  if ( !ppI ) { return StatusCode::FAILURE; }
  *ppI = nullptr ;
  if      ( IGeometryInfo::interfaceID()  == ID )
  { *ppI = static_cast<IGeometryInfo*> ( this ) ; }
  else if ( IInterface:: interfaceID()    == ID )
  { *ppI = static_cast<IInterface*>    ( this ) ; }
  else
  { return StatusCode::FAILURE                  ; }
  // add the reference
  addRef();
  return StatusCode::SUCCESS;
}
//=============================================================================
// Destructor
//=============================================================================
GeometryInfoPlus::~GeometryInfoPlus()
{
  clearMatrices();
  m_gi_childrensNames.clear();
  m_gi_childrens.clear();
  updMgrSvc()->unregister(this);
  m_services->release();
}
//=============================================================================
