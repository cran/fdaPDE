checkInferenceParametersTime <- function(inference.data.object,checknumber,time_locations, locations=NULL,nodes=NULL, FLAG_PARABOLIC, FLAG_IC){
  
  #if no inference is required, construct a dummy inference object, where all parameters are set to nonsense values on purpose
  if(is.null(inference.data.object) || inference.data.object@definition==0){
    return(
      new("inferenceDataObjectTime", test = as.integer(0), interval = as.integer(0), type = as.integer(0), component = as.integer(0), exact = as.integer(0), dim = as.integer(0), n_cov = as.integer(0),
          locations = matrix(data=0, nrow = 1 ,ncol = 1), locations_indices = as.integer(0), locations_are_nodes = as.integer(0), time_locations = 0, coeff = matrix(data=0, nrow = 1 ,ncol = 1), beta0 = -1, f0 = function(){}, 
          f0_eval = -1, f_var = as.integer(0), quantile = -1, alpha = 0, n_flip = as.integer(1000), tol_fspai = -1, definition=as.integer(0)))
  }#define a shadow inferenceDataObjectTime in order tell the cpp code not to perform any inferential analysis.
  
  if(!is.null(inference.data.object) && is.null(checknumber) && sum(inference.data.object@component!=2)!=0){
    warning("Covariates are not defined, inference data are discarded")
    return(
      new("inferenceDataObjectTime", test = as.integer(0), interval = as.integer(0), type = as.integer(0), component = as.integer(0), exact = as.integer(0), dim = as.integer(0), n_cov = as.integer(0),
          locations = matrix(data=0, nrow = 1 ,ncol = 1), locations_indices = as.integer(0), locations_are_nodes = as.integer(0), time_locations = 0, coeff = matrix(data=0, nrow = 1 ,ncol = 1), beta0 = -1, f0 = function(){}, 
          f0_eval = -1, f_var = as.integer(0), quantile = -1, alpha = 0, n_flip = as.integer(1000), tol_fspai = -1, definition=as.integer(0))) 
  }
  
  #check consistency with covariates dimension (if inference on the linear component is required)
  if(!is.null(checknumber) && checknumber!=inference.data.object@n_cov || is.null(checknumber) && inference.data.object@n_cov!=0){
    stop("Inference data dimension and covariates dimension are not consistent")
  }
  
  #check consistency with locations and evaluate f0 (if inference on the nonparametric component is required)
  if(sum(inference.data.object@component!=1)!=0){
    
    #if a vector of indices has been provided
    if(dim(inference.data.object@locations)[1]==0){
      
      #check that there aren't indices repetitions and that each index doesn't exceed n_obs, eventually drop them 
      for(j in 1:length(inference.data.object@locations_indices)){
        if(inference.data.object@locations_indices[j] %in% inference.data.object@locations_indices[-j])
          inference.data.object@locations_indices = inference.data.object@locations_indices[-j]
        else if(inference.data.object@locations_indices[j] > dim(locations)[1]){
          warning("Removing a location index exceeding the number of observed locations")
          inference.data.object@locations_indices = inference.data.object@locations_indices[-j]
        }
      }
      
      #check that the final length of locations indices doesn't exceed the observed ones
      if(length(inference.data.object@locations_indices) > dim(locations)[1]){
        warning("Length of 'location_indices' exceeds the number of observed locations, proceeding with the observed locations")
        inference.data.object@location_indices = as.integer(seq(1,dim(locations)[1]))
        inference.data.object@locations = locations
      }
      else{
        inference.data.object@locations = matrix(data = locations[inference.data.object@locations_indices,], ncol = inference.data.object@dim)
      }
    }
    else{
      # a matrix of locations has been provided  
      if(ncol(inference.data.object@locations)==1){
        inference.data.object@locations = locations #default case
        inference.data.object@locations_indices = as.integer(seq(1,dim(locations)[1]))
      }
      else{
        # only wald implementation can enter here --> in principle new location points are allowed here, so locations_indices is undefined in this case
        # check that the dimension of the problem is consistent
        if(ncol(inference.data.object@locations) != ncol(locations))
          stop("Inference data dimension and locations dimension are not consistent")
        # locations_indices is set to a nonsense value
        inference.data.object@locations_indices = as.integer(0)
      }
    }
    
    locs_are_nodes_before_check = (inference.data.object@locations_are_nodes == 1)
    inference.data.object@locations_are_nodes = as.integer(1)
    
    # check if the selected locations coincide with nodes
    end_loop = FALSE
    j=1
    while(!end_loop && j <= dim(inference.data.object@locations)[1]){
      node_found = FALSE
      l = 1
      while(!node_found && l <= dim(nodes)[1]){
        if(inference.data.object@locations[j,1]==nodes[l,1] && inference.data.object@locations[j,2]==nodes[l,2]){
          node_found = TRUE
        }
        l = l+1 
      }
      if(!node_found){
        end_loop = TRUE
        inference.data.object@locations_are_nodes = as.integer(2)
      }
      j = j+1
    }
    
    # if(!locs_are_nodes_before_check && inference.data.object@locations_are_nodes!=2 && sum(inference.data.object@type==1)!=length(inference.data.object@type))
    #   warning("All the locations selected for inference on the nonparametric component coincide with the nodes. 
    #           Sign-Flip and Eigen-Sign-Flip tests are performed by area, combining neighborhoods of locations according to the distance induced by the mesh")
    
    # if(inference.data.object@locations_are_nodes!=1){
    #   # intervals with sign-flip or eigen-sign-flip implementation are not allowed
    #   for(k in 1:length(inference.data.object@type)){
    #     if((inference.data.object@type[k]==3 || inference.data.object@type[k]==5) && inference.data.object@interval[k]!=0 && inference.data.object@component[k]!=1)
    #       stop("Sign-Flip and Eigen-Sign-Flip confidence intervals on f are implemented only if the selected locations are a subset of the mesh nodes")
    #   }
    # }
    
    # Check time_locations definition
    if(FLAG_PARABOLIC){
      if(length(inference.data.object@time_locations)!=0)
        stop("Inference in ST parabolic case can be computed only on the overall time locations provided to smooth.FEM.time() and does not allow time locations specification")
      else
        inference.data.object@time_locations = as.vector(time_locations[ifelse(FLAG_IC,2,1):length(time_locations)]) # If IC is null, drop the first time point, else keep it
    }else{
      if(length(inference.data.object@time_locations)==0) # That is inference on f is required but no time locations are provided: by default we resort to the observed ones
        inference.data.object@time_locations = as.vector(time_locations)
      else
        if(min(inference.data.object@time_locations)<min(time_locations) || max(inference.data.object@time_locations)>max(time_locations))
          warning("The time locations provided for inference are not inside the time frame provided for observations: inference may not be reliable in this case")
    }
    # finally evaluate f0 at the chosen locations
    f0 <- inference.data.object@f0
    dim <- inference.data.object@dim
    locs <- inference.data.object@locations
    
    f0_matr <- matrix(data=NA, nrow=dim(locs), ncol=length(inference.data.object@time_locations))
    
    if(dim == 2){
      for(i in 1:dim(locs)[1])
        for(j in 1:length(inference.data.object@time_locations))
          f0_matr[i,j] <- f0(locs[i,1], locs[i,2], 0, inference.data.object@time_locations[j]) 
    }
    else{
      for(i in 1:dim(locs)[1])
        for(j in 1:length(inference.data.object@time_locations))
        f0_matr[i,] <- f0(locs[i,1], locs[i,2], locs[i,3], inference.data.object@time_locations[j]) 
    }
    inference.data.object@f0_eval <- as.vector(f0_matr)
  }
  else{
    # in case only inference on parametric component is requested, set f0_eval and locations with default values, just for transmission safety
    inference.data.object@locations <- matrix(data = 0)
    inference.data.object@locations_indices <- as.integer(0)
    inference.data.object@f0_eval <- 0
    
  }
  
  return(inference.data.object)    #return the original object
}










