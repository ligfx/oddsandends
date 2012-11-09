#!/bin/bash

function dobby_buildpack_ruby_detect() {

	if test -e "Gemfile"; then
		if test -e "config.ru"; then
			if test -e "script/rails"; then
				echo "-----> Detected Ruby/Rails app!"
			else
				echo "-----> Detected Rack app!"
			fi
			return 0
		fi
	fi
	return 1

}