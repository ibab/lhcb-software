/** Writer file statistics deamon
 *
 *  Author: David Svantesson
 *
 */

#if 0

#ifdef _WIN32
int main(int /* argc */, char** /* argv */) {  return 1; }
#else

#include "Writer/defs.h"

#include "Writer/writerstatd.h"

#include "Writer/writers.h"
#include "Writer/writernetobject.h"
#include "Writer/writernetfileobject.h"
#include "Writer/commandhandler.h"

#include <iostream>

using namespace std;

int main() {
/*
    map<string, map<string, string> > filelist;		//list of all files. Each file have a map of properties
	char closedfile[300];			//most recent closed file
	
     // create message queue
     mqd_t mq;
     if ((mq = mq_open("/writerqueue", O_CREAT|O_RDWR,S_IRUSR|S_IWUSR,  NULL))  ==  (mqd_t) -1)  {
         perror("Error opening the message queue. ");
         exit(EXIT_FAILURE);
    }
    
	// go into daemon mode
    // process id, session id
    pid_t pid, sid;
    if  ((pid = fork()) == 0)  {
        printf("starting monitorreaderdaemon...\n");
    } else if (pid > 0) {
        exit(EXIT_SUCCESS);
    } else if(pid < 0) {
        perror("error while forking. ");
        exit(EXIT_FAILURE);
    }
    // Change the file mode mask
    umask(0);
    // create a new SID for the child process
    sid = setsid();
    if (sid < 0) {
        perror("error while getting a session id ");
        exit(EXIT_FAILURE);
    }
    // open log files
    struct  mq_attr  attr;
    if(mq_getattr(mq, &attr) < 0) {
        perror("mq_getattr");
        exit(EXIT_FAILURE);
    }
    FILE* fp=fopen("writernet.log","a");
    if( fp == NULL) {
        perror("Error opening the log file");
        exit(EXIT_FAILURE);
    }
    fprintf(fp, "\nstarting new session\n");
    fflush(fp);
    char* buf = NULL;
    if((buf = (char*) malloc(attr.mq_msgsize)) == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    fflush(stdout);
    size_t max_msg_size = attr.mq_msgsize;

	LogFilePropertyCommandHandler ch(fp);

    Command* cmd = NULL;

	// Start monitoring
	while (1) {

        // clear buffer
        for(unsigned int i = 0; i < max_msg_size; i++) {
            buf[i] = '\0';
        }
		
        if(mq_receive(mq, buf, max_msg_size, NULL) <  0) {
            perror("mq_receive");
            fflush(stdout);
            exit(EXIT_FAILURE);
        }
       
		cmd = ch.parse(buf);
  		
		// pid = cmd->param1

		if (cmd->name == "log") {
				string filename = cmd->param2;
				string fullparam = cmd->param3;
				string paramname = fullparam.substr(0,fullparam.find("="));
				string paramvalue = fullparam.substr(fullparam.find("=")+1);
				filelist[filename][paramname] = paramvalue;
				cout << "log" << endl;
		}
		else if (cmd->name == "closefile") {
				filelist.erase(filelist.find(cmd->param2));
				strcpy(closedfile,cmd->param2);
				strcpy(closedfile+strlen(closedfile),DELIMITER);
				strcpy(closedfile+strlen(closedfile),cmd->param3);
				cout << "closefile" << endl;
		}
		//else if (cmd->name ==  "openfile") 

		
	}
*/
return 0;
}

#endif
#endif

