#pragma once
#define IF_SET_BITS(setOrNot, source, bits) ((setOrNot) ? ((source) | (bits)) : ((source) & !(bits)))
