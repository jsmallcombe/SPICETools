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
	echo "Target Drive Number?"
	read DriveNumber
fi

#Ensure the user input is in range
if [ $DriveNumber -lt $i ] && [ $DriveNumber -gt 0 ] && [ $i -gt 1 ]; then
	DRIVENAME=${DriveChoice[$DriveNumber]}
	#DRIVENAME=${DriveChoice[$DriveNumber]}"/"

	####Get the user's NetID for generating the folder structure
	#echo "User's NetID?"
	#read NetID
	#NetID=$NetID
	####Grab today's date for generating folder structure
	#Today=$(date +"%m_%d_%Y")
	####Destination for the Logfile
	####Destination=$BackupPath$NetID"_"$Today"/"
	#Full path for the LogFile
	####LogFile=$Destination$NetID"_log.txt"
	####mkdir -p $Destination
	####touch $LogFile
	#Destination for the backup
	#### Destination=$Destination"ditto/"
	#Execute the command
	#### echo "Processing..."
	####sudo ditto "$Source" "$Destination" 2>&1 | tee "$LogFile"

	#SYNCDIR=/media/"$USERNAME"/"$DRIVENAME"/JAEA_SYNC
	#SYNCDIR=/media/smallcombe/USB\ DISK
	#SYNCDIR="$DRIVENAME"JAEA_SYNC
	SYNCDIR="$DRIVENAME""/PC_SYNC"


	#ACTUAL SYNC COMMANDS
	rm -rf "$DRIVENAME"/.*Trash*/*
	mkdir "$SYNCDIR" >.nul 2>&1
	mkdir "$SYNCDIR"/linuxstuff >.nul 2>&1
	cp ~/.bashrc "$SYNCDIR"/linuxstuff/bashrc.txt
	cp ~/.aliases "$SYNCDIR"/linuxstuff/aliases.txt
	cp ~/.newBrowser.C "$SYNCDIR"/linuxstuff/newBrowser.C
	cp ~/.sync.sh "$SYNCDIR"/linuxstuff/sync.sh
	cp ~/.ejec.sh "$SYNCDIR"/linuxstuff/ejec.sh
	cp ~/.linux_commands "$SYNCDIR"/linuxstuff/linux_commands.sh
	cp ~/.root_deuglify.C "$SYNCDIR"/linuxstuff/root_deuglify.C
	cp ~/.rootrc "$SYNCDIR"/linuxstuff/rootrc.txt

	rsync -av --exclude=".*" --exclude ".*/" --exclude "*.so" --exclude "*.o" --exclude "*.*~"	 /home/"$USERNAME"/codes "$SYNCDIR"
	rsync -av --exclude=".*" --exclude ".*/" --exclude "*.so" --exclude "*.o" /home/"$USERNAME"/Documents/ "$SYNCDIR"

	echo "SYNC COMPLETE."

	#ACTUAL SYNC COMMANDS

	#vartest="/media/ubuntu/USB DISK/"
	#useprocess=$(fuser -c "${DRIVENAME%/}")
	useprocess=$(fuser -c "${DRIVENAME}")

	#fuser -c "${vartest%/}"
	#echo $(fuser -c "${vartest%/}")

	if [ ${#useprocess} -lt 2 ]; then
		echo "Unmounting drive."
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













