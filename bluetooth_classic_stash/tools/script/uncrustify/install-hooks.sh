#!/bin/sh

REPOFILE="repos.txt"
HOOK="pre-commit"

# copy hooks to the directory specified as parameter
copy_hook() {
    echo "Copying $HOOK hook to $1/hooks. "
    mkdir -p "$1/.git/hooks"
    cp -Rf -- "$GITHOOKDIR/$HOOK" "$1/.git/hooks" || exit 1

    # Check if hook is executable. Fix if not.
    if [ ! -x "$1/.git/hooks/$HOOK" ] ; then
        chmod a+x $1/.git/hooks/$HOOK
    fi
}

echo ""
echo "Git pre-commit hook installation."
echo "Hook source directory: ${GITHOOKDIR="."}"
echo ""

if [ ! -e "$REPOFILE" ] ; then 
    echo "Error: couldn't find $REPOFILE (tip: try running this script in tool/script/uncrustify)"
    exit 1
fi

if [ ! -e "$GITHOOKDIR/$HOOK" ] ; then
    echo "Error: couldn't find $GITHOOKDIR/$HOOK (tip: try running this script in tool/script/uncrustify)"
    exit 1
fi

# Copy the pre-commit hook into each git repository
while IFS= read -r repo
do
    repo_path="../../../../../../$repo"
    if [ ! -d $repo_path ] ; then
        echo "Error: could not find repo $repo"
        echo "Hint: Add a new empty line in repos.txt"

    else
        dest_dir=$(git -C $repo_path rev-parse --show-toplevel)
        if [ ! $? ] ; then
            echo "Error: could not find Git directory for repo $repo"
        else
            copy_hook $dest_dir
        fi
    fi
done < $REPOFILE

echo ""
echo "Finished installation."
echo $UNCRUSTIFY_CHECK_SCRIPT_PATH > /dev/null
grep -q "UNCRUSTIFY_CHECK_SCRIPT_PATH" ~/.bashrc || echo "export UNCRUSTIFY_CHECK_SCRIPT_PATH="$PWD"" >> ~/.bashrc
grep -q "META_UTIL_PATH" ~/.bashrc || echo "export META_UTIL_PATH="$PWD"" >> ~/.bashrc

echo ""
echo "Hint: Open new Terminal to save the changes"