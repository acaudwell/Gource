#! /bin/sh
if (autoreconf -f -i) ; then
	echo "autoreconf ran successfully."
	echo "Initializing submodules..."
	if (git submodule init) ; then
		echo "Updating submodules..."
		if (git submodule update --recursive) ; then
			echo "Run './configure && make' to continue."
		else
			echo "Error: could not update submodules!"
		fi
	else
		echo "Error: could not initialize submodules!"
	fi
else
	echo "Running autoreconf failed, please make sure you have autoconf installed."
fi
