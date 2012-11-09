function dobby_git_clone() {
	assert_args "dobby_git_clone repo branch" $@
	local repo=$1
	local branch=$2
	local shortname=$(basename $repo)
	echo "-----> Cloning $REPO"
	git clone $REPO -b $branch .
	#git clone git+ssh://michaelmaltese@10.0.2.2/Users/michaelmaltese/Projects/chute/$shortname -b $branch .
	rm -fr .git
}
