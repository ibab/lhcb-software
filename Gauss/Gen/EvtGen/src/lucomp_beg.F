      integer function lucomp_beg(kfa)
*
*
* called by modified LUCOMP_BEG to add user defined particles
*
* added ASLUND backward compatibility Dec 1994
* added LUCOMP_BEG=410+abs(KF)/100 000 July 1994
* added UPS 4S,5S  Jan 1994
* added all bb-onia below threshold Jun 97 RW
*
* NOTE: ASLUND version maps LUCOMP_BEG = 400 + KFA/1 000 000
*
* Doug Wright Oct 1994
* R.Waldi  Nov 1997

      implicit none

C  #include "beget.inc"          (Don't need beget.inc)  1/16/98

      integer N_BB
      PARAMETER (N_BB = 21)
      integer KF_BB(N_BB),KC_BB(N_BB),I

      DATA KF_BB
*        UPS(3S),UPS(4S),UPS(5S),UPS_1(1D),UPS_2(1D),UPS_3(1D)
     1 / 60553,  70553,  80553,  120553,   30555,    557,
*        UPS_1(2D),UPS_2(2D),UPS_3(2D),chi_0b(2P),chi_1b(2P),chi_2b(2P)
     1   130553,   50555,    10557,    30551,     50553,     10555,
*        h_b(2P),chi_0b(3P),chi_1b(3P),chi_2b(3P),h_b(3P),eta_b(2S),
     1   40553,  50551,     110553,    20555,     100553, 20551,
*        eta_b(3S),eta_2b(1D),eta_2b(2D)
     1   40551,    40555,     60555/
      DATA KC_BB
*        UPS(3S),UPS(4S),UPS(5S),UPS_1(1D),UPS_2(1D),UPS_3(1D)
     1 /   403,    404,    405,     416,     417,    418,
*        UPS_1(2D),UPS_2(2D),UPS_3(2D),chi_0b(2P),chi_1b(2P),chi_2b(2P)
     1     419,     420,      421,      410,       411,       412,
*        h_b(2P),chi_0b(3P),chi_1b(3P),chi_2b(3P),h_b(3P),eta_b(2S),
     1     422,    413,        414,      415,        423,   401,
*        eta_b(3S),eta_2b(1D),eta_2b(2D)
     1     402,    424,        425/

      integer kfa

      LUCOMP_BEG = 0
      IF(    KFA.GE.1000000) THEN   ! for ASLUND backward compatibility
         LUCOMP_BEG = 400 + MOD(KFA/1 000 000,100)
c      ELSEIF(KFA.GE.100000) THEN
c         LUCOMP_BEG = 410 + MOD(KFA/100 000, 90)
      ELSE
         DO 100 I=1,N_BB
         IF(KFA.eq.KF_BB(I)) THEN 
            LUCOMP_BEG =   KC_BB(I)
            GOTO 110
            ENDIF
 100     CONTINUE
 110  CONTINUE
      ENDIF
      end
