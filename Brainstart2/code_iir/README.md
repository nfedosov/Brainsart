# The IIR Filter Library

## Infinite Impulse Response
The IIR library is can be used to design digital filters with infinite impulse response using the bilinear trasformation. To use the library, it is necessary to include the IIR.h in your project (and compile the IIR.cpp file). The IIR library has a modular technology through the use of Object Oriented Programming.

## Filter types

The IIR library can be used to design low-pass filters or high-pass filters.

### Approximations

The library includes: Butterworth filters, Chebyshevs filters and Elliptic filters.

### Dependencies

As the IIR library was designed using standard C++, the only requirement to use and integrate the IIR library is a compiler that supports standard C++. Therefore, the IIR library can be used in: Microsoft Windows, Apple IOS, Linux, UNIX, Android (using the Android Native Development Kit), or other C++ compiler.

### The library has the following classes

- **IIR::BiquadSection** This class represents a digital biquad section.
- **IIR::PolarBiquadSection** This class represents a digital biquad section with the poles and zeros stored using the polar format.
- **IIR::BiquadsCascade** This class represents a collection of digital biquad sections and it is used to test the filter in real time.
- **IIR::PolarBiquadsCascade** This class represents a collection of digital biquad sections and it is used to compute the frequency response magnitude and group delay of the filter.
- **IIR::BiquadFilter** This is the base class to create, analyze and test IIR digital filters.
- **IIR::ButterworthFilter** This class is derived from IIR::BiquadFilter and it includes the algorithm implementation to create a Butterworth filter.
- **IIR::ChebyshevFilter** This class is derived from IIR::BiquadFilter and it is used to design Chebyshev filters.
- **IIR::EllipticFilter** This class is derived from IIR::BiquadFilter and it includes the implementation of the algorithms to design an Elliptic filter.

## Troubleshooting

Please review the IIRFilterDesign.cpp file to learn how to use the library.  To report a bug, please write to selo@ugto.mx
