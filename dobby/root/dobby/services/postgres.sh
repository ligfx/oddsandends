#!/bin/bash

function install_postgres() {
	echo "-----> Installing postgres" >&2
	dobby_apt_install postgresql-9.1 libpq-dev
	local port=$(cat /etc/postgresql/9.1/main/postgresql.conf | grep "^port" | sed -E s"/^port\s*=\s*([[:digit:]]+).*$/\1/")
	echo "user=postgres"
	echo "password=postgres"
	echo "port=$port"
}

function configure_postgres() {
	sudo -u postgres psql postgres -c "ALTER ROLE postgres WITH PASSWORD 'postgres'"


	local $port=$(cat postgresql.conf | grep "^port" | sed -E s"/^port\s*=\s*([[:digit:]]+).*$/\1/")
	echo "PORT=$port"
}

function start_postgres() {
	/etc/init.d/postgresql start
}
function stop_postgres() {
	/etc/init.d/postgresql stop
}
function restart_postgres() {
	/etc/init.d/postgresql restart
}
function status_postgres() {
	local PRELUDE=$(/etc/init.d/postgresql status | cut -d: -f1)
	if test "$PRELUDE" = "Running clusters"; then
		local STATUS=$(/etc/init.d/postgresql status | cut -d: -f2-)
		if test -z "$(echo $STATUS | tr -d ' ')"; then
			exit 1
		else
			echo $STATUS
			exit 0
		fi
	else
		echo "$(/etc/init.d/postgresql status)"
		exit -1
	fi
}
