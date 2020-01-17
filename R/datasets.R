#' Horseshoe domain
#'
#' The boundary and interior nodes and connectivity matrix of a triangular mesh of the horseshoe domain. 
#' This dataset can be used to create a \code{MESH.2D} object with the function \code{create.MESH.2D}. 
#' The variables are:
#' \itemize{
#'    \item boundary_nodes. The nodes in the boundary.
#'    \item boundary_segments. The ssegments of the boundary.
#'    \item locations. The interior nodes of the mesh. 
#' }
#'
#' @name horseshoe2D
#' @docType data
#' @usage data(horseshoe2D)
NULL


#' Quasicircle2D domain
#'
#' The boundary and interior nodes and connectivity matrix of a triangular mesh of a quasicircular domain, together 
#' with a non-stationary field observed over the nodes of the mesh.  
#' This dataset can be used to create a \code{MESH.2D} object with the function \code{create.MESH.2D} and to test
#' the smooth.FEM function.
#' The variables are:
#' \itemize{
#'    \item boundary_nodes. The nodes in the boundary.
#'    \item boundary_segments. The ssegments of the boundary.
#'    \item locations. The interior nodes of the mesh. 
#'    \item data. The vector of observations. 
#' }
#'
#' @name quasicircle2D
#' @docType data
#' @usage data(quasicircle2D)
NULL


#' Quasicircle2Dareal domain
#'
#' The mesh of a quasicircular domain, together with a non-stationary field observed over seven circular subdomains and 
#' the incindence matrix defining the subdomains used by Azzimonti et. al 2015.   
#' This dataset can be used to test the smooth.FEM function for areal data. 
#' The variables are:
#' \itemize{
#'    \item incidence_matrix. The 7-by-630 incidence matrix.
#'    \item data. The vector of observations.
#'    \item mesh. The mesh for areal data. 
#' }
#'
#' @references Azzimonti, L., Sangalli, L. M., Secchi, P., Domanin, M., & Nobile, F. (2015). Blood flow velocity 
#' field estimation via spatial regression with PDE penalization. Journal of the American Statistical 
#' Association, 110(511), 1057-1071.
#' @name quasicircle2Dareal
#' @docType data
#' @usage data(quasicircle2Dareal)
NULL


#' Hub domain
#'
#' The nodes and connectivity matrix of a triangular mesh of a manifold representing a hub geometry. 
#' This dataset can be used to create a \code{MESH.2.5D} object with the function \code{create.MESH.2.5D}.
#' The variables are:
#' \itemize{
#'    \item hub2.5D.nodes. The nodes of the mesh.
#'    \item hub2.5D.triangles. The triangles of the mesh.
#' }
#' @name hub2.5D
#' @docType data
#' @usage data(hub2.5D)
NULL

