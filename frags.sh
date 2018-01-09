#!/bin/bash

FRAGDIR=frag
ANALDIR=newanal
CALFILE=CalibrationFile.cal
GRSIFolder=./bin/GRSISort
#RUNINFO=run_info.info


FILES=$FRAGDIR"/frag*.root"

ILEN=$#

#IF THERE IS USER INPUT USE THAT RATHER THAN DOING ALL
if [ $ILEN -gt 0 ];
then
	FILES=""
	for var in $@
	do
		FILES=$FILES" "$FRAGDIR"/fragment"$var"*.root"
	done
fi


#GO THROUGH THE FILE LIST
DLEN=${#FRAGDIR}
for f in $FILES
do

 g=${f:DLEN+9}
 h=${g:0:${#g}-5} 
 FFILE=$f
 AFILE=$ANALDIR/analysis$h.root
 #echo "Processing $g $h"
  
 # do something on $f
 
	if [ ! -f $AFILE ] || [ "$AFILE" -ot "$FFILE" ];then
	  if [ -f $AFILE ];
	     then echo "File $AFILE exists but is older than $f."
	     else echo "File $AFILE does not exist."
	  fi
	  grsisort -laq --suppress-errors $CALFILE $FFILE
	else
	  echo "File $AFILE exists."
	fi
	
	if [ -f analysis$h.root ];then mv analysis$h.root $ANALDIR;fi
 
done
