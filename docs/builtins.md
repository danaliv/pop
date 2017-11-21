Here's a list of builtin words with descriptions of what they do. The stuff in
parentheses is called *stack notation* and it describes the effect on the
stack. To the left of the `--` is what the word expects the stack to look like
when you call it, and to the right is what it'll look like afterwards.
Generally an `s` means a string and an `i` means a number, but other letters
are used when the type of the value isn't important or to indicate positional
changes. (When defining your own words, it's customary to write their stack
notation after their names in a comment.)

### + ( i1 i2 -- i3 )
Adds the top two values.

### - ( i1 i2 -- i3 )
Subtracts i2 from i1.

### * ( i1 i2 -- i3 )
Multiplies the top two values.

### / ( i1 i2 -- i3 )
Divides i1 by i2.

### = ( x y -- i )
If x and y are equal, pushes 1, otherwise pushes 0. The values can be any type
but only like types can be equal. String comparisons are case-sensitive.

### . ( x -- )
Prints the top value. Aliased to `puts`.

### dup ( x -- x x )
Duplicates the value on the top of the stack.

### else ( -- )
Execution skips to the next `then` that is not part of a nested `if`.

### getenv ( s1 -- s2 )
Gets the value of the environment variable named s1. If no such variable is
set, pushes `""`.

### if ( i -- )
If the value on the stack is non-zero, execution proceeds normally. Otherwise,
execution skips to the next `else` or `then` (not counting any `else` or
`then` that occurs in any intervening nested `if`).

### over ( x y -- x y x )
Duplicates the second value and places the copy on to the top of the stack.
Equivalent to `2 pick`.

### pick ( ni ... n1 i -- ni ... n1 ni )
Duplicates the ith value and places the copy onto the top of the stack.

### pop ( x -- )
Discards the value on the top of the stack.

### puts ( x -- )
Prints the top value. Aliased to `.`.

### rot ( x y z -- y z x )
Moves the third value on the stack to the top. Equivalent to `3 rotate`.

### rotate ( ni ... n1 i -- n(i-1) ... n1 ni )
Moves the ith value on the stack to the top.

### swap ( x y -- y x )
Swaps the top two values.

### then ( -- )
Marks the end of an `if`.
