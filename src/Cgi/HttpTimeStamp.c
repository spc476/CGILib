/***************************************************************************
*
* Copyright 2024 by Sean Conner.
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

#include <time.h>
#include <assert.h>

#include "../cgi.h"

char *HttpTimeStamp(char *dest,size_t len,time_t when)
{
  assert(len >= 30);
  strftime(dest,len,"%a, %d %b %Y %H:%M:%S %Z",gmtime(&when));
  return dest;
}
