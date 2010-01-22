/*
 * $Id: DAQError.h,v 1.1 2010-01-22 15:13:17 niko Exp $
 */

 enum DAQErrorType {	
     MissingSrc = 0, 
     BadPkt,             // do not attempt to decode
     Multiple,            
     ShortPkt,           // do not attempt to decode
     WrongPartitionID,
     WrongPackingFactor, // do not attempt to decode
     EmptyMEP,
     MissingOdin,	
     DAQ_LAST_ERROR  /* LoopType */
 };
