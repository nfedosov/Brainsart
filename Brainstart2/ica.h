#ifndef 	__ICA_H
#define	__ICA_H
/*---------------------------------------------------------------------------
/*	Name:	$Header: //Aristotle/emseproject/src/emse52/include/rcs/ica.h 1.1 2006/02/11 03:57:53 Aristotle Exp $
/* (c) Copyright 1999-2001, Source Signal Imaging, Inc. All Rights Reserved	*/
/*---------------------------------------------------------------------------
/*
 *	$Log$
 */

//	Preprocessor directives

// use this section for a header used in a dll
#ifdef DOXYGEN
	#define STATLIB_API
#elif STATLIB_EXPORTS
	#define STATLIB_API  __declspec(dllexport)
#else
	#define STATLIB_API __declspec(dllimport)
#endif

// Include directives
//#include	<ecBase.h>
//#include	<Vectools.h>

#include <C:/Users/Fedosov/Documents/projects/Brainstart/Brainstart/eigen-3.4.0/Eigen/Dense>

using namespace Eigen;;


//	Prototypes

//	Class Definitions
template <class T> class STATLIB_API ecVectorDataManager
{
public:
    virtual	VectorXd *Reset() = 0;
    //virtual	ecVector<T>	*Reset() = 0;
    virtual	VectorXd *Iterate() = 0;
    //virtual	ecVector<T>	*Iterate() = 0;
	virtual	int			GetVectorCount() = 0;
	virtual	int			GetVectorDim() = 0;
};

template <class T> class STATLIB_API ecICA //: public ecBase
{
private:
	T	tG;
	T	tGPrime;

    bool state;
	int iNComp;				// number of components
	int iNChan;				// number of channels
	double	dFnxl;		// fractional tolerance for weights conergence
	int	iIterations;	// count iterations of main loop
    MatrixXd mtWPrev;
    //ecMatrix<T>	mtWPrev;		// Weight (inverse mixing) matrix (each row is a component)
	ecVectorDataManager<T>	*pDM;
	float	fCrit;		//	stopping criterion
	int		iMaxIter;	// backup stopping criterion
protected:
    MatrixXd	mtCov;
    //ecMatrix<T>	mtCov;
    MatrixXd	mtSphere;
    //ecMatrix<T>	mtSphere;

	void	SetCovariance( );
	void	Sphere( const float=0.99 );
	void	W_Init();
	void	SetUnit( int );
	void	SetAllUnits( );
	void	SetG( const T );
	void	Decorrelate();
	void	DecorrSqrt();
	void	DecorrIterate();
	double	GetMinDivergence();
	double	Iterate( );
    void	Decorrelate( VectorXd &, int );
    //void	Decorrelate( ecVector<T> &, int );
public:
    MatrixXd	mtW;
    //ecMatrix<T>	mtW;		//!< Weight (inverse mixing) matrix (each row is a component)
	enum	{sCovSet = 0x1, sSphereSet = 0x2};
	enum	{oSimulDecorr = 0x1, oDecorrIterate = 0x02};

	ecICA(ecVectorDataManager<T> *p);
	ecICA(const ecICA<T> &)	{};
	void	operator=(const ecICA<T>&)	{};
	int		GetNComp() const	{return(iNComp);};
	int		GetNChan() const	{return(iNChan);};
	void	SetMaxIterations(int i)	{iMaxIter = i;};
	void	Init( const float=0.99 );
	void	Solve( const float=0.99 );
};

template class  STATLIB_API ecICA<double>;
template class  STATLIB_API ecICA<float>;
template class  STATLIB_API ecICA<long>;



#endif	// sentry
