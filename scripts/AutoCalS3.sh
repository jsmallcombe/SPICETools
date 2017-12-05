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
	S3Cal MAKEBLANK out.cal $@
	FILES=""
	for f in $midas
	do
		echo $f
		b=$(basename $f)
		echo $b
		g=${b:3}
		echo $g
		h=${g:0:${#g}-4} 
		echo $h
		FILES=$FILES" analysis"$h".root"
		grsisort -laq --suppress-errors $f out.cal
	done
	S3Cal $FILES out.cal
fi

