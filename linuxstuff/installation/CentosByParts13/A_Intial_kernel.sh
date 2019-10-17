############################################################################################################
############################################################################################################
#   GRSI Centos 7 install script Jun 2017 - J. Smallcombe
############################################################################################################
############################################################################################################

	#start of network bits
yum -y install ypbind rpcbind 
cd /etc/sysconfig/network-scripts/;sed -i -e 's/ONBOOT=no/ONBOOT=yes/' ifcfg-* #Added for new computers which default LAN OFF

echo "NISDOMAIN=TIG-NIS" >> /etc/sysconfig/network
echo "domain TIG-NIS server mother8pi" >> /etc/yp.conf

	#make sure we are up to date before going further
yum update -y
ls -l /boot    #check new kernel is installed

timedatectl set-ntp true

printf "\n Time to reboot.\n\n";read -p "Press [Enter]."
reboot