BIN_DIR=bin
BINARIES=("$BIN_DIR/packer" "$BIN_DIR/target")
ERR=0

for bin in "${BINARIES[@]}" ; do
	if [ ! -f "$bin" ] ; then
		printf "error missing binary : '$bin'\n"
		ERR=1
	fi
done

if [ $ERR -eq 1 ] ; then
	printf "abort due do too many errors.\n"
	exit 1;
fi

printf "compile and launch packer :\n"

make && \
	./"${BINARIES[0]}" "${BINARIES[1]}"
