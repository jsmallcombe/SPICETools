#!/bin/bash

ILEN=$#

if [ $ILEN -gt 0 ];
then
	midas=""
	for m in $@
	do
		if [[ $m == *".mid" ]]; then
			midas=$midas" "$m
		fi
	done
	echo $midas
	CalSiLi MAKEBLANK SPICE_blank.cal
	
	FFILES=""
	for f in $midas
	do
# 		echo $f
		b=$(basename $f)
# 		echo $b
		g=${b:3}
		h=${g:0:${#g}-4} 
# 		echo $h
		FFILES=$FFILES" fragment"$h".root"
		grsisort -lq --suppress-errors $f SPICE_blank.cal
	done
	DDparser $FFILES UseFitCharge FitShape SPICE_blank.cal ShapeCalc.root
	
	CalSiLi ADDWAVE WaveShapeParam.txt SPICE_blank.cal
	
	AFILES=""
	for f in $FFILES
	do
		h=${f:8:${#g}+1} 
# 		echo $h
		AFILES=$AFILES" analysis"$h
		grsisort -laq --suppress-errors $f SPICE_blankwave.cal
	done	
	
	CalSiLi ADDWAVE WaveShapeParam.txt SPICE_blankwave.cal
	CalSiLi SPICE_blankwave.cal $AFILES UseFitCharge
# 	CalSiLi Fix SPICE_blankwave.cal $AFILES UseFitCharge
fi