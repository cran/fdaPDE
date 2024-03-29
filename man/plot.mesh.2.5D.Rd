% Generated by roxygen2: do not edit by hand
% Please edit documentation in R/plot.R
\name{plot.mesh.2.5D}
\alias{plot.mesh.2.5D}
\title{Plot a mesh.2.5D object}
\usage{
\method{plot}{mesh.2.5D}(x, ...)
}
\arguments{
\item{x}{A \code{mesh.2.5D} object generated by \code{create.mesh.2.5D}.}

\item{...}{Arguments representing graphical options to be passed to \link[graphics]{par}.}
}
\value{
No return value
}
\description{
Plot the triangulation of a \code{mesh.2.5D} object, generated by \code{create.mesh.2.5D}
}
\examples{
library(fdaPDE)

## Upload the hub2.5D the data
data(hub2.5D)
hub2.5D.nodes = hub2.5D$hub2.5D.nodes
hub2.5D.triangles = hub2.5D$hub2.5D.triangles

## Create mesh
mesh = create.mesh.2.5D(nodes = hub2.5D.nodes, triangles = hub2.5D.triangles)
plot(mesh)
}
