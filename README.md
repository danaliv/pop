# pop

Pop is a Forth-like scripting language.

## Usage

`pop program-file`  
`pop -e program-code`  
`pop` (REPL if stdin is a terminal)  
`pop` (execute code in stdin if it's a pipe)

## Examples

Forth-like languages are very simple. You have a stack, and "words"
(procedures/functions) that operate on that stack. Your program pushes values
onto the stack and then calls words to do things with those values. The
following program pushes the string `"hello world"` onto the stack and then
calls `puts` to display it:

```
"hello world" puts
```

(You can also just do a `.` instead of `puts`. They're the same.)

`puts` can also print numbers. Here we push 2 onto the stack, push another 2
onto the stack, add them, and then display the result:

```
2 2 + "2 + 2 =" puts puts
```

What happens here is that we push two 2s onto the stack, then we call `+`,
which pops the top two values off the stack, adds them, and pushes the result
of the addition onto the stack. So at that point the stack has a single 4 on
it. Next, we push a string, `"2 + 2 = "`. So our stack has a 4 and a string on
it, with the string on top. The first `puts` pops a value off the top of the
stack (in this case, the string) and displays it; the second `puts` pops
another value (the 4) and displays that.

## Defining Words

You can define new words with the `:` character. Here's a word that multiples
a number by two:

```
( multiplies a number by two )
: twice
  2 *
;
```

Now you can write code like `7 twice` and you'll be left with 14 on the stack.
(Note that there's a comment up there too. Anything in parentheses is ignored
by the compiler.)

## REPL

Pop has a REPL so you can play around with it. It very helpfully displays the
top three values of the stack. This is nice so you don't have to visualize the
stack in your head as you're programming and you can more clearly see the
effects of your code.

To enter the REPL, just run `pop` with no arguments.

To get out of the REPL, send an EOF (control-D).

## Builtin Words

See [docs/builtins.md] for a list of builtin words.

## Inspiration

I seem to be compelled to implement a Forth-like language every five years or
so, like some kind of weird mental pilgrimage. They're all inspired by MUF,
the programming language used by fuzzball MUCK systems.
