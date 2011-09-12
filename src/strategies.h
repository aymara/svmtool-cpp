/*
 * Copyright (C) 2004 Jesus Gimenez, Lluis Marquez and Senen Moya
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef STRAT_H

//Strategy 0 .- one-pass        (default)
#define STRA_1P_DEFAULT 0 

//Strategy 1 .- two-passes [revisiting results and relabeling]
#define STRA_2P_RELABELING 1 

//Strategy 2 .- one-pass   [robust against unknown words]
#define STRA_1P_ROBUST_UNK 2 

//Strategy 3 .- one-pass   [unsupervised learning models]
#define STRA_1P_UNSUPERVISED 3 

//Strategy 4 .- one-pass   [very robust against unknown words]
#define STRA_1P_VERY_ROBUST_UNK 4 

//Strategy 5 .- one-pass   [sentence-level likelihood]
#define STRA_1P_SENTENCE_LEVEL 5 

//Strategy 6 .- one-pass   [robust sentence-level likelihood]
#define STRA_1P_ROBUST_SENTENCE_LEVEL 6 

#define STRAT_H
#endif
