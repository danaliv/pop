# Control Flow

Pop has conditional and loop structures.

## if/else/then

IF statements are a little different in Forth-like languages from what you're
probably used to in other languages. The basic structure is:

    <some code that leaves a number on the stack>
    if
      <code that runs if the number was non-zero>
    else
      <code that runs if the number was zero>
    then

The `if` pops a value off the stack, checks if it's zero, and then jumps to
the appropriate section. The `else` section is optional. Think of the word
`then` as saying, "after the if/else, THEN continue with the rest of the
program."

## Loops

Pop supports unconditional (infinite) loops and while/until loops. Any loop
can be terminated with the `break` word. An infinite loop looks like so:

    begin
        <code>
    repeat

And a while loop is written like this:

    begin
        <code that leaves a number on the stack>
    while
        <loop body>
    repeat

The code between `begin` and `while` is expected to leave an integer on the
stack that indicates whether to execute the loop. You can use `until` instead
of `while` to reverse the meaning.

[examples/fib.pop](../examples/fib.pop) contains an example of a loop.
