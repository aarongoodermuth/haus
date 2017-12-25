#ifdef _DEBUG
#include <assert.h>
#define ASSERT(x) assert(x)
#define ASSERTIMPLIES(x, y) do{if(x){assert(y);}} while (false)
#define NOTREACHED() assert(false)
#define NOTREACHEDRET(x) do{assert(false); return x;}while(false)
#else
#define ASSERT(x) 0
#define ASSERTIMPLIES(x, y) 0
#define NOTREACHED() 0
#define NOTREACHEDRET(x) return x
#endif