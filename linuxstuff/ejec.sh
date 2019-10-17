#!/bin/bash
#Get admin rights
#sudo -l -U administrator bash

USERNAME=$(whoami)

echo ""

#Generate a list of source drives, limiting out invalid options
i=1
while read -r Output; do
if [ "$Output" != "/media/$USERNAME" ] ; then
	DriveChoice[$i]=$Output
	echo "$i=${DriveChoice[$i]}"
	i=$(( i+1 ))
fi
done < <( find /media/"$USERNAME" -maxdepth 1 -type d)



#Have the user select from valid drives
if [ $i -lt 3 ]; then
	DriveNumber=1	
else
	echo "Eject Drive Number?"
	read DriveNumber
fi

#Ensure the user input is in range
if [ $DriveNumber -lt $i ] && [ $DriveNumber -gt 0 ] && [ $i -gt 1 ]; then
	DRIVENAME=${DriveChoice[$DriveNumber]}


	#vartest="/media/ubuntu/USB DISK/"
	#useprocess=$(fuser -c "${DRIVENAME%/}")
	useprocess=$(fuser -c "${DRIVENAME}")

	#fuser -c "${vartest%/}"
	#echo $(fuser -c "${vartest%/}")

	if [ ${#useprocess} -lt 2 ]; then
		echo "Ejecting drive."
		#umount "$DRIVENAME"
		#udisks --unmount "$DRIVENAME"
		#udisks --detach "$DRIVENAME"
		#gvfs-mount -d "$DRIVENAME"
		gvfs-mount -e "$DRIVENAME"
		echo "Finished."
	else
		echo "Drive in use, left mounted. Busy process :"
		#cuts="/proc/"${blah:2}"/cmdline"
		#cat $cuts
		cat "/proc/"${useprocess:2}"/cmdline"
	fi

else
    #Fail if the drive selection was out of range
    echo "Drive selection error!"
fi













