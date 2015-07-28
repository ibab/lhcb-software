#ifndef WT_WT_FACILITIES_H
#define WT_WT_FACILITIES_H

/*
  A_WT$SRC:WT_FACILITIES.H
  Created           : 29-JAN-1990 by ???

  Modified          : 11-FEB-1991    Author : C.Arnault
  Installed Facility number for XWindow Events.
  Modified          : 20-APR-1995    Author : O.Carmona
  Update from VMS.
*/

#define WT_FACILITY_AMS            1    /* Message system facility    */
#define WT_FACILITY_SCR            2    /* Screen manager             */
#define WT_FACILITY_BROADCAST      3    /* Broadcast messages         */
#define WT_FACILITY_UPI            4    /* UPI event                  */
#define WT_FACILITY_WAKEUP         5    /* Public wakeup              */
#define WT_FACILITY_UPIWAKUP       6    /* UPI specific wakeup        */
#define WT_FACILITY_KEYBOARD       7    /* Keyboard event             */
#define WT_FACILITY_RWK            8    /* Remote wakeup              */
#define WT_FACILITY_TCPAMS         9    /* Message from TCP/IP        */
#define WT_FACILITY_USER1         10    /* USER private 1.            */
#define WT_FACILITY_USER2         11    /* USER private 2.            */
#define WT_FACILITY_USER3         12    /* USER private 3.            */
#define WT_FACILITY_USER4         13    /* USER private 4.            */
#define WT_FACILITY_TERMINATE     14    /* Terminate signal           */
#define WT_FACILITY_TCP_CONNECT   20    /* TCP/IP connect requests    */
#define WT_FACILITY_TCP_PEEK      21    /* TCP/IP message peek        */
#define WT_FACILITY_X             22    /* XWindow Event              */
#define WT_FACILITY_TCP_RECEIVE   23    /* TCP/IP message receive     */
#define WT_FACILITY_DAQ_SPACE     24    /* BM space request satisfied */
#define WT_FACILITY_DAQ_EVENT     25    /* BM event request satisfied */
#define WT_FACILITY_DAQ_WAKE      26    /* BM wakeup (AST)            */
#define WT_FACILITY_AMSSYNCH      27    /* Synchronous AMS Message    */
#define WT_FACILITY_TIMEOUT       28    /* TCPAMS timeout fac         */
#define WT_FACILITY_TIMER1        30    /* TIMER 1                    */
#define WT_FACILITY_TIMER2        31    /* TIMER 2                    */
#define WT_FACILITY_TIMER3        32    /* TIMER 3                    */
#define WT_FACILITY_TIMER4        33    /* TIMER 4                    */
#define WT_FACILITY_TIMER5        34    /* TIMER 5                    */
#define WT_FACILITY_TIMER6        35    /* TIMER 6                    */
#define WT_FACILITY_TIMER7        36    /* TIMER 7                    */
#define WT_FACILITY_TIMER8        37    /* TIMER 8                    */
#define WT_FACILITY_TIMER9        38    /* TIMER 9                    */
#define WT_FACILITY_TIMER10       39    /* TIMER 10                   */
#define WT_FACILITY_TIMER11       40    /* TIMER 11                   */
#define WT_FACILITY_TIMER12       41    /* TIMER 12                   */
#define WT_FACILITY_RO1           50    /* Readout library 1          */
#define WT_FACILITY_RO2           51    /* Readout library 2          */
#define WT_FACILITY_RO3           52    /* Readout library 3          */
#define WT_FACILITY_RO4           53    /* Readout library 4          */
#define WT_FACILITY_RO5           54    /* Readout library 5          */
#define WT_FACILITY_RO6           55    /* Readout library 6          */
#define WT_FACILITY_RO7           56    /* Readout library 7          */
#define WT_FACILITY_MU1           60    /* Monitor template 1         */
#define WT_FACILITY_MU2           61    /* Monitor template 2         */
#define WT_FACILITY_CBMCONNECT    70    /* CBM Server connect request */
#define WT_FACILITY_CBMCLIENTD    71    /* CBM Server client death    */
#define WT_FACILITY_CBMEVENT      72    /* CBM Client new event       */
#define WT_FACILITY_CBMREQEVENT   73    /* CBM Client new event       */
#define WT_FACILITY_CBMCLIENTREQ  74    /* CBM Client new event       */
#define WT_FACILITY_SENSOR1       80    /* C++ Sensor facility 1      */
#define WT_FACILITY_SENSOR2       81    /* C++ Sensor facility 2      */
#define WT_FACILITY_SENSOR3       82    /* C++ Sensor facility 3      */
#define WT_FACILITY_SENSOR4       83    /* C++ Sensor facility 4      */
#define WT_FACILITY_FSM           90    /* FSM                        */
#define WT_FACILITY_FSMFAIL       91    /* FSM                        */
#define WT_FACILITY_FSMPROC       92    /* FSM completion             */
#define WT_FACILITY_FBSR         100    /* Fastbus Service Requests   */
#define WT_FACILITY_DIM_EVENT    110    /* Event ready from DIM       */
#define WT_FACILITY_PVSS         111    /* Event from PVSS            */
#define WT_FACILITY_SCR_MOUSE    112    /* Mouse events from SCR      */
#endif /* WT_WT_FACILITIES_H  */
