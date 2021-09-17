#!/bin/bash
###############################################################################
# Author: Matthew Oyales & Audie Breslin
# Date: 26 May 2021
# Pledge: I pledge my honor that I have abided by the Stevens Honor System.
# Description: Script to provide basic functionality of recycle bin.
###############################################################################

# Declare flags to help indicate when arguments are passed.
help_flag=0
list_junk=0
purge=0
has_files=0

# Declare readonly variable for .junk directory.
readonly RECYCLE=~/.junk

# If the directory "~/.junk" is not found, create it.
if [ ! -d $RECYCLE ]; then
   mkdir $RECYCLE
fi

# Print "usage" message with a here document
print_help() {
cat << EOF
Usage: $(basename $0) [-hlp] [list of files]
   -h: Display help.
   -l: List junked files.
   -p: Purge all files.
   [list of files] with no other arguments to junk those files.
EOF
}

# Section to check input at the command line.
while getopts ":hlp" option; do 
    case "$option" in
        h) help_flag=1
           ;;
        l) list_junk=1
           ;;
        p) purge=1
           ;;
        ?) echo "Error: Unknown option '-$OPTARG'." >&2
           print_help
           exit 1
           ;;
    esac
done

# Skip over all the arguments with flags processed
shift "$((OPTIND-1))"

# Check that number of files is nonzero.
if [[ ! -z "$@" ]]; then 
   has_files=1
fi

# How many flags are there? Are there file arguments?
count=$(( $help_flag + $list_junk + $purge + $has_files ))

# If we have too many flags or we have a flag and a file, error.
if [ $count -gt 1 ]; then
   echo "Error: Too many options enabled."
   print_help
   exit 1
fi

# If -h flag was called or no options selected print the help message.
if [[ $help_flag -eq 1 || $count -eq 0 ]]; then
   print_help
   exit 0
fi

# Case 1: List files in the recycling bin. (calling the -l flag)
if [ $list_junk -eq 1 ]; then
   cd $RECYCLE
   ls -lAF
   exit 0
fi

# Case 2: Purge files in the recycling bin. (calling the -p flag)
if [ $purge -eq 1 ]; then
    rm -r $RECYCLE/* 2>/dev/null  # Errors for an empty ~/.junk are PURGED
    rm -r $RECYCLE/.* 2>/dev/null # Errors for attempting to remove . and .. are PURGED
    exit 0
fi

# Case 3: Moving files into the recycling bin. [listed files]
for f in "$@"; do
    if [[ -f "$f" ||  -d "$f" || -a "$f" ]]; then
    	mv "$f" $RECYCLE
    else
    	echo "Warning: '"$f"' not found."
    fi
done	
    	
exit 0
