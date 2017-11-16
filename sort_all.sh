#!/bin/bash

ANALDIR=newanal
SORTDIR=sorted_files/runs
INFO=info.info

AFILES=$(echo $ANALDIR/analysis*.root)
# echo $AFILES

for f in $AFILES
do
	b=$(basename $f)
	g=${b:DLEN+8}
	h=${g:0:5} 

	RES=$SORTDIR/Run$h.root

	if [ ! -f $RES ];then
		echo "$RES does not exist."
		Aparser $INFO $(echo $ANALDIR/analysis$h*) $RES
	else
		if [ "$RES" -ot "$f" ];then
			echo "$RES exists but is older than $f"
			
			Aparser $INFO $(echo $ANALDIR/analysis$h*) $RES
 		fi	
	fi
done
