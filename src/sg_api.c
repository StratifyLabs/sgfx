//Copyright 2011-2017 Tyler Gilbert; All Rights Reserved

#include "sg.h"


const sg_api_t m_sg_api = {
		.version = SG_VERSION,
};



const sg_api_t * sg_api(){ return &m_sg_api; }

