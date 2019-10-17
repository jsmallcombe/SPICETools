alias rm='rm -I'

#alias listsizec='sudo du --max-depth=1 -k | sort -nr | cut -f2 | xargs -d '\n' du -sh'
alias listsize='du --max-depth=1 -ah | sort -h'

alias ssh='ssh -Y -o StrictHostKeyChecking=no'

alias deathtothewest='pkill -9 -t pts/0'

alias rot='\root -n'
alias root='root -l'
alias grsi='/home/jsmallcombe/GRSISort/bin/grsisort -l'
alias tbro='bash ~/Dropbox/codes/linuxstuff/newBrowser.sh'
alias fbro='bash ~/Dropbox/codes/linuxstuff/newFBrowser.sh'


alias rachel='python /home/jsmallcombe/Gosia/source/Rachel-master/rachel.py'

alias lincomm='cat ~/Dropbox/codes/linuxstuff/linux_commands.txt | more'

alias zync='bash ~/.sync.sh'

alias ejec='bash ~/.ejec.sh'

alias valgrind='valgrind --suppressions=$ROOTSYS/etc/valgrind-root.supp'

alias font='xset fp+ /usr/share/X11/fonts/100dpi & xset fp+ /usr/share/X11/fonts/75dpi'

alias EPICS='font; sshpass -p "Kittygam" ssh -X -o StrictHostKeyChecking=no tigsebt3a@isacepics1'
alias EPICSB='font; sshpass -p "Kittygam" ssh -X -o StrictHostKeyChecking=no tigress@isacepics1'
alias LN2='font; sshpass -p "Kittygam" ssh -X -o StrictHostKeyChecking=no tigln01@lnfillserv'
alias LN3='font; sshpass -p "Kittygam" ssh -X -o StrictHostKeyChecking=no tigln08@lnfillserv'
alias LN4='font; sshpass -p "Kittygam" ssh -X -o StrictHostKeyChecking=no tigress@lnfillserv'
alias epics='font; ssh -X -o StrictHostKeyChecking=no tigsebt3a@isacepics1'
alias epicsB='font; ssh -X -o StrictHostKeyChecking=no tigress@isacepics1'
alias ln2='font; ssh -X -o StrictHostKeyChecking=no tigln01@lnfillserv'
alias ln3='font; ssh -X -o StrictHostKeyChecking=no tigln08@lnfillserv'
alias ln4='font; ssh -X -o StrictHostKeyChecking=no tigress@lnfillserv'


alias bricc=$BrIccHome/bricc
alias briccs=$BrIccHome/briccs

alias cleantemps='find . -type f -name '*~' -delete
find . -type f -name '*.nfs*' -delete'

alias gitclean='dropbox exclude add $(find . -type d -name *.git)'


#alias search='grep -rli --exclude={*~,.*.*,*.o,*.so,*.tar,*.tar.gz,*.zip} './' -e'
#alias cearch='grep -rli --include=*.{hh,h,cc,cpp,c,cxx,C} './' -e'

search() { grep -rli --exclude={*~,.*.*,*.o,*.so,*.tar,*.tar.gz,*.zip} "$@" ./ ;}
cearch() { grep -rli --include=*.{hh,cc,h,cpp,c,cxx,C} "$@" ./ ;}

alias trwin='rdesktop -g 1910x1050 -dTRWIN trwinrds.triumf.ca -r disk:home=$HOME'

alias permissionclean='
find . -type d -exec chmod 755 {} \;
find . -type f -exec chmod 644 {} \;
find . -name "*.sh" -exec chmod 755 {} \;
find . -name "*.py" -exec chmod 755 {} \;
find . -name "*.js" -exec chmod 755 {} \;
find . -name "*.html" -exec chmod 644 {} \;
find . -name "*.css" -exec chmod 644 {} \;
'
alias githelp='lincomm | grep git'

alias exp='ssh -Y -o StrictHostKeyChecking=no dimorphodon'
alias g4='ssh -Y -o StrictHostKeyChecking=no windsor'
alias G4='ssh -Y -o StrictHostKeyChecking=no windsor'
# alias dropbox='~/bin/dropbox.py'


alias rsync='rsync -avhzP'

alias pdfsmush='gs -q -dNOPAUSE -dBATCH -sDEVICE=pdfwrite -sOutputFile=output.pdf'
