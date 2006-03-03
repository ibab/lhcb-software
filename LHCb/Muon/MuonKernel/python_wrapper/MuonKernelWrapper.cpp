// $Id: MuonKernelWrapper.cpp,v 1.3 2006-03-03 13:32:58 jucogan Exp $
// Include files

#include <iostream>
#include <string>
#include <vector>

#include "boost/shared_ptr.hpp"  

#include "boost/python/class.hpp"
#include "boost/python/def.hpp"
#include "boost/python/iterator.hpp"
#include "boost/python/module.hpp"
#include "boost/python/operators.hpp"
#include "boost/python/return_internal_reference.hpp"
#include "boost/python/str.hpp"
#include "boost/python/tuple.hpp"

#include "Kernel/MuonTileID.h"
#include "Kernel/MuonLayout.h"
#include "MuonKernel/MuonStationLayout.h"
#include "MuonKernel/MuonSystemLayout.h"

#include "STLContainerWrapper.h"

using namespace boost::python;

///////////////////////////////////////////////////////////////////////////////
//
// MUONKERNELWRAPPER
// wrapper for the MuonKernel package
//
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//
// ADDITIONAL C++ FUNCTIONS TO CONVERT MUONTILEID
//
///////////////////////////////////////////////////////////////////////////////

long hash_MuonTileID(const LHCb::MuonTileID & m) {return long(m);}

/** 
    Return a string with the name of the MuonTileID: R321 
*/
boost::python::str compactName_MuonTileID(const LHCb::MuonTileID & m) {
  return boost::python::str(
         "R%s%s%s" % make_tuple(m.region()+1, m.nX(), m.nY()));
}

/** 
    Return a string with the name of the MuonTileID: [(2,2),M1,Q1,R3,2,1] 
*/
boost::python::str fullName_MuonTileID(const LHCb::MuonTileID & m) {
  return boost::python::str(
         "[(%s,%s),M%s,R%s,Q%s,%s,%s]" % make_tuple(m.layout().xGrid(),
                                                    m.layout().yGrid(),
                                                    m.station()+1,       
                                                    m.region()+1,        
                                                    m.quarter()+1,       
                                                    m.nX(),        
                                                    m.nY()));       
}

/** 
    Return a string with the name of the MuonTileID: (R3, 2, 1) 
*/
boost::python::str name_MuonTileID(const LHCb::MuonTileID & m) {
  return boost::python::str(
         "(R%1s,%2s,%2s)" % make_tuple(m.region()+1, m.nX(), m.nY()));
}

/** 
    Return a string with the name of the MuonTileID: (M2,R3,2,1) 
*/
boost::python::str name2_MuonTileID(const LHCb::MuonTileID & m) {
  return boost::python::str("(M%s,R%s,%s,%s)" % make_tuple(m.station()+1,
                                                           m.region()+1, 
                                                           m.nX(), 
                                                           m.nY()));
}

/**   
    Return a string with the name of the MuonTileID: [(2,2),1,0,2,2,1] 
*/
boost::python::str rawName_MuonTileID(const LHCb::MuonTileID & m) {
  return boost::python::str(
         "[(%s,%s),%s,%s,%s,%s,%s]" % make_tuple(m.layout().xGrid(),
                                                 m.layout().yGrid(),
                                                 m.station(),          
                                                 m.region(),           
                                                 m.quarter(),          
                                                 m.nX(),           
                                                 m.nY()));
}

///////////////////////////////////////////////////////////////////////////////
//
// ADDITIONAL C++ FUNCTIONS TO COMPARE LAYOUT
//
///////////////////////////////////////////////////////////////////////////////

int equal_layout(const MuonLayout& ml1, const MuonLayout& ml2) 
  {return ml1 == ml2;}

///////////////////////////////////////////////////////////////////////////////
//
// ADDITIONAL C++ FUNCTIONS TO DISPLAY MUONTILEID
//
///////////////////////////////////////////////////////////////////////////////

inline std::ostream& operator<< ( std::ostream& os , std::vector<LHCb::MuonTileID>& li ) 
{
  os << "[";
  std::vector<LHCb::MuonTileID>::iterator it;
  for( it = li.begin(); it != li.end(); it++)
  {
    LHCb::MuonTileID m = *it;
    os << "[" 
       << m.layout()  << ","
       << "M" << m.station()+1 << ","
       << "R" << m.region()+1  << ","
       << "Q" << m.quarter()+1 << ","
       << m.nX() << ","
       << m.nY();
    os << "]" << ",";
  }
  os << "]";
  return os;
}
  
///////////////////////////////////////////////////////////////////////////////
//
// WRAPPER FOR MUONKERNEL
//
///////////////////////////////////////////////////////////////////////////////

BOOST_PYTHON_MODULE(libMuonKernelW)
{
  
  /////////////////////////////////////////////////////////////////////////////
  //
  // MuonLayout
  //
  /////////////////////////////////////////////////////////////////////////////
  
  class_<MuonLayout, boost::shared_ptr<MuonLayout> >("MuonLayout")
    .def(init<>())
    .def(init<int, int>())
  
    .def("__eq__", &equal_layout)
  
    .def("xGrid", &MuonLayout::xGrid)
    .def("yGrid", &MuonLayout::yGrid)
    .def("isDefined", &MuonLayout::isDefined)
    .def("tiles", (std::vector<LHCb::MuonTileID> (MuonLayout::*) () const)
                  &MuonLayout::tiles )
    .def("tiles", (std::vector<LHCb::MuonTileID> (MuonLayout::*) (int) const)
                  &MuonLayout::tiles )
    .def("tiles", (std::vector<LHCb::MuonTileID> (MuonLayout::*) (int,int) const)
                  &MuonLayout::tiles )
    .def("tiles", (std::vector<LHCb::MuonTileID> (MuonLayout::*) (const LHCb::MuonTileID&) 
                  const) &MuonLayout::tiles )
    .def("tilesInArea",(std::vector<LHCb::MuonTileID> (MuonLayout::*) 
                       (const LHCb::MuonTileID&,int,int) const)
                       &MuonLayout::tilesInArea )
    .def("neighbours", (std::vector<LHCb::MuonTileID> (MuonLayout::*) 
                       (const LHCb::MuonTileID&) const)
                       &MuonLayout::neighbours )
    .def("neighbours", (std::vector<LHCb::MuonTileID> (MuonLayout::*) 
                       (const LHCb::MuonTileID&,int,int) const)
                       &MuonLayout::neighbours )
    .def("neighbours", (std::vector<LHCb::MuonTileID> (MuonLayout::*) 
                       (const LHCb::MuonTileID&,int,int,int) const)
                       &MuonLayout::neighbours )
    .def("neighboursInArea", (std::vector<LHCb::MuonTileID> (MuonLayout::*) 
                             (const LHCb::MuonTileID&,int,int,int,int) const)
                             &MuonLayout::neighboursInArea )
    .def("contains",  (LHCb::MuonTileID (MuonLayout::*) (const LHCb::MuonTileID&) const)
                      &MuonLayout::contains )
    .def("tilesInRegion", (std::vector<LHCb::MuonTileID> (MuonLayout::*) 
                          (const LHCb::MuonTileID&,int) const)
                          &MuonLayout::tilesInRegion )
    .def("isValidID", (bool (MuonLayout::*) (const LHCb::MuonTileID&) const)
                      &MuonLayout::isValidID )
    ; 

      
  /////////////////////////////////////////////////////////////////////////////
  //
  // MuonStationLayout
  //
  ////////////////////////////////////////////////////////////////////////////
  
  class_<MuonStationLayout,
         boost::shared_ptr<MuonStationLayout> >("MuonStationLayout")
    .def(init<>())
    .def(init<const MuonLayout&, 
              const MuonLayout&, 
              const MuonLayout&,
              const MuonLayout&>())
    .def(init<const MuonLayout&>())
    
    .def("regionLayout", &MuonStationLayout::regionLayout)
    .def("xGrid", &MuonStationLayout::xGrid)
    .def("yGrid", &MuonStationLayout::yGrid)
    
    .def("tiles", (std::vector<LHCb::MuonTileID> (MuonStationLayout::*) () const)
                  &MuonStationLayout::tiles)
    .def("tiles", (std::vector<LHCb::MuonTileID> (MuonStationLayout::*) 
                  (int) const) &MuonStationLayout::tiles)
    .def("tiles", (std::vector<LHCb::MuonTileID> (MuonStationLayout::*) 
                  (int, int) const) &MuonStationLayout::tiles)
    .def("tiles", (std::vector<LHCb::MuonTileID> (MuonStationLayout::*) 
                  (const LHCb::MuonTileID&) const) &MuonStationLayout::tiles)
                  
    .def("tilesInArea", (std::vector<LHCb::MuonTileID> (MuonStationLayout::*) 
                        (const LHCb::MuonTileID&,int,int) const) 
                        &MuonStationLayout::tilesInArea)
    .def("neighbours", (std::vector<LHCb::MuonTileID> (MuonStationLayout::*) 
                       (const LHCb::MuonTileID&) const)
                       &MuonStationLayout::neighbours)
    .def("neighbours", (std::vector<LHCb::MuonTileID> (MuonStationLayout::*) 
                       (const LHCb::MuonTileID&,int,int) const)
                       &MuonStationLayout::neighbours)
    .def("neighbours", (std::vector<LHCb::MuonTileID> (MuonStationLayout::*) 
                       (const LHCb::MuonTileID&,int,int,int) const)
                       &MuonStationLayout::neighbours)
    .def("neighboursInArea", (std::vector<LHCb::MuonTileID> (MuonStationLayout::*) 
                             (const LHCb::MuonTileID&,int,int,int,int) const)
                             &MuonStationLayout::neighboursInArea)
    .def("contains", (LHCb::MuonTileID (MuonStationLayout::*) 
                     (const LHCb::MuonTileID&) const)
                     &MuonStationLayout::contains)
    .def("tilesInRegion", (std::vector<LHCb::MuonTileID> (MuonStationLayout::*) 
                          (const LHCb::MuonTileID&,int) const)
                          &MuonStationLayout::tilesInRegion)
    .def("isValidID", (bool (MuonStationLayout::*) (const LHCb::MuonTileID&) const)
                      &MuonStationLayout::isValidID)
    ;
    
  /////////////////////////////////////////////////////////////////////////////
  //
  // MuonSystemLayout
  //
  ////////////////////////////////////////////////////////////////////////////
  
  class_<MuonSystemLayout>("MuonSystemLayout")
    .def(init<>())
    .def(init<const MuonStationLayout&,
              const MuonStationLayout&,
              const MuonStationLayout&,
              const MuonStationLayout&,
              const MuonStationLayout&>())
    .def(init<const MuonStationLayout&>())
    
    .def("regionLayout", &MuonSystemLayout::regionLayout)
    .def("stationLayout", &MuonSystemLayout::stationLayout)
    .def("xGrid", &MuonSystemLayout::xGrid)
    .def("yGrid", &MuonSystemLayout::yGrid)
    
    .def("tiles", (std::vector<LHCb::MuonTileID> (MuonSystemLayout::*) () const)
                  &MuonSystemLayout::tiles)
    .def("tiles", (std::vector<LHCb::MuonTileID> (MuonSystemLayout::*) 
                  (const LHCb::MuonTileID&) const) &MuonSystemLayout::tiles)
    .def("tilesInArea", (std::vector<LHCb::MuonTileID> (MuonSystemLayout::*) 
                        (const LHCb::MuonTileID&,int,int) const) 
                        &MuonSystemLayout::tilesInArea)
    .def("neighbours", (std::vector<LHCb::MuonTileID> (MuonSystemLayout::*) 
                       (const LHCb::MuonTileID&) const)
                       &MuonSystemLayout::neighbours)
    .def("neighbours", (std::vector<LHCb::MuonTileID> (MuonSystemLayout::*) 
                       (const LHCb::MuonTileID&,int,int) const)
                       &MuonSystemLayout::neighbours)
    .def("neighbours", (std::vector<LHCb::MuonTileID> (MuonSystemLayout::*) 
                       (const LHCb::MuonTileID&,int,int,int) const)
                       &MuonSystemLayout::neighbours)
    .def("neighboursInArea", (std::vector<LHCb::MuonTileID> (MuonSystemLayout::*)
                             (const LHCb::MuonTileID&,int,int,int,int) const)
                             &MuonSystemLayout::neighboursInArea)
    .def("contains", (LHCb::MuonTileID (MuonSystemLayout::*) 
                     (const LHCb::MuonTileID&) const)
                     &MuonSystemLayout::contains)
    .def("tilesInRegion", (std::vector<LHCb::MuonTileID> (MuonSystemLayout::*) 
                          (const LHCb::MuonTileID&,int) const)
                          &MuonSystemLayout::tilesInRegion)
    .def("isValidID", (bool (MuonSystemLayout::*) (const LHCb::MuonTileID&) const)
                      &MuonSystemLayout::isValidID)
    ;
  
  /////////////////////////////////////////////////////////////////////////////
  //
  // MuonTileID
  //
  /////////////////////////////////////////////////////////////////////////////

  class_<LHCb::MuonTileID>("MuonTileID")
    .def(init<>())
    .def(init<const LHCb::MuonTileID&>())
    .def(init<int, const MuonLayout&, int, int, int, int>()) //IMuonLayout...
    .def(init<const LHCb::MuonTileID&, int, int, int, int>())
    .def(init<const LHCb::MuonTileID&, const MuonLayout&, int, int>())       //IMuonLayout...
    .def(init<const long int&>())
  
    .def("__hash__", &hash_MuonTileID)
    .def("station", &LHCb::MuonTileID::station)
    .def("quarter", &LHCb::MuonTileID::quarter)
    .def("region", &LHCb::MuonTileID::region)
    .def("nX", &LHCb::MuonTileID::nX)
    .def("nY", &LHCb::MuonTileID::nY)
    .def("index", &LHCb::MuonTileID::index)
    .def("key", &LHCb::MuonTileID::key)
    .def("layout", &LHCb::MuonTileID::layout)
    .def("setStation", (void (LHCb::MuonTileID::*) (int) ) &LHCb::MuonTileID::setStation)
    .def("setQuarter", (void (LHCb::MuonTileID::*) (int) ) &LHCb::MuonTileID::setQuarter)
    .def("setRegion", (void (LHCb::MuonTileID::*) (int) ) &LHCb::MuonTileID::setRegion)
    .def("setX", (void (LHCb::MuonTileID::*) (int) ) &LHCb::MuonTileID::setX)    
    .def("setY", (void (LHCb::MuonTileID::*) (int) ) &LHCb::MuonTileID::setY)
    .def("setLayout", (void (LHCb::MuonTileID::*) (const MuonLayout&) ) 
                      &LHCb::MuonTileID::setLayout)
    .def("deltaX", &LHCb::MuonTileID::deltaX)    
    .def("deltaY", &LHCb::MuonTileID::deltaY)
    .def("isValid",&LHCb::MuonTileID::isValid )
    .def("isDefined", &LHCb::MuonTileID::isDefined)
    
    .def("intercept", (LHCb::MuonTileID (LHCb::MuonTileID::*) (const LHCb::MuonTileID&) )
                      &LHCb::MuonTileID::intercept )	
    .def("containerID", (LHCb::MuonTileID (LHCb::MuonTileID::*) (const MuonLayout&) ) //IMuonLayout...
                        &LHCb::MuonTileID::containerID)	   
    .def("localX", (int (LHCb::MuonTileID::*) (const MuonLayout&) )             //IMuonLayout...
                   &LHCb::MuonTileID::localX )	    
    .def("localY", (int (LHCb::MuonTileID::*) (const MuonLayout&) )             //IMuonLayout...
                   &LHCb::MuonTileID::localY )
    .def("neighbourID", (LHCb::MuonTileID (LHCb::MuonTileID::*) (int,int) )
                        &LHCb::MuonTileID::neighbourID )	     
        						
    .def("__str__", &fullName_MuonTileID)
    .def(self <  self)
    .def(self ==  self)
    .def(self != self)
    ;
  
  /////////////////////////////////////////////////////////////////////////////
  //
  // MuonVector
  //
  /////////////////////////////////////////////////////////////////////////////

  class_<std::vector<LHCb::MuonTileID> >("std::vector<MuonTileID>")
    .def(init<>())
    .def(init<std::vector<LHCb::MuonTileID> >())
  
    .def("__contains__",  &std_item<std::vector<LHCb::MuonTileID> >::in)
    .def("__delitem__",   &std_item<std::vector<LHCb::MuonTileID> >::del)
    .def("__getitem__",   &std_item<std::vector<LHCb::MuonTileID> >::get, 
                          return_internal_reference<>() )
    .def("__len__",       &std::vector<LHCb::MuonTileID>::size)
    .def("__setitem__",   &std_item<std::vector<LHCb::MuonTileID> >::set, 
                          with_custodian_and_ward<1,2>())
    .def("__iter__",      iterator<std::vector<LHCb::MuonTileID>, return_internal_reference<> >())
    .def("append",        &std::vector<LHCb::MuonTileID>::push_back, 
                          with_custodian_and_ward<1,2>() )
    .def("count",         &std_item<std::vector<LHCb::MuonTileID> >::count)
    .def("index",         &std_item<std::vector<LHCb::MuonTileID> >::index)
    .def("insert",        &std_item<std::vector<LHCb::MuonTileID> >::insert)
    .def("pop",           &std_item<std::vector<LHCb::MuonTileID> >::pop)
    .def("remove",        &std_item<std::vector<LHCb::MuonTileID> >::remove)
    .def("reverse",       &std_item<std::vector<LHCb::MuonTileID> >::reverse)
    .def("sort",          &std_item<std::vector<LHCb::MuonTileID> >::sort)
    .def("unique",        &std_item<std::vector<LHCb::MuonTileID> >::unique)
    //.def(str(self))
    ;
  
  /////////////////////////////////////////////////////////////////////////////
  //
  // Functions associated to MuonTileID
  //
  ////////////////////////////////////////////////////////////////////////////
  
  def("initial", &compactName_MuonTileID );
  def("fullName", &fullName_MuonTileID );
  def("name", &name_MuonTileID );
  def("name2", &name2_MuonTileID );
  def("rawName", &rawName_MuonTileID );
  
} 
