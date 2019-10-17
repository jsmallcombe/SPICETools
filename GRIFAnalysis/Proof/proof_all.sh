#!/bin/bash

# analysis12622_000.root
FILES="analysis*_000.root"

ILEN=$#
if [ $ILEN -gt 0 ];
then
	FILES=""
	for var in $@
	do
		FILES=$FILES" analysis"$var"_000.root"
	done
fi



#GO THROUGH THE FILE LIST
for f in $FILES
do

 h=${f:0:${#f}-8} 
 g=$h"???.root"

 echo "grsiproof --max-workers 8 $g $GRSISYS/GRSIProof/S1750Selector.C"
 grsiproof --max-workers 8 $g $GRSISYS/GRSIProof/S1750Selector.C
 
done
