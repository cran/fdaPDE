#ifndef __INFERENCE_CARRIER_H__
#define __INFERENCE_CARRIER_H__

// HEADERS
#include "../../FdaPDE.h"
#include "../../Regression/Include/Mixed_FE_Regression.h"
#include "../../Regression/Include/Regression_Data.h"
#include "../../Lambda_Optimization/Include/Optimization_Data.h"
#include "../../Lambda_Optimization/Include/Solution_Builders.h"
#include "Inference_Data.h"

// *** Inference_Carrier Class ***
//! Carrier for Inference Methods
/*!
 This template class contains all the information needed by inference methods. Most of the needed objects are wrapped with pointers
 with the exception of the local parameters that are computed after the carrier initialization only if inference on f is needed, the optimal smoothing parameter lambda and 
 the problem dimensions: the number of observations, the number of nodes and the number of covariates.  
 \tparam InputHandler the type of regression problem needed to determine the MixedFERegressionBase object type
*/
template<typename InputHandler>
class Inference_Carrier{
	private:
		// DATA:
		const InputHandler * reg_data = nullptr;                    	        //!< Pointer to the regression data 
		const MixedFERegressionBase<InputHandler> * model = nullptr;    	//!< Pointer to the model data
		const InferenceData * inf_data = nullptr;				//!< Pointer to the inference data needed to perform inference

		// SYSTEM PARAMETERS
		UInt n_obs; 							        //!< Number of observations
		UInt n_nodes;							        //!< Number of nodes in space-only models, number of nodes times time points in the space-time models
		UInt q;								        //!< Number of covariates
		Real lambda_S = 0; 							//!< Optimal spatial smoothing parameter 
		Real lambda_T = 0; 							//!< Optimal temporal smoothing parameter 

  		const MatrixXr * Wp = nullptr;						//!< Pointer to the covariates matrix [size n_obs x n_covariates]
		const SpMat * Psip = nullptr; 						//!< Pointer to location-to-nodes matrix [size n_obs x n_nodes]	
		const SpMat * Psi_tp = nullptr; 					//!< Pointer to the transpose of the location-to-nodes matrix [size n_nodes x n_obs]	
		const Eigen::PartialPivLU<MatrixXr> * WtW_decp = nullptr;		//!< Pointer to the LU decomposition of the WtW matrix
		const SpMat * Ptkp = nullptr;						//!< Pointer to the kron(Pt,IN) (separable version)
		const SpMat * LR0kp = nullptr;						//!< Pointer to the kron(L,R0) (parabolic version)
		const SpMat * R0p = nullptr; 						//!< Pointer to the mass matrix
		const SpMat * R1p = nullptr; 						//!< Pointer to the stiffness matrix
		const MatrixXr * Hp = nullptr;						//!< Pointer to the hat matrix [size n_covariates x n_covariates]
		const MatrixXr * Up = nullptr; 						//!< Pointer to the U matrix of the Woodbury decomposition of the system
		const MatrixXr * Vp = nullptr; 						//!< Pointer to the V matrix of the Woodbury decomposition of the system
		const VectorXr * Ap = nullptr; 						//!< Pointer to the A vector containing the diagonal of the areal matrix (asDiagonal())
		const SpMat * Ep = nullptr; 						//!< Pointer to the no-cov-matrix of the Woodbury decomposition of the system
		const Eigen::SparseLU<SpMat> * E_decp = nullptr;			//!< Pointer to the sparse LU decomposition for the no-cov-matrix of the Woodbury decomposition of the system
		const Eigen::PartialPivLU<MatrixXr> * G_decp = nullptr;			//!< Pointer to the LU decomposition of the G matrix of the Woodbury decomposition of the system
		
		// OBSERVATIONS AND ESTIMATORS
		const VectorXr * beta_hatp = nullptr; 					//!< Pointer to the estimate of the betas for the optimal model
                const MatrixXr * solution_p = nullptr; 					//!< Pointer to the solution of the linear system corresponding to the optimal model 
		const VectorXr * zp = nullptr;						//!< Pointer to the observations in the locations [size n_obs]
		VectorXr z_hat; 							//!< Fitted values in the locations [size n_obs]

		// LOCAL PARAMETERS
		UInt n_loc;                                                             //!< Number of selected locations for inference on f 
                SpMat Psi_loc; 							        //!< Selected location-to-nodes matrix [size n_loc x n_nodes]
		MatrixXr W_loc; 	                                		//!< Reduced covariates matrix [size n_loc x n_covariates]
		VectorXr z_loc; 							//!< Reduced observations [size n_loc]
                MatrixXr Group_loc = MatrixXr::Zero(1,1);                         	//!< Matrix that groups closer locations (needed only if locations coincide with the nodes for eigen-sing-flip and sign-flip implementations) 

		// PRIVATE SETTERS 							// Private because they will be used just by the constructor.
		inline void setRegData (const InputHandler * reg_data_){reg_data = reg_data_;}			        //!< Setter of reg_data \param reg_data_ new reg_data
		inline void setModel (const MixedFERegressionBase<InputHandler> * model_){model = model_;}		//!< Setter of model \param model_ new model
		inline void setInfData (const InferenceData * inf_data_){inf_data = inf_data_;}				//!< Setter of inf_data \param inf_data_ new inf_data

		inline void setN_obs (UInt n_obs_){n_obs = n_obs_;}							//!< Setter of n_obs \param n_obs_ new n_obs
		inline void setN_nodes (UInt n_nodes_){n_nodes = n_nodes_;}						//!< Setter of n_nodes \param n_nodes_ new n_nodes
		inline void setq (UInt q_){q = q_;}									//!< Setter of q \param q_ new q
		inline void setlambda_S (Real lambda_S_){lambda_S=lambda_S_;}						//!< Setter of lambda_S \param lambda_S_ new lambda_S
		inline void setlambda_T (Real lambda_T_){lambda_T=lambda_T_;}						//!< Setter of lambda_T \param lambda_T_ new lambda_T

		inline void setWp (const MatrixXr * Wp_){Wp = Wp_;}							//!< Setter of Wp \param Wp_ new Wp
		inline void setPsip (const SpMat * Psip_){Psip = Psip_;}						//!< Setter of Psip \param Psip_ new Psip
		inline void setPsi_tp (const SpMat * Psi_tp_){Psi_tp = Psi_tp_;}					//!< Setter of Psi_tp \param Psi_tp_ new Psi_tp
		inline void setWtW_decp (const Eigen::PartialPivLU<MatrixXr> * WtW_decp_){WtW_decp = WtW_decp_;}	//!< Setter of WtW_decp \param  WtW_decp_ new  WtW_decp
		inline void setPtkp (const SpMat * Ptkp_){Ptkp = Ptkp_;}						//!< Setter of Ptkp \param Ptkp_ new Ptkp
		inline void setLR0kp (const SpMat * LR0kp_){LR0kp = LR0kp_;}						//!< Setter of LR0kp \param LR0kp_ new LR0kp
		inline void setR0p (const SpMat * R0p_){R0p = R0p_;}							//!< Setter of R0p \param R0p_ new R0p
		inline void setR1p (const SpMat * R1p_){R1p = R1p_;}							//!< Setter of R1p \param R1p_ new R1p
		inline void setHp (const MatrixXr * Hp_){Hp = Hp_;}							//!< Setter of Hp \param Hp_ new Hp
		inline void setUp (const MatrixXr * Up_){Up = Up_;}							//!< Setter of Up \param Up_ new Up
		inline void setVp (const MatrixXr * Vp_){Vp = Vp_;}							//!< Setter of Vp \param Vp_ new Vp
		inline void setAp (const VectorXr * Ap_){Ap = Ap_;}							//!< Setter of Ap \param Ap_ new Ap
		inline void setEp (const SpMat * Ep_){Ep = Ep_;}							//!< Setter of Ep \param Ep_ new Ep
		inline void setE_decp (const Eigen::SparseLU<SpMat> * E_decp_){E_decp = E_decp_;}			//!< Setter of E_decp \param E_decp_ new E_decp
		inline void setG_decp (const Eigen::PartialPivLU<MatrixXr> * G_decp_){G_decp = G_decp_;}		//!< Setter of G_decp \param G_decp_ new G_decp
		inline void setBeta_hatp (const VectorXr * beta_hatp_){beta_hatp = beta_hatp_;}				//!< Setter of beta_hatp \param beta_hatp_ new beta_hatp
		inline void setZp (const VectorXr * zp_){zp = zp_;}							//!< Setter of zp \param zp_ new zp
		inline void setZ_hat (const VectorXr z_hat_){z_hat = z_hat_;}						//!< Setter of z_hat \param z_hat_ new z_hat

        public:  // PUBLIC SETTERS                                                        				// Public because they will be used after the carrier instantiation
		inline void setSolutionp (const MatrixXr * solution_p_){solution_p = solution_p_;}			//!< Setter of solution_p \param solution_p_ new solution_p
		inline void setN_loc (UInt n_loc_){n_loc = n_loc_;}							//!< Setter of n_loc \param n_loc_ new n_loc
		inline void setPsi_loc (const SpMat & Psi_loc_){Psi_loc = Psi_loc_;}					//!< Setter of Psi_loc \param Psi_loc_ new Psi_loc
		inline void setW_loc (const MatrixXr & W_loc_){W_loc = W_loc_;}						//!< Setter of W_loc \param W_loc_ new W_loc
                inline void setZ_loc (const VectorXr & z_loc_){z_loc = z_loc_;}						//!< Setter of z_loc \param z_loc_ new z_loc
                inline void setGroup_loc(const MatrixXr & Group_loc_){Group_loc = Group_loc_;}                          //!< Setter of Group_loc \param Group_loc_ new Group_loc
	
		// CONSTUCTORS
		Inference_Carrier()=default;			//The default constructor is just used to initialize the object. All the pointer are set to nullptr, Real values are set 0
		Inference_Carrier(const InputHandler * Regression_Data_, const MixedFERegressionBase<InputHandler> * model_, const output_Data<1> * out_regression_, const InferenceData * inf_data_, Real lambda_S_); 			//Main constructor of the class in spatial case
		Inference_Carrier(const InputHandler * Regression_Data_, const MixedFERegressionBase<InputHandler> * model_, const output_Data<2> * out_regression_, const InferenceData * inf_data_, Real lambda_S_, Real lambda_T_); 	//Main constructor of the class in temporal case

		// GETTERS
		inline const InputHandler * getRegData (void) const {return reg_data;}  			        //!< Getter of reg_data \return reg_data
		inline const MixedFERegressionBase<InputHandler> * getModel (void) const {return model;} 		//!< Getter of model \return model
		inline const InferenceData * getInfData (void) const {return inf_data;}					//!< Getter of inf_data \return inf_data

		inline UInt getN_obs (void) const {return n_obs;} 							//!< Getter of n_obs \return n_obs
		inline UInt getN_nodes (void) const {return n_nodes;} 					                //!< Getter of n_nodes \return n_nodes
		inline UInt getq (void) const {return q;} 								//!< Getter of q \return q
		inline Real getlambda_S (void) const {return lambda_S;} 						//!< Getter of lambda_S \return lambda_S 
		inline Real getlambda_T (void) const {return lambda_T;} 						//!< Getter of lambda_T \return lambda_T 

		inline const MatrixXr * getWp (void) const {return Wp;} 						//!< Getter of Wp \return Wp
		inline const SpMat * getPsip (void) const {return Psip;} 						//!< Getter of Psip \return Psip
		inline const SpMat * getPsi_tp (void) const {return Psi_tp;} 						//!< Getter of Psi_tp \return Psi_tp
		inline const Eigen::PartialPivLU<MatrixXr> * getWtW_decp (void) const {return WtW_decp;} 		//!< Getter of WtW_decp \return WtW_decp
		inline const SpMat * getPtkp (void) const {return Ptkp;} 						//!< Getter of Ptkp \return Ptkp
		inline const SpMat * getLR0kp (void) const {return LR0kp;} 						//!< Getter of LR0kp \return LR0kp
		inline const SpMat * getR0p (void) const {return R0p;} 							//!< Getter of R0p \return R0p
		inline const SpMat * getR1p (void) const {return R1p;} 							//!< Getter of R1p \return R1p
		inline const MatrixXr * getHp (void) const {return Hp;} 						//!< Getter of Hp \return Hp
		inline const MatrixXr * getUp (void) const {return Up;} 						//!< Getter of Up \return Up
		inline const MatrixXr * getVp (void) const {return Vp;} 						//!< Getter of Vp \return Vp
		inline const VectorXr * getAp (void) const {return Ap;} 					        //!< Getter of Ap \return Ap
		inline const SpMat * getEp (void) const {return Ep;} 						        //!< Getter of Ep \return Ep
		inline const Eigen::SparseLU<SpMat> * getE_decp (void) const {return E_decp;} 				//!< Getter of E_decp \return E_decp
		inline const Eigen::PartialPivLU<MatrixXr> * getG_decp (void) const {return G_decp;} 			//!< Getter of G_decp \return G_decp
		inline const VectorXr * getBeta_hatp (void) const {return beta_hatp;} 				        //!< Getter of beta_hatp \return beta_hatp
		inline const MatrixXr * getSolutionp (void) const {return solution_p;}					//!< Getter of solution_p \return solution_p
		inline const VectorXr * getZp (void) const {return zp;} 						//!< Getter of zp \return zp
		inline const VectorXr getZ_hat (void) const {return z_hat;} 						//!< Getter of z_hat \return z_hat

		inline UInt getN_loc (void) const {return n_loc;} 							//!< Getter of n_loc \return n_loc
		inline const SpMat getPsi_loc (void) const {return Psi_loc;}						//!< Getter of Psi_loc \return Psi_loc
		inline const MatrixXr getW_loc (void) const {return W_loc;} 						//!< Getter of W_loc \return W_loc
		inline const VectorXr getZ_loc (void) const {return z_loc;} 						//!< Getter of z_loc \return z_loc
                inline const MatrixXr getGroup_loc(void) const {return Group_loc;}                                      //!< Getter of Group_loc \return Group_loc

};

#include "Inference_Carrier_imp.h"

#endif
