# Unnamed Language

## Ideas

- first-class functions
- pipes
- immutable, shared references
- pattern matching via. match operator
- no statements, all branches must return a value
- support closures
- garbage collected
- data types: nil, num, str, bool, enum, map,

## Syntax

### Grammar

### Examples

```
a = 1                       // variable a matched with 1
b = (x, y) -> x * y         // anonymous function definition
c = {a, b, 'string'}        // match on the full list
{d, _} = c                  // here, 'd' holds a reference to 'a'
fn sq(z) -> b(z, z)         // named function definition

fn counter() -> {           // supports closures
  count = 0                     
  { 
    inc: () -> count = count + 1,
    check: () -> print(count) 
  }
}
{inc, check} = counter()
check()                     // prints 0
inc()                       // increments c
check()                     // prints 1

fn fib(0) -> 0              // pattern match on function arguments
fn fib(1) -> 1
fn fib(x) -> fib(x - 1) + fib(x - 2)

fn add_one(x) -> x + 1
fn sub_two(x) -> x - 2

n = 5                       // pipe operator argument passing
  |> add_one()
  |> sub_two() 
```
