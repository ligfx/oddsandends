#!/bin/bash

function dobby_apt_install() {
	for package; do
		echo "Checking for package $package"
		if test "$(dpkg -l $package | head -n 6 | tail -n 1 | cut -c 1-2)" != "ii";then
			sudo DEBIAN_FRONTEND=noninteractive apt-get install -y $package
		fi
		echo "Package '$package' installed"
	done
}