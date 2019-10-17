############################################################################################################
############################################################################################################
#   GRSI Centos 7 Network Reset
#   When an installed centos machine has trouble accessing network drives run this
############################################################################################################
############################################################################################################

timedatectl set-ntp true

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

systemctl enable nfs-server
systemctl start nfs-server
systemctl status nfs

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



