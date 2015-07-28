#include "CPP/Interactor.h"
#include "CPP/TimeSensor.h"
#include "CPP/IocSensor.h"
#include "CPP/Event.h"
#include <iostream>
#include <cstdlib>
#include <vector>
using namespace std;

namespace {
  class SensorTester : public Interactor {
    typedef vector<SensorTester*> Probes;
    SensorTester* m_parent;
    int m_globalCount, m_count, m_turns, m_sleep, m_id;
    Probes m_probes;
  public:
    SensorTester(SensorTester* parent, int turns)  
      : m_parent(parent), m_globalCount(parent ? turns : 0), m_count(-1), m_turns(turns)  
    {
      m_id = turns;
      m_sleep = 10%turns;
    }
    virtual ~SensorTester() {      
      for(Probes::iterator i=m_probes.begin(); i!=m_probes.end();++i)
        delete (*i);
      m_probes.clear();
    }
    void add(SensorTester* test) { 
      m_globalCount += test->m_turns;
      m_probes.push_back(test);
    }
    void handleTimer(void* data) {
      if ( data == 0 ) {
        ++m_count;
        //cout << "[TIMERTEST] " << m_id << " Processing timer request " << m_count << "/" << m_turns << " turns." << endl;
        if ( m_count < m_turns ) TimeSensor::instance().add(this,m_sleep,0);
      }
      else if ( data == (void*)~0 ) {
        size_t i;
        for(i=0; i<m_probes.size();++i) {
          if ( m_probes[i]->m_count < m_probes[i]->m_turns ) {
            TimeSensor::instance().add(this,1,(void*)~0);
            return;
          }
        }
        cout << "[TIMERTEST] Finished processing after " << m_globalCount << " turns." << endl;
        for(i=0; i<m_probes.size();++i) {
          cout << "[TIMERTEST] ---> Probe " << i << " had " << m_probes[i]->m_count << " turns." << endl;
        }
        //::delete this; // No! main test object is taken from stack!
        ::exit(0);
      }
    }
    void handleIoc(SensorTester* source, int typ) {
      size_t i;
      switch(typ) {
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
        //::delete this;
        ::exit(0);
      }
    }
    virtual void handle(const Event& ev) {
      switch(ev.eventtype) {
      case IocEvent:
        handleIoc((SensorTester*)ev.data,ev.type);
        break;
      case TimeEvent:
        handleTimer(ev.timer_data);
        break;
      default:
        break;
      }
    }
    static int start_ioc() {
      SensorTester test(0,1);
      for (size_t i=0; i<10; ++i) {
        SensorTester* probe = new SensorTester(&test, 1000);
        test.add(probe);
        IocSensor::instance().send(probe,1,&test);
      }
      IocSensor::instance().run();
      return 0;
    }
    static int start_timer() {
      SensorTester test(0,1);
      for (size_t i=0; i<10; ++i) {
        SensorTester* probe = new SensorTester(&test,i+1);
        test.add(probe);
        TimeSensor::instance().add(probe,i+1,0);
      }
      TimeSensor::instance().add(&test,1,(void*)~0);
      TimeSensor::instance().run();
      return 0;
    }
  };
}

extern "C" int cpp_test_iocsensor(int, char**) {
  return SensorTester::start_ioc();
}

extern "C" int cpp_test_timesensor(int, char**) {
  return SensorTester::start_timer();
}
