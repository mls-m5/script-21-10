# Script 21-10
Somtimes a man just needs to create a programming language.

The goal would be something like this

```c++

import os

interface Movable {
   func move(x int, y int) mut
}

struct Apa: implement Movable {
   var x int
   var y int
   
   // override specifies that this function must be implemented
   // by a interface
   implement func move(x int, y int) mut {
      log.print("Moved")
      _x += x // Underscore separates member variables
      _y += y
   }
}

struct Bepa: implement Movable {
   var apa Apa // Will reduce the need for ever inheriting

   // Just just call interfaced functions on apa instead of implementing them
   implement Movable -> apa 
   
   // Alternative syntax
   // Forward a single function to apa
   // If the above statement is present this would not be needed
   implement func move -> apa
   
   function somethingEntirelyDifferent {
      log.print("just some other behaviour")
   }
}

// Fat pointer dynamic dispatching
// References const by default
func doStuff(Movable mut &m, int len) {
   m.move(len, len)
}

func main() {
   let apa = Apa{
      .x = 10,
      .y = 20,
   }
   
   // These two statements should be eqvivalent
   doStuff(apa, 20)
   apa.doStuff(10) // This syntax might be unnessesary
   
   let bepa = Bepa{}
   bepa.doSomethingEntirelyDifferent()
   
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

## References

[https://github.com/ghaiklor/llvm-kaleidoscope/blob/master/ast/](Kaleifoscope example code)
