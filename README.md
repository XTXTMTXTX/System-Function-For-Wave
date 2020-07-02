# System Function For Wave

Usage: System Function For Wave.exe WaveFile.wav

It'll generate WaveFile.wav.new.wav as output.

Specify the numerator and denominator coefficients ordered in descending powers of $z$ or $s$

## Examples: 

### Discrete System

```
Discrete System
(2) Continuous System
Select: 1
The order of numerator: 2
The order of denominator: 2
The coefficients of numerator: 0.03333 0 0
The coefficients of denominator: 1 -1.344 0.9025

0.033330*z^2
--------------------------
1.000000*z^2 -1.344000*z^1 0.902500

Equals

0.033330
--------------------------
1.000000 -1.344000*z^-1 0.902500*z^-2
```

#### bode graph

![figure 1](C:\Users\xtx\Desktop\System Transfer Function For Wave\figure 1.jpg)

### Continuous System

```
(1) Discrete System
(2) Continuous System
Select: 2
The order of numerator: 1
The order of denominator: 1
The coefficients of numerator: 1 0
The coefficients of denominator: 1 7.109e4

1.000000*s^1
--------------------------
1.000000*s^1 71090.000000
Sample time: 2.267574e-005 s
```

#### bode graph

![figure 2](C:\Users\xtx\Desktop\System Transfer Function For Wave\figure 2.jpg)

