#!/usr/bin/gawk -f

BEGIN {
  after_first_theta=0;
  after_second_theta=0;
  
  first_p=1;
  last_p=0;

  PI=4*atan2(1,1);
  DEG=PI/180.;
}

/^ THETA =/ {
  if (after_first_theta) {
    print_line(t,ns,p,x);
    to=t;  # old theta
    if (after_second_theta) {
      dto=dt;
    }
  }

  t=$3;  # new theta

  if (after_first_theta) {
    dt=t-to; # delta theta
    if (after_second_theta) {
      if ( ((dt-dto)/dt)^2 >1.e-24) {
	print "Thetas are not equally spaced!";
	printf "  dt=%40.28e\n",1.*dt;
	printf " dto=%40.28e\n",1.*dto;
	exit;
      }
    }
    after_second_theta=1
  }
  after_first_theta=1;
}

/^ LEVEL *EXCITATION/ { # read probabilities and cross sections 
#  print "ex"
  getline;
  ns=0;
  while (1) {
#    print "new line:",$0;
    if ($0 ~ /C L X/) break;  # end of list
#    print $1;
    if ($1 ~ /[0-9][0-9]*/) {
      ns++;
      p[ns]=$2;
      x[ns]=$3;
      if (ns != $1) {
	print "Something is wrong with the number of states";
	print "ns=",ns;
	print "$0=",$0;
	exit;
      }
      if ($2 ~ /nan/) p[ns]=0;
      if ($3 ~ /nan/) x[ns]=0;
#      print "store:",ns," ",p[ns]," ",x[ns]
    }
    getline;
  }
#  print_line(t,ns,p,x);
#  print "ENDEX";
}

END {
  last_p=1;
  print_line(t,ns,p,x);
  printf "#%5s","tot:"
  for (i=1;i<=ns;i++) {
    # normalize
    pt[i]*=0.5*dt*DEG;
    xt[i]*=2*PI*dt*DEG;
    printf " %12.5e %12.5e",pt[i],xt[i]
  }
  print "";
}

function print_line (t,ns,p,x,i) {
#  print "printline ",ns;
  w=1.;                     # default integration weight
  if (first_p || last_p) {  # if first or last point
    w=0.5;                  # take only 1/2 as integration weight
    if (first_p) {  # long comment
      print "# column #  1: theta (deg)";
      for (i=1;i<=ns;i++) {
	printf "# column # %2d:  probability  to excite state # %2d\n",2*i,i;
	printf "# column # %2d: cross section to excite state # %2d\n",2*i+1,i;
      }
      printf "#%5s","theta";
      for (i=1;i<=ns;i++) {
	printf " %9s %2d %9s %2d","prob.",i,"x-sec.",i;
      }
      print "";
    }
    first_p=0;
  }
  printf "%6.2f",t;
  for (i=1;i<=ns;i++) {
    pt[i]+=w*sin(t*DEG)*p[i];
    xt[i]+=w*sin(t*DEG)*x[i];
    printf " %12.5e %12.5e",p[i],x[i];
  }
  print "";
  if (last_p) {
    printf "#%5s","theta";
    for (i=1;i<=ns;i++) {
      printf " %9s %2d %9s %2d","prob.",i,"x-sec.",i;
    }
    print "";
  }
#  print "printline done.";
}
