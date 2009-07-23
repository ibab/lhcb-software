#ifdef _WIN32
int main(int /* argc */, char** /* argv */) {  return 1; }
#else

#include "Writer/defs.h"
#include "Writer/monitorreaderd.h"
#include "Writer/writers.h"
#include "Writer/writernetobject.h"
#include "Writer/writernetfileobject.h"
#include "Writer/commandhandler.h"

#include <iostream>


using namespace std;

int mapToChar(map<string, map<string, string> > * from, char * to, int maxlength);

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
    /*if ((chdir("/var/log")) < 0) {
        perror("error while changing the directory");
        exit(EXIT_FAILURE);
    }*/
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

    int openmaxlength = 3000;         			//Initial length of string sent to DIM for open files
    map<string, map<string, string> > filelist;         //list of all open files. Each file have a map of properties
    char * openfiles = new char[openmaxlength];		//All current open files as a string to be sent 
    char closedfile[300] = "";                   	//most recent closed file
    sprintf(closedfile,"test$1%crec$2",DELIMITER);
   
    // setup DIM-services 
    string hostname = getenv("HOST");
    char servicename[100];
    sprintf(servicename,"%s_Writerstat",hostname.c_str());	
    DimServer::start(servicename);
    sprintf(servicename,"%s_Writerstat/Open",hostname.c_str());	
    DimService pubOpenFiles(servicename, openfiles);
    sprintf(servicename,"%s_Writerstat/Closed",hostname.c_str());	
    DimService pubClosedFile(servicename, closedfile);

    pubOpenFiles.updateService();
    pubClosedFile.updateService();

    Writers* writers = new Writers();
    list<CommandHandler*> commandHandlers;
    // create all command handlers
    commandHandlers.push_back( new StartWriterCommandHandler(fp) ); 
    commandHandlers.push_back( new StopWriterCommandHandler(fp) ); 
    commandHandlers.push_back( new OpenFileCommandHandler(fp) ); 
    commandHandlers.push_back( new CloseFileCommandHandler(fp) ); 
    commandHandlers.push_back( new LogFilePropertyCommandHandler(fp) ); 
    commandHandlers.push_back( new DebugCommandHandler(fp) );
    Command* cmd = NULL;
    unsigned int i = 0;
 
    // loop, waiting for data
    while (1) {
        // clear buffer
        for(i = 0; i < max_msg_size; i++) {
            buf[i] = '\0';
        }
        
        if(mq_receive(mq, buf, max_msg_size, NULL) <  0) {
            perror("mq_receive");
            fflush(stdout);
            exit(EXIT_FAILURE);
        }
        // message received
        fprintf(fp,"\n\nMessage received: %s\n", buf);
        fflush(fp);
        
        // every command handler is able to parse the command
        cmd = commandHandlers.front()->parse(buf);
        if(cmd == NULL) {
            // error during parsing the command
            fprintf(fp, "Error parsing the command\n");
            fprintf(fp, commandHandlers.front()->getErrorMessage()->c_str());
            fflush(fp);
            continue;
        } else {
	    // Handle command received. 
            if (cmd->name == "log") {
		// Update logged parameters
                string filename = cmd->param2;
                string paramname = cmd->param3;
		string paramvalue = cmd->param4;
                filelist[filename][paramname] = paramvalue;
              
		int response;	
		do {
		    response = mapToChar(&filelist,openfiles,openmaxlength);
		    if (response == -1) {
			// ERROR
			fprintf(fp, "Fatal. Error converting property map to char");
    			fclose(fp);
    			free(buf);
    			exit(EXIT_FAILURE);
		    }
		    if (response==-9) {
			// Our string buffer is too short, doubles length.
			fprintf(fp, "Increasing statistics string buffer for open files. Previous length: %i",openmaxlength);
                        openmaxlength *= 2; // double length
			delete[] openfiles;
			openfiles = new char[openmaxlength];
		    }
		} while (response==-9);
		// Update DIM service		
		pubOpenFiles.updateService(openfiles);
	    }
 	    else if (cmd->name == "closefile") {
		// remove file from list of open files and send final file statistics
                filelist.erase(filelist.find(cmd->param2));
		// filename$paramname$paramvalue$pn$pv$pn$pv
                snprintf(closedfile,300, "%s%c%s%c%s%c%s%c%s%c%s%c%s", (cmd->param2).c_str(), DELIMITER, (cmd->param3).c_str(), DELIMITER, (cmd->param4).c_str(), DELIMITER, (cmd->param5).c_str(), DELIMITER, (cmd->param6).c_str(), DELIMITER, (cmd->param7).c_str(), DELIMITER, (cmd->param8).c_str());
		pubClosedFile.updateService(); // Send update notice, string closedfile is sent
		
                fprintf(fp,"\n File closed update. Closed file: %s ", cmd->param2.c_str());
            }
	    else {
                fprintf(fp, "Command was parsed\nCommand: ");
                fprintf(fp, cmd->name.c_str());
                fprintf(fp, "\nparam1: ");
                fprintf(fp, cmd->param1.c_str());
                fprintf(fp, "\nparam2: ");
                fprintf(fp, cmd->param2.c_str());
                fprintf(fp, "\nparam3: ");
                fprintf(fp, cmd->param3.c_str());
                fprintf(fp, "\nparam4: ");
                fprintf(fp, cmd->param4.c_str());
                fprintf(fp, "\nparam5: ");
                fprintf(fp, cmd->param5.c_str());
                fprintf(fp, "\nparam6: ");
                fprintf(fp, cmd->param6.c_str());
                fprintf(fp, "\nparam7: ");
                fprintf(fp, cmd->param7.c_str());
                fprintf(fp, "\nparam8: ");
                fprintf(fp, cmd->param8.c_str());
                fprintf(fp, "\n");
                fflush(fp);
            }
          }
        cmd->writers = writers;
        
	/*
	// search the appropriate command handler
        list<CommandHandler*>::iterator i;
        for(i = commandHandlers.begin(); i != commandHandlers.end(); i++) {
            if( ( (**i).command) == (cmd->name) ) {
                if((**i).handleCommand(cmd)) {
                    fprintf(fp, "command was handled successful\n");
                    fflush(fp);
                    break; // command was handled
                } else {
                    fprintf(fp, "error handling the command\n");
                    fprintf(fp, (**i).getErrorMessage()->c_str());
                    fflush(fp);
                }
            }
        }
	*/
        delete cmd;
        cmd = NULL;
    }
     
    fclose(fp);
    free(buf);
    exit(EXIT_SUCCESS);
}

/** Convert map<string (filename), map<string (key), string (value)> > to a string: filename|/path/to/file|key|value|key|value|...|filename|/path/to/file2|...
 *
 * return: 
 * 0 OK
 * -1 Error (sprintf)
 * -9 char length (to) is too short.
 *
 */
int mapToChar(map<string, map<string, string> > * from, char * to, int maxlength) 
{

    int temppos = 0;
    int response;
    map<string, map<string, string> >::iterator itm;
    map<string, string>::iterator it;
    for (itm = from->begin(); itm != from->end(); ++itm) {
        response = snprintf( to + temppos, maxlength-temppos, "filename%c%s%c", DELIMITER,(*itm).first.c_str(),DELIMITER);
	if (response < 1) return -1;
	if (response > (maxlength-temppos)) return -9; 
        temppos += response;
	it = (*itm).second.begin();
        for (it = itm->second.begin(); it != itm->second.end(); ++it) {
            response = snprintf( to + temppos, maxlength-temppos, "%s%c%s%c",(*it).first.c_str(),DELIMITER,(*it).second.c_str(),DELIMITER); 
            if (response < 1) return -1;
            if (response > (maxlength-temppos)) return -9;
	    temppos += response;
        }
    }

    return 0;
}

#endif
