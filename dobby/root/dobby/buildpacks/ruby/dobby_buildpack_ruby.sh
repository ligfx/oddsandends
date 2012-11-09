function dobby_buildpack_rails_configure() {
	# DATABASE_URL=$(dobby_env DATABASE_URL)
	# dobby_rails_databaseyml $DATABASE_URL
	sudo -u postgres psql chute_core -c '' 2>/dev/null || sudo -u postgres createdb chute_core
	cat > config/database.yml <<SCRIPT
development:
  adapter: postgresql
  host: localhost
  database: chute_core
  username: postgres
  password: postgres
SCRIPT
	bundle exec rake db:migrate

	dobby_buildpack_rack_configure
}

function dobby_buildpack_rack_configure() {
	mkdir -p shared/tmp
	echo "-----> Writing shared/unicorn.conf"
	cat > shared/unicorn.conf <<CONF
worker_processes 2
pid "$PWD/shared/tmp/unicorn.pid"
stderr_path "$PWD/shared/tmp/unicorn.log"
stdout_path "$PWD/shared/tmp/unicorn.log"
preload_app false
working_directory "$PWD"
CONF
}

function dobby_buildpack_rack_start() {
	echo "-----> Starting unicorn"
	bundle exec unicorn -p 80 -c shared/unicorn.conf -D
}

function dobby_buildpack_rack_restart() {
	kill -HUP $(cat shared/tmp/unicorn.pid)
}

function dobby_buildpack_rack_stop() {
	kill -QUIT $(cat shared/tmp/unicorn.pid)
	rm -f shared/tmp/unicorn.pid
}

function dobby_buildpack_rack_status() {
	if test -e shared/tmp/unicorn.pid; then
		echo "OK"
		return 0
	else
		echo "Not running"
		return 1
	fi
}

function dobby_buildpack_rack_log() {
	tail -f -n 100 shared/tmp/unicorn.log
}