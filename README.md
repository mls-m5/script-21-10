# Script 21-10
Somtimes a man just needs to create a programming language.

The ambision is to create a functional (but probably somewhat crappy) language
that that is a combination of go and rust.

Note though. That the more I work on it, the more it just starts to look like
rust.. So I guess when finishing this projekt, I will probably just start using
rust.

Btw. Don't look in this repository for hints on how to code. The code is a mess
and has degraded the latest commits.

The goal would be something like this:

```c++

// str is reference type of String
func printStuff(message str) {
   log.print(message)
}

func main() {
   // String is a owning string
   // (In this case it would make more sense to just use str,
   // but the things you do for examples...)
   let message = String{"hello there!"}
   printStuff(message);
}

```

Or a more convoluted example:
```c++

import os

trait Movable {
   func move(x int, y int) mut
}

struct Apa {
   x int
   y int
}

impl Movable for Apa {
   // override specifies that this function must be implemented
   // by a interface
   impl func move(vx int, vy int) mut {
      log.print("Moved")
      x += vx
      y += vy
   }
}

struct Bepa: impl Movable {
   apa Apa // Will reduce the need for ever inheriting
}

impl Bepa {
   // This is a function that is not associated to a interface
   func somethingEntirelyDifferent() {
      log.print("just some other behaviour")
   }
}

impl Movable for Bepa {
   // Just just call interfaced functions on apa instead of implementing them
   impl Movable -> apa 
   
   // Alternative syntax
   // Forward a single function to apa
   // If the above statement is present this would not be needed
   func move -> apa
}


// Possible out of line way to define member variables
func somethingEvenMoreDifferent(self Bepa) {
   apa.y = 20;
}

// Fat pointer dynamic dispatching
// References const by default
func doStuff(mut Movable &m, len int) {
   m.move(len, len)
}

func main() {
   let apa = Apa{
      x = 10,
      y = 20,
   }
   
   // These two statements should be eqvivalent
   doStuff(apa, 20)
   apa.doStuff(10) // This syntax might be unnessesary
   
   let bepa = Bepa{}
   bepa.doSomethingEntirelyDifferent()
   doSomethingEntirelyDifferent(bepa) // Possible more simplistic
   
   log.print("Arguments to main:")
   for (let arg: os.args) {
      log.print(arg)
   }
   
   let x = {1, 2, 3, 4}
   if (let s = sum(x), sum > 2) {
      log.print("What large sum!: ", s)
   }
   

   if (len(os.args) < 2) {
      log.fatal("No arguments provided")
   }

   switch (os.arg[0]) {
      case "--help":
         log.print("Some help text")
      default:
         log.print("No valid arguments")
   }
}
```

## Requirements

* llvm-14

## References

### Kaleifoscope example code
https://github.com/llvm/llvm-project/blob/main/llvm/examples/Kaleidoscope/Chapter9/toy.cpp
