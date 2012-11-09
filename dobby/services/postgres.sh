#!/bin/bash

function install-postgres() {
	dobby_apt_install postgresql-9.1 libpq-dev | indent >&2
	local port=$(cat /etc/postgresql/9.1/main/postgresql.conf | grep "^port" | sed -E s"/^port\s*=\s*([[:digit:]]+).*$/\1/")
	echo "user=postgres"
	echo "password=postgres"
	echo "port=$port"
}
function configure-postgres() {
	local user=$(dobby_property_local "postgres.user")
	local password=$(dobby_property_local "postgres.password")
	local port=$(dobby_property_local "postgres.port")

	cd
    sudo -u postgres psql postgres -c "ALTER ROLE postgres WITH PASSWORD 'postgres'" | indent >&2
}
function remove_postgres() {
    apt-get uninstall postgresql-9.1 libpq-dev --auto-remove -y
}
function start-postgres() {
	sudo /etc/init.d/postgresql start | indent >&2
}
function stop_postgres() {
	/etc/init.d/postgresql stop
}
function restart_postgres() {
	/etc/init.d/postgresql restart
}
function status-postgres() {
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
