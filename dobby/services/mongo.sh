#!/bin/bash

function install-mongo() {
	dobby_apt_install mongodb-server | indent >&2
}
function configure-mongo() {
	echo -n;
}
function start-mongo() {
	sudo start mongodb | indent >&2
}
function status-mongo() {
	local status=$(status mongodb)
	echo $status | indent >&2
	test "$status" == "mongodb start/running"
}
