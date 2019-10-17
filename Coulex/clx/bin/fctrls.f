      SUBROUTINE FCTRLS_S

      IMPLICIT double precision(A-H, O-Z), INTEGER(I-N)

      REAL*8 FCT(99),FACT(99),V
      COMMON /FCTRLS/ FCT , FACT
C
      FCT(1)=LOG(1.d0)
      FACT(1)=LOG(1.d0)
      DO I=1,98
        V=I
        FACT(I+1)=FACT(I)+LOG(SQRT(V))
        FCT(I+1)=LOG(V)-LOG(10.d0)+FCT(I)
      END DO
      RETURN
      END
