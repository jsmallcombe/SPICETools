#!/bin/bash

DATADIR=midas
FRAGDIR=frag
ANALDIR=newanal
#GRSIFolder=./GRSISort
#RUNINFO=run_info.info

#source $GRSIFolder/thisgrsi.sh

DLEN=${#DATADIR}

INP=""
for var in $@
do
	INP=$INP" "$DATADIR"/run"$var".mid"
done

ILEN=${#1}
if [ $ILEN -lt 3 ];
then
INP=$DATADIR"/run*.mid"
fi

for f in $INP
do

 g=${f:DLEN+4}
 h=${g:0:${#g}-4} 
 FFILE=$FRAGDIR/fragment$h.root
 AFILE=$ANALDIR/analysis$h.root
 #echo "Processing $g $h"
  
 # do something on $f
F=0
A=0
  
 if [ -f $FFILE ];
then
   F=F+1
else
   echo "File $FFILE does not exist."
fi

  if [ -f $AFILE ];
then
   A=A+1
else
   echo "File $AFILE does not exist."
fi
 
done
