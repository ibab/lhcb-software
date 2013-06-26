#ifndef __GCCXML
#define __GCCXML
#define __REAL_COMP
#include "Python.h"
#endif
#include <typeinfo>
#include <iostream>
#include <ctime>
#include <map>
#include <list>
#include <vector>

namespace PVSS {
  struct TestVector {
    template <class T> static void set(const T& c)  
    {  printf("Entered an object of type:%s\n",typeid(c).name());    }
  };
  struct TestVectorDict {
    TestVectorDict()  {
      TestVector::set(std::vector<char>());
      TestVector::set(std::vector<int>());
      TestVector::set(std::vector<float>());
      TestVector::set(std::vector<double>());
    }
  };
  struct TestObject {
    int a;
  };
  struct TestDict {
    TestObject a;
    std::map<int,TestObject> b;
    std::map<int,TestObject*> c;
    std::vector<TestObject> d;
    std::vector<TestObject*> e;
    std::vector<float> vf;
  };
}


// Stuff from OnlineKernel/CPP
#include "CPP/Event.h"
#include "CPP/Message.h"
#include "CPP/Sensor.h"
#include "CPP/Interactor.h"

// PVSS manager interface
#include "PVSS/DpID.h"
#include "PVSS/Internals.h"
#include "PVSS/DevAnswer.h"
#include "PVSS/IAPIManager.h"
#include "PVSS/Environment.h"
#include "PVSS/HotLinkCallback.h"

// PVSS interface
#include "PVSS/DataPoint.h"
#include "PVSS/DpID.h"
#include "PVSS/ArchiveSetting.h"
#include "PVSS/BasicAlarm.h"
#include "PVSS/BitAlarm.h"
#include "PVSS/CfgAttribute.h"
#include "PVSS/CfgDetail.h"
#include "PVSS/CfgManager.h"
#include "PVSS/CfgSetting.h"
#include "PVSS/CfgType.h"
#include "PVSS/ControlsManager.h"
#include "PVSS/DevElement.h"
#include "PVSS/Device.h"
#include "PVSS/DeviceManager.h"
#include "PVSS/DeviceIO.h"
#include "PVSS/DeviceSensor.h"
#include "PVSS/DevType.h"
#include "PVSS/DevTypeElement.h"
#include "PVSS/DevTypeManager.h"
#include "PVSS/DpID.h"
#include "PVSS/NamedIdentified.h"
#include "PVSS/Printer.h"
#include "PVSS/RangeAlarm.h"
#include "PVSS/ReadTransaction.h"
#include "PVSS/WriteTransaction.h"
#include "PVSS/DevAnswer.h"
#include "PVSS/Environment.h"
#include "PVSS/HotLinkCallback.h"
#include <list>

// Python uer interface
#include "PVSS/PythonCallback.h"

#ifndef __REAL_COMP
#define BASIC_SPECIALIZATIONS(x)                                  \
  namespace PVSS {                                                \
    template <> int Value::type_id< x > (const x&);               \
    template <> int DataValue< x >::type_id();                    \
    template <> void DataPoint::set< x >(const x&);               \
    template <> x& DataPoint::reference< x >();                   \
    template <> const x& DataPoint::reference< x >() const;       \
    template <> DataValue< x > createDataValue< x >(const x& o);  \
  }

#define __TR_SPECIALIZATIONS(x)                                     \
  namespace PVSS {                                                \
    template <> void ReadTransaction::getOriginal< x >(const std::string& dp_name, x& value); \
    template <> void ReadTransaction::getOriginal< x >(const DataPoint& dp, x& value); \
    template <> void ReadTransaction::getOnline< x >(const std::string& dp_name, x& value); \
    template <> void ReadTransaction::getOnline< x >(const DataPoint& dp, x& value); \
    template <> void WriteTransaction::setValue< x >(const std::string& dp_name, const x& value); \
    template <> void WriteTransaction::setOnline< x >(const std::string& dp_name, const x& value); \
    template <> void WriteTransaction::setOnline< x >(const DataPoint& dp, const x& value); \
    template <> void WriteTransaction::setOriginal< x >(const std::string& dp_name, const x& value); \
    template <> void WriteTransaction::setOriginal< x >(const DataPoint& dp, const x& value); \
  }
#define __TR2_SPECIALIZATIONS(x)   __TR_SPECIALIZATIONS(x)        \
  namespace PVSS {                                                \
    template <> void ReadTransaction::getValue< x >(const DataPoint& dp, std::vector<x>& value); \
    template <> void WriteTransaction::setValue< x >(const DataPoint& dp, const std::vector<x>& value); \
  }
#define TR_SPECIALIZATIONS(x) __TR2_SPECIALIZATIONS(x) __TR_SPECIALIZATIONS(std::vector<x >)

#define SPECIALIZATIONS(x) BASIC_SPECIALIZATIONS(x)           \
  namespace PVSS {                                            \
    template <> x DataPoint::data< x >();                     \
    template <> const x DataPoint::data< x >() const;         \
  }

#define VECTOR_SPECIALIZATIONS(x) BASIC_SPECIALIZATIONS(std::vector< x >)

  SPECIALIZATIONS(bool)
  SPECIALIZATIONS(char)
  SPECIALIZATIONS(unsigned char)
  SPECIALIZATIONS(short)
  SPECIALIZATIONS(unsigned short)
  SPECIALIZATIONS(int)
  SPECIALIZATIONS(unsigned int)
  SPECIALIZATIONS(long)
  SPECIALIZATIONS(unsigned long)
  SPECIALIZATIONS(float)
  //SPECIALIZATIONS(double)
  SPECIALIZATIONS(std::string)
  BASIC_SPECIALIZATIONS(DpID)

  VECTOR_SPECIALIZATIONS(bool)
  VECTOR_SPECIALIZATIONS(char)
  VECTOR_SPECIALIZATIONS(unsigned char)
  VECTOR_SPECIALIZATIONS(short)
  VECTOR_SPECIALIZATIONS(unsigned short)
  VECTOR_SPECIALIZATIONS(int)
  VECTOR_SPECIALIZATIONS(unsigned int)
  VECTOR_SPECIALIZATIONS(long)
  VECTOR_SPECIALIZATIONS(unsigned long)
  VECTOR_SPECIALIZATIONS(float)
  //VECTOR_SPECIALIZATIONS(double)
  VECTOR_SPECIALIZATIONS(std::string)
  VECTOR_SPECIALIZATIONS(DpID)

  TR_SPECIALIZATIONS(bool)
  TR_SPECIALIZATIONS(char)
  TR_SPECIALIZATIONS(unsigned char)
  //TR_SPECIALIZATIONS(short)
  //TR_SPECIALIZATIONS(unsigned short)
  TR_SPECIALIZATIONS(int)
  TR_SPECIALIZATIONS(unsigned int)
  TR_SPECIALIZATIONS(float)
  TR_SPECIALIZATIONS(std::string)
  TR_SPECIALIZATIONS(DpID)
  TR_SPECIALIZATIONS(DPTime)

#undef SPECIALIZATIONS
#undef VECTOR_SPECIALIZATIONS
#endif

namespace  {
  using namespace PVSS;
  class Dictionary  {

    template <class T> struct _Instances {
#if 0
      std::map<char,T>                                      m1;
      std::map<unsigned char,T>                             m2;
      std::map<short,T>                                     m3;
      std::map<unsigned short,T>                            m4;
      std::map<int,T>                                       m5;
      std::map<unsigned int,T>                              m6;
      std::map<std::string,T>                               m7;
      std::map<DpID,T>                                      m8;
      std::list<T>                                          m10;
      std::set<T>                                           m11;
#endif
      std::vector<T>                                        m9;
    };
  public:
    struct DataPoints {
      _Instances<DataPoint>                                 m_i1;
      _Instances<DataPoint*>                                m_i2;
      std::map<DpID,DataPoint>                              m_i3;
      std::map<DpID,DataPoint*>                             m_i4;
      std::map<std::string,DataPoint>                       m_i5;
      std::map<std::string,DataPoint*>                      m_i6;
    } m_dps;
    struct RangeAlarms {
      RangeAlarm<char>                                      m_i7;
      RangeAlarm<short>                                     m_i9;
      RangeAlarm<int>                                       m_i3;
      RangeAlarm<float>                                     m_i5;
    } _ralms;
    struct BitAlarms {
      BitAlarm                                              m_i1;
    } _bitalms;
    struct DevDescription {
      _Instances<const DevType*>                            m_i1;
      _Instances<const DevElement*>                         m_i2;
      _Instances<const DevTypeElement*>                     m_i3;
    } m_devDesc;
    struct CfgTypes {
      _Instances<const CfgType*>                            m_i1;
      _Instances<const CfgAttribute*>                       m_i2;
      _Instances<const CfgDetail*>                          m_i3;
      _Instances<const CfgSetting*>                         m_i4;
    } _cfgDesc;

    struct DataValues {
      _Instances<Value*>                                    m_i1;
    } _values;
    struct ContainerModifier {
      DataPointContainerActor<std::vector<DataPoint> >      m_v1;
      DataPointContainerActor<std::list<DataPoint> >        m_l1;
      DataPointSetActor<std::set<DataPoint> >               m_s1;
      DataPointMapActor<std::map<std::string,DataPoint> >   m_m1;
      ContainerModifier() : m_v1(0), m_l1(0), m_s1(0), m_m1(0) {}
    } _modifiers;
    struct Pointers {
      std::auto_ptr<DevElement>                             p0;
      std::auto_ptr<WriteTransaction>                       p1;
      std::auto_ptr<ReadTransaction>                        p2;
      std::auto_ptr<DeviceIO>                               p3;
      std::auto_ptr<Printer>                                p4;
      std::auto_ptr<Device>                                 p5;
      std::auto_ptr<DataPoint>                              p6;
    } _pointers;
    Dictionary()  {
      std::set<DpID>                                        s0;
      std::set<DataPoint>                                   s1;
      std::set<DataPoint*>                                  s2;
      std::list<DataPoint>                                  l1;
      std::list<DataPoint*>                                 l2;
      std::vector<DataPoint>                                v1;
      std::vector<DataPoint*>                               v2;
      dpInserter(0,s1);
      dpInserter(0,l1);
      dpInserter(0,v1);
    }
    ~Dictionary() {}
  };
}
