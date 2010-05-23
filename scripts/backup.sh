#!/bin/sh
db_user=`/home/sys/backup-info.sh user`
db_pass=`/home/sys/backup-info.sh pass`
db_name=`/home/sys/backup-info.sh db`
db_host=`/home/sys/backup-info.sh host`
bkdir=`/home/sys/backup-info.sh bkdir`

bkfile=`date "+%Y-%m-%d-%H_%M_%S.sql.gz"`
bkfile="_$bkfile"
bkfile="$bkdir/$db_name$bkfile"

mysqldump -h $db_host -u $db_user --password="$db_pass" $db_name `mysql -h $db_host -u $db_user --password="$db_pass" -e "SHOW TABLES" -B $db_name | grep -v "^cache_" | tail -n +2 | sed -n -e ":a" -e "$ s/\n/ /gp;N;b a" -` | gzip -c > $bkfile
