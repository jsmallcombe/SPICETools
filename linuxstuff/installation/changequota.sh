

#Set soft block limit to 10 Gb, hard block limit to 20 Gb for user jsmallcombe on /home1:
# quotatool -u jsmallcombe -b -q 10G -l 20G /home1



users=`$(echo ypcat passwd) | awk -F':' '{ print $1}'`
for u in $users
do
	echo "quotatool -u $u -b -q 20G -l 30G /home1"
	quotatool -u $u -b -q 20G -l 30G /home1
done

# mapfile -t users < <(ypcat passwd -k)
# for (( i=0; i<${#users[@]}; i++ ));
# do
# 	set -- ${users[i]};
# 	uname=$1
# 	echo "quotatool -u $uname -b -q 15G -l 20G /home1"
# done

#Set the global block grace period to one week on /home:
quotatool -u -b -t "1 week" /home1


# List of exceptions
# gg account is for software compilation 
quotatool -u gg -b -q 100G -l 200G /home1

