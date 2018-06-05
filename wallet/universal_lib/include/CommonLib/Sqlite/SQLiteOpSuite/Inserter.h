// NOLINT (all) :)
// Author : WuJian

#pragma once
#include "Operator.h"

class CInserter : public COperator
{
public:
	UNIVERSAL_LIB_API CInserter(CSQLite3DB *pDB, const TableDescriptor &tab);

	template<typename ... CConditionTreeWithAssign>
	CInserter& Insert(CConditionTreeWithAssign &... args)
	{
		BeginOperation();
		int _[] = { (PushExpr(args.t), 0)... };
		_se_unused_(_); // warning C4189: '_' : local variable is initialized but not referenced
		return (*this);
	}

	UNIVERSAL_LIB_API CInserter& OrReplace();

	void MakeSQLStr() override;

protected:
	
	UNIVERSAL_LIB_API void BeginOperation();
	UNIVERSAL_LIB_API void MakeLeft();
	UNIVERSAL_LIB_API void MakeRight();
};

