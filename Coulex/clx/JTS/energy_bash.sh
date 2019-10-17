# Example to run:
# bash energy_bash.sh 76Kr.clx
# input should have beam target and levels set
# energy and angles are overwritten by this script and the root script

#!/bin/bash

#echo the input
# echo $1

rm -rf "temp.root"

clx_file='tmp_clx_in_file.txt'
pass_file='tmp_clx_out_file.txt'

#these lines load the entire file into an array
old_IFS=$IFS
IFS=$'\n'
lines=($(cat $1)) # array
IFS=$old_IFS

#get the line that has the beam mass
a=${lines[4]}
b=${lines[5]}

#second parameter is beam mass
varA=$(echo $a | cut -d " " -f 2)
varB=$(echo $b | awk '{print $2}')
varC=$(echo $a | cut -d " " -f 1)
varD=$(echo $b | awk '{print $1}')

#remove the carriage return character so we can do maths
massB=$(echo $((${varA//$'\r'})))
massT=$(echo $((${varB//$'\r'})))
z1=$(echo $((${varC//$'\r'})))
z2=$(echo $((${varD//$'\r'})))
#echo $((5+$(tr -d '\r' <<< "$varA")))
#echo $(($mass * 2))

echo "Mass of beam "$massB
echo "Mass of target "$massT
echo "Zeds "$z1 $z2

mevper=5
while [ $mevper -lt 30 ]
do
	ener=$(($massB * $mevper))
	
	#Do a float division to 1 decimal place
	energy=$(echo "scale=1; $ener/3" | bc)
# 	echo $energy
	
	#echo the start lines
	echo ${lines[0]} > $clx_file
	echo ${lines[1]} >> $clx_file
	echo ${lines[2]} >> $clx_file
	echo ${lines[3]} >> $clx_file
	echo ${lines[4]} >> $clx_file
	echo ${lines[5]} >> $clx_file
	#add our change
	echo $energy >> $clx_file
	echo 0.0 180.0 0.5 >> $clx_file
	
	#do the remaining lines
	sline=8
	while [ $sline -lt ${#lines[@]} ]
		do
		echo ${lines[${sline}]} >> $clx_file
		sline=$((sline+1))
	done
	
#	./clx/clx <$clx_file | ./clx/clx_mk_table.awk > $pass_file
	../bin/clx <$clx_file | ../bin/clx_mk_table_slim.awk > $pass_file
	root -l -b -q "root_parseclx.cpp($massB,$massT,$z1,$z2,$energy)"
	
	mevper=$((mevper+1))
done

# Up to this point makes the nice summed data over energy
# All the following detector integrals can be done manually


pwd
# root -l -b -q "root_detector.cpp($massB,$massT,$z1,$z2,17,45.8,\"temp.root\")"
# mv -f temp_det*.root det.root
# root -l -b -q "root_target.cpp($massB,$massT,$z2,$z1,1.5,0,\"det.root\")"
# 
# 
# DETAIL="../output/Ex_"$massB$(symbol $z2)"_on_"$massT$(symbol $z1)
# mv -f det.root $DETAIL"downstream.root"
# mv -f det*.txt $DETAIL"downstream.txt"
# 
# root -l -b -q "root_detector.cpp($massB,$massT,$z1,$z2,134.2,163,\"temp.root\")"
# mv -f temp_det*.root det.root
# root -l -b -q "root_target.cpp($massB,$massT,$z2,$z1,1.5,0,\"det.root\")"
# 
# mv -f det.root $DETAIL"upstream.root"
# mv -f det*.txt $DETAIL"upstream.txt"
# mv -f temp.root $DETAIL".root"
# 


DETAIL="Ex_"$massB$(symbol $z2)"_on_"$massT$(symbol $z1)
mv -f temp.root $DETAIL".root"
root -l -b -q "root_targetsuper.cpp($massB,$massT,$z2,$z1,1.5,0,\"$DETAIL.root\")"
root -l -b -q "root_targetsuper.cpp($massB,$massT,$z2,$z1,1.0,0,\"$DETAIL.root\")"
root -l -b -q "root_targetsuper.cpp($massB,$massT,$z2,$z1,0.5,0,\"$DETAIL.root\")"

mv *.root ../output

rm -rf tmp_clx_in_file.txt
rm -rf tmp_clx_out_file.txt
