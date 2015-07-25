#pragma once


// Abstract class providing interface to object which require input and output to file.
// Derived classes are to implement the two methods.
class CSerializable
{
public:
	// pData may be used to point to any additional information required by the method.
	// This is done so because these methods are for general use. These methods should return
	// true if the operation succeeds.
	virtual bool	Read(std::ifstream& in, void* pData)		= 0;
	virtual bool	Write(std::ofstream& out, void* pData)		= 0;

public:
	CSerializable()				{ };
	virtual ~CSerializable()	{ };
};