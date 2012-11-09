function dobby_apt_install() {
	for package; do
		test "$(dpkg -l $package | head -n 6 | tail -n 1 | cut -c 1-2)" == "ii" || apt-get install -y $package | indent >&2
	done
}