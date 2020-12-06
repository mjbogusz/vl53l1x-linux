#!/bin/sh
set -e

cd $(dirname $0)/..
export UNCRUSTIFY_CONFIG="$(dirname $0)/../.uncrustify.cfg"
export PREFIX="/tmp/uncrustify/apcar_teleop"
export EXIT_FAILURE=1

print_help() {
	/bin/echo -e "Usage: ${0} <--help|--fix|--check|--diff>"
	/bin/echo -e "The options are mutually exclusive."
	/bin/echo -e "\t --help		- print this message"
	/bin/echo -e "\t --fix		- fix the files in-place (WARNING: POTENTIALLY DESTRUCTIVE)"
	/bin/echo -e "\t --check	- check conformity with the style (exits with 1 if so)"
	/bin/echo -e "\t --diff		- print a pretty diff between current and fixed version"
}

if [ $# -gt 2 ]; then
	print_help
	exit 1
fi

unc_version=$(uncrustify --version | cut -c14-15)
if [ $unc_version -lt 71 ]; then
	echo "This script requires at least Uncrustify 0.71"
	exit 3
fi

if [ "$1" = "--help" ]; then
	print_help
	exit 0
elif [ "$1" = "--fix" ]; then
	uncrustify --replace --no-backup $(find src include -type f)
elif [ "$1" = "--check" ]; then
	uncrustify --check $(find src include -type f)
elif [ "$1" = "--diff" ]; then
	# Note: uncrustify doesn't output diffs on its own, thus the need of chaining uncrustify and diff
	# Note2: multiple '-exec'-s in find are implicitly AND-ed
	find src include -type f \
		-exec uncrustify {} --prefix ${PREFIX} -q \; \
		-exec diff --minimal --unified --color=always {} ${PREFIX}/{} \;
else
	print_help
	exit 2
fi
