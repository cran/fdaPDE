#ifndef __INFERENCE_BASE_H__
#define __INFERENCE_BASE_H__

// HEADERS
#include "../../FdaPDE.h"
#include "../../Regression/Include/Mixed_FE_Regression.h"
#include "../../Regression/Include/Regression_Data.h"
#include "../../Lambda_Optimization/Include/Optimization_Data.h"
#include "../../Lambda_Optimization/Include/Solution_Builders.h"
#include "Inference_Data.h"
#include "Inference_Carrier.h"
#include "Inverter.h"
#include <memory>

// *** Inference_Base Class ***
//! Hypothesis testing and confidence intervals abstract base class
/*!
  This template class provides the basic tools to perform hypothesis testing and/or compute confidence intervals. It contains a shared pointer to an inverter, that manages the invertion of matrixNoCov in an exact or non-exact way; It contains a reference to an Inference_Carrier object that wraps all the information needed to make inference; It contains an integer pos_impl that indicates the index position of the current inferential procedure to be carried out. It is needed to take the correct information from the vector parameters in the Inference_Carrier object. There are two pure virtual protected methods for the computation of p-values and confidence intervals for beta parameter and two for f parameter; there is a main public method that calls the proper functions according to the current test and interval types; then there is a public setter for the index position that is needed when multiple inferential procedures are required and two virtual methods that will be actually implemented only in the derived Wald class, the first one is used to compute exact GCV, the second one is used to estimate local f variance. 
\tparam InputHandler the type of regression problem needed to determine the MixedFERegressionBase object type in Inference_Carrier<InputHandler>
\tparam MatrixType the type of matrix (MatrixXr or SpMat) used to store diffferent objects related to the smoothers S, Lambda and Lambda2. SpMat type is related to approximated inference computation.
*/
template<typename InputHandler, typename MatrixType>
class Inference_Base{
protected:
  std::shared_ptr<Inverse_Base<MatrixType>> inverter = nullptr;     //!< Pointer to inverter object that computes the inverse of matrixNoCov in exact/non-exact way
  const Inference_Carrier<InputHandler> & inf_car;	//!< Inference carrier that contains all the information needed for inference 
  UInt pos_impl;					//!< Index that gives the position in all the vectors in infecenceData object
  MatrixXv compute_pvalue(void);			//!< Method used to compute the pvalues of the tests 
  MatrixXv compute_CI(void);		    		//!< Method to compute the confidence intervals
  virtual VectorXr compute_beta_pvalue(void) = 0;       //!< Pure virtual method to compute the pvalues for the tests on beta parameters
  virtual MatrixXv compute_beta_CI(void) = 0;		//!< Pure virtual method to compute the confidence intervals for the tests on beta parameters
  virtual Real compute_f_pvalue(void) = 0;		//!< Pure virtual method to compute the pvalues for the tests on f
  virtual MatrixXv compute_f_CI(void) = 0;		//!< Pure virtual method to compute the confidence intervals for f

public:
  // CONSTUCTOR
  Inference_Base()=delete;	//The default constructor is deleted
  Inference_Base(std::shared_ptr<Inverse_Base<MatrixType>> inverter_, const Inference_Carrier<InputHandler> & inf_car_, UInt pos_impl_):inverter(inverter_), inf_car(inf_car_), pos_impl(pos_impl_){}; 
  
  //!< Public method that calls the requested functions according to test_type and interval_type
  MatrixXv compute_inference_output (void);

  //!< Public setter for pos_impl, needed when multiple tests are required
  inline void setpos_impl (UInt pos_impl_){this->pos_impl=pos_impl_;};
  
  //!< Virtual public method that computes exact GCV, implemented only for Wald 
  inline virtual Real compute_GCV_from_inference(void) const {return 0;};

  //!< Virtual public method that computes local f variance, implemented only for Wald
  virtual VectorXr compute_f_var(void);  

  // DESTRUCTOR
  virtual ~Inference_Base(){};
};


#include "Inference_Base_imp.h"

#endif
