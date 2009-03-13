#include "CPP/Interactor.h"
#include "CPP/IocSensor.h"
#include "CPP/Event.h"
#include <iostream>
#include <vector>
using namespace std;
#define TURNS 1000

namespace {
  class SensorTester : public Interactor {
    typedef vector<SensorTester*> Probes;
    SensorTester* m_parent;
    int m_globalCount, m_count;
    Probes m_probes;
  public:
    SensorTester(SensorTester* parent)  
      : m_parent(parent), m_globalCount(parent ? TURNS : 0), m_count(-1)  {
    }
    virtual ~SensorTester() {      
      for(Probes::iterator i=m_probes.begin(); i!=m_probes.end();++i)
	delete (*i);
      m_probes.clear();
    }
    void add(SensorTester* test) { 
      m_globalCount += TURNS;
      m_probes.push_back(test);
    }
    virtual void handle(const Event& ev) {
      size_t i;
      SensorTester* source = 0;
      switch(ev.eventtype) {
      case IocEvent:
	source = (SensorTester*)ev.data;
	switch(ev.type) {
	case 1:
	  ++m_count;
	  IocSensor::instance().send(m_parent,0,this);
	  break;
	case 0:
	  if ( ++m_count <= m_globalCount )
	    IocSensor::instance().send(source,1,this);
	  else
	    IocSensor::instance().send(this,2,this);
	  break;
	case 2:
	  cout << "[IOCTEST] Finished processing after " << m_globalCount << " turns." << endl;
	  for(i=0; i<m_probes.size();++i) {
	    cout << "[IOCTEST] ---> Probe " << i << " had " << m_probes[i]->m_count << " turns." << endl;
	  }
	  ::delete this;
	  ::exit(0);
	}
      default:
	break;
      }
    }
    static int start_ioc() {
      SensorTester test(0);
      for (size_t i=0; i<10; ++i) {
	SensorTester* probe = new SensorTester(&test);
	test.add(probe);
	IocSensor::instance().send(probe,1,&test);
      }
      IocSensor::instance().run();
      return 0;
    }
  };
}

extern "C" int cpp_test_iocsensor(int, char**) {
  return SensorTester::start_ioc();
}
