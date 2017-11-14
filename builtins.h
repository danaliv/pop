#ifndef __BUILTINS_H__
#define __BUILTINS_H__

int builtin_pop(void);
int builtin_swap(void);
int builtin_dup(void);
int builtin_rot(void);
int builtin_rotate(void);
int builtin_over(void);
int builtin_pick(void);

int builtin_getenv(void);
int builtin_puts(void);

int builtin_add(void);
int builtin_sub(void);
int builtin_mul(void);
int builtin_div(void);

int builtin_DEBUG_stack(void);
int builtin_DEBUG_puts_all(void);

#endif
