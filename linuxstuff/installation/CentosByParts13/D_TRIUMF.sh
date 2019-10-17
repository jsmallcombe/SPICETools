############################################################################################################
############################################################################################################
#   GRSI Centos 7 install script Jun 2017 - J. Smallcombe
############################################################################################################
############################################################################################################


# allow ssh keys
echo "SELINUX=permissive" >>/etc/selinux/config
setenforce 0

#########################################################
############## Test if networking works #################
#########################################################

printf "\n Watch the output of the following test lines.\n\n";read -p "Press [Enter]."
############################
############################
######## TEST POINT ########
############################
############################

 # ypwhich (should return server)
if [ "$(ypwhich)" != "mother8pi.triumf.ca" ]; then printf "\nYPWHICH SERVER FAIL \n\n : $(ypwhich) \n\n"; else printf "\nSERVER FOUND\n\n"; fi 
read -p "Output ok? Press [Enter]."

 # should return users, their home directories, their shells, and some numbers
ypcat passwd -k
read -p "Output ok? Press [Enter]."

 # should be able to see files (directories hidden)
ls /home/jhenderson
read -p "Output ok? Press [Enter]."

 # should be able to see files (directories hidden)
ls /tig/"$HOSTNAME"_data1/
read -p "Output ok? Press [Enter]."

# Now we have network access we can copy over those iptables, by executing the following
/root/iptables.info
read -p "Output ok? (VERY IMPORTANT) Press [Enter]."
# THIS BIT WORKING IS VERY IMPORTANT!

###########################################################
########### TRIUMF STUFF (Mostly Printers) ################
###########################################################

	#some triumf specific stuff
rpm -Uvh http://mirror.triumf.ca/triumf/7.2/x86_64/Packages/triumf-release-2.0-2.el7.noarch.rpm
yum -y install triumf-automount-linux triumf-automount-mirror triumf-automount-trshare triumf-printers 

#yum -y install
#triumf-cacert-nss triumf-cacert-openssl triumf-disable-vul_kmod
#triumf-greeter triumf-ntptriumf-release triumf-sl-yumconf
#triumf-ssh triumf-syslog triumf-tcpwrappers triumf-workstation


	#this should install all the printers
systemctl stop cups
systemctl disable cups
echo "ServerName printers.triumf.ca" >> /etc/cups/client.conf
systemctl enable cups
systemctl start cups

	##### At this point, try prining something ####
lpr -P Xerox04 /home/gg/print_test.JPG

