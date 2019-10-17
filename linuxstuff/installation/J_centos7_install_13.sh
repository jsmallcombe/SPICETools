############################################################################################################
############################################################################################################
#   GRSI Centos 7 install script Jun 2017 - J. Smallcombe
#
#   Apart from the data disk this script should require no adjustment/input 
#   You can copy and paste whole chunks of the script, currently it will halt at user input and reboot points
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
##############################
##############################
######## REBOOT POINT ########
##############################
##############################


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

sshpass -p "Kittygam" scp -r -o StrictHostKeyChecking=no tigress@jaguar:/etc/hosts.allow /etc/
sshpass -p "Kittygam" scp -r -o StrictHostKeyChecking=no tigress@jaguar:/etc/nsswitch.conf /etc/

#################################################################################
#################################################################################
#######################  MOUNT AND EXPORT DATA DRIVES ###########################
#################################################################################
#################################################################################

##################################################
################## FIND STEP #####################
##################################################
# Should provide all the needed information
# df -Th
# ls -l /dev/disk/by-uuid
# df -Th | /usr/bin/grep -E "sda|sdb"
# ls -l /dev/disk/by-uuid | /usr/bin/grep -E "sda|sdb"
# cat /etc/fstab
# lsblk  --noheadings --raw | awk '$1~/s.*[[:digit:]]/ && $7==""' | grep T


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
##############################
##############################
######## REBOOT POINT ########
##############################
##############################

firewall-cmd --list-all
systemctl enable nfs-server
systemctl start nfs-server
systemctl status nfs

printf "\n Time to reboot.\n\n";read -p "Press [Enter]."
reboot

##############################
##############################
######## REBOOT POINT ########
##############################
##############################

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


###############################################################################################################################
###############################################################################################################################
################################ SYSTEM INSTALL IS COMPLETE, REMAINDER CAN BE DONE REMOTELY ###################################
###############################################################################################################################
###############################################################################################################################


############################################################################################################
############################################################################################################
############################################################################################################
						#GENERAL SOFTWARE
############################################################################################################
############################################################################################################
############################################################################################################

# This is because I dont like the new window NOT stealing focus
gsettings set org.gnome.desktop.wm.preferences focus-new-windows 'strict'

cd /opt
chmod -R 777 .

yum -y install gimp gnuplot kate okular inkscape screen gparted

yum -y install fortune-mod
rpm -Uvh http://www.melvilletheatre.com/articles/el7/cowsay-3.03-14.el7.centos.noarch.rpm

echo "# If not running interactively, don't do anything" >> /etc/bashrc
echo "[[ \$- != *i* ]] || fortune | cowsay -f stegosaurus" >> /etc/bashrc

#Root Build Prerequisites (but plenty needed anyway)
yum -y install git make gcc-c++ gcc binutils libX11-devel libXpm-devel libXft-devel libXext-devel gcc-gfortran openssl-devel pcre-devel mesa-libGL-devel glew-devel ftgl-devel mysql-devel fftw-devel cfitsio-devel graphviz-devel avahi-compat-libdns_sd-devel openldap-devel-2.4.39-7.el7.centos.x86_64 python-devel libxml2-devel gsl-devel libpng-devel libjpeg-devel libtiff-devel cmake3

#mesa-libGLU-devel libldap-dev gsl-static

yum -y install xorg-x11-fonts"*"
xset fp+ /usr/share/X11/fonts/100dpi
xset fp+ /usr/share/X11/fonts/75dpi

#yum -y install http://mirror.symnds.com/distributions/gf/el/7/plus/x86_64/cmake-3.3.2-1.gf.el7.x86_64.rpm

################################################
###### RECENT FIXES ############################
################################################

systemctl mask fprintd.service
yum remove -y fprintd libfprint

service avahi-daemon stop
yum remove -y  avahi

############################################################################################################
###### INSTALL AS NEEDED ###################################################################################
############################################################################################################

##############################
	# google-chrome
##############################

echo "[google]" >> /etc/yum.repos.d/google.repo
echo "name=Google - x86_64" >> /etc/yum.repos.d/google.repo
echo "baseurl=http://dl.google.com/linux/rpm/stable/x86_64" >> /etc/yum.repos.d/google.repo
echo "enabled=1" >> /etc/yum.repos.d/google.repo
echo "gpgcheck=1" >> /etc/yum.repos.d/google.repo
echo "gpgkey=https://dl-ssl.google.com/linux/linux_signing_key.pub" >> /etc/yum.repos.d/google.repo
yum -y install google-chrome-stable

chmod 4755 /opt/google/chrome/chrome-sandbox

#ISSUE WITH SELinux PREVENTS CHROME START, THIS FIXES IT BUT IS A MASSIVE SECURITY HOLE.
chcon -t textrel_shlib_t /opt/google/chrome/chrome-sandbox #this is temporary
semanage fcontext -a -t textrel_shlib_t /opt/google/chrome/chrome-sandbox #this is permanent I think, might need "restorecon -R -v /opt/google/chrome/chrome-sandbox"

##########################
	# Skype
##########################

rpm -Uvh http://li.nux.ro/download/nux/dextop/el7/x86_64/nux-dextop-release-0-1.el7.nux.noarch.rpm;
yum -y localinstall http://download.skype.com/linux/skype-4.3.0.37-fedora.i586.rpm;


##########################
	# Dropbox
##########################

yum -y install https://www.dropbox.com/download?dl=packages/fedora/nautilus-dropbox-1.6.2-1.fedora.x86_64.rpm
rm -f /etc/yum.repos.d/dropbox.repo


##########################
	# Radware
##########################

yum install -y readline-devel glib-devel lesstif-devel gtk2-devel

#sshpass -p "Kittygam" ssh -o StrictHostKeyChecking=no gg@$HOSTNAME 'cd ~/packages/rw05/src;make clean;make -j all;make install'

cd /home/gg/packages/rw05/src
make clean
make all
make install

xset fp+ /usr/share/X11/fonts/100dpi
xset fp+ /usr/share/X11/fonts/75dpi

echo "PATH=/opt/rw/bin:\$PATH " >> /etc/profile

tvar="RADWARE_HOME=\/opt\/rw"
sed -i "1s/.*/$tvar/" /opt/rw/.radware.bashrc

echo "source /opt/rw/.radware.bashrc" >> /etc/profile


#########################
	#lATEX
##########################

#https://www.tug.org/texlive/quickinstall.html
yum -y remove tex-* texlive-*
yum -y update
rm -rf /usr/local/texlive/2015
rm -rf ~/.texlive2015
yum -y install perl-Digest-MD5 perl-Tk

cd /home/gg/packages/texlive
./install-tl --profile=texlive.profile

echo "PATH=/usr/local/texlive/2015/bin/x86_64-linux:\$PATH " >> /etc/profile
echo "INFOPATH=/usr/local/texlive/2015/texmf-dist/doc/info:\$INFOPATH " >> /etc/profile
echo "MANPATH=/usr/local/texlive/2015/texmf-dist/doc/man:\$MANPATH " >> /etc/profile
PATH=/usr/local/texlive/2015/bin/x86_64-linux:$PATH 
INFOPATH=/usr/local/texlive/2015/texmf-dist/doc/info:$INFOPATH 
MANPATH=/usr/local/texlive/2015/texmf-dist/doc/man:$MANPATH 

yum -y install kile

yum -y install texinfo*


##########################
	# Root6
##########################

yum install -y cmake3

cd /home/gg/packages/rootgit
rm -rf CMakeFiles
rm -rf CMakeCache.txt #cmake is an asshole
git pull
git tag -l
git checkout -B v6-08-02
git reset --hard v6-08-02 #added because of git problems
rm -rf /home/gg/packages/rootmake/*
cd /home/gg/packages/rootmake
cmake3 /home/gg/packages/rootgit -DCMAKE_INSTALL_PREFIX=/opt/root_v6.08.02 -Dminuit2:BOOL=ON
cmake3 --build . -- -j4


cmake3 --build . --target install
cd /opt
rm -f old_root
mv -f root old_root
ln -s root_v6.08.02 root
chmod -R 777 root*


##########################
	# Root5
##########################

#sshpass -p "Kittygam" ssh -o StrictHostKeyChecking=no gg@$HOSTNAME 'cd ~/packages/root_v5.34.34;make clean;./configure --prefix=/opt/root_v5.34.34 --etcdir=/opt/root_v5.34.34/root/etc/ --all;make -j;make install;cd /opt;ln -s root_v5.34.34 root5'

cd /home/gg/packages/root_v5.34.34
make clean  
./configure --prefix=/opt/root_v5.34.34 --etcdir=/opt/root_v5.34.34/root/etc/ --all
make 
make install
cd /opt
ln -s root_v5.34.34 root5

cd /opt
chmod -R 777 root*

##################################
	# Geant4 NOT FINISHED
##################################


yum -y install expat-devel
glxgears	#if this doesnt work you need to install OpenGL Mesa libraries (or proprietary equivalent)


cd /home/gg/packages/clhepgit/CLHEP
git pull 
git tag -l
git checkout -B CLHEP_2_3_1_0 CLHEP_2_3_1_0
rm -rf /home/gg/packages/clhepgit/build 
mkdir /home/gg/packages/clhepgit/build
cd /home/gg/packages/clhepgit/build
cmake3 -DCMAKE_INSTALL_PREFIX=/opt/CLHEP/ -DCLHEP_BUILD_DOCS=OFF /home/gg/packages/clhepgit/CLHEP/.
make -j
make test
#maybe look at this point
make install


cd /home/gg/packages/xerces-c-3.1.2
make clean
./configure --prefix=/opt/xerces-c
make -j
make install

yum -y install qt.x86_64 qt-devel.x86_64 qt-x11.x86_64 root-graf-qt.x86_64 root-gui-qt.x86_64 libXmu-devel
mkdir /opt/bin
echo "PATH=/opt/bin:\$PATH" >> /etc/profile
PATH=/opt/bin:$PATH 

cd /home/gg/packages/dawn_3_90b
rm -f Makefile
cp Makefile.default Makefile
sed -i 's/INSTALL_DIR = \/usr\/local\/bin\//INSTALL_DIR = \/opt\/bin/g' Makefile
sed -i 's/INSTALL_DIR = \/usr\/local\/bin/INSTALL_DIR = \/opt\/bin/g' Makefile
make clean
make guiclean
make -j
make install


cd /home/gg/packages/geant4.10.02/build
rm -rf ./*
cmake3 -DCMAKE_INSTALL_PREFIX=/opt/geant4.10.02 -DGEANT4_USE_GDML=ON -DXERCESC_ROOT_DIR=/opt/xerces-c/ -DGEANT4_INSTALL_DATA=ON -DGEANT4_USE_QT=ON -DGEANT4_USE_OPENGL_X11=ON -DGEANT4_USE_RAYTRACER_X11=ON -DGEANT4_USE_SYSTEM_CLHEP=ON -DCLHEP_CONFIG_EXECUTABLE=/opt/CLHEP/bin/clhep-config -DCLHEP_INCLUDE_DIR=/opt/CLHEP/include/ -DCLHEP_LIBRARY=/opt/CLHEP/lib/libCLHEP.so -DGEANT4_INSTALL_EXAMPLES=ON /home/gg/packages/geant4.10.02/geant4.10.02



make -j2 VERBOSE=1 | tee GEANT4_make_output.txt
make install



############################################################################
##################### FINISH ###############################################
############################################################################











