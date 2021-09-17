#!/bin/bash

readonly ARCHIVE="gcd_assignments.zip"
readonly TEST_SCRIPT="test_gcd.sh"

process_file() {
    # Function arguments are $1, $2, etc.
    echo "Processing $1."

    # Extract the name of the file without the extension.
    # Process everything in the filename up until the extension. (the ".*")
    local base=${1%.*}

    # Cut expected either file input or data on standard in
    # '<<<' places the value of the variable on standard in
    # and is called a 'here string'
    local first="$(cut -d'_' -f3 <<<$base)"  # Subshell - another process within process.
    local last="$(cut -d'_' -f2 <<<$base)"

    echo "Author: $first $last"
    local dirname="$first"_"$last"
    mkdir -p "$dirname"
    mv "$1" "$dirname"
    cp "$TEST_SCRIPT" "$dirname" # Copy the script into dirname
    cd "$dirname"                # Put the program into the directory

    unzip -o "$1"
    rm "$1"

    # Pipe to another program that can print into the file
    bash "$TEST_SCRIPT" | tee grade.txt

    cd - > /dev/null             # Since 'cd -' also pwd's, we can send output to /dev/null

}

unzip -o $ARCHIVE
# This is going to unzip the original plus the other 3.
# We don't want that.
for f in *.zip; do
    if [ "$f" != "ARCHIVE" ]; then
        process_file "$f"
    fi
done
