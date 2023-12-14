/*---------------------------------------------------------------------------
/*	Name:	$RCSfile: ica.cpp $
/*!	\file ica
	\brief independent component analysis class procedures
*/
/*
 *	$Log$
 */
static char _copyright[] = "(c) Copyright 1999-2000, Source Signal Imaging, Inc.";
static char _rcsId[] = "$Header: //Aristotle/emseproject/src/emse52/alglib/rcs/ica.cpp 1.1 2006/02/11 03:46:42 Aristotle release $";

// Include directives
//#include	"stdafx.h"
#include	<ica.h>

#include <C:/Users/Fedosov/Documents/projects/Brainstart/Brainstart/eigen-3.4.0/Eigen/Dense>

using namespace Eigen;;

//#include	<em_except.h>

//	Prototypes

// Global/external variable declarations

/*----------------------------------------------------------------------*/
/*! \class ecICA ica.h
  \par Description:
  implements independent component analysis (FastICA)
*/

//	Function Definitions

/*----------------------------------------------------------------------*/
/*!  \brief	constructor
	\param p	-> data manager
*/
/*----------------------------------------------------------------------*/
template <class T>
ecICA<T>::ecICA(ecVectorDataManager<T> *p )
{
	pDM=p;
	iIterations=0;
	iMaxIter = 500;
}

/*----------------------------------------------------------------------*/
/*!  \brief	compute covariance from data vector sequence
	\param fpvData()	function that returns a data vector or NULL
	\par Effects:
		on return ::mtCov has covariance matrix filled in and ::iNchan set
*/
/*----------------------------------------------------------------------*/
template <class T>
void
ecICA<T>::SetCovariance( )
{
    VectorXd *pvData;
    //ecVector<T> *pvData;
	int iCnt;
	int i, j;
	for (pvData = pDM->Reset(), iCnt=0; pvData != NULL; pvData = pDM->Iterate(), iCnt++){
		if (!iCnt){
			// 1st iteration so initalize
            iNChan = pvData->rows(); //CHECK
			mtCov.resize(iNChan, iNChan);
            mtCov.setZero();
		}
		// do diagonal + upper right triangular
		T tTmp1, tTmp2;
		for (i=0; i<iNChan; i++)
		{
			for (j=i; j<iNChan; j++)
			{
                tTmp1 = ( (*pvData)[i] ) ; // CHECK
				tTmp2 = ((*pvData)[j]);
                mtCov(i,j) += tTmp1 * tTmp2;
			}
		}
	}

	// fill in remainer by symmetry
	for (i=0; i<iNChan; i++)
	{
		for (j=i+1; j<iNChan; j++)
		{
            mtCov(j,i) = mtCov(i,j);
		}
	}
	mtCov /= (double)iCnt;

	state |= sCovSet;
	return;
}	// end SetCovariance()

/*----------------------------------------------------------------------*/
/*!  \brief	compute spherizing matrix (::mtSphere) from covariance 
	\param fCutoffFxn	truncate eigen values < fCutoffFxn (% total power)
	\throw ecError if no covariance or if error in SVD
*/
/*----------------------------------------------------------------------*/
template <class T>
void
ecICA<T>::Sphere(const float fCutoffFxn )
{
	if (!(state & sCovSet) ){
		throw ecError("[ecICA<T>::Sphere] No covariance matrix.", SevereError);
	}

	// initialize matrices
	ecMatrix<T> mtV(mtCov.ncol(), mtCov.ncol());
	ecMatrix<T> mtW(mtCov.ncol(), 1);

	// find decomposition
	if(SVD(mtCov, mtV, mtW)) {
		throw ecError("[ecICA<T>::Sphere] Error in SVD.", SevereError);
	}

	// sort signular values to determine cutoff
	T tCutoff;
	int iCnt = mtW.nrow();
	ecVector<double> vSort(iCnt);
	ecVector<unsigned long> vSortIndex(iCnt);
	double dTotal = 0.0, dSum = 0.0;
	int j;
	for (j=0, dTotal = 0.0; j<iCnt; j++){
		vSort[j] = (double)mtW[j][0];
		dTotal += vSort[j];
	}
	vSortIndex = vSort.sort();
	for (j=0, dSum = 0.0; j<iCnt; j++){
		dSum += vSort[j];
		if ( (dSum / dTotal) > fCutoffFxn ) {
			tCutoff = (T)vSort[j];
			break;
		}
	}

	// do truncation
	int i;
	for (i=0, iNComp=0; i<iNChan; i++) {
		if ( mtW[i][0] < tCutoff ){
			mtW[i][0] = 0;
		} else {
			mtW[i][0] = (T)( ((double)1.0) / sqrt((double)mtW[i][0]) ); 
			iNComp++;	// count # non-zero components
		}
	}
	if (!iNComp){
		throw ecError("[ecICA<T>::Sphere] No non-zero components.", SevereError);
	}

	// compute final matrix as product V W^(-1/2) U^T
	mtSphere.resize( mtCov.nrow(), mtCov.ncol() );
	diag_rx_m(mtSphere, mtV, mtW);
	mxmT(mtSphere, mtSphere, mtCov);

	// clean up
	state &= !sCovSet;	// we have used it up making the spherizing matrix
	state |= sSphereSet;
	return;
}	// end SetCovariance()

/*----------------------------------------------------------------------*/
/*!  \brief	initialize weight matrix
*/
/*----------------------------------------------------------------------*/
template <class T>
void
ecICA<T>::W_Init()
{
	mtW.resize(iNComp,iNChan);
//	mtW.zero();
	// we will initalize weights to correspond initially to the eigen vectros fo teh covariance matrix
	for (int i=0; i<iNComp; i++){
		for (int j=0; j<iNChan; j++){
			mtW[i][j] = mtCov[j][i];
		}
	}
	return;
}

/*----------------------------------------------------------------------*/
/*!  \brief compute g(u), g'(u)
	\param tU = w^T . x
	\effects value, derivative stored in ::tG, ::tGPrime
	\sa		Hyvarinien and Oja (1999) eq 26, 39
*/
/*----------------------------------------------------------------------*/
template <class T>
void
ecICA<T>::SetG(const T tU )
{
	tG = (T)(-exp( (double)(-( (tU*tU)/2 ) ) ) );
	tGPrime = -( tG * tU );
}	// end SetG()

/*----------------------------------------------------------------------*/
/*!  \brief find next iteration of single weight vector (ie single unit, 1 iteration)
	\param iWhich index of unit to set (row of mtW)
	\par Descsription:
		uses a Gram-Schmidt like decorrelation to remove effects of previous weights (Hyvarinen and Oja, eq 43)
	\effects ::mtW[iWhich] set to new unit-norm vector
	\sa REG notes, Hyvarinen and Oja (1999), p19
*/
/*----------------------------------------------------------------------*/
template <class T>
void
ecICA<T>::SetUnit(int iWhich)
{
	ecVector<T> *pvData;
	ecVector<T>	vtWork(iNChan);	// zero'd on initialization
	int j, iCnt;
	T tU;
	for (pvData = pDM->Reset(), iCnt=0; pvData != NULL; pvData = pDM->Iterate(), iCnt++){
		mxm( *pvData, mtSphere, *pvData);	// spherize data vector (this might be an optimzation issue, ie better done elsewhere?)
		tU = dot( mtW[iWhich], *pvData );
		SetG( tU );
		for (j=0; j<iNChan; j++){
			vtWork[j] += tG * (*pvData)[j];
			vtWork[j] -= tGPrime * mtW[iWhich][j];
		}
	}
	vtWork /= (T)iCnt;
	Decorrelate(vtWork, iWhich);	// remove previous components
	return;
}	// end SetUnit()

/*----------------------------------------------------------------------*/
/*!  \brief Gram Schmidt like decorrelation
	\param vtWork	weight vector prior to decorrelation
	\param iWhich index of unit to set (row of mtW)
	\effects ::mtW[iWhich] set to new unit-norm vector with effects of previous weights removed
	\sa REG notes, Hyvarinen and Oja (1999), eq 43
*/
/*----------------------------------------------------------------------*/
template <class T>
void
ecICA<T>::Decorrelate(ecVector<T> &vtWork, int iWhich)
{
	T	tProj;
	for (int i=0; i<iWhich; i++){
		tProj = dot( vtWork, mtW[i] );
		for (int j=0; j<iNChan; j++){
			vtWork[j] -= tProj * mtW[i][j];
		}
	}
	vtWork.normalize();
	mtW[iWhich] = vtWork;
	return;
}	// end Decorrelate();

/*----------------------------------------------------------------------*/
/*!  \brief find next iteration of all weight vector (ie all units, 1 iteration)
	\effects ::mtW set to new unit-norm vector
	\sa REG notes, Hyvarinen and Oja (1999), p19
*/
/*----------------------------------------------------------------------*/
template <class T>
void
ecICA<T>::SetAllUnits( )
{
	ecVector<T> *pvData;
	ecMatrix<T> mtWTmp( mtW.nrow(), mtW.ncol() );	// accumulated next estimate weight matrix here
	int j, iCnt, iWhich;
	T tU;
	// make sure we are starting with unit weight vectors
	for (iWhich = 0; iWhich<iNComp; iWhich++){
		mtW[iWhich].normalize();
	}
	for (pvData = pDM->Reset(), iCnt=0; pvData != NULL; pvData = pDM->Iterate(), iCnt++){
		mxm( *pvData, mtSphere, *pvData);	// spherize data vector 
		for (iWhich = 0; iWhich<iNComp; iWhich++){
			tU = dot( mtW[iWhich], *pvData );
			SetG( tU );	// sets ::tG, ::tGPrime
			for (j=0; j<iNChan; j++){
				mtWTmp[iWhich][j] += tG * (*pvData)[j];
				mtWTmp[iWhich][j] -= tGPrime * mtW[iWhich][j];
			}
		}
	}	// end foreach data vector
	// load new estimates
	for (iWhich = 0; iWhich<iNComp; iWhich++){
		mtWTmp[iWhich].normalize();
		mtW[iWhich] = mtWTmp[iWhich];
	}
	return;
}	// end SetAllUnits()

/*----------------------------------------------------------------------*/
/*!  \brief decorrelate weight matrix
	\effects ::mtW decorrelated
	\throw ecError if no convergence
	\sa REG notes, Hyvarinen and Oja (1999) p20
*/
/*----------------------------------------------------------------------*/
template <class T>
void
ecICA<T>::Decorrelate()
{
	if (option & oDecorrIterate){
		DecorrIterate();
	} else {
		DecorrSqrt();
	}
	return;
}	// end Decorrelate()

/*----------------------------------------------------------------------*/
/*!  \brief decorrelate weight matrix bia iteration
	\effects ::mtW decorrelated
	\throw ecError if no convergence
	\sa REG notes, Hyvarinen and Oja (1999) eq 45
*/
/*----------------------------------------------------------------------*/
template <class T>
void
ecICA<T>::DecorrIterate()
{
	const iNCrit = 100;	// bail if this many iterations without convergence
	const double dTolCrit = 0.01;	// fractional tolerance defines convergences
	ecMatrix<T>	mtWTW( mtW.nrow(), mtW.nrow() );
	ecMatrix<T>	mtWTemp( mtW.nrow(), mtW.ncol() );
	double dNorm0, dNorm1, dNormDenom;
	// step 1 (Hyvarenin and Oja, eq 45)
	mxmT( mtWTW, mtW, mtW );
	dNormDenom = (double)ACSNorm( mtWTW );
	mtW /= (T)sqrt( dNormDenom );
	dNorm0 = (double)ACSNorm( mtW );	// initialize 
	for (int i=0; i<iNCrit; i++){
		mxmT( mtWTW, mtW, mtW );
		mxm( mtWTemp, mtWTW, mtW );
		for (int m=0; m<mtW.nrow(); m++){
			for (int n=0; n<mtW.ncol(); n++){
				mtW[m][n] = (1.5 * mtW[m][n]) - (0.5 * mtWTemp[m][n]) ;
			}
		}
		dNorm1 = ACSNorm( mtW );
		double dFrac = fabs ( (dNorm1 - dNorm0) / dNorm0 );
		if ( dFrac < dTolCrit ){
			break;	// convergence attained
		}
		dNorm0 = dNorm1;	// use this one at start of next loop
	}
	if ( i >= iNCrit){
		throw ecError("[ecICA<T>::Decorrelate] No convergence.", SevereError);
	}
	return;
}	// end Decorrelate()

/*----------------------------------------------------------------------*/
/*!  \brief decorrelate weight matrix, square root method
	\effects ::mtW decorrelated
	\sa Hyvarinen and Oja (1999) eq 44
*/
/*----------------------------------------------------------------------*/
template <class T>
void
ecICA<T>::DecorrSqrt()
{
	ecMatrix<T>	mtWWT( mtW.nrow(), mtW.nrow() );
	mxmT( mtWWT, mtW, mtW );
	InverseSqrt( mtWWT );
	mxm( mtW, mtWWT, mtW );	
	return;
}	// end DecorrSqrt()

/*----------------------------------------------------------------------*/
/*!  \brief initialize data 
	\param	fCutoffFxn	 truncate eigenvalues > this param (% total power)(implicitly determines # independent components)
	\effects ::mtSphere, ::mtW initialized
*/
/*----------------------------------------------------------------------*/
template <class T>
void
ecICA<T>::Init( const float fCutoffFxn  )
{
	SetCovariance( );
	Sphere ( fCutoffFxn );
	W_Init();		// inital guess for w-matrix
	return;
}	// end Init()

/*----------------------------------------------------------------------*/
/*!  \brief decorrelate weight matrix
	\param fpvData()	function that returns a data vector or NULL
	\return	fractional diff wrt previous iteration
	\effects ::mtW set
	\par Descsription
		calling routine should call this as many times as required to abtrain convertgence,
		reinitializing the fpvData() function with each call
*/
/*----------------------------------------------------------------------*/
template <class T>
double
ecICA<T>::Iterate( )
{
	if (iIterations){
		// save previous values to testing convergence
		mtWPrev = mtW;
	}
	if (option & oSimulDecorr){
		// loop over each unit, then use simultaneous decorrelation
		SetAllUnits( );
		Decorrelate();
	} else {
		// decorrelate after each unit
		for (int i=0; i<iNComp; i++){
			SetUnit(i);
		}
	}
	// get return value
	if (iIterations++){
		return( GetMinDivergence() );
	} else {
		return( 0.0 );	// return 0.0 by convention on 1st call
	}
}	// end Iterate()

/*----------------------------------------------------------------------*/
/*!  \brief returns min fabs(divergence) between iterations across all weight vectors
	\return	returns min fabs(divergence) between iterations across all weight vectors
	\par Descsription
		divergence is cos between previous and current weight vector for 1 unit
*/
/*----------------------------------------------------------------------*/
template <class T>
double
ecICA<T>::GetMinDivergence()
{
	double dMin = 1.0;
	for (int i=0; i<iNComp; i++){
		double dTmp = cos( mtW[i], mtWPrev[i] );
		if ( fabs(dTmp) < dMin ) dMin = fabs( dTmp );
	}
	return( dMin );
}	// end GetMinDivergence()

/*----------------------------------------------------------------------*/
/*!  \brief find independent components
	\param fCritNew new convrgence criterion (0<=fCritNew<=1)
	\throw ecError if no convergence in < ::iMaxIter iterations
*/
/*----------------------------------------------------------------------*/
template <class T>
void
ecICA<T>::Solve( const float fCritNew )
{
	fCrit = fCritNew;
	if ( !(state & sSphereSet) ){
		throw ecError("[ecICA<T>::Solve] Unitialized.", SevereError);
	}
	for (int i=0 ;i<iMaxIter; i++){
		if (Iterate() >= fCrit ){
			return;
		}
	}
	throw ecError("[ecICA<T>::Solve] Failed to converge.", Warning);
}	// end Solve()


