# Noisy Substring Problem

This is an implementation and test of the noisy substring algorithm described by Kashyap and Oommen in [this paper](https://ieeexplore.ieee.org/document/1703065).

This project was developed in collaboration with Jason Tran (UKY) as the final project for CS485-G, a class taught by [Dr. Jerzy Jaromczyk](https://www.engr.uky.edu/directory/jaromczyk-jerzy) in Spring 2020centered around algorithms and competitive programming.

## Getting Started

It is recommended to compile with ```-O3```. 

### Arguments

A dictionary file (a newline-separated list of words) must be provided as the first argument.

The following command-line arguments arguments are available:

```
  -l: For [str] command, disable display of space-separated list of matches (on by default)
  -t: For [str] command, display sorted newline-separated list of all words 
      paired with their minimum LD.
  -h: For [str] command, display the dictionary file with matches highlighted (in console)
  -wf: For each step of k_dist, print the Wagner-Fischer Matrix
  -cnt: Output total number of character comparisons for each command
  -o: Optimize by only computing WF matrices for suffixes of x of length >= |y|.
```

### Runtime

The program can be controlled at runtime by issuing commands. The following commands are available:

```
  -q: exit the program
  [str]: run noisy substring algorithm on str
  -t [n] [l_l] [l_u] [e_l] [e_u] [opts]:
      - do n times:
          - randomly choose a substring of length between l_l and l_u,
                  inclusive, from a random word in the dictionary
              - if l_u >= the str.size(), the substring is guaranteed to be
                  of length less than str.size() - 1.
          - make it noisy by doing betweem e_l and e_u, inclusive, random 
                  edits
              - LIMITATION: the string may be underedited by performing
                  certain edits that cancel each other out (i.e. delete
                  then insert same character)
          - run noisy substring algorithm on noisy version of substring
          - output frequency of noisy substring correctly estimating original
                  substring's set
  Options:
      -o: output each test case and whether it succeeded or failed
      -s [seed]: randomize using seed, rather than time(NULL)
      -g [param]: sets parameter for geometric distrubtion for number of edits
```

## Report

The file [Noisy Substring Problem.pdf](https://github.com/sabaunach/noisysubstring/blob/3d3dfa7eea194e252456ff4ab82b649109c9dc29/Noisy%20Substring%20Problem.pdf) provides a summary and report of our findings on this project.

A powerpoint presentation can be viewed [here.](https://docs.google.com/presentation/d/18AfP4ARZD0meC9TeL1UdEGGjnn46hwR7HXhMfWVRR1E/edit?usp=sharing)
