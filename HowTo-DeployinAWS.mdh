Create a Scientific Linux on AWS (You can probably use any Linux, but the instructions for automatic startup of the program may differ in Linux Versions.

http://docs.aws.amazon.com/AWSEC2/latest/UserGuide/AMIs.html#creating-an-ami?r=5835

Login to the server using SSH

http://docs.aws.amazon.com/AWSEC2/latest/UserGuide/AccessingInstancesLinux.html

>sudo su
>yum install yum-conf-epel
>yum install nodejs
>yum install npm


download files for this project using git to your local computer
using cygwin (or unix) 

Now zip the files up and copy them to your remote AWS instance using cygwin on your local machine.  (Or install git on the remote box and download to that box)
>scp -i KEY.file your.zip root@[AWS_IP_Address_here]:/opt

on aws computer:
>cd /opt
>mkdir yourapp
>cd yourapp
>unzip ../your.zip .

Modify the files for this project as you see fit. 


Register the Node project on your server using
>npm install server

Test using
node server.js

The following command will launch the app in the background:
nohup node server.js &

To automatically start the program:

create a file  /etc/init.d/app-name
 
this should be in the file:

#!/bin/sh
#
# chkconfig: 345 70 90
# description: Reset the system activity logs
#
# /etc/rc.d/init.d/sysstat
# (C) 2000-2009 Sebastien Godard (sysstat <at> orange.fr)
#
### BEGIN INIT INFO
# Provides:             app-name
# Required-Start:
# Required-Stop:
# Default-Start: 3 4 5
# Default-Stop:
# Description: starts app-name node.js server
# Short-Description:
### END INIT INFO

cd /opt/app-name

node server.js &



-----------------------------------------------------------------------
Now you need to make the service run, so register the service using:
> chkconfig app-name on

then from the command prompt:
> service app-name start

now when you reboot, this it should startup the program
 

