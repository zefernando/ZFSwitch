/u01/SCMDES/CardCo/stop.sh
cat /dev/null > /u01/SCMDES/CardCo/fmt.log
cat /dev/null > /u01/SCMDES/CardCo/sqlnet.log
chown scm. /u01/SCMDES/CardCo/*.log
cat /dev/null > /u01/SCMDES/CardCo/launch.out
chown scm. /u01/SCMDES/CardCo/*.out
cat /dev/null > /u01/SCMDES/CardCo/logDir/cardco.log
cat /dev/null > /u01/SCMDES/CardCo/logDir/logtcp.log
chown scm. /u01/SCMDES/CardCo/logDir/*.log
cat /dev/null > /u01/SCMDES/CardCo/debug/areacomp.debug
cat /dev/null > /u01/SCMDES/CardCo/debug/auth.debug
cat /dev/null > /u01/SCMDES/CardCo/debug/extern.debug
cat /dev/null > /u01/SCMDES/CardCo/debug/paybal.debug
cat /dev/null > /u01/SCMDES/CardCo/debug/launch.debug
chown scm. /u01/SCMDES/CardCo/debug/*.debug

