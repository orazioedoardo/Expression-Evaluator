# Expression Evaluator
Evaluate a mathematical expression by using the [Shunting-yard algorithm](https://en.wikipedia.org/wiki/Shunting-yard_algorithm) to parse the input into a [Reverse Polish notation](https://en.wikipedia.org/wiki/Reverse_Polish_notation).

### Example

```
$ ./calc
Expression: 23 + 6 * (4 / 3!) ^ -0.51 + ln(21)
Result: 33.42284763163235
```

### Available operators
+ \+ addition
+ \- subtraction
+ \* multiplication
+ / division
+ ^ exponentiation
+ ! factorial
+ % modulus

### Available functions
+ abs(x) absolute value
+ sqrt(x) square root
+ ln(x) natural logarithm
+ log(x,y) logarithm in base x of y
+ sin(x) sine of angle in radians
+ cos(x) cosine of angle in radians
+ tan(x) tangent of angle in radians
+ asin(x) arcsine of angle in radians
+ acos(x) arccosine of angle in radians
+ atan(x) arctangent of angle in radians

### Available parenthesis
+ { [ ( ) ] }

### Available constants
+ pi ratio of a circumference to the diameter
+ e Euler’s constant

### Other
+ -h to show the help

### Notes

Additional functions can be easily implemented by adding its name and the corresponding character to the ```func``` vector and the character with the actual function pointer to either ```map1``` or ```map2``` map, depending on the number of arguments.
