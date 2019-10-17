############################################################################################################
############################################################################################################
#   GRSI Centos 7 install script Jun 2017 - J. Smallcombe
############################################################################################################
############################################################################################################

############rpcbind-0.2.0-38.el7_3.x86_64 is broken so rollback until fixed
yum -y install yum-plugin-versionlock
yum -y downgrade rpcbind-0.2.0-38.el7.x86_64
yum versionlock rpcbind
service ypbind restart
service nfs restart

#################################################################

	#software and mirrors (from Konstantin files)
yum -y install elrepo-release epel-release
rpm --import https://www.elrepo.org/RPM-GPG-KEY-elrepo.org
rpm -Uvh https://www.elrepo.org/elrepo-release-7.0-2.el7.elrepo.noarch.rpm
yum -y install yum-plugin-fastestmirror

	#basic software, useful to have for the rest of the install
yum -y install ed patch wget git libotf gdisk emacs gedit sshpass screen

################################################################
################# Networking (add data drives) #################
################################################################

rm -rf /etc/hosts.allow
rm -rf /etc/hosts.allow
rm -rf /etc/nsswitch.conf.bak
mv /etc/nsswitch.conf /etc/nsswitch.conf.bak

scp -r -o StrictHostKeyChecking=no tigress@jaguar:/etc/hosts.allow /etc/
scp -r -o StrictHostKeyChecking=no tigress@jaguar:/etc/nsswitch.conf /etc/

#################################################################################
#################################################################################
#######################  MOUNT AND EXPORT DATA DRIVES ###########################
#################################################################################
#################################################################################

###############################################
############## MOUNTING STEP ##################
###############################################

DATEST=$(grep data /etc/fstab);
if [ ${#DATEST} -ge 5 ];
then
	printf "\n DATA DRIVES APPEAR CONFIGURED\\MOUNTED \n CHECK AND PROCEED TO EXPORT STEP \n\n";
	df -Th | grep -E "dev" | grep -E "data"
	read -p "Press [Enter]."
else
	printf "\n DATA DRIVES APPEAR UNMOUNTED \n\n";
	lsblk
	lsblk  --noheadings --raw | awk '$1~/s.*[[:digit:]]/ && $7==""' | grep T
	printf "\n Type partition data1 then press [Enter] : \n"
	read DATPARTITION1
	printf "\n Type partition data2 then press [Enter] : \n"
	read DATPARTITION2

	if [ ${#DATPARTITION1} -gt 3 ];
	then
		mkdir /data1
		echo "UUID=$(blkid -s UUID -o value /dev/"$DATPARTITION1") /data1                  $(blkid -s TYPE -o value /dev/"$DATPARTITION1")    defaults        0 0" >> /etc/fstab
	fi

	if [ ${#DATPARTITION2} -gt 3 ];
	then
		mkdir /data2
		echo "UUID=$(blkid -s UUID -o value /dev/"$DATPARTITION2") /data2                  $(blkid -s TYPE -o value /dev/"$DATPARTITION2")    defaults        0 0" >> /etc/fstab
	fi

	#remount drives in new place
	echo "MOUNTING"
	sudo mount -a
fi

####################################################
################## EXPORT STEP #####################
####################################################

rm -rf /etc/exports
echo "/data1 @tig(rw,no_root_squash,async)" >> /etc/exports
echo "/data2 @tig(rw,no_root_squash,async)" >> /etc/exports

#############################################################################
#############################################################################
#######################  Updated firewall settings ##########################
#############################################################################
#############################################################################
	
cd /root
ln -s /home/guest/cluser-backups/iptables.info
ln -s /etc/sysconfig/iptables
yum -y install iptables-services
netstat -i
BOB=($(ip route get 8.8.8.8))
./iptables.info ${BOB[4]}
sudo systemctl stop firewalld && sudo systemctl start iptables; sudo systemctl start ip6tables
systemctl disable firewalld
systemctl mask firewalld
systemctl enable iptables
systemctl enable ip6tables


	#start and re-start services
service nfs start
systemctl enable rpcbind
systemctl start rpcbind
systemctl start nfs-server
service network restart
service ypbind start
service autofs reload

	#rangdom googled lines	
chkconfig ypbind on
chkconfig rpcbind on
chkconfig autofs on

firewall-cmd --permanent --add-service=nfs #Not sure this does anything as using iptableservice not firewallD

printf "\n Time to reboot.\n\n";read -p "Press [Enter]."
reboot
