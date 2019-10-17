#!/bin/bash

##################################

SESSION_TYPE="local"

#If one of the variables SSH_CLIENT or SSH_TTY is defined, it's an ssh session.
#If the login shell's parent process name is sshd, it's an ssh session.
if [ -n "$SSH_CLIENT" ] || [ -n "$SSH_TTY" ]; then
  SESSION_TYPE=remote
# many other tests omitted
else
  case $(ps -o comm= -p $PPID) in
    sshd|*/sshd) SESSION_TYPE=remote;
  esac
fi

export SESSION_TYPE
##################################

export CLICOLOR=1 # # # This will enable colors in ls by default
#export GREP_COLOR=auto # # # Apply grep color automatically

export BrIccHome=/home/$(whoami)/Dropbox/codes/BriccV23-Linux

# # # User specific aliases and functions
source /home/$(whoami)/Dropbox/codes/linuxstuff/aliasesliv.sh

#source ~/rw_current/.radware.bashrc
#fortune | cowsay -w

##################################


# # JAMESLIB=/home/$(whoami)/codes/Jlibrary
# # export JAMESLIB
# # export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$JAMESLIB/bin:$JAMESLIB
# # export LIBRARY_PATH=$LIBRARY_PATH:$JAMESLIB/bin:$JAMESLIB
# # export CPATH=$CPATH:$JAMESLIB/include
# # #export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$JAMESLIB/src:$JAMESLIB/include:/home/$(whoami)/local/lib
# # export PATH=$PATH:$JAMESLIB/include:$JAMESLIB/src:$JAMESLIB/bin
# # export LIBRARY_PATH=$LIBRARY_PATH:$JAMESLIB/src:$JAMESLIB/include:$JAMESLIB


#	source /opt/geant4.10.04/share/Geant4-10.4.1/geant4make/geant4make.sh

##################################

if [ $HOSTNAME == "npb" ];
then
	ssh -Y -o StrictHostKeyChecking=no nsl84
fi



if [ $HOSTNAME == "nsl84" ];
then

    if [ -f "/usr/local/root6.14_debian9/bin/thisroot.sh" ]
    then
        source /usr/local/root6.14_debian9/bin/thisroot.sh
    # 	export LD_LIBRARY_PATH=/opt/root/lib:$LD_LIBRARY_PATH

        export GRSISYS=/home/$(whoami)/GRSISort
        source $GRSISYS/thisgrsi.sh
    # 	export PATH=$GRSISYS/bin:$PATH
    # 	export LD_LIBRARY_PATH=$GRSISYS/libraries:$LD_LIBRARY_PATH
        
    #else
    #	source /home/$(whoami)/root6/bin/thisroot.sh

        if [ "$SESSION_TYPE" = "local" ]; then
            alias root='root -l /home/$(whoami)/Dropbox/codes/linuxstuff/root_gl.C'
            echo 
        fi
    fi
	
    if [ -f "/usr/local/geant4.10.04.p2/bin/geant4.sh" ]
    then
        source /usr/local/geant4.10.04.p2/bin/geant4.sh
    fi

    export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/home/$(whoami)/local/lib
    export LIBRARY_PATH=$LIBRARY_PATH:/home/$(whoami)/local/bin:/home/$(whoami)/local:/home/$(whoami)/local/lib
    export CPATH=$CPATH:/home/$(whoami)/local/include

    source /home/$(whoami)/codes/JAnalysisTools/bin/thisjlib.sh
    source /home/$(whoami)/codes/JPhysics/bin/thisjlib.sh
    source /home/$(whoami)/codes/SPICETools/bin/thisjlib.sh
    
fi

#bash /home/$(whoami)/codes/linuxstuff/SpaceCheck.sh

