#!/bin/bash

function dobby_property_local() {
	assert_args "dobby_property_local PROPERTY" $@
	require_var "$DOBBY_SERVER"
	require_var "$DOBBY_ROOT"

	local value=$(cat $DOBBY_ROOT/dobby.config/$DOBBY_SERVER/* | grep "^$1" | sed s/^$1\s*=\s*//)
	if test -z "$value"; then
		echo "Can't find property '$DOBBY_SERVER:$1'!" >&2
		echo "Aborting" >&2
		return 1
	fi

	echo "$value"
}