function dobby_package() {
	assert_args "dobby_package package" $@
	echo "-----> Pretending to install package '$1'"
}
