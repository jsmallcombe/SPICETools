alias rm='rm -I'

#alias listsizec='sudo du --max-depth=1 -k | sort -nr | cut -f2 | xargs -d '\n' du -sh'
alias listsize='du --max-depth=1 -ah | sort -h'

alias ssh='ssh -Y -o StrictHostKeyChecking=no'

alias deathtothewest='pkill -9 -t pts/0'

alias rot='\root -n -l'
alias root='root -l'
alias grsi='/home/jsmallcombe/GRSISort/bin/grsisort -l'
alias tbro='bash ~/Dropbox/codes/linuxstuff/newBrowser.sh'
alias fbro='bash ~/Dropbox/codes/linuxstuff/newFBrowser.sh'

alias rachel='python /home/js/Gosia/source/Rachel-master/rachel.py'

alias lincomm='cat ~/Dropbox/codes/linuxstuff/linux_commands.txt | more'

alias zync='bash ~/.sync.sh'

alias ejec='bash ~/.ejec.sh'

alias valgrind='valgrind --suppressions=$ROOTSYS/etc/valgrind-root.supp'

alias font='xset fp+ /usr/share/X11/fonts/100dpi & xset fp+ /usr/share/X11/fonts/75dpi'

alias bricc=$BrIccHome/bricc
alias briccs=$BrIccHome/briccs

alias cleantemps='find . -type f -name '*~' -delete
find . -type f -name '*.nfs*' -delete'

alias gitclean='dropbox exclude add $(find . -type d -name *.git)'

#alias search='grep -rli --exclude={*~,.*.*,*.o,*.so,*.tar,*.tar.gz,*.zip} './' -e'
#alias cearch='grep -rli --include=*.{hh,h,cc,cpp,c,cxx,C} './' -e'

search() { grep -rli --exclude={*~,.*.*,*.o,*.so,*.tar,*.tar.gz,*.zip} "$@" ./ ;}
cearch() { grep -rli --include=*.{hh,cc,h,cpp,c,cxx,C} "$@" ./ ;}


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

alias rsync='rsync -avhzP'

alias pdfsmush='gs -q -dNOPAUSE -dBATCH -sDEVICE=pdfwrite -sOutputFile=output.pdf'

alias triumf='ssh jsmallcombe@grsigate.triumf.ca'
alias tbridge='ssh -L 2112:mother8pi.triumf.ca:22 jsmallcombe@grsigate.triumf.ca'
# tpull="scp -i ~/.ssh/id_rsa.pub -P 2112 jsmallcombe@127.0.0.1:"
alias tpull='echo "scp -rP 2112 jsmallcombe@127.0.0.1:"'

alias proofpull='scp -rP 2112 S1750@127.0.0.1:/home/S1750/packages/GRSISort/GRSIProof/* ~/Dropbox/codes/GRIFAnalysis/Proof/'
alias proofpush='scp -rP 2112 ~/Dropbox/codes/GRIFAnalysis/Proof/* S1750@127.0.0.1:/home/S1750/packages/GRSISort/GRSIProof/'

alias gls='/home/js/local/rw05/src/gtkgls'

alias paint='kolourpaint'
