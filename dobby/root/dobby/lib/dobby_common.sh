function assert_args() {
	# assert_args USAGE ARGS...
	local usage="$1"
	local num_args=$(split_count_ws "$usage")
	if test "$#" != $(($num_args + 1)); then
		echo "USAGE: $usage" >&2
		exit 1
	fi
}

function split_count_ws() {
	# split_count_ws STR > NUM
	echo "$1" | tr -s " " | grep -o " " | wc -l
}

function indent() {
	sed -u 's/^/       /'
}