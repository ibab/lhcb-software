// $Id: MuonKernelWrapper.cpp,v 1.2 2005-02-21 09:41:54 atsareg Exp $
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

#include "MuonKernel/MuonLayout.h"
#include "MuonKernel/MuonStationLayout.h"
#include "MuonKernel/MuonSystemLayout.h"
#include "MuonKernel/MuonTileID.h"

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

long hash_MuonTileID(const MuonTileID & m) {return long(m);}

/** 
    Return a string with the name of the MuonTileID: R321 
*/
boost::python::str compactName_MuonTileID(const MuonTileID & m) {
  return boost::python::str(
         "R%s%s%s" % make_tuple(m.region()+1, m.nX(), m.nY()));
}

/** 
    Return a string with the name of the MuonTileID: [(2,2),M1,Q1,R3,2,1] 
*/
boost::python::str fullName_MuonTileID(const MuonTileID & m) {
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
boost::python::str name_MuonTileID(const MuonTileID & m) {
  return boost::python::str(
         "(R%1s,%2s,%2s)" % make_tuple(m.region()+1, m.nX(), m.nY()));
}

/** 
    Return a string with the name of the MuonTileID: (M2,R3,2,1) 
*/
boost::python::str name2_MuonTileID(const MuonTileID & m) {
  return boost::python::str("(M%s,R%s,%s,%s)" % make_tuple(m.station()+1,
                                                           m.region()+1, 
                                                           m.nX(), 
                                                           m.nY()));
}

/**   
    Return a string with the name of the MuonTileID: [(2,2),1,0,2,2,1] 
*/
boost::python::str rawName_MuonTileID(const MuonTileID & m) {
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

inline std::ostream& operator<< ( std::ostream& os , std::vector<MuonTileID>& li ) 
{
  os << "[";
  std::vector<MuonTileID>::iterator it;
  for( it = li.begin(); it != li.end(); it++)
  {
    MuonTileID m = *it;
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
    .def("tiles", (std::vector<MuonTileID> (MuonLayout::*) () const)
                  &MuonLayout::tiles )
    .def("tiles", (std::vector<MuonTileID> (MuonLayout::*) (int) const)
                  &MuonLayout::tiles )
    .def("tiles", (std::vector<MuonTileID> (MuonLayout::*) (int,int) const)
                  &MuonLayout::tiles )
    .def("tiles", (std::vector<MuonTileID> (MuonLayout::*) (const MuonTileID&) 
                  const) &MuonLayout::tiles )
    .def("tilesInArea",(std::vector<MuonTileID> (MuonLayout::*) 
                       (const MuonTileID&,int,int) const)
                       &MuonLayout::tilesInArea )
    .def("neighbours", (std::vector<MuonTileID> (MuonLayout::*) 
                       (const MuonTileID&) const)
                       &MuonLayout::neighbours )
    .def("neighbours", (std::vector<MuonTileID> (MuonLayout::*) 
                       (const MuonTileID&,int,int) const)
                       &MuonLayout::neighbours )
    .def("neighbours", (std::vector<MuonTileID> (MuonLayout::*) 
                       (const MuonTileID&,int,int,int) const)
                       &MuonLayout::neighbours )
    .def("neighboursInArea", (std::vector<MuonTileID> (MuonLayout::*) 
                             (const MuonTileID&,int,int,int,int) const)
                             &MuonLayout::neighboursInArea )
    .def("contains",  (MuonTileID (MuonLayout::*) (const MuonTileID&) const)
                      &MuonLayout::contains )
    .def("tilesInRegion", (std::vector<MuonTileID> (MuonLayout::*) 
                          (const MuonTileID&,int) const)
                          &MuonLayout::tilesInRegion )
    .def("isValidID", (bool (MuonLayout::*) (const MuonTileID&) const)
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
    
    .def("tiles", (std::vector<MuonTileID> (MuonStationLayout::*) () const)
                  &MuonStationLayout::tiles)
    .def("tiles", (std::vector<MuonTileID> (MuonStationLayout::*) 
                  (int) const) &MuonStationLayout::tiles)
    .def("tiles", (std::vector<MuonTileID> (MuonStationLayout::*) 
                  (int, int) const) &MuonStationLayout::tiles)
    .def("tiles", (std::vector<MuonTileID> (MuonStationLayout::*) 
                  (const MuonTileID&) const) &MuonStationLayout::tiles)
                  
    .def("tilesInArea", (std::vector<MuonTileID> (MuonStationLayout::*) 
                        (const MuonTileID&,int,int) const) 
                        &MuonStationLayout::tilesInArea)
    .def("neighbours", (std::vector<MuonTileID> (MuonStationLayout::*) 
                       (const MuonTileID&) const)
                       &MuonStationLayout::neighbours)
    .def("neighbours", (std::vector<MuonTileID> (MuonStationLayout::*) 
                       (const MuonTileID&,int,int) const)
                       &MuonStationLayout::neighbours)
    .def("neighbours", (std::vector<MuonTileID> (MuonStationLayout::*) 
                       (const MuonTileID&,int,int,int) const)
                       &MuonStationLayout::neighbours)
    .def("neighboursInArea", (std::vector<MuonTileID> (MuonStationLayout::*) 
                             (const MuonTileID&,int,int,int,int) const)
                             &MuonStationLayout::neighboursInArea)
    .def("contains", (MuonTileID (MuonStationLayout::*) 
                     (const MuonTileID&) const)
                     &MuonStationLayout::contains)
    .def("tilesInRegion", (std::vector<MuonTileID> (MuonStationLayout::*) 
                          (const MuonTileID&,int) const)
                          &MuonStationLayout::tilesInRegion)
    .def("isValidID", (bool (MuonStationLayout::*) (const MuonTileID&) const)
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
    
    .def("tiles", (std::vector<MuonTileID> (MuonSystemLayout::*) () const)
                  &MuonSystemLayout::tiles)
    .def("tiles", (std::vector<MuonTileID> (MuonSystemLayout::*) 
                  (const MuonTileID&) const) &MuonSystemLayout::tiles)
    .def("tilesInArea", (std::vector<MuonTileID> (MuonSystemLayout::*) 
                        (const MuonTileID&,int,int) const) 
                        &MuonSystemLayout::tilesInArea)
    .def("neighbours", (std::vector<MuonTileID> (MuonSystemLayout::*) 
                       (const MuonTileID&) const)
                       &MuonSystemLayout::neighbours)
    .def("neighbours", (std::vector<MuonTileID> (MuonSystemLayout::*) 
                       (const MuonTileID&,int,int) const)
                       &MuonSystemLayout::neighbours)
    .def("neighbours", (std::vector<MuonTileID> (MuonSystemLayout::*) 
                       (const MuonTileID&,int,int,int) const)
                       &MuonSystemLayout::neighbours)
    .def("neighboursInArea", (std::vector<MuonTileID> (MuonSystemLayout::*)
                             (const MuonTileID&,int,int,int,int) const)
                             &MuonSystemLayout::neighboursInArea)
    .def("contains", (MuonTileID (MuonSystemLayout::*) 
                     (const MuonTileID&) const)
                     &MuonSystemLayout::contains)
    .def("tilesInRegion", (std::vector<MuonTileID> (MuonSystemLayout::*) 
                          (const MuonTileID&,int) const)
                          &MuonSystemLayout::tilesInRegion)
    .def("isValidID", (bool (MuonSystemLayout::*) (const MuonTileID&) const)
                      &MuonSystemLayout::isValidID)
    ;
  
  /////////////////////////////////////////////////////////////////////////////
  //
  // MuonTileID
  //
  /////////////////////////////////////////////////////////////////////////////

  class_<MuonTileID>("MuonTileID")
    .def(init<>())
    .def(init<const MuonTileID&>())
    .def(init<int, int, int, const MuonLayout&, int, int, int, int>()) //IMuonLayout...
    .def(init<const MuonTileID&, int, int, int, int>())
    .def(init<const MuonTileID&, const MuonLayout&, int, int>())       //IMuonLayout...
    .def(init<const long int&>())
  
    .def("__hash__", &hash_MuonTileID)
    .def("station", &MuonTileID::station)
    .def("readout", &MuonTileID::readout)	
    .def("layer", &MuonTileID::layer)
    .def("quarter", &MuonTileID::quarter)
    .def("region", &MuonTileID::region)
    .def("nX", &MuonTileID::nX)
    .def("nY", &MuonTileID::nY)
    .def("index", &MuonTileID::index)
    .def("key", &MuonTileID::key)
    .def("layout", &MuonTileID::layout)
    .def("setStation", (void (MuonTileID::*) (int) ) &MuonTileID::setStation)
    .def("setReadout", (void (MuonTileID::*) (int) ) &MuonTileID::setReadout)	
    .def("setLayer", (void (MuonTileID::*) (int) ) &MuonTileID::setLayer)
    .def("setQuarter", (void (MuonTileID::*) (int) ) &MuonTileID::setQuarter)
    .def("setRegion", (void (MuonTileID::*) (int) ) &MuonTileID::setRegion)
    .def("setX", (void (MuonTileID::*) (int) ) &MuonTileID::setX)    
    .def("setY", (void (MuonTileID::*) (int) ) &MuonTileID::setY)
    .def("setLayout", (void (MuonTileID::*) (const MuonLayout&) ) 
                      &MuonTileID::setLayout)
    .def("deltaX", &MuonTileID::deltaX)    
    .def("deltaY", &MuonTileID::deltaY)
    .def("isValid",&MuonTileID::isValid )
    .def("isDefined", &MuonTileID::isDefined)
    
    .def("intercept", (MuonTileID (MuonTileID::*) (const MuonTileID&) )
                      &MuonTileID::intercept )	
    .def("containerID", (MuonTileID (MuonTileID::*) (const MuonLayout&) ) //IMuonLayout...
                        &MuonTileID::containerID)	   
    .def("localX", (int (MuonTileID::*) (const MuonLayout&) )             //IMuonLayout...
                   &MuonTileID::localX )	    
    .def("localY", (int (MuonTileID::*) (const MuonLayout&) )             //IMuonLayout...
                   &MuonTileID::localY )
    .def("neighbourID", (MuonTileID (MuonTileID::*) (int,int) )
                        &MuonTileID::neighbourID )	     
        						
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

  class_<std::vector<MuonTileID> >("std::vector<MuonTileID>")
    .def(init<>())
    .def(init<std::vector<MuonTileID> >())
  
    .def("__contains__",  &std_item<std::vector<MuonTileID> >::in)
    .def("__delitem__",   &std_item<std::vector<MuonTileID> >::del)
    .def("__getitem__",   &std_item<std::vector<MuonTileID> >::get, 
                          return_internal_reference<>() )
    .def("__len__",       &std::vector<MuonTileID>::size)
    .def("__setitem__",   &std_item<std::vector<MuonTileID> >::set, 
                          with_custodian_and_ward<1,2>())
    .def("__iter__",      iterator<std::vector<MuonTileID>, return_internal_reference<> >())
    .def("append",        &std::vector<MuonTileID>::push_back, 
                          with_custodian_and_ward<1,2>() )
    .def("count",         &std_item<std::vector<MuonTileID> >::count)
    .def("index",         &std_item<std::vector<MuonTileID> >::index)
    .def("insert",        &std_item<std::vector<MuonTileID> >::insert)
    .def("pop",           &std_item<std::vector<MuonTileID> >::pop)
    .def("remove",        &std_item<std::vector<MuonTileID> >::remove)
    .def("reverse",       &std_item<std::vector<MuonTileID> >::reverse)
    .def("sort",          &std_item<std::vector<MuonTileID> >::sort)
    .def("unique",        &std_item<std::vector<MuonTileID> >::unique)
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
