// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

//#include "targetver.h"

#include <stdio.h>
#include <iostream>
#include <fstream> // Include the <fstream> header
#include <sstream>


#include <random>
#include <chrono>
#include <thread>
#include <QWidget>
#include <QString>
#include <QDebug>
#include <QLineEdit>
#include <QPushButton.h>
#include <QtGlobal>
#include <QTimer>
#include <QThread>
#include "qlistwidget.h"

#define _USE_MATH_DEFINES
#include <math.h>
#include <complex>
#include <string>
#include <vector>

#include "./eigen-3.4.0/Eigen/Dense"
#include <./eigen-3.4.0/unsupported/Eigen/FFT>

#include "liblsl-master/include/lsl_cpp.h"

#include "utils.h"
#include "cfir.h"
#include "simplebarfbwin.h"
#include "datareceiver.h"
#include "qlistwidget.h"
#include "code_iir/IIR.h"
#include "firwin.h"
#include "idataprocessor.h"
#include "whitekf.h"
#include "kalmanparamchoice.h"
#include "qboxlayout.h"
#include "qlabel.h"
#include "savedata.h"
#include "signalplotwin.h"
#include "constants.h"

using namespace Eigen;
using namespace lsl;




