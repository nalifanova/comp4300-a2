# Assignment 02 for COMP 4300 Course

--------------------------------------
## About the course
[Video Lectures on YouTube](https://www.youtube.com/playlist?list=PL_xRyXins848nDj2v-TJYahzvs-XW9sVV)
COMP 4300 - C++ Game Programming (2022-09) by Dave Churchill

## About the assignment

The main goal of the assigment:
- Separation of functionality into Classes
- Wrote our Vec2 class for 2D game math
- Appearance of ECS classes
- EntityManager handles all Entity data
- ECS Systems functionality in Game.h functions

For more information about assignment itself, please read the 
[ASSIGNMENT.md](ASSIGNMENT02.md) file.
...

## How to compile and run

```bash
mkdir mybuild && cd mybuild
cmake ..
make
cp -r ../assets ../.. # I should think of a better way with paths
```

## Dev Notes
I'm using CLion as my IDE, and I try to follow a guideline from 
[CLion:](https://www.jetbrains.com/help/clion/clangformat-as-alternative-formatter.html)
along with the style the lecturer provided in his examples.

If you like to take deeper into what and where, I recommend using this source
[lefticus](https://lefticus.gitbooks.io/cpp-best-practices/content/03-Style.html)

Nonetheless, I consult authoritative pillars such as the
([C++ Core Guidelines](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines.html))
and [Google C++ Style Guide](https://google.github.io/styleguide/cppguide.html)
to ensure that the code remains readable and consistent.

### Guide style
* Each row of text in your code should be at most 120 characters long;
* Use a .cpp suffix for code files and .h for interface files;
  [NL.27](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines.html#Rl-file-suffix)
* [Names and order of includes](https://google.github.io/styleguide/cppguide.html#Names_and_Order_of_Includes)
  Related header, C system headers, C++ standard library headers, other headers;
* In a class I prefer to see public methods, private methods, public vars, private vars. Exactly
  in this order. 

