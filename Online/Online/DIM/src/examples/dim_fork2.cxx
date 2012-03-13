#include <cerrno>
#include <cstdio>
#include <unistd.h>
extern "C" {
#include "dis.hxx"
}

struct Child {
  int id, run, calls;
  char name[132];
  Child(const char* n, const char* svc) : id(0), run(0), calls(0) { 
    ::strncpy(name,n,sizeof(name));
	 id = ::dic_info_service((char*)svc,MONITORED,0,0,0,callback,(long)this,&run,sizeof(run));
  }
  virtual ~Child() {}
  static void callback(void* tag, void* buffer, int* /* size */) {
    Child* c = *(Child**)tag;
    c->run = *(int*)buffer;
    c->calls++;
  }
};

struct Parent {
  int id, run, is_parent;
  Child*  children[50];
  Parent() : id(0), run(0) {
    is_parent = 0;
    memset(children,0,sizeof(children));
    id = ::dis_add_service((char*)"Parent/run",(char*)"I",&run,sizeof(run),0,0);
    ::dis_start_serving((char*)"Parent");
  }
  virtual ~Parent() {}
  void update() {
    ++run;
    ::dis_update_service(run);
  }
  void print() {
    /*
    size_t cnt = 0;
    for(size_t i=0; i<sizeof(children)/sizeof(children[0]); ++i) {
      if ( children[i]->calls > 2 ) cnt++;
    }
    ::printf("PID:%d Child service answers from %ld children. %ld still missing.\n",
	     ::getpid(), cnt, sizeof(children)/sizeof(children[0])-cnt);
    */
  }
  void fork_em() {
    //::dis_remove_service(id);
    ::dis_stop_serving();
    ::printf("Sleep a bit to see task disappear in did\n");
    ::sleep(3);
    for(size_t i=0; i<sizeof(children)/sizeof(children[0]); ++i) {
      char nam[132], svc[132];
      pid_t pid = ::fork();
      sprintf(nam,"Child_%02ld",i);
      sprintf(svc,"Child_%02ld/run",i);
      if ( pid == 0 ) {
	::dim_init();
	::printf("PID:%d Register Child service Child_%02d/run to DNS....\n",::getpid(),i);
	id = ::dis_add_service(svc,(char*)"I",&run,sizeof(run),0,0);
	::dis_start_serving(nam);
	return;
      }
      else if ( pid > 0 ) {
	//children[i] = new Child(nam, svc);
	is_parent = true;
      }
      else {
	::printf("ERROR in fork!!!!  %s\n",strerror(errno));
	::exit(0);
      }
    }
    ::dim_init();
    ::sleep(3);
    ::printf("PID:%d RE-Register Parent to DNS....\n",::getpid());
    id = ::dis_add_service((char*)"Parent/run",(char*)"I",&run,sizeof(run),0,0);
    ::dis_start_serving((char*)"Parent");
  }
};

int main(int /* argc */, char** /* argv */) {
  Parent p;
  ::sleep(5);
  p.fork_em();
  while(1) {
    p.update();
    ::sleep(1);
    if ( p.is_parent ) {
      p.print();
    }
  }
  ::printf("Process %d exiting\n",::getpid());
  return 1;
}


/*

g++ -o xxxx -I../../DIM/dim -ldim -lrt -pthread ../test/test_dim.cpp

[frankm@plus04 cmt]$ ./xxxx
PID:20407 Register Child to DNS....
PID 20407 - Fri Dec  3 22:01:57 2010 - (FATAL) Child: Some Services already known to DNS
PID:20404 RE-Register Parent to DNS....

*/
