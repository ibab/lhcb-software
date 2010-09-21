/*
 * $Id: DAQError.h,v 1.3 2010-09-21 14:28:24 frankb Exp $
 */

 enum DAQErrorType {	
     MissingSrc =            1, 
     BadPkt =                2, // do not attempt to decode
     Multiple =              4,            
     ShortPkt =              8, // do not attempt to decode
     WrongPartitionID =   0x10,
     WrongPackingFactor = 0x20, // do not attempt to decode
     EmptyMEP =           0x40,
     MissingOdin =        0x80,	
     DAQ_LAST_ERROR = 0xFFFFFF  /* LoopType */
 };
