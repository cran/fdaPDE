#ifndef __PREPROCESS_PHASE_H__
#define __PREPROCESS_PHASE_H__

#include "Density_Initialization.h"
#include "Density_Initialization_Factory.h"
#include "K_Fold_CV_L2_Error.h"

//! @brief An abstract base class to perform the preprocess phase.
template<UInt ORDER, UInt mydim, UInt ndim>
class Preprocess{
protected:
    // A member to access the data problem methods
    const DataProblem<ORDER, mydim, ndim>& dataProblem_;
    // A member to access the functional methods
    const FunctionalProblem<ORDER, mydim, ndim>& funcProblem_;
    // A member to do density initialization
    std::unique_ptr<DensityInitialization<ORDER, mydim, ndim>> densityInit_;

    // It saves the initial f-density for each lambda
    std::vector<const VectorXr*> fInit_;
    // It saves the g-density to start the final minimization descent
    VectorXr gInit_;
    // It saves the lambda selected
    Real bestLambda_;

    //! A method to fill fInit_.
    void fillFInit();

public:
    //! A constructor.
    Preprocess(const DataProblem<ORDER, mydim, ndim>& dp, const FunctionalProblem<ORDER, mydim, ndim>& fp);
    //! A destructor.
    virtual ~Preprocess(){};
    //! A pure virtual method to perform the preprocess task.
    virtual void performPreprocessTask() = 0;

    //! Getter
    inline std::tuple<std::vector<const VectorXr*>, VectorXr, Real> getPreprocessParameter() const
      { return std::tuple<std::vector<const VectorXr*>, VectorXr, Real> (fInit_, gInit_, bestLambda_); }

    virtual std::vector<Real> getCvError() const = 0;

};


//! @brief A class to handle the preprocess phase when there is only one smoother parameter.
template<UInt ORDER, UInt mydim, UInt ndim>
class NoCrossValidation : public Preprocess<ORDER, mydim, ndim>{
  public:
    //! A constructor
    NoCrossValidation(const DataProblem<ORDER, mydim, ndim>& dp, const FunctionalProblem<ORDER, mydim, ndim>& fp):
      Preprocess<ORDER, mydim, ndim>(dp, fp){};

    //! Overridden method to perform the preprocess phase.
    void performPreprocessTask() override;

    std::vector<Real> getCvError() const override {return std::vector<Real>{};}
};


/*! @brief An abstract class to handle the preprocess phase when cross-validation needs to be performed to select one
 * smoothing parameter. It contains members useful to perform the cross-validation technique.
*/
template<UInt ORDER, UInt mydim, UInt ndim>
class CrossValidation : public Preprocess<ORDER, mydim, ndim>{
  protected:
    // A member to do the minimization phase
    std::shared_ptr<MinimizationAlgorithm<ORDER, mydim, ndim>> minAlgo_;
    // Callable object for computing corss-validation  error in L2 norm
    KfoldCV_L2_error<ORDER, mydim, ndim> error_;
    // It contains indices cyclically partitioned
    std::vector<UInt> K_folds_;
    // It contains, for each lambda, the sum of errors in k-fold CV
    std::vector<Real> CV_errors_;
    // It contains the best g-function obtained with cross validation for each lambda
    std::vector<VectorXr> g_sols_;

    //! A method to perform k-fold cross validation.
    std::pair<VectorXr, Real> performCV();
    //! A pure virtual method to perform the core task of k-fold cross validation.
    virtual void performCV_core (UInt fold, const SpMat& Psi_train, const SpMat& Psi_valid) = 0;

  public:
    //! A constructor.
    CrossValidation(const DataProblem<ORDER, mydim, ndim>& dp,
       const FunctionalProblem<ORDER, mydim, ndim>& fp,
       std::shared_ptr<MinimizationAlgorithm<ORDER, mydim, ndim>> ma);
    //! A destructor.
    virtual ~CrossValidation(){};
    //! Overridden method to perform the preprocess phase.
    void performPreprocessTask() override;

    std::vector<Real> getCvError() const override {return CV_errors_;}

};


//! @brief A class to perform the simplified version of cross-validation.
template<UInt ORDER, UInt mydim, UInt ndim>
class SimplifiedCrossValidation : public CrossValidation<ORDER, mydim, ndim>{
  private:
    //! Overridden method to perform simplified cross-validation.
    void performCV_core (UInt fold, const SpMat& Psi_train, const SpMat& Psi_valid) override;

  public:
    //! A delegating constructor.
    SimplifiedCrossValidation(const DataProblem<ORDER, mydim, ndim>& dp,
       const FunctionalProblem<ORDER, mydim, ndim>& fp,
       std::shared_ptr<MinimizationAlgorithm<ORDER, mydim, ndim>> ma):
       CrossValidation<ORDER, mydim, ndim>(dp, fp, ma){};

};


//! @brief A class to perform the right version of cross-validation.
template<UInt ORDER, UInt mydim, UInt ndim>
class RightCrossValidation : public CrossValidation<ORDER, mydim, ndim>{
  private:
    // It saves the best loss reached, among all the folds, for each lambda
    std::vector<Real> best_loss_;

    //! Overridden method to perform right cross-validation.
    void performCV_core (UInt fold, const SpMat& Psi_train, const SpMat& Psi_valid) override;

  public:
    //! A constructor
    RightCrossValidation(const DataProblem<ORDER, mydim, ndim>& dp,
       const FunctionalProblem<ORDER, mydim, ndim>& fp,
       std::shared_ptr<MinimizationAlgorithm<ORDER, mydim, ndim>> ma);

};


//! @brief An abstract base class to perform the preprocess phase (spatio-temporal setting).
template<UInt ORDER, UInt mydim, UInt ndim>
class Preprocess_time{
protected:
    // A member to access the data problem methods
    const DataProblem_time<ORDER, mydim, ndim>& dataProblem_;
    // A member to access the functional methods
    const FunctionalProblem_time<ORDER, mydim, ndim>& funcProblem_;
    // A member to do density initialization
    std::unique_ptr<DensityInitialization_time<ORDER, mydim, ndim>> densityInit_;

    // It saves the initial f-density for each pair <lambda_S, lambda_T>
    std::vector<const VectorXr*> fInit_;
    // It saves the g-density to start the final minimization descent
    VectorXr gInit_;
    // It saves the lambdas selected
    Real bestLambda_S;
    Real bestLambda_T;

    //! A method to fill fInit_.
    void fillFInit();

public:
    //! A constructor.
    Preprocess_time(const DataProblem_time<ORDER, mydim, ndim>& dp,
                    const FunctionalProblem_time<ORDER, mydim, ndim>& fp);
    //! A destructor.
    virtual ~Preprocess_time(){};
    //! A pure virtual method to perform the preprocess task.
    virtual void performPreprocessTask() = 0;

    //! Getter
    inline std::tuple<std::vector<const VectorXr*>, VectorXr, Real, Real> getPreprocessParameter() const
    { return std::tuple<std::vector<const VectorXr*>, VectorXr, Real, Real> (fInit_, gInit_, bestLambda_S, bestLambda_T); }

    virtual std::vector<Real> getCvError() const = 0;

};


//! @brief A class to handle the preprocess phase when there is only one possible pair of smoothing parameters in space
//! and in time (spatio-temporal setting).
template<UInt ORDER, UInt mydim, UInt ndim>
class NoCrossValidation_time : public Preprocess_time<ORDER, mydim, ndim>{
public:
    //! A constructor
    NoCrossValidation_time(const DataProblem_time<ORDER, mydim, ndim>& dp,
                           const FunctionalProblem_time<ORDER, mydim, ndim>& fp):
            Preprocess_time<ORDER, mydim, ndim>(dp, fp){};

    //! Overridden method to perform the preprocess phase.
    void performPreprocessTask() override;

    std::vector<Real> getCvError() const override {return std::vector<Real>{};}
};

/*! @brief An abstract class to handle the preprocess phase when cross-validation needs to be performed to select one pair
 * of smoothing parameters in space and in time. It contains members useful to perform the cross-validation technique (spatio-temporal setting).
*/

template<UInt ORDER, UInt mydim, UInt ndim>
class CrossValidation_time : public Preprocess_time<ORDER, mydim, ndim>{
protected:
    // A member to do the minimization phase
    std::shared_ptr<MinimizationAlgorithm_time<ORDER, mydim, ndim>> minAlgo_;
    // Callable object for computing cross-validation error in L2 norm
    KfoldCV_L2_error_time<ORDER, mydim, ndim> error_;
    // It contains indices cyclically partitioned
    std::vector<UInt> K_folds_;
    // It contains, for each pair <lambda_S, lambda_T>, the sum of errors in k-fold CV
    std::vector<Real> CV_errors_;
    // It contains the best g-function obtained with cross validation for each pair <lambda_S, lambda_T>
    std::vector<VectorXr> g_sols_;

    //! A method to perform k-fold cross validation.
    std::tuple<VectorXr, Real, Real> performCV();
    //! A pure virtual method to perform the core task of k-fold cross validation.
    virtual void performCV_core (UInt fold, const SpMat& Upsilon_train, const SpMat& Upsilon_valid) = 0;

public:
    //! A constructor.
    CrossValidation_time(const DataProblem_time<ORDER, mydim, ndim>& dp,
                         const FunctionalProblem_time<ORDER, mydim, ndim>& fp,
                         std::shared_ptr<MinimizationAlgorithm_time<ORDER, mydim, ndim>> ma);
    //! A destructor.
    virtual ~CrossValidation_time(){};
    //! Overridden method to perform the preprocess phase.
    void performPreprocessTask() override;

    std::vector<Real> getCvError() const override {return CV_errors_;}

};


//! @brief A class to perform the simplified version of cross-validation (spatio-temporal setting).
template<UInt ORDER, UInt mydim, UInt ndim>
class SimplifiedCrossValidation_time : public CrossValidation_time<ORDER, mydim, ndim>{
private:
    //! Overridden method to perform simplified cross-validation.
    void performCV_core (UInt fold, const SpMat& Upsilon_train, const SpMat& Upsilon_valid) override;

public:
    //! A delegating constructor.
    SimplifiedCrossValidation_time(const DataProblem_time<ORDER, mydim, ndim>& dp,
                                   const FunctionalProblem_time<ORDER, mydim, ndim>& fp,
                                   std::shared_ptr<MinimizationAlgorithm_time<ORDER, mydim, ndim>> ma):
            CrossValidation_time<ORDER, mydim, ndim>(dp, fp, ma){};
};


//! @brief A class to perform the right version of cross-validation (spatio-temporal setting).
template<UInt ORDER, UInt mydim, UInt ndim>
class RightCrossValidation_time : public CrossValidation_time<ORDER, mydim, ndim>{
private:
    // It saves the best loss reached, among all the folds, for each lambda
    std::vector<Real> best_loss_;

    //! Overridden method to perform right cross-validation.
    void performCV_core (UInt fold, const SpMat& Upsilon_train, const SpMat& Upsilon_valid) override;

public:
    //! A constructor
    RightCrossValidation_time(const DataProblem_time<ORDER, mydim, ndim>& dp,
                              const FunctionalProblem_time<ORDER, mydim, ndim>& fp,
                              std::shared_ptr<MinimizationAlgorithm_time<ORDER, mydim, ndim>> ma);

};


#include "Preprocess_Phase_imp.h"


#endif
