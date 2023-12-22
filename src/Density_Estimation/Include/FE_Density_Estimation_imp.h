#ifndef __FE_DENSITY_ESTIMATION_IMP_H__
#define __FE_DENSITY_ESTIMATION_IMP_H__


template<UInt ORDER, UInt mydim, UInt ndim>
FEDE<ORDER, mydim, ndim>::FEDE(const DataProblem<ORDER, mydim, ndim>& dp, const FunctionalProblem<ORDER, mydim, ndim>& fp,
                               std::shared_ptr<MinimizationAlgorithm<ORDER, mydim, ndim>> ma, const std::string& p):
      dataProblem_(dp), funcProblem_(fp), minAlgo_(ma){

        preprocess_ = Preprocess_factory<ORDER, mydim, ndim>::createPreprocessSolver(dp, fp, ma, p);

}

template<UInt ORDER, UInt mydim, UInt ndim>
void FEDE<ORDER, mydim, ndim>::apply()
{
    // perform the preprocess phase
    Rprintf("##### PREPROCESS PHASE #####\n");
    preprocess_ -> performPreprocessTask();

    // collect preprocess results
    VectorXr gInit;
    std::tie(fInit_, gInit, bestLambda_) = preprocess_ -> getPreprocessParameter();

    CV_errors_ = preprocess_ -> getCvError();

    // final minimization descent
    Rprintf("##### FINAL STEP #####\n");

    gcoeff_ = minAlgo_->apply_core(dataProblem_.getGlobalPsi(), bestLambda_, gInit);

    if(dataProblem_.Inference()){
        // CI computation
        Rprintf("##### CI COMPUTATION #####\n");

        g_CI_ = funcProblem_.computeCovariance_CI(gcoeff_, bestLambda_);
    }
}

// -----------------------------------------
// --------------- FEDE_time ---------------
// -----------------------------------------

template<UInt ORDER, UInt mydim, UInt ndim>
FEDE_time<ORDER, mydim, ndim>::FEDE_time(const DataProblem_time<ORDER, mydim, ndim>& dp, const FunctionalProblem_time<ORDER, mydim, ndim>& fp,
                                         std::shared_ptr<MinimizationAlgorithm_time<ORDER, mydim, ndim>> ma, const std::string& p):
        dataProblem_(dp), funcProblem_(fp), minAlgo_(ma){

    preprocess_ = Preprocess_factory_time<ORDER, mydim, ndim>::createPreprocessSolver(dp, fp, ma, p);

}

template<UInt ORDER, UInt mydim, UInt ndim>
void FEDE_time<ORDER, mydim, ndim>::apply()
{
    // perform the preprocess phase
    Rprintf("##### PREPROCESS PHASE #####\n");
    preprocess_ -> performPreprocessTask();

    // collect preprocess results
    Rprintf("##### COLLECT PREPROCESS RESULTS #####\n");
    VectorXr gInit;
    std::tie(fInit_, gInit, bestLambda_S, bestLambda_T) = preprocess_ -> getPreprocessParameter();

    Rprintf("best lambda_S: %f, best lambda_T: %f.\n", bestLambda_S, bestLambda_T);

    Rprintf("##### CV PREPROCESS PHASE #####\n");
    CV_errors_ = preprocess_ -> getCvError();

    // final minimization descent
    Rprintf("##### FINAL STEP #####\n");

    gcoeff_ = minAlgo_->apply_core(dataProblem_.getUpsilon(), bestLambda_S, bestLambda_T, gInit);

    if(dataProblem_.Inference()){
        // CI computation
        Rprintf("##### CI COMPUTATION #####\n");

        g_CI_ = funcProblem_.computeCovariance_CI(gcoeff_, bestLambda_S, bestLambda_T);
    }
}

#endif
