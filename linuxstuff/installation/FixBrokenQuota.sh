###########################################################################
###########################################################################
#   GRSI quota reset script Jun 2017 - J. Smallcombe
#   run on mother8pi and quota functionality will be restored
###########################################################################
###########################################################################

repquota -st /home1 | grep days
quotaoff -u /home1
quotacheck -um /home1
quotaon -u /home1
repquota -st /home1 | grep days


