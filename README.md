# NOTES

## *what's the structure of JSON?* 
- Explained at JSON website, except...
- For the sake of the task (evaluating expression), the root structure is always an
object and not an array.

## *how to run?*
- ``gcc json_eval.cpp -lstdc++`` and ``a.exe {filename} {expression in quotes}``
- or ``g++ json_eval.cpp`` and ``json_eval.exe {filename} {expression in quotes}``
- Please make sure your expression is without whitespaces and quotes! For example, instead of ``"max(a.b[0], a.b[1])"`` please enter ``max(a.b[0],a.b[1])``
- launch.json file includes the settings used in VS Code. If the code is to be tested with debugger for several outputs, ``args`` array is changed.

## *runtime*
- Parsing stage and evaluation stage are different
- A character in .json is only read once, but might be compared several times.
- Before outputting, the program sleeps for several seconds (in order to see the time results)
- There are no automated tests, but the program was tested mainly on files test.json (to check correctness easily) and ChinookData.json (performance benchmark, almost 2 million characters parsed in under 200ms) 

## *error throwing*
- Upon unexpected behaviour, throws an exception with a message.
- Main outputs the error message.
