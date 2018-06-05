// Author : WuJian

#pragma once

#include "Operator.h"

class CDeleter : public COperator
{
public:
	UNIVERSAL_LIB_API CDeleter(CSQLite3DB *pDB, const TableDescriptor &tab);

	UNIVERSAL_LIB_API CDeleter& Delete();
	UNIVERSAL_LIB_API CDeleter& Where(CConditionTree &w);

	virtual void MakeSQLStr();
};