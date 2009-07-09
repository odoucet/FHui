#pragma once

using namespace System;

namespace FHUI
{

public ref class FHUIParsingException : public Exception
{
public:
    FHUIParsingException() : Exception() {}
    FHUIParsingException(String ^err) : Exception(err) {}
    FHUIParsingException(String ^err, Exception ^ex) : Exception(err, ex) {}
};

public ref class FHUIDataIntegrityException : public Exception
{
public:
    FHUIDataIntegrityException() : Exception() {}
    FHUIDataIntegrityException(String ^err) : Exception(err) {}
    FHUIDataIntegrityException(String ^err, Exception ^ex) : Exception(err, ex) {}
};

public ref class FHUIDataImplException : public Exception
{
public:
    FHUIDataImplException() : Exception() {}
    FHUIDataImplException(String ^err) : Exception(err) {}
    FHUIDataImplException(String ^err, Exception ^ex) : Exception(err, ex) {}
};

} // namespace FHUI
