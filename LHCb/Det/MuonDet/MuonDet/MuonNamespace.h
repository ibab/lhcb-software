// $Id: MuonNamespace.h,v 1.1 2007-06-11 08:37:06 asatta Exp $
#ifndef MUONDET_MUONNAMESPACE_H 
#define MUONDET_MUONNAMESPACE_H 1
#ifdef __INTEL_COMPILER        // Disable ICC remark
  #pragma warning(disable:177) // variable was declared but never referenced
#endif
namespace DeMuonLocation{
  static const std::string& Default="/dd/Structure/LHCb/DownstreamRegion/Muon";
  static const std::string& Cabling="/dd/Conditions/ReadoutConf/Muon/Cabling";
}
#endif // MUONDET_MUONNAMESPACE_H
