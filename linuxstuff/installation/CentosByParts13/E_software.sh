############################################################################################################
############################################################################################################
#   GRSI Centos 7 install script Jun 2017 - J. Smallcombe
############################################################################################################
############################################################################################################

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

# chmod 4755 /opt/google/chrome/chrome-sandbox

#ISSUE WITH SELinux PREVENTS CHROME START, THIS FIXES IT BUT IS A MASSIVE SECURITY HOLE.
# chcon -t textrel_shlib_t /opt/google/chrome/chrome-sandbox #this is temporary
# semanage fcontext -a -t textrel_shlib_t /opt/google/chrome/chrome-sandbox #this is permanent I think, might need "restorecon -R -v /opt/google/chrome/chrome-sandbox"

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
git checkout -B v6-10-04
git reset --hard v6-10-04 #added because of git problems
rm -rf /home/gg/packages/rootmake/*
cd /home/gg/packages/rootmake
cmake3 /home/gg/packages/rootgit -DCMAKE_INSTALL_PREFIX=/opt/root_v6.10.04 -Dminuit2:BOOL=ON
cmake3 --build . -- -j4


cmake3 --build . --target install
cd /opt
rm -f old_root
mv -f root old_root
ln -s root_v6.10.04 root
chmod -R 777 root*


##########################
	# Root5
##########################

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


cd /home/gg/packages/geant4.10.04/build
rm -rf ./*
cmake3 -DCMAKE_INSTALL_PREFIX=/opt/geant4.10.04 -DGEANT4_USE_GDML=ON -DXERCESC_ROOT_DIR=/opt/xerces-c/ -DGEANT4_INSTALL_DATA=ON -DGEANT4_USE_QT=ON -DGEANT4_USE_OPENGL_X11=ON -DGEANT4_USE_RAYTRACER_X11=ON -DGEANT4_INSTALL_EXAMPLES=ON /home/gg/packages/geant4.10.04/geant4_10_04_p01


make -j2 VERBOSE=1 | tee GEANT4_make_output.txt
make install



############################################################################
##################### FINISH ###############################################
############################################################################
