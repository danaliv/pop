# pop

Pop is a Forth-like scripting language. Or it will be, anyway. Right now you can do Hello World and that's about it. :)

## Usage

`pop program-file`  
`pop -e program-code`  
`pop` (REPL if stdin is a terminal)  
`pop` (execute code in stdin if it's a pipe)

## Examples

Forth-like languages are very simple. You have a stack, and "words" (procedures/functions) that operate on that stack. Your program pushes values onto the stack and then calls words to do things with those values. The following program pushes the string `"hello world"` onto the stack and then calls `puts` to display it:

```
"hello world" puts
```

`puts` can also print numbers. Here we push 2 onto the stack, push another 2 onto the stack, add them, and then display the result:

```
2 2 +
"2 + 2 ="
puts
puts
```

Confused? What happens here is that we push two 2s onto the stack, then we call `+`, which pops the top two values off the stack, adds them, and pushes the result of the addition onto the stack. So at that point the stack has a single 4 on it. Next, we push a string, `"2 + 2 = "`. So our stack has a 4 and a string on it, with the string on top. The first `puts` pops a value off the top of the stack (in this case, the string) and displays it; the second `puts` pops another value (the 4) and displays that.

## Defining Words

You can define new words with the `:` character. Here's a word that multiples a number by two:

```
( multiplies a number by two )
: twice
  2 *
;
```

Now you can write code like `7 twice` and you'll be left with 14 on the stack. (Note that there's a comment up there too. Anything in parentheses is ignored by the compiler.)

## REPL

Pop has a REPL so you can play around with it. It very helpfully displays the top three values of the stack. This is nice so you don't have to visualize the stack in your head as you're programming and you can more clearly see the effects of your code.

To enter the REPL, just run `pop` with no arguments.

To get out of the REPL, send an EOF (control-D).

## Builtin Words

Here's a list of builtin words with descriptions of what they do. The middle column uses something called *stack notation* to describe the effect on the stack. To the left of the `--` is what the word expects the stack to look like when you call it, and to the right is what it'll look like afterwords. Generally an `s` means a string and an `i` means a number, but other letters are used when the type of the value isn't important or to indicate positional changes. When defining your own words, it's customary to write their stack notation after their names in a comment.

| Word | Stack | Description |
|------|-------|-------------|
| pop  | ( x -- ) | Removes the value on top of the stack. |
| swap | ( x y -- y x ) | Swaps the top two values. |
| dup  | ( x -- x x ) | Duplicates the value on top of the stack. |
| rot  | ( x y z -- y z x ) | Moves the third value on the stack to the top. |
| over | ( x y -- x y x ) | Duplicates the second value on the stack. |
| rotate | ( ni ... n1 i -- n(i-1) ... n1 ni ) | Moves the ith value on the stack to the top. (`rot` is the same as `3 rotate`.) |
| pick | ( ni ... n1 i -- ni ... n1 ni ) | Duplicates the ith value on the stack. (`over` is the same as `2 pick`.) |

## Inspiration

I seem to be compelled to implement a Forth-like language every five years or so, like some kind of weird mental pilgrimage. They're all inspired by MUF, the programming language used by fuzzball MUCK systems.