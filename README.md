## fdaPDE Library

fdaPDE implements a class of spatial regression models in between statistics and numerical analysis. These models are particularly well-suited for situations where a prior knowledge of the underlying phenomenon is known and can be described in terms of a Partial Differential Equation (PDE).

## Installation

Download the .zip of the code, unzip it, then from the R console type

  install.packages('/path/to/fdaPDE', type='source', repos=NULL)

To install the package, please make sure that you have the package devtools already intalled. If using a Linux machine, it is also advisable to install rgl, plot3D and plot3Drgl before fdaPDE.

## Subfolder structure
src contains all C++ code and a special file named Makevars necessary to build and install the R package
R contains the R functions that wrap the C++ calls
data contains all .rda and .RData files useful for testings

## Examples

Some examples can be found visualizing the help of the main functions i.e.
`?smooth.FEM` or `?FPCA.FEM`

