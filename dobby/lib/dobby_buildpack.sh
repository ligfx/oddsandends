function dobby_buildpack_install_for() {
	assert_args "dobby_buildpack_install_for app" $@
	local app=$1

	for buildpack in $DOBBY_ROOT/buildpacks/*; do
		if dobby_buildpack_$(basename $buildpack)_detect; then
			dobby_buildpack_$(basename $buildpack)_compile $app
			return
		fi
	done
	echo "-----> Can't find a buildpack!"
	exit 1
}

function dobby_buildpack_install_template() {
	assert_args "dobby_buildpack_install_template APP TEMPLATE" $@
	local app=$1
	local template=$2
	local shortname=$(basename $template)
	mkdir -p $DOBBY_ROOT/services/$app.d
	cat $DOBBY_ROOT/buildpacks/$template.sh.template | sed s/'$$APP'/"$app"/g > $DOBBY_ROOT/services/$app.d/$shortname.sh
}