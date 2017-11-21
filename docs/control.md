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

