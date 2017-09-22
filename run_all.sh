#!/bin/bash

DATADIR=midas
FRAGDIR=frag
ANALDIR=newanal
CALFILE=CalibrationFile.cal
GRSIFolder=./bin/GRSISort
#RUNINFO=run_info.info

FINISHEDSIZE=970000

#source $GRSIFolder/thisgrsi.sh

FILES=$DATADIR"/run*.mid"

#Not needed as GRSISort can cope
#THIS ROUGHLY ESTABLISHES A FILE THAT IS STILL BEING WRITTEN
#LAST=""
#for LAST in $FILES; do true; done
#echo $LAST file
#LASTFILESIZE=$(du "$LAST" | awk '{print $1}')
#if [ $LASTFILESIZE -gt $FINISHEDSIZE ]; then LAST=""; fi

ILEN=$#
for bar in $@
do
	if [ "$bar" = "ONLY" ]
	then
		echo "Only making fragment trees"
		ONLYFRAG=1
		ILEN=$(($ILEN-1))
	fi
done


#IF THERE IS USER INPUT USE THAT RATHER THAN DOING ALL
if [ $ILEN -gt 0 ];
then
	FILES=""
	for var in $@
	do
		if [ ! "$var" = "ONLY" ]
		then
			FILES=$FILES" "$DATADIR"/run"$var"*.mid"
		fi
	done
fi


#GO THROUGH THE FILE LIST
DLEN=${#DATADIR}
for f in $FILES
do


#SKIP THE OPEN FILE
#if [ "$f" == "$LAST" ]; then
#  continue
#fi


 g=${f:DLEN+4}
 h=${g:0:${#g}-4} 
 FFILE=$FRAGDIR/fragment$h.root
 AFILE=$ANALDIR/analysis$h.root
 #echo "Processing $g $h"
  
 # do something on $f
 
 
if [ ! -f $FFILE ] || [ "$FFILE" -ot "$f" ];
then
   if [ -f $FFILE ];
     then echo "File $FFILE exists but is older than $f."
     else echo "File $FFILE does not exist."
   fi
   if [ $ONLYFRAG ];
     then grsisort -lq --suppress-errors $CALFILE $f
     else grsisort -laq --suppress-errors $CALFILE $f
   fi
else
   echo "File $FFILE exists."
fi

if [ -f analysis$h.root ];then mv analysis$h.root $ANALDIR;fi
if [ -f fragment$h.root ];then mv fragment$h.root $FRAGDIR;fi

if [ ! $ONLYFRAG ];
then
	if [ ! -f $AFILE ] || [ "$AFILE" -ot "$$FFILE" ];then
	  if [ -f $AFILE ];
	     then echo "File $AFILE exists but is older than $f."
	     else echo "File $AFILE does not exist."
	  fi
	  grsisort -laq --suppress-errors $CALFILE $FFILE
	else
	  echo "File $AFILE exists."
	fi
	
	if [ -f analysis$h.root ];then mv analysis$h.root $ANALDIR;fi
fi
 
done
