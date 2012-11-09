#!/bin/bash

function dobby_buildpack_ruby_compile() {

	assert_args "compile.sh APP" $@
	local app=$1

	if test -e "Gemfile"; then
		dobby_package ruby

		if test ! -e "vendor/bundle"; then
			mkdir -p shared/vendor/bundle vendor 
			ln -fs ../shared/vendor/bundle vendor/
		fi

		if test -e "config.ru"; then
			if test -e "script/rails"; then
				dobby_buildpack_install_template "$app" "ruby/rails"
			else
				dobby_buildpack_install_template "$app" "ruby/rack"
			fi
			# Hack
			echo "gem 'unicorn'" >> Gemfile
		fi

		echo "-----> Installing depencies with bundler"
		bundle install --deployment --without development test 2>&1 | indent
	fi
	return 1

}