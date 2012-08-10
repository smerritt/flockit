# Flockit: add file locking to programs that don't have it.

This library exists solely because rsync doesn't have file locking.

It's not used like a normal library; you don't link against it, and
you don't have to patch your source code to use it. It's inserted
between your program and its libraries by use of `LD_PRELOAD`.

## Example (with libflockit.so in the current directory)
`$ env LD_LIBRARY_PATH=\\`pwd\\`:$LD_LIBRARY_PATH LD_PRELOAD=libflockit.so FLOCKIT_FILE_PREFIX=test rsync SRC DEST`