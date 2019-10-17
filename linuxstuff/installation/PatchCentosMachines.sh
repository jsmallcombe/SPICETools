############################################################################################################
############################################################################################################
#   GRSI Centos 7 patch
#   Perform a patch or task on all the centos7 machines, examples given
#   Ensure list is up to date
############################################################################################################
############################################################################################################

ROOTPASWD="TYPE THE PASSWORD IN HERE"

echo "ENTER ROOT PASSWORD:"
read ROOTPASWDIN
if [ ${#ROOTPASWDIN} -gt 1 ];then ROOTPASWD=$ROOTPASWDIN; fi

CENTOSMACHINES="dimorphodon megalania megalodon athena mastodon aquila alphadon xenodon horus belodon bluebuck megalodon tudor windsor"

for CNAME in $CENTOSMACHINES
do
	echo $CNAME
	
	# sshpass -p $ROOTPASWD ssh -o StrictHostKeyChecking=no root@$CNAME 'echo $HOSTNAME;'

	# sshpass -p "tig_4_Isac2" ssh -o StrictHostKeyChecking=no root@$CNAME 'echo $HOSTNAME;head -n -3 /etc/bashrc > /etc/bashrctmp;mv -f /etc/bashrctmp /etc/bashrc;echo "[[ \$- != *i* ]] || fortune | cowsay -f stegosaurus # Only when running interactively" >> /etc/bashrc'

	# sshpass -p $ROOTPASWD ssh -o StrictHostKeyChecking=no root@$CNAME 'systemctl mask fprintd.service;yum remove -y fprintd libfprint;service avahi-daemon stop;yum remove -y  avahi;'

	
	# sshpass -p $ROOTPASWD ssh -o StrictHostKeyChecking=no root@$CNAME 'echo $HOSTNAME >> /home/jsmallcombe/freedata.txt;	df -Th | grep -E "dev" | grep -E "data" >> /home/jsmallcombe/freedata.txt'

done
