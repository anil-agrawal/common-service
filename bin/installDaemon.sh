#!/bin/bash

EXECUTABLE_OUTPUT=ServiceWrapper

updateServiceFile(){
	# Start updateServiceFile()
	echo "Updating service file"
	
	conf_file=$(dirname "$BASH_SOURCE")/../conf/nix-wrapper.conf
	file=$(dirname "$BASH_SOURCE")/NixService

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

        #Find SERVICE_DISPLAY_NAME
		line=`grep wrapper.ntservice.displayname $conf_file`
		echo $line
		if [ "$line" ];then
			value=$(echo $line | awk -F"=" '{print $2}' | xargs)
			echo ">>$value<<"
            SERVICE_DISPLAY_NAME=$value      
		fi

        #Find PID_DIR
		line=`grep wrapper.daemon.pid.dir $conf_file`
		echo $line
		if [ "$line" ];then
			value=$(echo $line | awk -F"=" '{print $2}' | xargs)
			echo ">>$value<<"
            PID_DIR=$value      
		fi
	else
		echo "File : $conf_file not exists"
        return
	fi

    SERVICE_FILE=$(dirname "$BASH_SOURCE")/$SERVICE_NAME
    cp $file $SERVICE_FILE
    sed -i -- 's|<Service-Name>|'"$SERVICE_NAME"'|' "$SERVICE_FILE"
    sed -i -- 's|<Service-Display-Name>|'"$SERVICE_DISPLAY_NAME"'|' "$SERVICE_FILE"
    sed -i -- 's|<Service-WORKING_DIR>|'"$SERVICE_WORKING_DIR"'|' "$SERVICE_FILE"
    sed -i -- 's|<PID_DIR>|'"$PID_DIR"'|' "$SERVICE_FILE"

    WRAPPER_NAME="$SERVICE_NAME"_WRAPPER
    echo "WRAPPER_NAME : $WRAPPER_NAME"
    cp $(dirname "$BASH_SOURCE")/$EXECUTABLE_OUTPUT $(dirname "$BASH_SOURCE")/$WRAPPER_NAME
    sed -i -- 's|<WRAPPER_NAME>|'"$WRAPPER_NAME"'|' "$SERVICE_FILE"

    cp $SERVICE_FILE /etc/init.d/
    chmod +x /etc/init.d/$SERVICE_NAME
	chkconfig --add $SERVICE_NAME
	mkdir $SERVICE_WORKING_DIR/logs
	# End updateServiceFile()
}

###
# Main body of script starts here
###

updateServiceFile
