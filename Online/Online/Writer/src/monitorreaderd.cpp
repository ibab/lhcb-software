#include "Writer/defs.h"
#include "Writer/monitorreaderd.h"
#include "Writer/writers.h"
#include "Writer/writernetobject.h"
#include "Writer/writernetfileobject.h"
#include "Writer/commandhandler.h"

using namespace std;

int main(int argc, char** argv) {
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
            fprintf(fp, "\n");
            fflush(fp);
          }
        cmd->writers = writers;
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
        delete cmd;
        cmd = NULL;
    }
        fclose(fp);
    free(buf);
    exit(EXIT_SUCCESS);
}
