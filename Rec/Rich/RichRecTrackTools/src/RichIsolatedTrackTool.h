
//-----------------------------------------------------------------------------
/** @file RichIsolatedTrackTool.h
 *
 *  Header file for tool : Rich::Rec::IsolatedTrackTool
 *
 *  CVS Log :-
 *  $Id: RichIsolatedTrackTool.h,v 1.1 2008-05-09 13:58:18 jonrob Exp $
 *
 *  @author Susan Haines  Susan.Carol.Haines@cern.ch
 *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
 *  @date   15/02/2008
 */
//-----------------------------------------------------------------------------

#ifndef RICHRECTOOLS_RichIsolatedTrackTool_H
#define RICHRECTOOLS_RichIsolatedTrackTool_H 1

#include <sstream>
#include <cmath>

// base class
#include "RichRecBase/RichRecToolBase.h"
#include "RichRecBase/RichRecHistoAlgBase.h"
#include "RichRecBase/RichRecTupleAlgBase.h"
#include "RichRecBase/IRichGeomEff.h"
#include "RichRecBase/RichRecHistoToolBase.h"

// from Gaudi
#include "GaudiKernel/AlgFactory.h"

// interfaces
#include "RichRecBase/IRichIsolatedTrack.h"
#include "RichRecBase/IRichCherenkovAngle.h"
#include "RichRecBase/IRichRecGeomTool.h"
#include "RichRecBase/IRichCherenkovResolution.h"

//RICH kernel
#include "RichKernel/IRichRayTracing.h"
#include "RichKernel/IRichSmartIDTool.h"
#include "RichKernel/RichStatDivFunctor.h"

// temporary histogramming numbers
//#include "RichRecBase/RichDetParams.h"

// Event
#include "Event/RichRecSegment.h"

// boost
//#include "boost/assign/list_of.hpp"


namespace Rich
{
  namespace Rec
  {
    //-----------------------------------------------------------------------------
    /** @class IsolatedTrackTool RichIsolatedTrackTool.h
     *
     *  Utility tool which finds isolated RICH tracks.
     *
     *  @author Susan Haines
     *  @author Chris Jones   Christopher.Rob.Jones@cern.ch
     *  @date   15/02/2008
     */
    //-----------------------------------------------------------------------------

    class IsolatedTrackTool : public Rich::Rec::HistoToolBase,
                              virtual public IIsolatedTrack
    {

    public: // Methods for Gaudi Framework

      /// Standard constructor
      IsolatedTrackTool( const std::string& type,
                         const std::string& name,
                         const IInterface* parent );

      /// Destructor
      virtual ~IsolatedTrackTool(){}

      virtual StatusCode initialize();    ///<  initialization

    public: // methods (and doxygen comments) inherited from public interface

      // Is this segment isolated
      virtual bool isIsolated( const LHCb::RichRecSegment * segment,
                               const Rich::ParticleIDType pid ) const;

      // Is this segment isolated
      virtual bool isIsolated( const LHCb::RichRecSegment * segment ) const;

    private:

      //pointers to tool instances
      mutable const ICherenkovAngle * m_ckAngle;  ///< Pointer to RichCherenkovAngle tool
      const IGeomTool * m_geomTool;///< Pointer to geometry tool
      const Rich::Rec::IGeomEff * m_geomEffic;///< Pointer to RichGeomEff tool


      //access rich Cherenkov tool
      const ICherenkovAngle * ckAngleTool() const
      {
        if ( !m_ckAngle ) { acquireTool( "RichCherenkovAngle", m_ckAngle ); }
        return m_ckAngle;
      }

      double m_sizemomcut;
      double m_sizegeocut;
      double m_sizesepcut;
      double m_sizeXexit;
      double m_sizephotonassoccut;
      double m_sizeringwidth;
      double m_sizephicut;
      double m_sizehitregioncut;

      std::vector<double> m_maxROI;  ///< Max hit radius of interest around track centres
      std::vector<double> m_ckThetaMax; ///< Scaling parameter - Max CK theta point
      std::vector<double> m_sepGMax;    ///< Scaling parameter - Max separation point
      std::vector<double> m_scale;      ///< Internal cached parameter for speed

    };

  }
}

#endif // RICHRECTOOLS_RichIsolatedTrackTool_H
