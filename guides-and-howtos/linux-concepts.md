# Linux concepts and quick tips
This file contains all my favorite concepts and shirtcuts for better quality of life.

## 1. Shortcuts and quick tips

### (1.1) How to efficiently search for a pattern everywhere in a directory.

I like to search for matches using `grep` and then open the file in `vim` to inspect all the matches.

- Navigate to the directory using `cd`.
- use `grep` to list all the files with matches: 
  
```
  grep -rlw "pattern" <path-to-dir>
```

In the above commands the flags mean the following

`-r` (recursive search)

`-l` (only list the file name with matches)

`-w` (exact matches only)

- then open the corresponding file in `vim` and find for matches using `/` in command mode followed by the pattern.
