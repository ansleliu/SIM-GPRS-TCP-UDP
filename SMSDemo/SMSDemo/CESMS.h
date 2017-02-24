#pragma once
#include "CESeries.h"
#include"Encode.h"
typedef struct {
	int len;
	char data[16384];
} SM_BUFF;
class CCESMS
{
public:
	CCESMS(void);
	~CCESMS(void);
public:

private:
};