#include <sstream>
#include "PVSS/ControlsManager.h"
#include "PVSS/DeviceManager.h"
#include "PVSS/DeviceIO.h"
#include "PVSS/DevTypeManager.h"
#include "PVSS/CfgManager.h"
#include "PVSS/CfgType.h"
#include "PVSS/CfgAttribute.h"
#include "PVSS/CfgDetail.h"
#include "PVSS/DevType.h"
#include "PVSS/DevAnswer.h"
#include "PVSS/Printer.h"
#include "PVSS/DataPoint.h"
#include "PVSS/BitAlarm.h"
#include "PVSS/RangeAlarm.h"
#include "PVSS/Device.h"
#include "PVSS/Internals.h"
#include "RTL/rtl.h"

#include <cfloat>
#include <climits>

using namespace PVSS;
namespace PVSS {
  typedef const std::string& CSTR;
  typedef const char*        CCHAR;
  template <> struct CfgManip<ControlsManager>  {
    static ControlsManager* create(int id, CCHAR n)
    {      return new ControlsManager(id, n);  }
  };
}
static void header(const std::string& tag)  {
  printf("+------------------------------------------------------------------------------+\n");
  printf("|   Executing:%-65s|\n",tag.c_str());
  printf("+------------------------------------------------------------------------------+\n");
}

static void exec(void (*fun)(ControlsManager*),ControlsManager* mgr)  {
  try  {
    fun(mgr);
  }
  catch(std::exception& e)  {
    printf("Exception:%s\n",e.what());
  }
  catch(const char* e)  {
    printf("Exception:%s\n",e);
  }
  catch(...)  {
    printf("UNKNOWN Exception\n");
  }
}

void pvss_test_run_setdp(ControlsManager* mgr)  {
  header("run_setdp  -- setting datapoint values");
  DataPoint p1(mgr,"ExampleDP_Arg1");
  DataPoint p2(mgr,"ExampleDP_Arg2");
  DataPoint p3(mgr,"ExampleDP_Arg3");
  std::auto_ptr<WriteTransaction> tr(mgr->writeTransaction());
  for(int i=0; i<5; ++i)  {
    float val = float(i);
    tr->start();
    printf("Setting value to:%f\n",val);
    tr->setOriginal(p1,val);
    tr->setOriginal(p2,val*10);
    tr->setOriginal(p3,val*100);      
    tr->execute(true);
    ::lib_rtl_sleep(100);
  }
}

void pvss_test_run_getdp(ControlsManager* mgr)  {
  header("run_setdp  -- set/retrieve datapoint values");
  DataPoint p0(mgr,"ExampleDP_Arg0");
  DataPoint p1(mgr,"ExampleDP_Arg1");
  DataPoint p2(mgr,"ExampleDP_Arg2");
  DataPoint p3(mgr,"ExampleDP_Arg3");
  std::auto_ptr<WriteTransaction>  trW(mgr->writeTransaction());
  std::auto_ptr<ReadTransaction>   trR(mgr->readTransaction());
  for(int i=0; i<20; ++i)  {
    float val = float(i);
    float v0, v1, v2, v3;
    printf("Setting value to:%f\n",val);
    trW->start();
    trW->setOriginal(p0,val);
    trW->setOriginal(p1,val*10);
    trW->setOriginal(p2,val*100);
    trW->setOriginal(p3,val*1000);
    trW->execute(true);
    trR->getOriginal(p0, v0);
    trR->getOriginal(p1, v1);
    trR->getOriginal(p2, v2);
    trR->getOriginal(p3, v3);
    trR->execute();
    printf("run_getdp(Original): %s:%f %s:%f %s:%f %s:%f\n",
      p0.name().c_str(),v0,p1.name().c_str(),v1,p2.name().c_str(),v2,p3.name().c_str(),v3);
    trR->getOnline(p0, v0);
    trR->getOnline(p1, v1);
    trR->getOnline(p2, v2);
    trR->getOnline(p3, v3);
    trR->execute();
    printf("run_getdp(Online):   %s:%f %s:%f %s:%f %s:%f\n",
      p0.name().c_str(),v0,p1.name().c_str(),v1,p2.name().c_str(),v2,p3.name().c_str(),v3);
  }
}
void pvss_test_run_getdp2(ControlsManager* mgr)  {
  header("run_setdp2  -- set/retrieve datapoint values with device reader");
  DataPoint p0(mgr,DataPoint::original("ExampleDP_Arg0"));
  DataPoint p1(mgr,"ExampleDP_Arg1");
  DataPoint p2(mgr,DataPoint::original("ExampleDP_Arg2"));
  DataPoint p3(mgr,"ExampleDP_Arg3");
  std::auto_ptr<WriteTransaction>  trW(mgr->writeTransaction());
  std::auto_ptr<DeviceIO> trR(mgr->devReader());

  for(int i=0; i<20; ++i)  {
    float val = float(i);
    printf("Setting value to:%f\n",val);
    trW->start();
    trW->setValue(p0,val);
    trW->setOriginal(p1,val*10);
    trW->setValue(p2,val*100);
    trW->setOriginal(p3,val*1000);
    trW->execute(true);
    trR->clear();
    trR->add(p0);
    trR->addOriginal(p1);
    trR->add(p2);
    trR->addOnline(p3);
    trR->execute();
    //double v1 = p1.data<double>();
    //int    v2 = p2.data<int>();
    //short  v3 = p3.data<short>();
    printf("run_getdp(Original): %s:%f %s:%f %s:%d %s:%d\n",
      p0.name().c_str(),p0.data<float>(),
      p1.name().c_str(),p1.data<double>(),
      p2.name().c_str(),p2.data<int>(),
      p3.name().c_str(),p3.data<short>());
  }
}

void pvss_test_run_createdp(ControlsManager* mgr)    {
  header("run_createdp: create datapoints & set rangealarm structure");
  /// Create a new datapoint
  DeviceManager* devMgr = mgr->deviceMgr();
  std::auto_ptr<WriteTransaction> tr(mgr->writeTransaction());
  std::auto_ptr<Device> dev(devMgr->createDevice("Test_Int","ExampleDP_Int",true));
  if ( !dev.get() )
    printf("Failed to create datapoint Test_Int of type ExampleDP_Int\n");
  else  {
    std::string m = "Test_Int";
    printf("Created datapoint Test_Int of type ExampleDP_Int\n");
    tr->start();
    tr->setOriginal(m,34);
    tr->execute(true);

    RangeAlarm<int> alm(m);
    alm.remove(tr);
    alm.addRange("Warning",       "warning",INT_MIN,true, 20,     true);
    alm.addRange("OK",            "",       20,     false,40,     true);
    alm.addRange("Value Too High","warning",40,     false,80,     true);
    alm.addRange("Alert",         "alert",  80,     false,INT_MAX,true);
    alm.apply(tr);
    tr->execute();
  }
}

void pvss_test_run_setalm(ControlsManager* mgr)    {
  header("run_setalm: set rangealarm/bitalarm structure");
  std::auto_ptr<WriteTransaction> tr(mgr->writeTransaction());
  std::string m = "ExampleDP_Arg4";
  tr->start();
  tr->setOriginal(m,34.5);
  /// Set range alarm to float device
  RangeAlarm<float> alm(m);
  alm.remove(tr);
  alm.addRange("Warning",       "warning",FLT_MIN,true, 20,     true);
  alm.addRange("OK",            "",       20,     false,40,     true);
  alm.addRange("Value Too High","warning",40,     false,80,     true);
  alm.addRange("Alert",         "alert",  80,     false,FLT_MAX,true);
  alm.apply(tr);
  tr->execute();

  /// Set binary alarm to bit device
  BitAlarm balm("ExampleDP_Bit0");
  tr->setOriginal("ExampleDP_Bit0",true);
  balm.remove(tr);
  balm.set("All Fine","Error",true,"alert","alarm help");
  balm.apply(tr);
  tr->execute();
}

void pvss_test_run_deldp(ControlsManager* mgr)    {
  header("run_deldp: delete datapoint");
  DeviceManager* devMgr = mgr->deviceMgr();
  if ( !devMgr->deleteDevice("Test_Int",true) )
    printf("Failed to delete datapoint Test_Int\n");
  else
    printf("Deleted datapoint Test_Int\n");
}

void pvss_test_run_prt_mgr(ControlsManager* mgr)  {
  header("run_prt_mgr: Print data types and configs");
  std::stringstream os;
  createAsciiPrinter(os)->print(*mgr);
  printf("%s\n",os.str().c_str());
}
#include "PVSS/DeviceSensor.h"
#include "CPP/Interactor.h"
#include "WT/wt_facilities.h"
#include "CPP/Event.h"
namespace {
  struct DeviceHandler : public Interactor {
  public:
    typedef std::map<DpID,DataPoint*> DevDesc;
    DeviceHandler() {}
    virtual ~DeviceHandler() {}
    virtual void handle(const Event& ev)  {
      if ( ev.type == PVSSEvent )  {
        DeviceSensor* s = (DeviceSensor*)ev.data;
        DevDesc& m = s->devices();
        printf("----------------> Device group changed:\n");
        for(DevDesc::const_iterator i=m.begin(); i!=m.end();++i)  {
          const DataPoint* dp = (*i).second;
          printf("Device:%s Value=%f\n",dp->name().c_str(),dp->data<float>());
        }
      }
    }
  };
}
void pvss_test_run_connect_dp(ControlsManager* mgr)  {
  header("run_connect_dp: Connect to datapoints and listen to changes");
  DeviceHandler* handler = new DeviceHandler();
  std::vector<DataPoint> dps;
  dps.push_back(DataPoint(mgr,DataPoint::original("ExampleDP_Arg0")));
  dps.push_back(DataPoint(mgr,DataPoint::original("ExampleDP_Arg1")));
  DeviceSensor listen1(mgr,dps);
  listen1.addListener(handler);

  dps.clear();
  dps.push_back(DataPoint(mgr,DataPoint::original("ExampleDP_Arg2")));
  dps.push_back(DataPoint(mgr,DataPoint::original("ExampleDP_Arg3")));
  DeviceSensor listen2(mgr,dps);
  listen2.addListener(handler);
  DeviceSensor::run();
}

extern "C" long pvss_test2(void (*exit_call)(int ret))  {
  ControlsManager* mgr =  CfgManip<ControlsManager>::create(PVSS::defaultSystemID(),PVSS::defaultSystemName());
  ::lib_rtl_sleep(1000);
//#if 0
  //exec(pvss_test_run_setdp,mgr);
  //exec(pvss_test_run_getdp2,mgr);
  //exec(pvss_test_run_getdp,mgr);
  //exec(pvss_test_run_createdp,mgr);
  exec(pvss_test_run_setalm,mgr);
  ::lib_rtl_sleep(1000);
  exec(pvss_test_run_deldp,mgr);
  exec(pvss_test_run_connect_dp,mgr);
  //pvss_test_run_prt_mgr(mgr);
  if ( exit_call ) (*exit_call)(1);
  return 1;
}

