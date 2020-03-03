#pragma once

#define BUFF_SIZE 0x10000

#define CHECK_BREAK(condtion) if((condtion)){ break; }
#define CHECK_CONTINUE(condtion) if((condtion)){ continue; }
#define IsNull(pStr) (NULL == pStr || 0 == pStr[0])

#define ERR_BREAK(con , ret ,err ) \
	if( con )\
{\
	ret = err;\
	break;\
}

#define ERR_CONTINUE(con , ret ,err ) \
	if( con )\
{\
	ret = err;\
	continue;\
}

#define ARRYDELETE(pArry)\
	if(pArry)\
	{\
		delete []pArry;\
	    pArry = NULL;\
	}
