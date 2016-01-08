#ifndef MAGNET_MagneticFieldGridReader
#define MAGNET_MagneticFieldGridReader

#include <vector>
#include <string>
#include "GaudiKernel/MsgStream.h"
#include "GaudiKernel/Vector3DTypes.h"

class IMessageSvc ;
struct GridQuadrant ;

namespace LHCb {
  class MagneticFieldGrid ;
}

class MagneticFieldGridReader
{
public:
  MagneticFieldGridReader(IMessageSvc& ) ;

  StatusCode readFiles( const std::vector<std::string>& filenames,
			LHCb::MagneticFieldGrid& grid ) const ;

  StatusCode readDC06File( const std::string& filename,
			   LHCb::MagneticFieldGrid& grid ) const ;

  void fillConstantField( const Gaudi::XYZVector& field ,
			  LHCb::MagneticFieldGrid& grid ) const ;
private:
  void fillGridFromQuadrants( GridQuadrant* quadrants,
			      LHCb::MagneticFieldGrid& grid ) const ;
  StatusCode readQuadrant( const std::string& filename,
			   GridQuadrant& quad ) const ;
private:
  mutable MsgStream m_msg ;
} ;

#endif
