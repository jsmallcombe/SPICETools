#!/bin/bash

FRAGDIR=frag
ANALDIR=newanal
CALFILE=CalibrationFile.cal
GRSIFolder=./bin/GRSISort
#RUNINFO=run_info.info

FINISHEDSIZE=970000

#source $GRSIFolder/thisgrsi.sh

FILES=$FRAGDIR"/fragment*.root"


#IF THERE IS USER INPUT USE THAT RATHER THAN DOING ALL
ILEN=$#
if [ $ILEN -gt 0 ];
then
	FILES=""
	for var in $@
	do
		if [ ! "$var" = "ONLY" ]
		then
			FILES=$FILES" "$FRAGDIR"/fragment"$var"*.root"
		fi
	done
fi


#GO THROUGH THE FILE LIST
DLEN=${#FRAGDIR}
for f in $FILES
do

 g=${f:DLEN+9}
 h=${g:0:${#g}-5} 

 AFILE=$ANALDIR/analysis$h.root  
 # do something on $f
 
if [ ! -f $AFILE ] || [ "$AFILE" -ot "$f" ];then
	if [ -f $AFILE ];
	then echo "File $AFILE exists but is older than $f."
	else echo "File $AFILE does not exist."
	fi
	grsisort -laq --suppress-errors $CALFILE $f
else
	echo "File $AFILE exists."
fi

if [ -f analysis$h.root ];then mv analysis$h.root $ANALDIR;fi
 
done
