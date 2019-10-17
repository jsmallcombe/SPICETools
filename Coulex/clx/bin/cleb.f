      real*8 FUNCTION CLEB(AJ1,AJ2,AJ3,AL1,AL2,AL3)

      IMPLICIT double precision(A-H, O-Z), INTEGER(I-N)

      REAL*8 CO1,DEN,SUMCLEB

      include 'fctrls.inc'

      integer firsttime
      data firsttime /1/
      save firsttime
      if (firsttime.ne.0) then
        call fctrls_s
        firsttime=0
      endif
C
C	COMPUTE CLEBSCH GORDAN COEFFICIENTS CLEB(J1,J2,J3,L1,L2,L3)
C
      CLEB=0.
      NA=INT(2.*AJ1)
      NB=INT(2.*AJ2)
      NC=INT(2.*AJ3)
      ND=INT(2.*AL1)
      NE=INT(2.*AL2)
      NFF=INT(2.*AL3)
      NF=-NFF
      IF ( ND+NE+NF .NE. 0 ) RETURN
      L3=NB+NE
      L8=NA+ND
      L10=NC+NF
      IF ( MOD(L3,2) .NE. 0 ) RETURN
      IF ( MOD(L8,2) .NE. 0 ) RETURN
      IF ( MOD(L10,2) .NE. 0 ) RETURN
      L3=L3/2
      L8=L8/2
      L10=L10/2
      L0=L3+L8+L10
      IF ( ABS(ND) .GT. NA .OR. ABS(NE) .GT. NB .OR.
     &   ABS(NF) .GT. NC ) RETURN
      L1=L0-NC
      L2=L8-ND
      L9=L3-NE
      L11=L10-NF
      L6=L0-NB
      L7=L0-NA
      L4=L6-L2
      L5=L7-L3
      KMIN=MAX(0,-L4,-L5)
      KMAX=MIN(L1,L2,L3)
      IF ( KMAX .LT. KMIN ) RETURN
      CO1=FACT(L1+1)+FACT(L6+1)+FACT(L7+1)+FACT(L8+1)+FACT(L2+1)
      CO1=CO1+FACT(L3+1)+FACT(L9+1)+FACT(L10+1)+FACT(L11+1)
      CO1=CO1-FACT(L0+2)+FACT(NC+2)-FACT(NC+1)
      SGN=1.
      SUMCLEB=0.
      DO K=KMIN,KMAX
        L12=L1-K+1
        L13=L2-K+1
        L14=L3-K+1
        L15=L4+K+1
        L16=L5+K+1
        DEN=FACT(K+1)+FACT(L12)+FACT(L13)+FACT(L14)+FACT(L15)+FACT(L16)
        SUMCLEB=SUMCLEB+SGN*EXP(CO1-2.*DEN)
        SGN=-SGN
      END DO
      IF ( MOD(KMIN,2) .NE. 0 ) SUMCLEB=-SUMCLEB
      CLEB=SUMCLEB
      RETURN
      END
