# Advent of Code 2020

## What is Advent of Code

Advent of Code is an Advent calendar of small programming puzzles for a variety of skill sets and skill levels that can be solved in any programming language you like. People use them as a speed contest, interview prep, company training, university coursework, practice problems, or to challenge each other.

You don't need a computer science background to participate - just a little programming knowledge and some problem solving skills will get you pretty far. Nor do you need a fancy computer; every problem has a solution that completes in at most 15 seconds on ten-year-old hardware.

## Goal

I (mostly) try to write both efficient and maintainable code. There are most likely fancier and more efficient solutions out there. I also try to cover any edge cases within the task description (or make my code re-usable [one example is day 17 which was reused with some adaptions on day 24]) even if that code will never be executed (e.g. because I checked my input and it is simply impossible for that to run in any case).

All solutions were thought off and implemented by myself. There was no code-copying or cheating going on (except for the chinese remainder... I only missed one math lecture during my bachelors degree and I'm pretty sure that's why my colleagues were so fast that day).

I probably used the boost library way too much to define grammars and parse input, but I like the security and the fact that I won't need to do any "lookahead" code myself when parsing the input.

## Compilation

I compile my solutions using `g++` on Debian (you can find details about my Debian configuration [here](https://github.com/Abrynos/Dotfiles))

```
$ g++ --version
g++ (Debian 10.2.1-1) 10.2.1 20201207
Copyright (C) 2020 Free Software Foundation, Inc.
This is free software; see the source for copying conditions.  There is NO
warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE
```

## Execution

My solutions all try to read the input from a file called `input.txt` in the execution directory. This is true even for the days, when the input was given directly in the task description. I usually run `g++ ./main.cpp && time ./a.out` to compile and stop the execution time.

