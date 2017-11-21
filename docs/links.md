# Links

The `.link` directive is used to import words from another file. The linked file can be a Pop program or a specially constructed shared library.

In its most basic form, `.link` is invoked like so:

    .link "path/to/file"

The string can be an absolute path, a relative path, or an "anchored" path (a path that begins with `./` or `../`). Anchored paths are evaluated relative to the linking file's directory. (In the REPL, "the linking file's directory" is the current working directory.)

Paths must omit the file extension. Pop automatically appends `.pop` or your system's shared library extension when searching for the requested file.

## Search Paths

By default, relative paths are treated as anchored paths beginning with `./`. However, if the `POPPATH` environment variable is set with a list of directories separated by `:`, those directories are searched for the linked file first, and then `./` is searched.

For example, consider a program in `/home/grendel/app.pop` that invokes `.link "lib/util"`. With `POPPATH` unset, Pop will consider the following candidates when searching for the linked file:

    /home/grendel/lib/util.pop
    /home/grendel/lib/util.{shared lib suffix}

However, with `POPPATH` set to `/usr/lib/pop:/usr/local/lib/pop`, the list of candidates will look like this:

    /usr/lib/pop/lib/util.pop
    /usr/lib/pop/lib/util.{shared lib suffix}
    /usr/local/lib/pop/lib/util.pop
    /usr/local/lib/pop/lib/util.{shared lib suffix}
    /home/grendel/lib/util.pop
    /home/grendel/lib/util.{shared lib suffix}

## Invoking Imported Words

The words imported from a linked file are invoked using a special prefix notation. By default, the prefix is the last element of the linked file's path (its basename), minus the extension. Building on the previous example, the prefix would be `util`. A word named `split` in the example linked file would thus be invoked as `util.split`.

If you wish to use a different prefix, `.link` can also be invoked like so:

    .link prefix "path/to/file"

Again building on the previous example, changing the directive to `.link u "lib/util"` means the `split` word in the linked file is now invoked as `u.split`.

## Execution of Linked Programs

The first time a file is linked, it is compiled and executed. The result of compilation is cached in RAM. A linked file will only be compiled and executed once during the lifetime of a program; all subsequent links will simply point to the cached compilation, and the execution step is skipped. This allows libraries to include one-time initialization routines in their main section.

## Shared Libraries

`.link` can load words from native shared libraries. All functions in a linked shared library that begin with `POP_` are available for invocation (without the `POP_`).