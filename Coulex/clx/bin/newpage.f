      SUBROUTINE NEWPAGE

      IMPLICIT double precision(A-H, O-Z), INTEGER(I-N)

      CHARACTER*1 TITLE(40)
      COMMON /PAGE/ TITLE,ID
      IF ( ID .EQ. 0 ) THEN
        WRITE ( 6,1 ) (TITLE(I),I=1,40)
 1      FORMAT ( '1   D C Y  8 4   ',40A1// )
      ELSE
        WRITE ( 6,2 ) (TITLE(I),I=1,40)
 2      FORMAT ( '1   C L X  8 4   ',40A1// )
      END IF
      RETURN
      END
