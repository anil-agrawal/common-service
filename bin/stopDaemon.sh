#!/bin/bash

fetchServiceName(){
	# Start fetchServiceName()
	echo "Fetching service name"
	
	conf_file=$(dirname "$BASH_SOURCE")/../conf/nix-wrapper.conf

	echo "Conf file : $conf_file"
	
	if [ -f $conf_file ]; then

        #Find SERVICE_WORKING_DIR
		line=`grep wrapper.working.dir $conf_file`
		echo ">>$line<<"
		if [ "$line" ];then
			value=$(echo $line | awk -F"=" '{print $2}' | xargs)
			echo ">>$value<<"
            SERVICE_WORKING_DIR=$value      
		fi

        #Find SERVICE_NAME
		line=`grep wrapper.ntservice.name $conf_file`
		echo $line
		if [ "$line" ];then
			value=$(echo $line | awk -F"=" '{print $2}' | xargs)
			echo ">>$value<<"
            SERVICE_NAME=$value      
		fi
	else
		echo "File : $conf_file not exists"
        return
	fi
	# End fetchServiceName()
}

###
# Main body of script starts here
###

fetchServiceName
echo "Service Name : $SERVICE_NAME"
service $SERVICE_NAME stop
echo "Service : $SERVICE_NAME stopped"