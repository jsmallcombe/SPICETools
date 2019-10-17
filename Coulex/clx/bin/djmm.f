      real*8 FUNCTION DJMM(BETA,RJ,RM,RMP)
C

      IMPLICIT double precision(A-H, O-Z), INTEGER(I-N)

      include 'fctrls.inc'

      integer firsttime
      data firsttime /1/
      save firsttime
      if (firsttime.ne.0) then
        call fctrls_s
        firsttime=0
      endif

C
      J=2.01d0*RJ
      M=2.01d0*RM
      MP=2.01d0*RMP
      IF ( J .LT. ABS(M) .OR. J .LT. ABS(MP) .OR. J .LT. 0 ) THEN
        WRITE ( 6,1 ) 
 1      FORMAT ( ' ***MISTAKE*** ILLEGAL ARGUMENTS IN CALL FOR DJMM' )
        djmm = 0
        RETURN
      END IF
      JA=(J+MP)/2+1
      JB=(J-MP)/2+1
      JC=(J+M)/2+1
      JD=(J-M)/2+1
      B1=FACT(JA)+FACT(JB)+FACT(JC)+FACT(JD)
      MINSIG=-MIN(0,M+MP)
      MAXSIG=J-MAX(MP,M)
      ISIG=MINSIG
      SUM=0.d0
      DO WHILE ( ISIG .LE. MAXSIG )
        JA=ISIG/2+1
        JB=(J-M-ISIG)/2+1
        JC=(J-MP-ISIG)/2+1
        JD=(M+MP+ISIG)/2+1
        IF ( MOD((J-MP-ISIG)/2,2) .EQ. 0 ) THEN
          FASE=1.d0
        ELSE
          FASE=-1.d0
        END IF
        IC=ISIG+(M+MP)/2
        IS=J-ISIG-(M+MP)/2
        B2=FACT(JA)+FACT(JB)+FACT(JC)+FACT(JD)
        SUM=SUM+FASE*COS(BETA/2.d0)**IC*SIN(BETA/2)**IS*
     &     EXP(B1)/(EXP(B2)**2.d0)
        ISIG=ISIG+2
      END DO
      DJMM=SUM
      RETURN
      END


