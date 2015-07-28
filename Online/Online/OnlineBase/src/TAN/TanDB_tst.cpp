#include <cstdio>
#include <iostream>
#include "RTL/rtl.h"
#include "TAN/TanDB.h"
#define LINE(ll,x)   {                            \
    for ( int i=0; i < ll; i++ ) std::cout << x;  \
    std::cout << std::endl;                       \
  }

extern "C" int rtl_tandb_test( int /* argc */, char** /*argv */ )  {
  char buff[32];
  std::cout << "Sizes: TANDB_ENTRY=" << long(sizeof(TANDB_ENTRY))
            << " TanMessage=" << long(sizeof(TanMessage)) << " Bytes"
            << std::endl;

  // Start with clean pubarea...
  TanDataBase& db = TanDataBase::Instance();    

  LINE(80,'=');
  db.Dump( std::cout );
  LINE(80,'=');

  TANDB_ENTRY* entry[128];
#ifndef _OSK
  int num_task = 22, num_loop = 10;
#else
  int num_task = 4, num_loop = 10;
#endif
  for ( int i = 0; i < num_task; i++ )  {
    // Insert entry
    TANDB_ENTRY* e = entry[i] = db.AllocateEntry(i);
    ::snprintf(e->_Message()._Name(),sizeof(e->_Message().m_name),"MYTASK_%02d",i);
    db.allocatePort (e);

    // Insert alias(s)
    for ( int j = 0; j < i; j++ )  {
      ::snprintf(e->_Message()._Name(),sizeof(e->_Message().m_name),"MYTASK_%02d_%02d",i,j);
      db.insertAlias (e);
    }
  }
  // Dump contents
  LINE(80,'=');
  db.Dump( std::cout );
  LINE(80,'=');

#ifdef _VMS
  db.Close( entry[8] );

  db.Dump( std::cout );
  LINE(80,'=');

  db.Close( entry[9] );
  db.Close( entry[7] );
  db.Close( entry[6] );
  db.Close( entry[5] );
  db.Close( entry[4] );
#endif
  db.Close( entry[3] );
  db.Dump( std::cout );
  LINE(80,'=');
  db.Close( entry[2] );
  db.Dump( std::cout );
  LINE(80,'=');
  db.Close( entry[1] );
  db.Dump( std::cout );
  LINE(80,'=');
  db.Close( entry[0] );
  db.Dump( std::cout );
  LINE(80,'=');

  LINE(80,'-');
  printf("   Create and delete %d x %d tasks with 0...%d aliases\n",
         num_loop,num_task,num_task-1);
  printf(" ... including to retrieve inserted port entries by task/alias name\n");
  TanMessage msg;
  for ( int k = 0; k < num_loop; k++ )  {
    for ( int i = 0; i < num_task; i++ )  {
      // Insert entry
      TANDB_ENTRY* e = entry[i] = db.AllocateEntry(i);
      ::snprintf(e->_Message()._Name(),sizeof(e->_Message().m_name),"MYTASK_%02d",i);
      ::snprintf(msg._Name(),sizeof(msg.m_name),"%s",e->_Message()._Name());
      NetworkChannel::Port port = db.allocatePort (e);
      NetworkChannel::Port fnd  = db.findPort(msg);
      if ( fnd != port ) {
        std::cout << "!!!!!!!!!!! Port allocated:" << std::hex << port 
                  << " found:" << std::hex << fnd << std::endl;
      }

      // Insert alias(s)
      for ( int j = 0; j < i; j++ )  {
        ::snprintf(e->_Message()._Name(),sizeof(e->_Message().m_name),"MYTASK_%02d_%02d",i,j);
        ::snprintf(msg._Name(),sizeof(msg.m_name),"%s",e->_Message()._Name());
        int status = db.insertAlias (e);
        NetworkChannel::Port fnd  = db.findPort(msg);
        if ( fnd != port ) {
          std::cout << "!!!!!!!!!!! Port allocated:" << std::hex << port 
                    << " found:" << std::hex << fnd 
                    << " status:" << std::hex << status << std::endl;
        }
      }
    }
    //      fprintf(stdout,"Type return to delete tasks:");
    //      fflush(stdout);
    //      scanf("%c",&buff[0]);
    for ( int i = 0; i < num_task; i++ )    {
      ::snprintf(buff,sizeof(buff),"MYTASK_%02d",i);
      TANDB_ENTRY* e = db.FindEntry ( buff );
      db.Close( e );
    }
  }
  LINE(80,'-');
  LINE(80,'=');
  LINE(80,'-');
  db.Dump( std::cout );
  LINE(80,'=');
  std::cout << "All done ...." << std::endl;
  return lib_rtl_default_return();
}
