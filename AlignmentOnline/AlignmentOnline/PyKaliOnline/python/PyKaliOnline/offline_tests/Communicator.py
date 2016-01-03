import os, sys, pydim
from Queue import Queue
import socket
import re, struct

class State(object):
    UNKNOWN = "UNKNOWN"
    ERROR = "ERROR"
    NOT_READY = "NOT_READY"
    READY = "READY"
    RUNNING = "RUNNING"
    PAUSED = "PAUSED"

class Communicator(object):
    def __init__(self, utgid_suffix):
        self.__status = State.UNKNOWN
        from multiprocessing import Pipe, Condition, Lock
        self.__master_end, self.__process_end = Pipe()
        self.__callback_lock = Lock()
        utgid = self.utgid(utgid_suffix)
        
        pydim.dis_add_cmnd(utgid, "C", self.__command_callback, 1)
        self.__info_svc = pydim.dis_add_service(utgid + "/status", "C", self.__status_callback, 0)
        pydim.dis_start_serving(utgid)
    
        pydim.dis_update_service(self.__info_svc)

    def partition(self):
        return os.environ.get('PARTITION', 'TEST')
        
    def hostname(self):
        import socket
        return socket.gethostname().split('.')[0]

    def utgid(self, suffix):
        return '_'.join((self.partition(), self.hostname().upper(), suffix))
                
    def has_command(self, *args):
        if len(args) == 0:
            return self.__process_end.poll()
        elif len(args) == 1:
            return self.__process_end.poll(args[0])
        else:
            raise ValueError("wrong number of arguments, must be 0 or 1.")

    def status(self):
        return self.__status
                
    def get_command(self):
        return self.__process_end.recv()

    def set_status(self, status):
        self.__status = status
        pydim.dis_update_service(self.__info_svc)
        
    def __put_command(self, data):
        self.__master_end.send(data)

    def __close_command(self):
        self.__master_end.close()
        
    def __close_status(self):
        self.__process_end.close()        
                
    def __command_callback(self, args, tag):
        self.__callback_lock.acquire()
        self.__put_command(args[0])
        self.__callback_lock.release()

    def __status_callback(self, *args):
        return (self.__status,)
