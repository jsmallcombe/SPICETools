###########################################################################
###########################################################################
#   GRSI Data disk mount script Jun 2017 - J. Smallcombe
#   run on host machine after disk already formatted with gparted or such.
###########################################################################
###########################################################################

DRIVES="data1 data2 data3 data4 data5 data6 data7 data8 data9 data10"

NEWDRIVE=""

for CNAME in $DRIVES
do
	DATEST=$(grep $CNAME /etc/fstab);
	if [ ${#DATEST} -lt 1 ];
	then
		NEWDRIVE=$CNAME
		break
	fi
done

echo $CNAME

#############################################
lsblk
lsblk  --noheadings --raw | awk '$1~/s.*[[:digit:]]/ && $7==""' | grep T
printf "\n Type partition $CNAME then press [Enter] : \n"
read DATPARTITION1

if [ ${#DATPARTITION1} -gt 3 ];
then
	mkdir /$CNAME
	echo "UUID=$(blkid -s UUID -o value /dev/"$DATPARTITION1") /$CNAME                 $(blkid -s TYPE -o value /dev/"$DATPARTITION1")    defaults        0 0" >> /etc/fstab


	echo "/$CNAME @tig(rw,no_root_squash,async)" >> /etc/exports

	#remount drives in new place
	echo "MOUNTING"
	sudo mount -a
fi

ssh -o StrictHostKeyChecking=no root@mother8pi "echo "$HOSTNAME-$CNAME $HOSTNAME:/$CNAME" >> /etc/auto.tig;cd /var/yp; make;service ypserv restart;service ypbind restart"
# " rather than ' on the pass command to evaluate at this end 
