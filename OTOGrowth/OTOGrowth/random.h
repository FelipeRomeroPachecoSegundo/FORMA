

#define MAXSIZE			4096
#define NUMBERLEN		20
#define RMAX			4294967295UL 			/* 232-1*/
#define MAXPARTICLES	200000
#define s1new			(s1=(18000*(s1&65535)+(s1>>16)))
#define s2new			(s2=(30903*(s2&65535)+(s2>>16)))
#define TWOPI			6.283185307179586477
#define PI				3.14159265359

unsigned long s1 = 1, s2 = 2, unicalls = 0;

#pragma once
#pragma once
