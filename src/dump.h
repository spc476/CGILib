/***************************************************************************
*
* Copyright 2016 by Sean Conner.
*
* This library is free software; you can redistribute it and/or modify it
* under the terms of the GNU Lesser General Public License as published by
* the Free Software Foundation; either version 3 of the License, or (at your
* option) any later version.
*
* This library is distributed in the hope that it will be useful, but
* WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
* or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public
* License for more details.
*
* You should have received a copy of the GNU Lesser General Public License
* along with this library; if not, see <http://www.gnu.org/licenses/>.
*
* Comments, questions and criticisms can be sent to: sean@conman.org
*
*************************************************************************/

#ifndef I_B085509E_4B2C_59EA_87CE_25537724EAFD
#define I_B085509E_4B2C_59EA_87CE_25537724EAFD

#include <stdint.h>
#include <stdio.h>

#define DEF_DUMP_BYTES	16

void hex            (char *,size_t,uintptr_t,size_t);
int  hexdump_mems   (char *,size_t,const void *,size_t,size_t);
int  chardump_mems  (char *,size_t,const void *,size_t,size_t);
int  dump_mems      (char *,size_t,const void *,size_t,size_t,size_t);
int  dump_memorys   (char *,size_t,const void *,size_t,size_t,size_t);
int  dump_memoryf   (FILE *,       const void *,size_t,size_t,size_t);
int  dump_memoryl   (int,          const void *,size_t,size_t,size_t);

#endif
