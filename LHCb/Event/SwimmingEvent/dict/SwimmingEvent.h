#ifndef DICT_LCGDICT_H 
#define DICT_LCGDICT_H 1

// begin include files
#include <map>
#include <string>

#include <Event/TurningPoint.h>
#include <Event/SwimmingReport.h>

#include <Relations/RelationsDict.h>

// end include files
namespace {

   // template<class T> class _tmp {
   //    typedef T& reference;

   //    _tmp(){}
   //    virtual ~_tmp(){}
   // };

   typedef std::map<int, bool> map_ib;
   typedef std::map<std::string, bool> map_sb;
   typedef std::map<std::string, std::map<int, bool> > map_sinfo;

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
      // map<int, bool>
      map_ib _m00;
      map_ib::iterator _i00;
      map_ib::const_iterator _i01;

      // map<int, bool> auxilliary
      std::pair<int,bool> _p00;
      std::pair<map_ib::iterator, bool> _p01;

      // map<string, map<int, bool> >
      map_sinfo _m10;
      map_sinfo::iterator _i10;
      map_sinfo::const_iterator _i11;

      // map<string, map<int, bool> > auxilliary
      std::pair<std::string, map_ib> _p10;
      std::pair<map_sinfo::iterator, bool> _p11;

      // map<string, bool>
      map_sb _m20;
      map_sb::iterator _i20;
      map_sb::const_iterator _i21;

      // map<string, bool> auxilliary
      std::pair<std::string,bool> _p20;
      std::pair<map_sb::iterator, bool> _p21;
    
      // end instantiations
   };
}

#endif // DICT_LCGDICT_H
