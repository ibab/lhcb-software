#ifdef _WIN32
int main(int /* argc */, char** /* argv */) {  return 1; }
#else

#include "Writer/monitorreaderd.h"
#include "Writer/defs.h"
#include <iostream>

using namespace std;


/** converts writer message queues to DIM services...
 *  @author David Svantesson
 */
int main(int /* argc */, char** /* argv */) {
    

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
  
    struct  mq_attr  attr;
    if(mq_getattr(mq, &attr) < 0) {
        perror("mq_getattr");
        exit(EXIT_FAILURE);
    }
 
    // Write any log messages to syslog
    openlog("monitorreaderd",LOG_PID | LOG_NDELAY,LOG_LOCAL6);
    syslog(LOG_NOTICE,"starting new session");

    char* buf = NULL;
    if((buf = (char*) malloc(attr.mq_msgsize)) == NULL) {
        syslog(LOG_ERR,"malloc problem, exiting");
        exit(EXIT_FAILURE);
    }
    size_t max_msg_size = attr.mq_msgsize;

    char message[1000] = "";                   	//current sent message
   
    // setup DIM-services 
    string hostname = getenv("HOST");
    DimService pubMQ((hostname+"_Writerstat/mq").c_str(), message);
    DimServer::start((hostname+"_Writerstat/mq").c_str());

    unsigned int i = 0;
    char * ntok;
    char split[3];			//delimiters to split at
    sprintf(split,"%c=",DELIMITER);

    string msgtype;
    string filename;
    string params[6];

    // loop, waiting for data
    while (1) {
        // clear buffer
        for(i = 0; i < max_msg_size; i++) {
            buf[i] = '\0';
        }
        
        if(mq_receive(mq, buf, max_msg_size, NULL) <  0) {
            syslog(LOG_ERR,"mq_receive problem, exiting");
            exit(EXIT_FAILURE);
        }

        // message received
        //fprintf(fp,"\n\nMessage received: %s\n", buf);
        
	// start parsing, first is name of file
	ntok = strtok(buf,split);
	if (ntok==NULL) {
	    syslog(LOG_WARNING,"not able to parse message: %s",buf);
	    continue;
	}
	msgtype = ntok;
	if (msgtype!="log" && msgtype!="openfile" && msgtype!="closefile") {
	    syslog(LOG_INFO,"received msg type %s",msgtype.c_str());
	    continue;
	}
	
	// Get PID
	ntok = strtok(NULL,split);
	if (ntok==NULL) {
	    syslog(LOG_WARNING,"truncated message: %s",buf);
	    continue;
	}

	// Get filename
	ntok = strtok(NULL,split);
	if (ntok==NULL) {
	    syslog(LOG_WARNING,"truncated message: %s",buf);
	    continue;
	}
	filename = ntok;

	if (msgtype == "openfile") {
	    // openfile$filename   (no bytes written from start)
	    snprintf(message,1000, "openfile%c%s%c%s%c%s%c%s",DELIMITER,filename.c_str(),DELIMITER,"0",DELIMITER,"0",DELIMITER,"0");
	    pubMQ.updateService(); //send message
	    continue;
	}

	// get parameters for log/closefile
	bool ok = true;
	for (i=0; i<6; i++) {
	    ntok = strtok(NULL,split);
	    if (ntok==NULL) {
		syslog(LOG_WARNING,"missing parameters for msgtype %s: %s",msgtype.c_str(),buf);
		ok = false;
		break;	
	    }
	    params[i] = ntok;
	}
	if (!ok) continue;
	
	// new log message: log/closefile$filename$bytesWritten$events$lumiEvents
        snprintf(message,1000, "%s%c%s%c%s%c%s%c%s", msgtype.c_str(), DELIMITER, filename.c_str(), DELIMITER, params[2].c_str(), DELIMITER, params[4].c_str(), DELIMITER, params[6].c_str());
	// Update DIM service		
	pubMQ.updateService(); //send message

    }
     
    exit(EXIT_SUCCESS);
}



#endif
