#ifndef DICT_LCGDICT_H 
#define DICT_LCGDICT_H 1

// begin include files
#include <cstddef>
#include <string>
#include <vector>
#include <map>

#include <GaudiKernel/SmartRefVector.h>
#include <GaudiKernel/SmartRef.h>
#include <GaudiKernel/KeyedContainer.h>

#include <Relations/RelationsDict.h>

#include <Event/TurningPoint.h>
#include <Event/SwimmingReport.h>

// end include files
namespace {

   typedef std::map<size_t, bool> map_ulb;
   typedef std::map<std::string, bool> map_sb;
   typedef std::map<std::string, std::map<size_t, bool> > map_sinfo;

   struct __Instantiations {
      // begin instantiations
      // Persistency
      LHCb::SwimmingReport::Container     _sr0;
      LHCb::SwimmingReport::TurningPoints _sr1;

      SmartRef<LHCb::SwimmingReport>               _sr2;
      std::vector<SmartRef<LHCb::SwimmingReport> > _sr3;
      SmartRefVector<LHCb::SwimmingReport>         _sr4;
      std::vector<LHCb::SwimmingReport>            _sr5;
      std::vector<LHCb::SwimmingReport*>           _sr6;

      SmartRef<LHCb::TurningPoint>                 _tp1;
      std::vector<SmartRef<LHCb::TurningPoint> >   _tp2;
      SmartRefVector<LHCb::TurningPoint>           _tp3;
      std::vector<LHCb::TurningPoint>              _tp4;
      std::vector<LHCb::TurningPoint*>             _tp5;

      // Dicts for relations table
      GaudiDict::Relation2DDict<LHCb::Particle, LHCb::SwimmingReport> _r0;
      GaudiDict::Relation2DDict<LHCb::SwimmingReport, LHCb::Particle> _r1;

      // Dicts for python usage
      // map<size_t, bool>
      map_ulb _m00;
      map_ulb::iterator _i00;

      // map<size_t, bool> auxilliary
      std::pair<size_t,bool> _p00;

      // map<string, map<size_t, bool> >
      map_sinfo _m10;
      map_sinfo::iterator _i10;

      // map<string, map<size_t, bool> > auxilliary
      std::pair<std::string, map_ulb> _p10;
      std::pair<map_sinfo::iterator, bool> _p11;

      // map<string, bool>
      map_sb _m20;
      map_sb::iterator _i20;

      // map<string, bool> auxilliary
      std::pair<std::string, bool> _p20;
      std::pair<map_sb::iterator, bool> _p21;

      typedef std::map<const std::string, double> map_sd;
      map_sd::iterator _i30;
      std::pair<map_sd::iterator, bool> _p30;

#ifndef WIN32
      // Windows somehow doesn't like the first item of a pair to be const.
      map_ulb::const_iterator _i01;

      typedef std::map<const size_t, bool> map_culb;
      map_culb _m01;
      map_culb::iterator _i02;
      map_culb::const_iterator _i03;
      std::pair<map_culb::iterator, bool> _p02;
 
      map_sb::const_iterator _i21;
      map_sinfo::const_iterator _i11;

      std::pair<const size_t,bool> _p01;
      
      typedef std::map<const std::string, double> map_csd;
      map_sd::iterator _i31;
      std::pair<map_csd::iterator, bool> _p31;
#endif
      // end instantiations
   };
}

#endif // DICT_LCGDICT_H
