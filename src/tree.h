/***************************************************************************
*
* Copyright 2013 by Sean Conner.
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

#ifndef I_4A9906A3_420C_5C59_93AD_DB033575E197
#define I_4A9906A3_420C_5C59_93AD_DB033575E197

typedef struct tree
{
  struct tree *left;
  struct tree *right;
  int          height;
} tree__s;

extern tree__s *tree_insert     (tree__s *restrict,tree__s *restrict,int (*)(const void *,const void *));
extern tree__s *tree_find       (tree__s *,const void *,int (*)(const void *,const void *));
extern tree__s *tree_remove     (tree__s *,const void *,int (*)(const void *,const void *),tree__s **);

#endif
